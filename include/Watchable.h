#ifndef WATCHABLE_H_
#define WATCHABLE_H_

#include <string>
#include <vector>


class Session;

class Watchable{
public:
    Watchable(long id, int length, const std::vector<std::string>& tags);
    virtual ~Watchable() = default;
    virtual std::string toString() const = 0;
    virtual Watchable* getNextWatchable(Session&) const = 0;
    int getLength() const;
    int getID() const ;
    std::string toStringTags(Watchable*);
    virtual Watchable* clone() const=0;
    std::vector<std::string> getTags() const;

private:
    const long id;
    int length;
    std::vector<std::string> tags;
};

class Movie : public Watchable{
public:
    Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags);
    virtual std::string toString() const;
    virtual Watchable* getNextWatchable(Session&) const;
    virtual Watchable* clone() const;
private:
    std::string name;
};


class Episode: public Watchable{
public:
    Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags);
    Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags,long);
    virtual std::string toString() const;
    virtual Watchable* getNextWatchable(Session&) const;
    virtual Watchable* clone() const;

private:
    std::string seriesName;
    int season;
    int episode;
    long nextEpisodeId;
};

#endif
