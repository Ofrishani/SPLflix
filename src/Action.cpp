    //
    // Created by benhoday@wincs.cs.bgu.ac.il on 25/11/2019.
    //

    #include "../include/Action.h"
    //#include "Action.h"
    #include "../include/Session.h"
    #include "../include/Watchable.h"

    using namespace std;

    //BaseAction
    BaseAction::BaseAction(): errorMsg(""),status(PENDING){ }
    BaseAction::BaseAction(std::string error,ActionStatus status):errorMsg(error),status(status) { }
    BaseAction::~BaseAction() {}

    ActionStatus BaseAction::getStatus() const { return status;}

    void BaseAction::complete() {
        status = COMPLETED;

    }

    void BaseAction ::error(const std::string &errorMsg) {
        status = ERROR;
        this->setErrormsg(errorMsg);
        cout<<(errorMsg);

    }

    std::string BaseAction ::getErrorMsg() const {
        if (errorMsg != ""){
            return (": "+errorMsg);
        } else {
            return errorMsg;
        }

        }

    std::string BaseAction::statusToString(ActionStatus s) const {
        if(s == PENDING){
            return "PENDING";
        } else if (s==COMPLETED){
            return "COMPLETED";
        }   else {
            return "ERROR";
        }
    }

    void BaseAction::setErrormsg(std::string n) { errorMsg=n;}

    //CreateUser
    CreateUser::CreateUser() {}
    CreateUser::CreateUser(std::string error,ActionStatus status):BaseAction(error,status) {}

    void CreateUser::act(Session &sess) {
        vector<std::string> input = sess.getInput(sess);

        string name = input[1];
        string algo = input[2];

        unordered_map<string, User *> map = sess.getMap(sess);
        if (map.find(name)!= map.end()){
            this->error(" Error- There is already a user with this name \n");

        } else {
            if ((algo != "len") & (algo != "rer") & (algo != "gen")) {
                this->error(" Error- Invalid algorithm code \n");
            } else {
                if (algo == "len") {
                    sess.addLenUser(name);
                }
                else if (algo == "gen") {
                    sess.addgenUser(name);
                }
                else if (algo == "rer") {
                    sess.addRerUser(name);
                }
                this->complete();
            }
        }

        sess.addToLog(this);

    }

    std::string CreateUser ::toString() const {
        return (" Create User "+ this->statusToString(this->getStatus()) + this->getErrorMsg()); }

    BaseAction* CreateUser::clone() {
        CreateUser* OUTPUT = new CreateUser(this->getErrorMsg(),this->getStatus());
        return OUTPUT;
    }

    //ChangeActiveUser
    ChangeActiveUser::ChangeActiveUser() {}
    ChangeActiveUser::ChangeActiveUser(std::string error,ActionStatus status) :BaseAction(error,status) {}
    void ChangeActiveUser::act(Session &sess) {
        vector<std::string> input = sess.getInput(sess);
        string name = input[1];
        unordered_map<string, User *> map = sess.getMap(sess);
        if (map.find(name)== map.end()) {
            this->error(" Error- This User doesn't exist \n");
        }else{sess.setActive(map.at(name));
            this->complete();
        }

        sess.addToLog(this);
    }
    std::string ChangeActiveUser::toString() const {
        return (" change active user " +this->statusToString(getStatus())+ this->getErrorMsg()); }

        BaseAction* ChangeActiveUser::clone() {
            ChangeActiveUser* OUTPUT = new ChangeActiveUser(this->getErrorMsg(),this->getStatus());
            return OUTPUT;
    }



    //DeleteUser
    DeleteUser::DeleteUser() {}
    DeleteUser::DeleteUser(std::string error,ActionStatus status) :BaseAction(error,status){}
    void DeleteUser::act(Session &sess) {
        vector<std::string> input = sess.getInput(sess);
        string name = input[1];
        unordered_map<string, User *> map = sess.getMap(sess);
        if (map.find(name)== map.end()) {
            this->error(" Error- This User doesn't exist \n");
        } else {
            sess.deleteUser(name);
            this->complete();
        }
        sess.addToLog(this);
    }

    std::string DeleteUser ::toString() const {
        return (" Delete user "+this->statusToString(getStatus())+ this->getErrorMsg()); }

        BaseAction* DeleteUser::clone() {
            DeleteUser* OUTPUT = new DeleteUser(this->getErrorMsg(),this->getStatus());
            return OUTPUT;
    }

    //DuplicateUser
    DuplicateUser::DuplicateUser() {}
    DuplicateUser::DuplicateUser(std::string error,ActionStatus status):BaseAction(error,status) {}
    void DuplicateUser::act(Session &sess) {
        vector<std::string> input = sess.getInput(sess);
        string nameOriginal = input[1];
        string nameNew = input[2];
        unordered_map<string, User *> map = sess.getMap(sess);
        if (map.find(nameNew)!= map.end()) {
            this->error(" Error- There is already a user with this name \n");
        } else if (map.find(nameOriginal)== map.end()){
            this->error(" Error-The original user doesn't exist \n");
        } else {
            User* original = map.at(nameOriginal);
            User* newUser = original->cloneDup(nameNew);
            sess.addToUserMap(nameNew,newUser);
            this->complete();
        }
        sess.addToLog(this);
    }

    std::string DuplicateUser::toString() const {
        return (" Duplicate user "+ this->statusToString(getStatus())+ this->getErrorMsg());}

    BaseAction* DuplicateUser::clone() {
        DuplicateUser *OUTPUT = new DuplicateUser(this->getErrorMsg(),this->getStatus());
        return OUTPUT;
    }

    //PrintContentList
    PrintContentList::PrintContentList() {}
    PrintContentList::PrintContentList(std::string error,ActionStatus status) :BaseAction(error,status) {}
    void PrintContentList::act(Session &sess) {
        vector<Watchable *> content = sess.getContent(sess);

        for (int i=1; i<(int)content.size(); i++){
            int length =(int) content[i]->getLength();
            cout <<(" " + content[i]->toString() + " " + to_string(length) + " minutes " + content[i]->toStringTags(content[i]) )<< endl;
        }
        this->complete();
        sess.addToLog(this);

    }

    std::string PrintContentList::toString() const {
        return (" Print content list "+ this->statusToString(getStatus())+ this->getErrorMsg());}

    BaseAction* PrintContentList::clone() {
        PrintContentList *OUTPUT = new PrintContentList( this->getErrorMsg(),this->getStatus());
        return OUTPUT;
    }

    //PrintWatchHistory
    PrintWatchHistory::PrintWatchHistory() {}
    PrintWatchHistory::PrintWatchHistory(std::string error,ActionStatus status):BaseAction(error,status){}
    void PrintWatchHistory::act(Session &sess) {
        User* user = sess.getActiveUser(sess);
        vector<Watchable *> history = user->get_history();
        cout<<("Watch History for "+user->getName())<<endl;
        for (int i=0; i<(int)history.size(); i++){
            cout <<(" "+ to_string(i+1) +" "+ history[i]->toString().substr(3))<< endl;
        }
        this->complete();
        sess.addToLog(this);
    }
    std::string PrintWatchHistory::toString() const {
        return (" Print watch list "+ this->statusToString(getStatus())+ this->getErrorMsg());}

    BaseAction* PrintWatchHistory::clone() {
        PrintWatchHistory *OUTPUT = new PrintWatchHistory(this->getErrorMsg(),this->getStatus());
        return OUTPUT;
    }

    //Watch
    Watch::Watch() {}
    Watch::Watch(std::string error,ActionStatus status) :BaseAction(error,status) {}
    void Watch::act(Session &sess) {

        vector<string> input = sess.getInput(sess);
        int content_id = stoi(input[1]);
        vector<Watchable *> content = sess.getContent(sess);
        Watchable *currContent = content[content_id];
        User* active = sess.getActiveUser(sess);

        cout<<("Watching " + currContent->toString().substr(3) + "\n");
        active->addToHistory(currContent);
        if (active->whoIam(active)== "gen"){
            vector<string> newTags = currContent->getTags();
            for (int i=0; i<(int)newTags.size(); i++){
                active->setGenre(newTags[i]);
            }
        }

        this->complete();
        sess.addToLog(this);
        Watchable* recommand = currContent->getNextWatchable(sess);
        if (recommand!= nullptr){
            cout<<("We recommend watching " + recommand->toString().substr(3)+ ", continue watching? [y/n] \n");
            string answer;
            cin >> answer;
            if (answer == "y"){
                Watch* watch = new Watch;
                sess.setInput(to_string(recommand->getID()));
                watch-> act(sess);

            }
        }

    }

    std::string Watch::toString() const {
        return (" Watch "+ this->statusToString(getStatus())+ this->getErrorMsg());}

    BaseAction* Watch::clone() {
        Watch *OUTPUT = new Watch(this->getErrorMsg(),this->getStatus());
        return OUTPUT;
    }

    //PrintActionsLog
    PrintActionsLog::PrintActionsLog() {}
    PrintActionsLog::PrintActionsLog(std::string error,ActionStatus status):BaseAction(error,status) {}
    void PrintActionsLog::act(Session &sess) {
        vector<BaseAction*> log = sess.getActionLog(sess);
        for (int i=0; i<(int)log.size(); i++){
            BaseAction* currAction = log[i];
            if (currAction->statusToString(currAction->getStatus())!= "PENDING"){
                cout <<(" "+ currAction->toString()+ "\n");
            }
        }
        this->complete();
        sess.addToLog(this);

    }
    std::string PrintActionsLog::toString() const {
        return (" Print action log "+ this->statusToString(getStatus())+ this->getErrorMsg());}

    BaseAction* PrintActionsLog::clone() {
        PrintActionsLog *OUTPUT = new PrintActionsLog(this->getErrorMsg(),this->getStatus());
        return OUTPUT;
    }

    //Exit
    Exit::Exit() {}
    Exit::Exit(std::string error,ActionStatus status) :BaseAction(error,status) {}
    void Exit::act(Session &sess) {
        this->complete();
        sess.addToLog(this);
    }

    std::string Exit::toString() const {
        return (" Exit "+ this->statusToString(getStatus())+ this->getErrorMsg());}

    BaseAction* Exit::clone() {
        Exit *OUTPUT = new Exit(this->getErrorMsg(),this->getStatus());
        return OUTPUT;
    }