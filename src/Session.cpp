
#include "../include/Session.h"
#include "../include/json.hpp"
#include <fstream>
#include <string>
#include "../include/User.h"
#include "../include/Watchable.h"
#include "../include/Action.h"
//#include "Action.cpp"

using namespace std;

//constructor
Session::Session(const std::string &configFilePath)
        : content(), actionsLog(), userMap(), activeUser(nullptr), action(), series() {
    // read a JSON file
    using json = nlohmann::json;
    std::ifstream i(configFilePath);
    json j;
    i >> j;

    int id = 1;
    // Extract movies from JSON
    std::vector<std::string> zero;
    content.push_back(new Movie(-100, "save order", 50000000, zero));
    for (int i = 0; i < (int) j["movies"].size(); i++) {
        std::string name = j["movies"][i]["name"].dump();
        int length = stoi(j["movies"][i]["length"].dump());
        std::vector<std::string> tags;
        for (int n = 0; n < (int) j["movies"][i]["tags"].size(); n++) {

            tags.push_back(j["movies"][i]["tags"][n]);
        }
        content.push_back(new Movie(id++, name, length, tags));
    }

    // Extract TV_Series from JSON
    for (int i = 0; i < (int) j["tv_series"].size(); i++) {
        std::string name = j["tv_series"][i]["name"].dump();
        int length = stoi(j["tv_series"][i]["episode_length"].dump());
        std::vector<std::string> tags;
        for (int n = 0; n < (int) j["tv_series"][i]["tags"].size(); n++) {

            tags.push_back(j["tv_series"][i]["tags"][n]);
        }

        vector<int> seas = j["tv_series"][i]["seasons"];
        series[name] = seas;

        for (int n = 0; n < (int) j["tv_series"][i]["seasons"].size(); n++) {
            int counter = 1;
            vector<int> episodesNum = series.at(name);
            while (counter <= (int) j["tv_series"][i]["seasons"][n]) {
                if ((counter < episodesNum[n]) | (n < (int) series[name].size() - 1)) {

                    content.push_back(new Episode(id, name, length, n + 1, counter, tags, id + 1));
                    id++;
                } else {
                    content.push_back(new Episode(id++, name, length, n + 1, counter, tags));
                }
                counter++;
            }
        }
    }
    userMap["Default"] = new LengthRecommenderUser("Default");
    activeUser = userMap["Default"];
}

Session::Session(std::vector<Watchable *> content, std::vector<BaseAction *> actionsLog,
                 std::unordered_map<std::string, User *> userMap, User *activeUser, std::vector<std::string> action,
                 std::unordered_map<std::string, std::vector<int>> series) : content(content), actionsLog(actionsLog),
                                                                             userMap(userMap), activeUser(activeUser),
                                                                             action(action), series(series) {}

//Destructor
Session::~Session() {
    for (int i = 0; i < (int) content.size(); i++) {
        delete content[i];
    }
    for (int i = 0; i < (int) actionsLog.size(); i++) {
        delete actionsLog[i];
    }
    for (std::unordered_map<std::string, User *>::iterator it = userMap.begin(); it != userMap.end(); ++it) {
        delete it->second;
    }
    userMap.clear();


}//todo

// Copy Constructor
Session::Session(const Session &other)
        : content(), actionsLog(), userMap(), activeUser(nullptr), action(other.action), series(other.series) {

    for (int i = 0; i < (int) other.content.size(); i++) {
        Watchable *e = other.content[i]->clone();
        content.push_back(e);
    }

    for (int i = 0; i < (int) other.actionsLog.size(); i++) {
        BaseAction *b = other.actionsLog[i]->clone();
        actionsLog.push_back(b);
    }

    for (std::unordered_map<std::string, User *>::const_iterator it = other.userMap.begin();
         it != other.userMap.end(); ++it) {
        User *u = other.userMap.at(it->first)->clone(*this);
        userMap[it->first] = u;
    }

    string copyActiveUser = other.getActiveUser(const_cast<Session &>(other))->getName();
    activeUser = userMap.at(copyActiveUser);
}

// Move Constructor
Session::Session(Session &&other) : content(other.content), actionsLog(other.actionsLog), userMap(other.userMap),
                                    activeUser(other.activeUser), action(other.action), series(other.series) {
    other.content.clear();
    other.actionsLog.clear();
    other.userMap.clear();
    other.activeUser = nullptr;
    other.series.clear();
}

