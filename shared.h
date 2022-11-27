#ifndef SHARED_H
#define SHARED_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/IO/WKT.h>
#include <CGAL/convex_hull_2.h>
#include <iostream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Polygon_2<Kernel> Polygon_2;
typedef CGAL::Point_2<Kernel> Point_2;
typedef std::vector<Point_2> PointList;
typedef std::vector<Point_2>::iterator PointListIterator;
typedef Kernel::Segment_2 Segment_2;
typedef Polygon_2::Edge_const_iterator EdgeIterator;
typedef std::pair<Point_2, Point_2> PointPair;
typedef std::vector<PointPair> PointPairList;
typedef std::vector<PointPair>::iterator PointPairListIterator;

enum GenerationAlgorithm {incremental, convex_hull, onion};
enum EdgeSelection {randomSelection, min, max};
enum Initialization {a1, a2, b1, b2};

enum OptimazationAlgorithm {local_search, simulated_annealing, ant_colony};
enum AnnealingType {local, global, subdivision};
enum OptimizationType {minimization, maximization};

struct ArgFlags{
    std::string inputFile;
    std::string outputFile;
    OptimazationAlgorithm algorithm;
    double L;
    OptimizationType optimizationType;
    double threshold;
    AnnealingType annealingType;

    double alpha;
    double beta;
    double ro;
    int elitism;

    bool error;
    std::string errorMessage;
};

#endif