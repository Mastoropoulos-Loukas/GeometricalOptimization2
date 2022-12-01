# include "local.h"



/*
    Constructor here
*/

/*
    Optimizer Here
*/

// Based on what kind of improvement we desire(min or max), we check whether the area has actually improved
bool areaImproves(long& areaNew, long& areaOld, bool mini){
  if(!mini){
    return (areaNew>areaOld);
  }else{
    return (areaNew<areaOld);
  }
}

// We determine whether <vertex> belongs to <poly>
bool findVertexInPoly(Polygon_2& poly, Point_2& vertex){
  for(auto veit=poly.vertices_begin();veit!=poly.vertices_end();veit++){
    if(*veit==vertex){
      return true;
    }
  }

  return false;
}

// We determine whether <edge> belongs to <poly>
bool findEdgeInPoly(Polygon_2& poly, Segment_2& edge){
  for(auto eit=poly.edges_begin();eit!=poly.edges_end();eit++){
    if(*eit==edge){
      return true;
    }
  }
  return false;
}

//Function used to compare two areaChanges, when we minimize the polygon
bool compareAlterMin(const areaChange& alter1, const areaChange& alter2){
  if(alter1.area!=alter2.area){
    return (alter1.area<alter2.area);
  }else{
    return (alter1.change.V.size()<alter2.change.V.size());
  }
}

//Function used to compare two areaChanges, when we maximize the polygon
bool compareAlterMax(const areaChange& alter1, const areaChange& alter2){
  if(alter1.area!=alter2.area){
    return !(alter1.area<alter2.area);
  }else{
    return (alter1.change.V.size()<alter2.change.V.size());
  }
}

// Checks if parts of vChain belong in edge edge
bool chainInEdge(std::vector<Point_2>& vChain,Segment_2& edge){

  for(int i=0;i<vChain.size();i++){
    if(pointInEdge(vChain[i],edge)){
      return true;
    }
  }

  return false;
}

// The function that applies the changes to the polygon
void applyChanges(Polygon_2& poly, std::vector<Point_2>& vChain,Polygon_2::Edge_const_iterator& edge){

  Segment_2 edgy=*edge;
  Point_2 u2=edgy[1];

  for(int i=0;i<vChain.size();i++){
    if(!pointInEdge(vChain[i],edgy)){
      auto veit=getVertexIt(vChain[i],poly);
      poly.erase(veit);      
    }

  }

  auto v2=getVertexIt(u2,poly);

  for(int i=vChain.size()-1;i>=0;i--){
    poly.insert(v2,vChain[i]);
  }

}

// We check whether <vertex> belong to <edge>
bool pointInEdge(Point_2& vertex,Segment_2& edge){
  return ( vertex==edge[0] || vertex==edge[1]);
}

// We get the edge after eit in Poly. Returns an iterator
void getNextEdge(Polygon_2::Edge_const_iterator& eit, Polygon_2& poly){

  if(eit!= poly.edges_end()-1 && eit!=poly.edges_end()){
    eit++;
  }else{
    eit=poly.edges_begin();
  }

}

// We get the edge before eit in Poly. Returns an iterator
void getPreviousEdge(Polygon_2::Edge_const_iterator& eit, Polygon_2& poly){

  if(eit!= poly.edges_begin()){
    eit--;
  }else{
    eit=poly.edges_end()-1;
  }

}

// We get the vertex after veit in Poly. Returns an iterator
void getNextIter(Polygon_2::Vertex_iterator& veit, Polygon_2& poly){

  if(veit!= poly.vertices_end()-1 && veit!=poly.vertices_end()){
    veit++;
  }else{
    veit=poly.vertices_begin();
  }

}

// We get the vertex before veit in Poly. Returns an iterator
void getPreviousIter(Polygon_2::Vertex_iterator& veit, Polygon_2& poly){

  if(veit!= poly.vertices_begin()){
    veit--;
  }else{
    veit=poly.vertices_end()-1;
  }

}