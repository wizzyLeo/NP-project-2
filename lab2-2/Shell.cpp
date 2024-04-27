#include"Shell.h"

Shell::Shell(int clientFd): input_fd(clientFd), cmdExec(CommandExecutor(clientFd)), userManager(UserManager::getInstance()),ids_available(userManager.getIdsAvailable()), id_fd(userManager.getIdFdMap()), fd_id(userManager.getFdIdMap()), id_name(userManager.getIdNameMap()), names(userManager.getNames()), id_env(userManager.getIdEnvMap()), user_pipe(userManager.getUserPipeMap()){}

bool Shell::isPipedIn(){
    return pipeCounter.count(0);
}
void Shell::agePipeCounter(){
    std::unordered_map<int, std::array<int, 2> >newCounter;
    for(const auto&[key, value] : pipeCounter){
        newCounter.emplace(key - 1, value);
    }

    pipeCounter = std::move(newCounter);
}

void Shell::registerNumberPipe(int pipeTo){
    if(pipeCounter.count(pipeTo) != 0){
        return;
    }

    std::array<int, 2> pipeFds;

    //wait for released source
    while(pipe(pipeFds.data()) == -1){
        if(errno == EMFILE || errno == ENFILE){
            wait(nullptr);
        }
    }

    // close pipe after execution
    fcntl(pipeFds[0], F_SETFD, FD_CLOEXEC);
    fcntl(pipeFds[1], F_SETFD, FD_CLOEXEC);
    pipeCounter.emplace(pipeTo, pipeFds);
    return;
}

void Shell::setupEnviroment(){
    setenv("PATH", "bin:.", 1);
}

void Shell::setRedirectFile(command_t& cmd, std::string fileName){
    cmd.fd_out = open(fileName.c_str(), O_CREAT | O_WRONLY | O_TRUNC | O_CLOEXEC, 0664);
}

void Shell::setCommandIO(command_t& cmd, int pipeTo, std::string arg){
    if(Parser::isPipe(arg)){
        cmd.fd_out = pipeCounter[pipeTo][1];
    }else if(Parser::isErrorPipe(arg)){
        cmd.fd_err = pipeCounter[pipeTo][1];
        cmd.fd_out = pipeCounter[pipeTo][1];
    }
}


bool Shell::run(){
    const int user_id = fd_id[input_fd];
    // setupEnviroment();
    std::string input;
    std::vector<std::string> tokens;
    std::string token;
    input = IOManager::getInput(input_fd);
    std::istringstream iss(input);
    if(input.empty()){
        dprintf(input_fd, "%s", "% ");
        return false;
    }
    // tokens = Parser::getTokenizedLine(input);
    while (iss >> token) { // Ensure there is no condition where this could loop indefinitely
        tokens.push_back(token);
    }

    std::vector<std::string> argsOfCurrentCommand;
    
    for(int i=0;i < tokens.size();++i){
        std::string arg;
        arg = tokens[i];
        if(Parser::isPipeOrRedirect(arg)){
            command_t cmd;
            cmd.sender_id = user_id;
            cmd.args = std::move(argsOfCurrentCommand);
            // flush args
            argsOfCurrentCommand.clear();

            if(Parser::isUserPipeIn(arg)){
                cmd.pipe[0] = open("/dev/null", O_RDWR | O_CLOEXEC);
                int sender_id = std::stoi(arg.substr(1));
                if(id_fd.count(sender_id) == 0){
                    dprintf(id_fd[user_id], "*** Error: user #%d does not exist yet. ***\n", sender_id);
                }else{
                    // check if user pipe exists
                    if (user_pipe.count(std::make_pair(user_id, sender_id)) == 0) { // user pipe doesn't exist
                        dprintf(id_fd[user_id],
                                "*** Error: the pipe #%d->#%d does not exist yet. ***\n",
                                sender_id,
                                user_id);
                    }else{
                        cmd.pipe[0] = user_pipe[std::make_pair(user_id, sender_id)];
                        user_pipe.erase(std::make_pair(user_id, sender_id));
                        // broadcast user pipe event
                        for (const auto &[key, value] : fd_id) {
                            dprintf(key,
                                "*** %s (#%d) just received from %s (#%d) by '%s' ***\n",
                                id_name[user_id].c_str(),
                                user_id,
                                id_name[sender_id].c_str(),
                                sender_id,
                                iss.str().c_str());
                        }
                    }
                    if (!std::getline(iss, arg, ' ')) { // get next arg for output
                        arg.clear();
                    }
                }
                return false;
            }

            if(isPipedIn()){
                cmd.pipe = pipeCounter[0];
                pipeCounter.erase(0);
            }
            std::string user_pipe_out_msg;
            if(Parser::isRedirect(arg)){
                std::string fileName;
                fileName = tokens[++i];
                setRedirectFile(cmd, fileName);
            }else if(Parser::isUserPipeOut(arg)){
                cmd.fd_out = open("/dev/null", O_RDWR | O_CLOEXEC);
                int receiver_id = std::stoi(arg.substr(1));
                if (id_fd.count(receiver_id) == 0) { // receiver doesn't exist
                    user_pipe_out_msg = " *** Error: user #" + arg.substr(1) + " does not exist yet. ***\n";
                } else {
                    if (user_pipe.count(std::make_pair(receiver_id, user_id)) == 1) { // user pipe already exist
                    user_pipe_out_msg =
                        " *** Error: the pipe #" + std::to_string(user_id) + "->#" + arg.substr(1) + " already exists. ***\n";
                    } else {
                    std::array<int, 2> pipe_fd;
                    while (pipe(pipe_fd.data()) == -1) {
                        if (errno == EMFILE || errno == ENFILE) {
                        wait(nullptr); // wait for any child process to release resource
                        }
                    }
                    fcntl(pipe_fd[0], F_SETFD, FD_CLOEXEC);

                    cmd.fd_out = pipe_fd[1];
                    user_pipe[std::make_pair(receiver_id, user_id)] = pipe_fd[0];
                    user_pipe_out_msg =
                        "*** " + id_name[user_id] + " (#" + std::to_string(user_id) + ") just piped \'" + iss.str() + "\' to "
                            + id_name[stoi(arg.substr(1))] + " (#" + arg.substr(1) + ") ***\n";
                    }
                }
            }else{
                // Pipe:
                int pipeTo = Parser::getPipeTo(arg);
                registerNumberPipe(pipeTo);
                setCommandIO(cmd, pipeTo, arg);
            }
            cmdExec.execute(cmd);
            if(arg != "|"){
                agePipeCounter();
            }
        }else{
            argsOfCurrentCommand.emplace_back(arg);
        }
    }
    if(!argsOfCurrentCommand.empty()){
        command_t cmd;
        cmd.sender_id = user_id;
        cmd.args = std::move(argsOfCurrentCommand);

        if(isPipedIn()){
            cmd.pipe = pipeCounter[0];
            pipeCounter.erase(0);
        }
        cmdExec.execute(cmd);
        agePipeCounter();
    }
    if (input == "exit") {
        return true;
    } else {
        dprintf(input_fd, "%s", "%");
        return false;
    }
}
