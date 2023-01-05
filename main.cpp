#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <string.h>

#include "shared.h"

#include "PolygonGenerator.h"
#include "PolygonOptimizer.h"

#include "ConvexHullAlgo.h"
#include "onion.h"
#include "incr.h"

#include "local.h"
#include "SimulatedAnnealing.h"
#include "ant.h"
  
using std::cout;
using std::endl;
using std::string;
using std::ifstream;

void getPointsFromFile(string filepath, int& size, PointList& points, long& convexHullArea);
void handleArgs(ArgFlags& argFlags, int& argc, char**& argv);
void writePolygonToFile(string filepath, Polygon_2 polygon, ArgFlags argFlags, double convexHullArea, double initialArea, std::chrono::milliseconds duration);
string getAlgorithmString(ArgFlags argFlags);
void printArguments(ArgFlags& argFlags);

//Dummy class just pushes all point in order to the polygon
class DummyOptimizer: public PolygonOptimizer{
    
public:
    DummyOptimizer(Polygon_2& polygon): PolygonOptimizer(polygon){};

    virtual Polygon_2 optimalPolygon(){
        return this->poly;
    };
};

int main(int argc, char **argv)
{
    ArgFlags argFlags;
    handleArgs(argFlags, argc, argv);
    
    //check for input error
    if(argFlags.error)
    {
        cout << argFlags.errorMessage << endl;
        cout << "Usage: /optimal_polygon -i <point set input file> -o <output file> -algorithm <local_search or simulated_annealing or ant_colony> -L [L parameter according to algorithm] -max [maximal area polygonization] -min [minimal area polygonization] -threshold <double> [in local search -annealing <'local' or 'global' or 'subdivision' in simulated annealing>" << endl;
        return -1;
    }
    

    //get input
    long convexHullArea; // long because in some files an int is not enoug e.g uniform-0000800-1
    int size;
    PointList list, result;
    getPointsFromFile(argFlags.inputFile, size, list, convexHullArea);

    //calculate convex hull
    Polygon_2 p, chp;

    CGAL::convex_hull_2(list.begin(), list.end(), std::back_inserter(result));
    for(auto it = result.begin(); it != result.end(); ++it) chp.push_back(*it);

    PolygonGenerator* generator;   //generation
    
    switch(argFlags.genAlg){
        case incremental:
            generator= new IncAlgo(list,argFlags.initialization,argFlags.edgeSelection);
            break;
        case convex_hull:
            generator= new ConvexHullAlgo(list,argFlags.edgeSelection);
            break;
        case onion:
            generator= new OnionAlgo(list,3);
            break;
        default:
            break;
    }

    auto start = std::chrono::high_resolution_clock::now();

    p = generator->generatePolygon();

    PolygonOptimizer* optimizer;    //optimization
    Polygon_2 optimalPolygon;
    

    switch(argFlags.algorithm){
        case local_search:
            optimizer= new LocalAlgo(p,convexHullArea,argFlags.threshold,argFlags.optimizationType,argFlags.L);
            break;
        case simulated_annealing:
            optimizer= new SimulatedAnnealing(p,(double) convexHullArea, argFlags);
            break;
        case ant_colony:
            optimizer= new Ant(argFlags,list,p);
            break;
        default:
            break;
    }
    //calculate optimal polygon
    std::ofstream initialDump("initial.wkt"), pointDump("points.wkt"), optimalDump("optimal.wkt");
    CGAL::IO::write_polygon_WKT(
        initialDump,
        p
    );
    


    optimalPolygon = optimizer->optimalPolygon();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    
    
    CGAL::IO::write_multi_point_WKT(
        pointDump,
        list
    );
    CGAL::IO::write_polygon_WKT(
        optimalDump,
        optimalPolygon
    );

    // printArguments(argFlags);

    //write output
    writePolygonToFile(argFlags.outputFile, optimalPolygon, argFlags, convexHullArea, abs(p.area()),duration);

    delete generator;
    delete optimizer;

    return 0;
}

