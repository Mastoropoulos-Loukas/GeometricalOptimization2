#include "SimulatedAnnealing.h"
#include <random>
#include <algorithm>
#include <math.h>

using std::cout; using std::endl;

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

    switch (annealingType)
    {
    case local:
        return localAnnealing();
        break;
    
    case global:
        return globalAnnealing();
        break;
    
    case subdivision:
        return subdivisionAnnealing();
        break;
    default:
        return poly;
    }
}

double minX(Point_2 a, Point_2 b, Point_2 c, Point_2 d)
{
    double res = a[0];
    if(b[0] < res)res = b[0];
    if(c[0] < res)res = c[0];
    if(d[0] < res)res = d[0];
    return res;
}

double maxX(Point_2 a, Point_2 b, Point_2 c, Point_2 d)
{
    double res = a[0];
    if(b[0] > res)res = b[0];
    if(c[0] > res)res = c[0];
    if(d[0] > res)res = d[0];
    return res;
}

double minY(Point_2 a, Point_2 b, Point_2 c, Point_2 d)
{
    double res = a[1];
    if(b[1] < res)res = b[1];
    if(c[1] < res)res = c[1];
    if(d[1] < res)res = d[1];
    return res;
}

double maxY(Point_2 a, Point_2 b, Point_2 c, Point_2 d)
{
    double res = a[1];
    if(b[1] > res)res = b[1];
    if(c[1] > res)res = c[1];
    if(d[1] > res)res = d[1];
    return res;
}

Fuzzy_iso_box getQueryBox(Point_2 a, Point_2 b, Point_2 c, Point_2 d) 
{
    double xmin = minX(a, b, c, d);
    double xmax = minX(a, b, c, d);
    double ymin = minX(a, b, c, d);
    double ymax = minX(a, b, c, d);
    
    return Fuzzy_iso_box(Point_2(xmin, ymin), Point_2(xmax, ymax));
}

void getQueryResult(PointList& result, Fuzzy_iso_box query, Tree& tree)
{
    tree.search(std::back_inserter(result), query);
}

void initializeTree(Tree& tree, Polygon_2& poly)
{
    PointListIterator begin = poly.vertices_begin();
    PointListIterator end = poly.vertices_end();
    
    for(PointListIterator iter = begin; iter != end; ++iter)
    {
        tree.insert(*iter);
    }
}


Segment_2 nullSegment(){return Segment_2(Point_2(0,0), Point_2(0,0));}

Segment_2 SimulatedAnnealing::getEdgeFromSource(Point_2 source)
{
    PointListIterator begin = poly.vertices_begin();
    PointListIterator end = poly.vertices_end();
    
    Point_2 target;
    for(auto iter = begin; iter != end; iter++)
    {
        if(*iter == source)
        {
            if(iter+1 == end)
                target = *begin;
            else    
                target = *(iter+1);

            return Segment_2(source, target);
        }
    }
    return nullSegment();
}

Segment_2 SimulatedAnnealing::getEdgeFromTarget(Point_2 target)
{
    PointListIterator begin = poly.vertices_begin();
    PointListIterator end = poly.vertices_end();
    
    Point_2 source;
    for(auto iter = begin; iter != end; iter++)
    {
        if(*iter == target)
        {
            if(iter == begin)
                source = *(end - 1);
            else    
                target = *(iter - 1);

            return Segment_2(source, target);
        }
    }
    return nullSegment();
}


bool SimulatedAnnealing::validityLocal(Point_2 q, Point_2 r, Point_2 s, Point_2 p, Tree& tree)
{

    Segment_2 newEdge1 = Segment_2(p, r);
    Segment_2 newEdge2 = Segment_2(q, s);
    

    //if new segments intersect each other, return false
    if(CGAL::do_intersect(newEdge1, newEdge2))
        return false;
    
    //check if the new segments intersect another edge with the tree
    
    //get points in box
    PointList result;
    getQueryResult(
        result,
        getQueryBox(p, q, r, s),
        tree
    );

    //for every edge that has a point in the list, check for intersection
    for(PointListIterator iter = result.begin(); iter != result.end(); ++iter)
    {
        Point_2 testPoint = *iter;
        Segment_2 s1 = getEdgeFromSource(testPoint);
        Segment_2 s2 = getEdgeFromTarget(testPoint);

        if(
            CGAL::do_intersect(s1, newEdge1) ||
            CGAL::do_intersect(s1, newEdge2) ||
            CGAL::do_intersect(s2, newEdge1) ||
            CGAL::do_intersect(s2, newEdge2)
        )
            return false;

    }

    return true;

}

Polygon_2 SimulatedAnnealing::localAnnealing()
{

    Tree tree;
    initializeTree(tree, poly);

    double T = 1;
    Point_2 q, r, s, p;
    PointListIterator qIndex, rIndex, sIndex, pIndex;

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);
        
    while(T > 0)
    {
        cout << "T: " << T << endl;
        double energyInitial = getEnergy();
        PointListIterator begin = poly.vertices_begin();
        PointListIterator end = poly.vertices_end();

        int selection = rand()%n;
        int transitionsTried = 1;

        //get random valid transition
        do
        {
            cout << ".";
            qIndex = begin + selection;
            rIndex = qIndex + 1; if(rIndex == end) rIndex = begin;
            sIndex = rIndex + 1; if(sIndex == end) sIndex = begin;
            pIndex = (selection == 0) ? end - 1 : qIndex - 1;

            q = *(qIndex);
            r = *(rIndex);
            s = *(sIndex);
            p = *(pIndex);

            selection = (selection + 1) % n;

            //avoid infinite loop, due to bad code
            if(transitionsTried == n)
            {
                cout << "could not find valid transition" << endl;
                return poly;
            }

        }while(validityLocal(q, r, s, p, tree));

        cout << endl;
        //make transition
        *rIndex = q;
        *qIndex = r;

        double energyFinal = getEnergy();
        double DE = energyFinal - energyInitial;

        //if energy increased, and Metropolis criterion doesn't hold revert change
        if(DE >= 0)
        {
            if(exp(-(DE/T)) < distribution(generator))
            {
                *rIndex = r;
                *qIndex = q;
            }
        }

        T = T - (1 / (double)L);    
    }
    return poly;
}

Polygon_2 SimulatedAnnealing::globalAnnealing()
{for(int annealingIteration = 0; annealingIteration < L; annealingIteration++)
    {
        cout << "global: "  << annealingIteration << endl;
    }
    return poly;
}

Polygon_2 SimulatedAnnealing::subdivisionAnnealing()
{
    for(int annealingIteration = 0; annealingIteration < L; annealingIteration++)
    {
        cout << "subdivision: "  << annealingIteration << endl;
    }
    return poly;
}

void SimulatedAnnealing::moveVertex(PointListIterator from, PointListIterator to)
{
    Point_2 p = *from;
    poly.erase(from);
    poly.insert(to, p);
    return;
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