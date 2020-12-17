
    #include "../include/User.h"
    #include "../include/Watchable.h"
    #include "../include/Session.h"

    //User constructor
    User ::User(const std::string &name): history{},name(name) {}
    User::User(std::vector<Watchable *> &history, const std::string &name):history(history),name(name) {}

    User::~User() {}

    //getName
    std::string User::getName() const {
        return name;
    }

    //get history
    std::vector<Watchable*> User::get_history() const {
        return history;
    }

    void User::addToHistory(Watchable* watch) {
        history.push_back(watch);
    }

    void User::setGenre(std::string) {}

    //LengthRecommenderUser
    LengthRecommenderUser::LengthRecommenderUser(const std::string &name) : User(name) {}
    LengthRecommenderUser::LengthRecommenderUser(std::vector<Watchable *> &history, const std::string &name):User(history,name) {}

    User* LengthRecommenderUser::cloneDup(std::string s) {
        std::vector<Watchable*> coptHistory = this->get_history();
        User* output= new LengthRecommenderUser(coptHistory,s);
        return output;
    }
    User* LengthRecommenderUser::clone(Session &s) {
        std::vector<Watchable*> coptHistory;
        for(int  i=0; i<(int)this->get_history().size();i++){
            int idHistory = this->get_history()[i]->getID();
            coptHistory.push_back(s.getContent(s)[idHistory]);
        }
        User* output= new LengthRecommenderUser(coptHistory,this->getName());
        return output;
}
    std::string LengthRecommenderUser::whoIam(User *) { return "len";}

     Watchable* LengthRecommenderUser::getRecommendation(Session &s) {

         std::vector<Watchable*> opt = s.getContent(s);

         //average
         int counter =0;
         int sum = 0;
         for(int i=0;i<(int)history.size();i++){
             sum=sum+history[i]->getLength();
             counter++;
         }
         int average = sum/counter;

         //map- distance,id
         std:: map<int,int> dis;

         for(int i=0; i<(int)opt.size() ;i++){
             int d = abs(opt[i]->getLength()-average);
             int id = opt[i]->getID();
             dis.insert(std::pair<int,int>(d,id));
         }

         //search in history
         int check;
         for(std::map<int,int>::iterator it = dis.begin() ;it!=dis.end() ;++it){
             check=it->second;
             int runHistory = 0;

             while(runHistory<(int)history.size() && history[runHistory]->getID()!=check ){
                 runHistory++;
             }
             if((runHistory==(int)history.size())& (check!=-100)){
                 return opt[check];
             }
         }
             return nullptr ;

       }



    //RerunRecommenderUser
    RerunRecommenderUser::RerunRecommenderUser(const std::string &name) : User(name) ,lastRecommand(){}
    RerunRecommenderUser::RerunRecommenderUser(std::vector<Watchable *> &history, std::string name, int lastRecommand): User(history, name), lastRecommand(lastRecommand) {}

    User* RerunRecommenderUser::cloneDup(std::string s) {
        std::vector<Watchable*> coptHistory = this->get_history();
        User* output= new RerunRecommenderUser(coptHistory,s,lastRecommand);
        return output;
}
    User* RerunRecommenderUser::clone(Session & s) {
        std::vector<Watchable *> coptHistory;
        for (int i = 0; i < (int) this->get_history().size(); i++) {
            int idHistory = this->get_history()[i]->getID();
            coptHistory.push_back(s.getContent(s)[idHistory]);
        }
        User* output= new RerunRecommenderUser(coptHistory,this->getName(),lastRecommand);
        return output;
    }

    std::string RerunRecommenderUser::whoIam(User *) {return "rer";}

    Watchable* RerunRecommenderUser::getRecommendation(Session &s) {
        size_t n = history.size();
        lastRecommand=  (lastRecommand+1)%n;
        std::vector<Watchable*> content = s.getContent(s);
        return history[lastRecommand];

}

    //GenreRecommenderUser
    GenreRecommenderUser::GenreRecommenderUser(const std::string &name) : User(name), genre{} {}
    GenreRecommenderUser::GenreRecommenderUser(std::vector<Watchable *> &history, std::string name, std::map<std::string, int> genre): User(history, name), genre(genre) {}

    std::map<std::string, int> GenreRecommenderUser::getGenre() { return genre; }
    std::string GenreRecommenderUser::whoIam(User *) {return "gen";}
    void GenreRecommenderUser::setGenre(std::string name) {
        bool done = false;
        for (std::map<std::string,int>::iterator it=genre.begin(); (it!=genre.end()) & (!done); ++it){
            if (it->first == (name)){
                it->second++;
                done = true;
            }
        }
        if(!done){
            genre.insert(std::pair<std::string, int> (name,1));
        }
    }

    Watchable* GenreRecommenderUser::getRecommendation(Session &s) {
        std::map <std::string, int> genre = getGenre();

        while (genre.size()>0){

            int max = 0;
            for (std::map<std::string,int>::iterator it=genre.begin(); it!=genre.end(); ++it){
                if(it->second > max){
                    max = it->second;
                }
            }
            std::string name;
            bool stop = false;
            for (std::map<std::string,int>::iterator it=genre.begin(); it!=genre.end(); ++it){
                if((it->second == max) & (!stop)) {
                    name = it->first;
                    stop=true;
                }
            }

            std::vector<Watchable*> content = s.getContent(s);
            for (size_t i=1; i<content.size(); i++){
                std::vector<std::string> tags = content[i]->getTags();
                    for(size_t j=0; j<tags.size(); j++){
                        if (tags[j]== name) {
                            Watchable* check = content[i];
                            int counter =0;
                                Watchable* curr = history[counter];
                                while ((counter<(int)history.size())& (check != curr)){
                                    counter++;
                                }

                                if (counter == (int)history.size()&& ((counter) !=(-100))){
                                    return check;
                            }
                        }
                    }
            }
            genre.erase(name);
        }
        return nullptr;
    }


//dup
    User* GenreRecommenderUser::cloneDup(std::string s) {
      std::vector<Watchable*> coptHistory = this->get_history();
        std::map<std::string, int>genre=this->getGenre();
        User* output= new GenreRecommenderUser(coptHistory,s,genre);
        return output;
    }

    User* GenreRecommenderUser::clone(Session &s) {
        std::vector<Watchable*> coptHistory;
        for(int  i=0; i<(int)this->get_history().size();i++){
            int idHistory = this->get_history()[i]->getID();
            coptHistory.push_back(s.getContent(s)[idHistory]);
        }
        std::map<std::string, int>genre=this->getGenre();
        User* output= new GenreRecommenderUser(coptHistory,this->getName(),genre);
        return output;
    }

