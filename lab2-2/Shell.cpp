#include"Shell.h"

Shell::Shell(int clientFd): input_fd(clientFd), cmdExec(CommandExecutor(clientFd)){}

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
    setupEnviroment();
    std::string input;
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    input = IOManager::getInput(input_fd);
    if(input.empty()){
        dprintf(input_fd, "%s", "% ");
        return false;
    }
    std::cout << "input: " << input << "\n";
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
            cmd.args = std::move(argsOfCurrentCommand);
            // flush args
            argsOfCurrentCommand.clear();

            if(isPipedIn()){
                cmd.pipe = pipeCounter[0];
                pipeCounter.erase(0);
            }

            if(Parser::isRedirect(arg)){
                std::string fileName;
                fileName = tokens[++i];
                setRedirectFile(cmd, fileName);
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
