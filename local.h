#ifndef LOCAL_SEARCH
#define LOCAL_SEARCH

#include "onion.h"
#include "PolygonOptimizer.h"


class LocalAlgo : public PolygonOptimizer{
private:
    double threshold; //the threshold of the optimization, given in iput
    OptimizationType type; // the type of the optimization, min or max
    int length; // the initialization option
public:
    LocalAlgo(Polygon_2&, double,OptimizationType,int);
    virtual Polygon_2 optimalPolygon();

};

typedef struct changePairs{
  Polygon_2::Edge_const_iterator e;
  std::vector<Point_2> V;
} changePair;

typedef struct areaAndChanges{
  changePair change;
  long area;
} areaChange;



void getNextIter(Polygon_2::Vertex_iterator&,Polygon_2&);
void getPreviousIter(Polygon_2::Vertex_iterator&,Polygon_2&);

void getNextEdge(Polygon_2::Edge_const_iterator&,Polygon_2&);
void getPreviousEdge(Polygon_2::Edge_const_iterator&,Polygon_2&);

void applyChanges(Polygon_2&,std::vector<Point_2>&,Polygon_2::Edge_const_iterator&);

bool pointInEdge(Point_2&,Segment_2&);
bool chainInEdge(std::vector<Point_2>&,Segment_2&);

bool compareAlterMax(const areaChange&,const areaChange&);
bool compareAlterMin(const areaChange&,const areaChange&);

bool findEdgeInPoly(Polygon_2&,Segment_2&);
bool findVertexInPoly(Polygon_2&,Point_2&);

bool areaImproves(long&,long&,OptimizationType);


#endif