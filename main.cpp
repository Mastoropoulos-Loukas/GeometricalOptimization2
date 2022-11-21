#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <string.h>
#include "PolygonGenerator.h"
#include "ConvexHullAlgo.h"
#include "shared.h"
#include "onion.h"
#include "incr.h"
  
using std::cout;
using std::endl;
using std::string;
using std::ifstream;

void getPointsFromFile(string filepath, int& size, PointList& points, long& convexHullArea);
void handleArgs(ArgFlags& argFlags, int& argc, char**& argv);
void writePolygonToFile(string filepath, Polygon_2 polygon, ArgFlags argFlags, int convexHullArea, std::chrono::milliseconds duration);
string getAlgorithmString(ArgFlags argFlags);

//Dummy class just pushes all point in order to the polygon
class DummyGenerator: public PolygonGenerator{
    
public:
    DummyGenerator(PointList& list): PolygonGenerator(list){};

    virtual Polygon_2 generatePolygon(){
        Polygon_2 p;
        for(PointListIterator iter = list.begin(); iter != list.end(); ++iter)
            p.push_back(*iter);

        return p;
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
        cout << "Usage: /to_polygon -i <point set input file> -o <output file> -algorithm <incremental or convex_hull or onion> -edge_selection <1 or 2 or 3 | not for onion> -initialization <1a or 1b or 2a or 2b | only for incremental> -onion_initialization <1 to 5>" << endl;
        return -1;
    }

    //get input
    long convexHullArea; // long because in some files an int is not enoug e.g uniform-0000800-1
    int size;
    PointList list, result;
    getPointsFromFile(argFlags.inputFile, size, list, convexHullArea);

    //calculate convex hull
    Polygon_2 p, chp;
    PolygonGenerator *generator;
    switch (argFlags.algorithm)
    {
    case incremental:
        generator = new IncAlgo(list, argFlags);  
        break;
    case convex_hull:
        generator = new ConvexHullAlgo(list, argFlags.edgeSelection);
        break;
    case onion:
        generator = new OnionAlgo(list, argFlags.onionInitialization); 
        break;
    default:
        break;
    }

    CGAL::convex_hull_2(list.begin(), list.end(), std::back_inserter(result));
    for(auto it = result.begin(); it != result.end(); ++it) chp.push_back(*it);

    //calculate polygon
    auto start = std::chrono::high_resolution_clock::now();

    p = (*generator).generatePolygon();
    
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    //write shapes
    if(argFlags.showShapes)
    {
        std::ofstream dump("polygon.wkt"), pointDump("points.wkt"), hullDump("hull.wkt");
        CGAL::IO::write_polygon_WKT(
            dump,
            p
        );
        CGAL::IO::write_multi_point_WKT(
            pointDump,
            list
        );
        CGAL::IO::write_polygon_WKT(
            hullDump,
            chp
        );
    }
    

    //write output
    writePolygonToFile(argFlags.outputFile, p, argFlags, convexHullArea, duration);

    return 0;
}

void handleArgs(ArgFlags& argFlags, int& argc, char**& argv)
{
    int waitingForArg = 0;
    argFlags.showPick = true;
    argFlags.showShapes = false;
    bool waitingInput = true;
    bool waitingOutput = true;
    bool waitingAlgorithm = true;
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
                else if (!strcmp(arg, "-edge_selection"))
                    waitingForArg = 4;
                else if (!strcmp(arg, "-initialization"))
                    waitingForArg = 5;
                else if (!strcmp(arg, "-onion_initialization"))
                    waitingForArg = 6;
                else if(!strcmp(arg, "-hide_pick"))
                    argFlags.showPick = false;
                else if(!strcmp(arg, "-show_shapes"))
                    argFlags.showShapes = true;
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
                if(!strcmp(arg, "incremental"))
                    argFlags.algorithm = incremental;
                else if(!strcmp(arg, "convex_hull"))
                    argFlags.algorithm = convex_hull;
                else if(!strcmp(arg, "onion"))
                    argFlags.algorithm = onion;
                waitingAlgorithm = false;
                waitingForArg = 0;
                break;
            case 4:
                if(atoi(arg) == 1)
                    argFlags.edgeSelection = randomSelection;
                else if(atoi(arg) == 2)
                    argFlags.edgeSelection = min;
                else if(atoi(arg) == 3)
                    argFlags.edgeSelection = max;
                waitingForArg = 0;
                break;
            case 5:
                if(!strcmp(arg, "1a"))
                    argFlags.initialization = a1;
                else if(!strcmp(arg, "1b"))
                    argFlags.initialization = b1;
                else if(!strcmp(arg, "2a"))
                    argFlags.initialization = a2;
                else if(!strcmp(arg, "2b"))
                    argFlags.initialization = b2;
                waitingForArg = 0;
                break;
            case 6:
                argFlags.onionInitialization = atoi(arg);
                waitingForArg = 0;
                break;
        }
    }

    if(waitingAlgorithm){
        argFlags.error = true;
        argFlags.errorMessage = string("Must select algorithm");
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

    argFlags.error = false;
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

void writePolygonToFile(string filepath, Polygon_2 polygon, ArgFlags argFlags, int convexHullArea, std::chrono::milliseconds duration){
    std::ofstream outfile(filepath);

    outfile << "Polygonization" << endl;
    for(auto iter = polygon.vertices_begin(); iter != polygon.vertices_end(); ++iter)
        outfile << *iter << endl;
    for(auto iter = polygon.edges_begin(); iter != polygon.edges_end(); ++iter)
        outfile << *iter << endl;
    outfile << getAlgorithmString(argFlags) << endl;

    double polygonArea =  abs(polygon.area());
    outfile << "area: " << polygonArea << endl;
    if(argFlags.showPick) outfile << "pick_calculated_area: " << Pick(polygon) << endl;
    outfile << "ratio: " << convexHullArea / polygonArea << endl;
    outfile << "construction time: " << duration.count() << endl;
    return;
}

string getAlgorithmString(ArgFlags argFlags){

    string res;

    res += string("Algorithm: ");
    if(argFlags.algorithm == incremental)
    {
        res += string("incremental");
        res += string("_edge_selection");
        
        if(argFlags.edgeSelection == randomSelection) res += string("1");
        else if(argFlags.edgeSelection == min) res += string("2");    
        else res += string("3");

        res += string("_initialization");
        if(argFlags.initialization == a1) res += string("1a");
        else if(argFlags.initialization == b1) res += string("1b");
        else if(argFlags.initialization == a2) res += string("2a");
        else res += string("2b");
    }
    else if(argFlags.algorithm == convex_hull) 
    {
        res += string("convex_hull");
        res += string("_edge_selection");
        if(argFlags.edgeSelection == randomSelection) res += string("1");
        else if(argFlags.edgeSelection == min) res += string("2");    
        else res += string("3");
    
    }else 
    {
        res += string("onion");
        res += string("_initialization");
        res += std::to_string(argFlags.onionInitialization);
    }

    return res;
}

void printArguments(ArgFlags& argFlags){
    cout << "Input file: " << argFlags.inputFile << endl;
    cout << "Output file: " << argFlags.outputFile << endl;
    cout << "Algorithm: " << argFlags.algorithm << endl;
    cout << "Edge selection: " << argFlags.edgeSelection << endl;
    cout << "Initialization: " << argFlags.initialization << endl;
    cout << "Onion initialization: " << argFlags.onionInitialization << endl;
}