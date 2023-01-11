#ifndef SMART_HANDLER
#define SMART_HANDLER

#include "shared.h"
#include "incr.h"
#include "ConvexHullAlgo.h"
#include "onion.h"
#include "ant.h"
#include "local.h"
#include "SimulatedAnnealing.h"
#include "AlgorithmHandler.h"


class SmartHandler : public AlgorithmHandler{
private:
    
public:
    SmartHandler(std::string filename): AlgorithmHandler(filename){};

    virtual double incrementalLocalSearch(OptimizationType type)
    {

        EdgeSelection selection;
        
        if(type==maximization){
            selection=max;
        }else{
            selection=min;

            std::string str = "stars";
            size_t found = filename.find(str);
            if(found!= std::string::npos){
                selection=randomSelection;
            }
        
        }

        int L=1;

        int setSize=points.size();

        if(setSize<50){
            L=3;
        }

        if(setSize<100){
            L=2;
        }

        double threshold=0.10;

        if(setSize>200 && setSize<=500){
            threshold = 0.08;
        }

        if (setSize>500){
            threshold = 0.05;
        }

        IncAlgo *generator = new IncAlgo(points, Initialization::a1, selection);
        Polygon_2 initial = generator->generatePolygon();

        LocalAlgo *optimizer = new LocalAlgo(initial, convexHullArea, threshold, type, L);
        Polygon_2 optimal = (*optimizer).optimalPolygon();

        return abs(optimal.area()) / convexHullArea;
    }

    virtual double incrementalAnnealing(OptimizationType type)
    {
        IncAlgo *generator = new IncAlgo(points, Initialization::a1, EdgeSelection::randomSelection);
        Polygon_2 initial = generator->generatePolygon();

        SimulatedAnnealing *optimizer = new SimulatedAnnealing(initial, convexHullArea, 500, type, AnnealingType::local);
        Polygon_2 optimal = (*optimizer).optimalPolygon();

        return abs(optimal.area()) / convexHullArea;
    }

    virtual double convexHullLocalSearch(OptimizationType type)
    {
        EdgeSelection selection;
        
        if(type==maximization){
            selection=max;
        }else{
            selection=min;
        
        }

        int L=1;
        int setSize=points.size();

        if(setSize<50){
            L=3;
        }

        if(setSize<100){
            L=2;
        }
        
        double threshold=0.10;

        if(setSize>200 && setSize<=500){
            threshold = 0.08;
        }

        if (setSize>500){
            threshold = 0.05;
        }
        
        ConvexHullAlgo *generator = new ConvexHullAlgo(points, selection);
        Polygon_2 initial = generator->generatePolygon();

        LocalAlgo *optimizer = new LocalAlgo(initial, convexHullArea, threshold, type, L);
        Polygon_2 optimal = (*optimizer).optimalPolygon();

        return abs(optimal.area()) / convexHullArea;
    }
     
    virtual double convexHullAnnealing(OptimizationType type)
    {
        EdgeSelection selection = (type == OptimizationType::maximization) ? EdgeSelection::max : EdgeSelection::min;
        ConvexHullAlgo *generator = new ConvexHullAlgo(points, selection);
        Polygon_2 initial = generator->generatePolygon();

        SimulatedAnnealing *optimizer = new SimulatedAnnealing(initial, convexHullArea, 500, type, AnnealingType::local);
        Polygon_2 optimal = (*optimizer).optimalPolygon();

        return abs(optimal.area()) / convexHullArea;
    }

    virtual double onionLocalSearch(OptimizationType type)
    {
        EdgeSelection selection;
        
        if(type==maximization){
            selection=max;
        }else{
            selection=min;      
        }

        int L=1;
        int setSize=points.size();

        if(setSize<50){
            L=3;
        }

        if(setSize<100){
            L=2;
        }
        
        double threshold=0.10;

        if(setSize>200 && setSize<=500){
            threshold = 0.08;
        }

        if (setSize>500){
            threshold = 0.05;
        }

        OnionAlgo *generator = new OnionAlgo(points, 3);
        Polygon_2 initial = generator->generatePolygon();

        LocalAlgo *optimizer = new LocalAlgo(initial, convexHullArea, threshold, type, L);
        Polygon_2 optimal = (*optimizer).optimalPolygon();

        return abs(optimal.area()) / convexHullArea;
    }

    virtual double onionAnnealing(OptimizationType type)
    {
        OnionAlgo *generator = new OnionAlgo(points, 1);
        Polygon_2 initial = generator->generatePolygon();

        SimulatedAnnealing *optimizer = new SimulatedAnnealing(initial, convexHullArea, 500, type, AnnealingType::local);
        Polygon_2 optimal = (*optimizer).optimalPolygon();

        return abs(optimal.area()) / convexHullArea;
    }

    virtual double antColony(OptimizationType type)
    {
        AntParameters dummy;
        Polygon_2 dummyPoly;
        dummy.alpha=1;
        dummy.elitism=0;
        dummy.beta=3;
        dummy.L=3;
        dummy.optimizationType=type;
        dummy.ro=0.05;
        dummy.enable_breaks=1;

        Ant *optimizer = new Ant(dummy, points, dummyPoly);
        Polygon_2 optimal = (*optimizer).optimalPolygon();

        return abs(optimal.area()) / convexHullArea;
    }
};

#endif