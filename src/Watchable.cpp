

    #include "../include/Watchable.h"
    #include <iostream>
    #include "../include/User.h"
    #include "../include/Session.h"
    #include "../include/Session.h"
    using namespace std;


    //watchable
    Watchable::Watchable(long id, int length, const std::vector<std::string> &tags): id(id),length(length),tags(tags){}

    //Get length
    int Watchable::getLength() const  { return length; }

    //Get ID
    int Watchable::getID() const {
        return id;
    }

    std::string Watchable::toStringTags(Watchable *) {
        string tagsOutput = "[";
        for(int i=0;i<(int)tags.size();i++){
            if (i<(int)tags.size()-1) {
                tagsOutput = tagsOutput + tags[i] + ",";
            }else{
                tagsOutput = tagsOutput + tags[i];
            }
        }
        tagsOutput = tagsOutput +"]";
        return tagsOutput;
    }

    std::vector<std::string> Watchable::getTags() const{ return tags;}

    //Watchable::~Watchable() {} //todo check

    //movie
    Movie::Movie(long id, const std::string &name, int length, const std::vector<std::string> &tags) :Watchable(id,length,tags), name(name) {}

    Watchable* Movie::clone() const {
       Watchable* OUTPUT = new Movie(*this);
        return  OUTPUT;
    }

    std::string Movie::toString() const {
        return (to_string(this->getID()) + ". " +name );

    }

    Watchable* Movie::getNextWatchable(Session &s) const {
        User* active = s.getActiveUser(s);
        Watchable* recommand = active->getRecommendation(s);
        return recommand;
    }




    //Episode defaule
    Episode::Episode(long id, const std::string &seriesName, int length, int season, int episode,
                     const std::vector<std::string> &tags): Watchable(id,length,tags), seriesName(seriesName),season(season),episode(episode), nextEpisodeId(-1){}

    Episode::Episode(long id, const std::string &seriesName, int length, int season, int episode,
                     const std::vector<std::string> &tags, long nextEpisode): Watchable(id,length,tags), seriesName(seriesName),season(season),episode(episode), nextEpisodeId(nextEpisode){}

    Watchable* Episode::clone() const {
        Watchable* OUTPUT = new Episode(*this);
        return  OUTPUT;
    }//todo ceek

    std::string Episode::toString() const {

        return (to_string(this->getID()) + ". "+ seriesName + " " + "S" + std::to_string(season) + "E" +std::to_string(episode));
    }


    Watchable* Episode::getNextWatchable(Session &s) const {

        std::vector<Watchable*> content = s.getContent(s);
        Watchable* recommand;
        if (nextEpisodeId == -1){
            User* active = s.getActiveUser(s);
            recommand = active->getRecommendation(s);
       } else {
            recommand = content[nextEpisodeId];
        }
        return recommand;

    }










