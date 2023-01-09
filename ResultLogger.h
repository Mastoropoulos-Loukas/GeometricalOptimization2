#ifndef RESULT_LOGGER_H
#define RESULT_LOGGER_H

#include <string>
#include <map>
#include <iostream>
#include <fstream>

typedef std::map<int, ResultEntry*> Dictionary;

struct ResultEntry
{
    double min_score;
    double max_score;
    double min_bound;
    double max_bound;
};

enum Combination
{
    incrLocal,
    incrAnn,
    chullLocal,
    chullAnn,
    onionLocal,
    onionAnn,
    antColony
};

std::string combinationName(Combination combo)
{
    switch (combo)
    {
    case incrLocal:
        return "Incremental & Local Search";
    case incrAnn:
        return "Incremental & Simulated Annealing";
    case chullLocal:
        return "Convex Hull & Local Search";
    case chullAnn:
        return "Convex Hull & Simulated Annealing";
    case onionLocal:
        return "Onion & Local Search";
    case onionAnn:
        return "Onion & Simulated Annealing";
    case antColony:
        return "Ant Colony";
    default:
        return "Unknown combination";
    }
}


class ResultLogger
{
private:
    Dictionary log; 
public:
    ResultLogger();
    ~ResultLogger();

    void updateEntry(int, Combination, double, double);
    void updateMinEntry(int, Combination, double);
    void updateMaxEntry(int, Combination, double);
    void printLogger(std::string);
};

ResultLogger::ResultLogger(){}
ResultLogger::~ResultLogger()
{
    for(auto it = log.begin(); it != log.end(); it++)
        delete [] log[it->first];
}

void ResultLogger::updateEntry(int key, Combination combination, double minScore, double maxScore)
{
    if(log.find(key) == log.end())
    {
        //key does not exist
        log[key] = new ResultEntry[7];
        
        for(int i = 0; i < 7; i++)
        {
            log[key][i].min_score = 0;
            log[key][i].max_score = 0;
            log[key][i].min_bound = 1;
            log[key][i].max_bound = 0;
        }
    }

    updateMinEntry(key, combination, minScore);
    updateMaxEntry(key, combination, maxScore);
}

void ResultLogger::updateMinEntry(int key, Combination combination, double minScore)
{
    log[key][combination].min_score += minScore;

    double prevMinBound = log[key][combination].min_bound;
    log[key][combination].min_bound = std::max(prevMinBound, minScore);
}

void ResultLogger::updateMaxEntry(int key, Combination combination, double maxScore)
{
    log[key][combination].max_score += maxScore;

    double prevMaxBound = log[key][combination].max_bound;
    log[key][combination].max_bound = std::min(prevMaxBound, maxScore);
}

void ResultLogger::printLogger(std::string streamName)
{
    
}

#endif