void handleArgs(ArgFlags& argFlags, int& argc, char**& argv)
{
    int waitingForArg = 0;
    bool waitingInput = true;
    bool waitingOutput = true;
    bool waitingAlgorithm = true;
    bool waitingGenAlgorithm=true;
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
                else if (!strcmp(arg, "-algorithm"))
                    waitingForArg = 3;
                else if (!strcmp(arg, "-L"))
                    waitingForArg = 4;
                else if (!strcmp(arg, "-threshold"))
                    waitingForArg = 5;
                else if (!strcmp(arg, "-annealing"))
                    waitingForArg = 6;
                else if (!strcmp(arg, "-alpha"))
                    waitingForArg = 7;
                else if (!strcmp(arg, "-beta"))
                    waitingForArg = 8;
                else if (!strcmp(arg, "-ro"))
                    waitingForArg = 9;
                else if (!strcmp(arg, "-elitism"))
                    waitingForArg = 10;
                else if (!strcmp(arg,"-gen"))
                    waitingForArg=11;
                else if (!strcmp(arg, "-min"))
                    argFlags.optimizationType = minimization;
                else if (!strcmp(arg, "-max"))
                    argFlags.optimizationType = maximization;
                break;
            case 1:
                argFlags.inputFile = string(arg);
                waitingInput = false;
                waitingForArg = 0;
                break;
            case 2:
                argFlags.outputFile = string(arg);
                waitingOutput = false;
                waitingForArg = 0;
                break;
            case 3:
                if(!strcmp(arg, "local_search"))
                    argFlags.algorithm = local_search;
                else if(!strcmp(arg, "simulated_annealing"))
                    argFlags.algorithm = simulated_annealing;
                else if(!strcmp(arg, "ant_colony"))
                    argFlags.algorithm = ant_colony;
                waitingAlgorithm = false;
                waitingForArg = 0;
                break;
            case 4:
                argFlags.L = atoi(arg);
                waitingForArg = 0;
                break;
            case 5:
                argFlags.threshold = atof(arg);
                waitingForArg = 0;
                break;
            case 6:
                if(!strcmp(arg, "local"))
                    argFlags.annealingType = local;
                else if(!strcmp(arg, "global"))
                    argFlags.annealingType = global;
                else if(!strcmp(arg, "subdivision"))
                    argFlags.annealingType = subdivision;
                waitingForArg = 0;
                break;
            case 7:
                argFlags.alpha = atof(arg);
                waitingForArg = 0;
                break;
            case 8:
                argFlags.beta = atof(arg);
                waitingForArg = 0;
                break;
            case 9:
                argFlags.ro = atof(arg);
                waitingForArg = 0;
                break;
            case 10:
                argFlags.elitism = atoi(arg);
                waitingForArg = 0;
                break;
            case 11:
                if(!strcmp(arg, "incremental")){
                    argFlags.genAlg = incremental;
                    if(argFlags.optimizationType==maximization){
                        argFlags.edgeSelection=max;
                    }else{
                        argFlags.edgeSelection=min;
                    }
                }else if(!strcmp(arg, "convex_hull")){
                    argFlags.genAlg = convex_hull;
                    if(argFlags.optimizationType==maximization){
                        argFlags.edgeSelection=min;
                    }else{
                        argFlags.edgeSelection=max;
                     }
                }else if(!strcmp(arg, "onion"))
                    argFlags.genAlg = onion;                

                argFlags.initialization=a1;
                waitingGenAlgorithm=false;
                waitingForArg=0;
                break;
        }
    }

    if(waitingAlgorithm){
        argFlags.error = true;
        argFlags.errorMessage = string("Must select optimization algorithm");
        return;
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

    if(waitingGenAlgorithm){
        argFlags.error = true;
        argFlags.errorMessage = string("Must select generation algorithm");
        return;
    }

    argFlags.error = false;
    return;
}

void getPointsFromFile(string filepath, int& size, PointList& points, long& convexHullArea)
{
    //get number of lines
    ifstream infile(filepath);
    int linesCount = 0;
    string line;

    while(getline(infile, line))linesCount++;
    int pointsCount = linesCount - 2;   //files always have 2 comments and then just points
    size = pointsCount;

    
    //reset reading cursor
    infile.clear();
    infile.seekg(0);

    //ignore first 2 lines
    getline(infile, line);
    getline(infile,line);

    //get convex hull area from second line
    convexHullArea = stol(   // we use stol instead of stoi because stoi return an integer and stol return a long
        line.substr(
            line.find("{\"area\": \"") + 10, 
            line.find("\"}"))
    );

    //read points
    int x,y;
    for(int i = 0; getline(infile, line); i++)
    {
        int ignore;
        std::istringstream iss(line);
        iss >> ignore >> x >> y;
        points.push_back(Point_2(x,y));
    }

    return;
}

string getAlgorithmString(ArgFlags argFlags)
{
    string algorithm;
    if(argFlags.algorithm == local_search)
        algorithm = "local_search";
    else if(argFlags.algorithm == simulated_annealing)
        algorithm = "simulated_annealing";
    else
        algorithm = "ant_colony";

    string optimizationType;
    if(argFlags.optimizationType == maximization)
        optimizationType = "max";
    else
        optimizationType = "min";

    return algorithm + "_" + optimizationType;
}

void writePolygonToFile(string filepath, Polygon_2 polygon, ArgFlags argFlags, double convexHullArea, double initialArea, std::chrono::milliseconds duration){
    std::ofstream outfile(filepath);

    outfile << "Optimal Area Polygonization" << endl;

    for(auto iter = polygon.vertices_begin(); iter != polygon.vertices_end(); ++iter)
        outfile << *iter << endl;
    for(auto iter = polygon.edges_begin(); iter != polygon.edges_end(); ++iter)
        outfile << *iter << endl;
    outfile << "Algorithm: " << getAlgorithmString(argFlags) << endl;

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

void printArguments(ArgFlags& argFlags)
{
    string algorithm;
    if(argFlags.algorithm == local_search)
        algorithm = "local search";
    else if(argFlags.algorithm == simulated_annealing)
        algorithm = "simulated annealing";
    else
        algorithm = "ant colony";

    string optimizationType;
    if(argFlags.optimizationType == maximization)
        optimizationType = "maximization";
    else
        optimizationType = "minimization";

    string annealingType;
    if(argFlags.annealingType == local)
        annealingType = "local";
    else if(argFlags.annealingType == global)
        annealingType = "global";
    else
        annealingType = "subdivision";

    cout << "Input file: " << argFlags.inputFile << endl;
    cout << "Output file: " << argFlags.outputFile << endl;
    cout << "Algorithm: " << algorithm << endl;
    cout << "L parameter: " << argFlags.L << endl;
    cout << "Optimization type: " << optimizationType << endl;

    if(argFlags.algorithm == local_search)
        cout << "Threshold: " << argFlags.threshold << endl;
    if(argFlags.algorithm == simulated_annealing)
        cout << "Annealing type: " << annealingType << endl;

    if(argFlags.algorithm == ant_colony)
    {
        cout << "alpha: " << argFlags.alpha << endl;
        cout << "beta: " << argFlags.beta << endl;
        cout << "ro: " << argFlags.ro << endl;
        cout << "elitism: " << argFlags.elitism << endl;
    }
}