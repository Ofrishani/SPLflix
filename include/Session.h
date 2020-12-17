#ifndef SESSION_H_
#define SESSION_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "Action.h"

class User;
class Watchable;

class Session{
public:
    // Constructor
    Session(const std::string &configFilePath);
    Session ( std::vector<Watchable*> content, std::vector<BaseAction*> actionsLog, std::unordered_map<std::string,User*> userMap, User* activeUser,std::vector<std::string> action, std::unordered_map<std::string,std::vector<int>> series);

    // Destructor
    ~Session();

    // Copy Constructor
    Session(const Session &other);

    // Move Constructor
    Session(Session &&other);

    // Copy Assignment
    Session& operator=(const Session& other);

    // Move Assignment
    Session& operator=(Session &&other);


    void start();
    User* getActiveUser(Session &) const;
    std::vector<Watchable*> getContent(Session &) const;
    std::vector<BaseAction*> getActionLog(Session &) const;
    std::vector<std::string> getInput(Session &) const;
    std:: unordered_map<std::string,User*> getMap(Session&) const ;
    void addLenUser(std::string);
    void addRerUser(std::string);
    void addgenUser(std::string);
    void addToUserMap (std::string, User*);
    void setActive(User*);
    void addToLog(BaseAction*);
    void setInput(std::string);
    void deleteUser(std::string);
    std:: vector<std::string> makeVector (std::string);


private:
    std::vector<Watchable*> content;
    std::vector<BaseAction*> actionsLog;
    std::unordered_map<std::string,User*> userMap;
    User* activeUser;
    std::vector<std::string> action;
    std::unordered_map<std::string,std::vector<int>> series;

};
#endif
