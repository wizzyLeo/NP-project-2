#ifndef USER_MANAGER_H
#define USER_MANAGER_H
#include <set>
#include <map>
#include <unordered_map>
#include <string>
#include <iostream>

class UserManager {
public:
    static UserManager& getInstance();

    std::set<int>& getIdsAvailable();
    std::map<int, int>& getIdFdMap();
    std::unordered_map<int, int>& getFdIdMap();
    std::unordered_map<int, std::string>& getIdNameMap();
    std::set<std::string>& getNames();
    std::unordered_map<int, std::unordered_map<std::string, std::string>>& getIdEnvMap();
    std::map<std::pair<int, int>, int>& getUserPipeMap();

private:
    UserManager();

    std::set<int> ids_available;
    std::map<int, int> id_fd;
    std::unordered_map<int, int> fd_id;
    std::unordered_map<int, std::string> id_name;
    std::set<std::string> names;
    std::unordered_map<int, std::unordered_map<std::string, std::string>> id_env;
    std::map<std::pair<int, int>, int> user_pipe;
};
#endif