#include "IOManager.h"

std::string IOManager::getInput(int input_fd) {
    std::string line;
    char recv_buf[1];
    while (recv(input_fd, recv_buf, 1, 0)) {
    if (recv_buf[0] == '\r') {
      continue;
    }
    if (recv_buf[0] == '\n') {
      break;
    }
    line.append(recv_buf, 1);
  }
  return line;
}