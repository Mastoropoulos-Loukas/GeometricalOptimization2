#include "SimulatedAnnealing.h"
#include <random>

using std::cout, std::endl;

SimulatedAnnealing::SimulatedAnnealing(Polygon_2& initial, double cHullArea, ArgFlags argFlags) : PolygonOptimizer(initial)
{
    this->optimizationType = argFlags.optimizationType;
    this->L = argFlags.L;
    this->annealingType = argFlags.annealingType;
    this->n = initial.size();
    this->chpArea = cHullArea;
}

Polygon_2 SimulatedAnnealing::optimalPolygon()
{
    std::srand(time(NULL));
    cout << "Energy at the start: " << getEnergy() << endl;

    


    return this->poly;
}

double SimulatedAnnealing::getEnergy()
{
    if(this->optimizationType == maximization)
        return maximizationEnergy();
    else
        return minimizationEnergy();
}

double SimulatedAnnealing::minimizationEnergy()
{
    return this->n * (polygonArea() / this->chpArea);
}

double SimulatedAnnealing::maximizationEnergy()
{
    return this->n * (1 - (polygonArea() / this->chpArea));
}


double SimulatedAnnealing::polygonArea()
{
    return abs(this->poly.area());
}