// Copy Assignment
Session &Session::operator=(const Session &other) {

    if (this == &other) {
        return *this;
    } else {
        for (int i = 0; i < (int) content.size(); i++) {
            delete content[i];
        }
        content.clear();

        for (int i = 0; i < (int) other.content.size(); i++) {
            Watchable *e = other.content[i]->clone();
            content.push_back(e);
        }

        for (int i = 0; i < (int) actionsLog.size(); i++) {
            delete actionsLog[i];
        }
        actionsLog.clear();

        for (int i = 0; i < (int) other.actionsLog.size(); i++) {
            BaseAction *b = other.actionsLog[i]->clone();
            actionsLog.push_back(b);
        }

        for (std::unordered_map<std::string, User *>::iterator it = userMap.begin(); it != userMap.end(); ++it) {
            delete it->second;
        }
        userMap.clear();

        for (std::unordered_map<std::string, User *>::const_iterator it = other.userMap.begin();
             it != other.userMap.end(); ++it) {
            User *u = other.userMap.at(it->first)->clone(*this);
            userMap[it->first] = u;
        }

        string copyActiveUser = other.getActiveUser(const_cast<Session &>(other))->getName();
        User *copyactive = userMap.at(copyActiveUser);
        this->setActive(copyactive);

        this->action = other.action;
        this->series = other.series;
    }
    return *this;
}

// Move Assignment
Session &Session::operator=(Session &&other) {
    if (this != &other) {
        for (int i = 0; i < (int) content.size(); i++) {
            delete content[i];
        }
        content.clear();
        for (int i = 0; i < (int) actionsLog.size(); i++) {
            delete actionsLog[i];
        }
        actionsLog.clear();
        //delete activeUser;

        for (std::unordered_map<std::string, User *>::iterator it = userMap.begin(); it != userMap.end(); ++it) {

            delete it->second;
        }
        userMap.clear();
        content = other.content;
        actionsLog = other.actionsLog;
        userMap = other.userMap;
        this->setActive(other.activeUser);
        action = other.action;
        series = other.series;
        vector<Watchable *> vec;
        other.content = vec;
        vector<BaseAction *> act;
        other.actionsLog = act;
        std::unordered_map<std::string, User *> map;
        other.userMap = map;
        other.activeUser = nullptr;
        vector<std::string> str;
        other.action = str;
        std::unordered_map<std::string, std::vector<int>> ser;
        other.series = ser;
    }
    return *this;
}


User *Session::getActiveUser(Session &) const {
    return activeUser;
}

std::vector<Watchable *> Session::getContent(Session &) const {
    return content;
}


std::vector<BaseAction *> Session::getActionLog(Session &) const {
    return actionsLog;
}

std::unordered_map<std::string, User *> Session::getMap(Session &) const {
    return userMap;
}

void Session::addLenUser(std::string name) {
    LengthRecommenderUser *user = new LengthRecommenderUser(name);
    userMap[name] = user;

}

void Session::addRerUser(std::string name) {
    RerunRecommenderUser *user = new RerunRecommenderUser(name);
    userMap[name] = user;
}

void Session::addgenUser(std::string name) {
    GenreRecommenderUser *user = new GenreRecommenderUser(name);
    userMap[name] = user;
}

void Session::addToUserMap(std::string name, User *user) { userMap[name] = user; }

void Session::setActive(User *user) {
    activeUser = user;
}

void Session::addToLog(BaseAction *action) {
    actionsLog.push_back(action);
}

void Session::setInput(std::string s) {
    action[1] = s;
}

void Session::deleteUser(string name) {

    delete (userMap[name]);
    userMap.erase(name);
}


std::vector<std::string> Session::getInput(Session &) const { return action; }

void Session::start() {

    cout << ("SPLFLIX is now on!\n");

    string actionString = "";
    bool on = true;

    while (on) {

        if (actionString != "" && action[0] == "start") { on = true; }

        getline(cin, actionString);
        actionString = actionString + " ";

        action = makeVector(actionString);

        if (action[0] == "createuser") {

            CreateUser *creat = new CreateUser();
            creat->act(*this);
        }

        if (action[0] == "changeuser") {
            ChangeActiveUser *active = new ChangeActiveUser;
            active->act(*this);
        }

        if (action[0] == "deleteuser") {
            DeleteUser *delUser = new DeleteUser;
            delUser->act(*this);
        }

        if (action[0] == "dupuser") {
            DuplicateUser *duplicate = new DuplicateUser;
            duplicate->act(*this);
        }

        if (action[0] == "content") {
            PrintContentList *content = new PrintContentList;
            content->act(*this);
        }

        if (action[0] == "watchhist") {
            PrintWatchHistory *watchlist = new PrintWatchHistory;
            watchlist->act(*this);
        }

        if (action[0] == "watch") {
            Watch *watch = new Watch;
            watch->act(*this);
        }

        if (action[0] == "log") {
            PrintActionsLog *log = new PrintActionsLog;
            log->act(*this);
        }

        if (action[0] == "exit") {
            Exit *exit = new Exit;
            exit->act(*this);
            on = false;
        }
    }
}

std::vector<std::string> Session::makeVector(std::string input) {
    std::vector<string> output;
    std::string delimiter = " ";
    size_t pos = 0;
    std::string token;
    while ((pos = input.find(delimiter)) != std::string::npos) {
        token = input.substr(0, pos);
        output.push_back(token);
        input.erase(0, pos + delimiter.length());
    }
    return output;
}

