#include "CommandExecutor.h"


void CommandExecutor::setupChildIO(command_t& cmd){
    if(cmd.pipe[0] != STDIN_FILENO){
        dup2(cmd.pipe[0], STDIN_FILENO);
        close(cmd.pipe[0]);
    }
    if(cmd.fd_out != STDOUT_FILENO){
        dup2(cmd.fd_out, STDOUT_FILENO);
        close(cmd.fd_out);
    }
    if(cmd.fd_err != STDERR_FILENO){
        dup2(cmd.fd_err, STDERR_FILENO);
        close(cmd.fd_err);
    }
}

void CommandExecutor::execute(command_t &cmd) {
  if (cmd.args[0] == "exit") {
    exit(0);
  }

  if (cmd.args[0] == "setenv") {
    setenv(cmd.args[1].c_str(), cmd.args[2].c_str(), 1);
    return;
  }

  if (cmd.args[0] == "printenv") {
    if (const char *env = getenv(cmd.args[1].c_str())) {
      std::cout << env << '\n';
    }
    return;
  }

  pid_t child;
  while ((child = fork()) == -1) {
    if (errno == EAGAIN) {
      wait(nullptr); // wait for any child process to release resource
    }
  }

  if (child != 0) { // parent process
    // std::cout << "Shell executing: " << cmd.args[0] << std::endl;
    // close fds relevant to child processes and unneeded by parent process
    // close pipe
    if (isUsingPipe(cmd)) {
      // std::cout << "Close Pipe: " << cmd.args[0] << std::endl;
      close(cmd.pipe[0]);
      close(cmd.pipe[1]);
    }
    // close file if fd_out isn't STDOUT_FILENO
    if (isRedirectingToFile(cmd)) {
      // std::cout << "Close File: " << cmd.args[0] << std::endl;
      close(cmd.fd_out);
    }
    // wait for child when fd_out isn't pipe
    if (isEndOfCommand(cmd)) {
      // std::cout << "Waiting: " << cmd.args[0] << std::endl;
      waitpid(child, nullptr, 0);
    }
    return;
  }

  // child process
  // std::cout << "Child: " << cmd.args[0] << std::endl;
  setupChildIO(cmd);
  executeCommand(cmd);
}

void CommandExecutor::executeCommand(const command_t& cmd){
  auto args = getExecCmdArgs(cmd);
  int execDescription = execvp(args[0], args);
  char* currentCommand = args[0];
  if(execDescription == -1 && errno == ENOENT){
    std::cerr << "Unknown command: [" << currentCommand << "].\n";
    exit(0);
  }
}

void CommandExecutor::cleanArgsMemory(char** args, const command_t& cmd){
  for (size_t i = 0; i < cmd.args.size(); ++i) {
        delete[] args[i];
    }
  delete[] args;
}


char** CommandExecutor::getExecCmdArgs(const command_t& cmd) {
    char** args = new char*[cmd.args.size() + 1];
    for (size_t i = 0; i < cmd.args.size(); ++i) {
        args[i] = new char[cmd.args[i].size() + 1];
        strcpy(args[i], cmd.args[i].c_str());
    }
    args[cmd.args.size()] = nullptr;
    return args;
}

bool CommandExecutor::isUsingPipe(command_t cmd){
    return cmd.pipe[0] != STDIN_FILENO;
}

bool CommandExecutor::isRedirectingToFile(command_t cmd){
    struct stat fd_stat;
    fstat(cmd.fd_out, &fd_stat);
    return(cmd.fd_out != STDOUT_FILENO && S_ISREG(fd_stat.st_mode));
}

bool CommandExecutor::isEndOfCommand(command_t cmd){
    struct stat fd_stat;
    fstat(cmd.fd_out, &fd_stat);
    return (!S_ISFIFO(fd_stat.st_mode));
}