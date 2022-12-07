

#ifndef AntAlg
#define AntAlg
 

#include "PolygonOptimizer.h"
#include"Pick.h"


class Ant : public PolygonOptimizer{
private:
ArgFlags argFlags;
PointList list;
public:
Ant( ArgFlags argFlags,PointList list);
    virtual Polygon_2 optimalPolygon();
};


#endif