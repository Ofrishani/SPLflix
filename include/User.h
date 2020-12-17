#ifndef USER_H_
#define USER_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <map>

class Watchable;
class Session;

class User{
public:
    User(const std::string& name);
    User(std::vector<Watchable*>& history,const std::string& name);
    virtual Watchable* getRecommendation(Session& s) = 0;
    std::string getName() const;
    std::vector<Watchable*> get_history() const;
    void addToHistory(Watchable*);
    virtual User* cloneDup(std::string)=0;
    virtual User* clone(Session&)=0;
    virtual std::string whoIam(User*) =0;
    virtual void setGenre(std::string);
    virtual ~User();

protected:
    std::vector<Watchable*> history;
private:
    std::string name;
};


class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser(const std::string& name);
    LengthRecommenderUser(std::vector<Watchable*>& history,const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual User* cloneDup(std::string);
    virtual User* clone(Session&);
    virtual std::string whoIam(User*);


private:
};

class RerunRecommenderUser : public User {
public:
    RerunRecommenderUser(const std::string& name);
    RerunRecommenderUser(std::vector<Watchable*>& history, std::string name, int lastRecommand);
    virtual Watchable* getRecommendation(Session& s);
    virtual User* cloneDup(std::string);
    virtual User* clone(Session&);
    virtual std::string whoIam(User*);

private:
    int lastRecommand;
};

class GenreRecommenderUser : public User {
public:
    GenreRecommenderUser(const std::string& name);
    GenreRecommenderUser(std::vector<Watchable*>& history, std::string name, std::map<std::string, int> genre);
    virtual Watchable* getRecommendation(Session& s);
    virtual User* cloneDup(std::string);
    virtual User* clone(Session&);
    std::map<std::string, int> getGenre();
    virtual std::string whoIam(User*);
    virtual void setGenre(std::string);


private:
    std::map <std::string, int> genre;
};

#endif
