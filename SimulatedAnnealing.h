#ifndef SIMULATED_ANNEALING_ALGO
#define SIMULATED_ANNEALING_ALGO

#include "shared.h"
#include "PolygonOptimizer.h"


class SimulatedAnnealing : public PolygonOptimizer{
private:
    OptimizationType optimizationType;
    int L;
    AnnealingType annealingType;
    int n;
    double chpArea;

public:

    double polygonArea();
    double minimizationEnergy();
    double maximizationEnergy();
    double getEnergy();

    SimulatedAnnealing(Polygon_2&, double, ArgFlags);
    virtual Polygon_2 optimalPolygon();
};

#endif