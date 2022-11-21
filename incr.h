
#ifndef INCREMENTAL
#define INCREMENTAL


#include "PolygonGenerator.h"
#include "shared.h"
#include"Pick.h"


class IncAlgo : public PolygonGenerator{
private:
ArgFlags argFlags;
public:
    IncAlgo(PointList&, ArgFlags);
    virtual Polygon_2 generatePolygon();
};


#endif