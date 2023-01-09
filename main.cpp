#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <string.h>
#include <filesystem>

#include "shared.h"

#include "PolygonGenerator.h"
#include "PolygonOptimizer.h"

#include "ConvexHullAlgo.h"
#include "onion.h"
#include "incr.h"

#include "local.h"
#include "SimulatedAnnealing.h"
#include "ant.h"

#include "DefaultHandler.h"
#include "ResultLogger.h"
  
using std::cout;
using std::endl;
using std::string;
using std::ifstream;

void handleArgs(ArgumentFlags& argFlags, int& argc, char**& argv);
void writePolygonToFile(string filepath, Polygon_2 polygon, ArgFlags argFlags, double convexHullArea, double initialArea, std::chrono::milliseconds duration);
void printArguments(ArgumentFlags& argFlags);

int main(int argc, char **argv)
{
    ArgumentFlags argFlags;
    handleArgs(argFlags, argc, argv);
    
    //check for input error
    if(argFlags.error)
    {
        cout << argFlags.errorMessage << endl;
        cout << "./evaluate -i <point set path> -o <output file> -preprocess <optional>" << endl;
        return -1;
    }

    ResultLogger logger;
    AlgorithmHandler *handler;
    bool starting = true;
    for (const auto & entry : std::filesystem::directory_iterator(argFlags.inputDirectory))
    {
        if(starting)
        {   
            handler = new DefaultHandler(entry.path());
            starting = false;
        } 
        else
            handler->resetFile(entry.path());

        int size = handler->getSize();
        double minScore =  handler->onionAnnealing(minimization);
        double maxScore =  handler->onionAnnealing(maximization);

        logger.updateEntry(size, Combination::onionAnn, minScore, maxScore);
    }

    delete handler;

    logger.printLogger(argFlags.outputFile);

    // AlgorithmHandler *handler = new DefaultHandler(argFlags.inputDirectory);
    
    // printArguments(argFlags);
    // cout << "Score: " << handler->onionAnnealing(minimization) << endl;

    auto start = std::chrono::high_resolution_clock::now();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    
    return 0;
}

void handleArgs(ArgumentFlags& argFlags, int& argc, char**& argv)
{
    int waitingForArg = 0;
    bool waitingInput = true;
    bool waitingOutput = true;

    argFlags.preprocess = "default";

    for (int i = 1; i < argc; i++)
    {
        char* arg = argv[i];
        switch (waitingForArg)
        {
            case 0:
                if (!strcmp(arg, "-i"))
                    waitingForArg = 1;
                else if (!strcmp(arg, "-o"))
                    waitingForArg = 2;
                else if (!strcmp(arg, "-preprocess"))
                    waitingForArg = 3;
                break;
            case 1:
                argFlags.inputDirectory = string(arg);
                waitingInput = false;
                waitingForArg = 0;
                break;
            case 2:
                argFlags.outputFile = string(arg);
                waitingOutput = false;
                waitingForArg = 0;
                break;
            case 3:
                argFlags.outputFile = string(arg);
                waitingForArg = 0;
                break;
        }
    }

    if(waitingInput){
        argFlags.error = true;
        argFlags.errorMessage = string("Must select input file");
        return;
    }
    if(waitingOutput){
        argFlags.error = true;
        argFlags.errorMessage = string("Must select output file");
        return;
    }

    argFlags.error = false;
    return;
}

void writePolygonToFile(string filepath, Polygon_2 polygon, ArgFlags argFlags, double convexHullArea, double initialArea, std::chrono::milliseconds duration){
    std::ofstream outfile(filepath);

    outfile << "Optimal Area Polygonization" << endl;

    for(auto iter = polygon.vertices_begin(); iter != polygon.vertices_end(); ++iter)
        outfile << *iter << endl;
    for(auto iter = polygon.edges_begin(); iter != polygon.edges_end(); ++iter)
        outfile << *iter << endl;
    // outfile << "Algorithm: " << getAlgorithmString(argFlags) << endl;

    double polygonArea =  abs(polygon.area());

    if(argFlags.algorithm != ant_colony)
        outfile << "area_initial: " << initialArea << endl;
    outfile << "area: " << polygonArea << endl;

    if(argFlags.algorithm != ant_colony)
        outfile << "ratio_initial: " << (double) convexHullArea / initialArea << endl;
    outfile << "ratio: " << (double) convexHullArea / polygonArea << endl;

    outfile << "construction time: " << duration.count() << endl;
    return;
}

void printArguments(ArgumentFlags& argFlags)
{
    cout << "Input file: " << argFlags.inputDirectory << endl;
    cout << "Output file: " << argFlags.outputFile << endl;
    cout << "preprocess: " << argFlags.preprocess << endl;
}