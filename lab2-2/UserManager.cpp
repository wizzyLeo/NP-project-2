#include "UserManager.h"
#define MAX_USER_ID 30

UserManager::UserManager() {
    for (int i = 1; i <= MAX_USER_ID; i++) {
        ids_available.insert(i);
    }
    std::cout << "User Manager Size: " << ids_available.size() << std::endl;
    std::cout << "UserManager created" << std::endl;
}

UserManager& UserManager::getInstance() {
    static UserManager instance;
    return instance;
}

std::set<int>& UserManager::getIdsAvailable() { return ids_available; }
std::map<int, int>& UserManager::getIdFdMap() { return id_fd; }
std::unordered_map<int, int>& UserManager::getFdIdMap() { return fd_id; }
std::unordered_map<int, std::string>& UserManager::getIdNameMap() { return id_name; }
std::set<std::string>& UserManager::getNames() { return names; }
std::unordered_map<int, std::unordered_map<std::string, std::string>>& UserManager::getIdEnvMap() { return id_env; }
std::map<std::pair<int, int>, int>& UserManager::getUserPipeMap() { return user_pipe; }