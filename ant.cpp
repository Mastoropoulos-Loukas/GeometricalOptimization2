#include"ant.h"
#include <climits>
#include <map>

Ant::Ant(ArgFlags argFlags,PointList list) : PolygonOptimizer(poly){
  this->argFlags = argFlags;
  this->list=list;
  
};
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_2                                          Point;
bool isReplaceable(Point_2, Segment_2, Polygon_2&);
bool IsFeasible(Polygon_2 ,Point );
int ProbFunction();
int ph=1;
std::string convert(Polygon_2);

struct ant
{
int father;
int pos;
};
//This is the node for the graph
struct table{

int poly;
double  t=ph;
int h;
bool hasbranch=0;
};



std::map <std::string,int> Pointpoped;


double ProbFunction(int a , int b,table tabletop,std::list<table> tablebot)//Prob function that returns the chance of an ant moving to node i
{



double top=0;
top=pow(tabletop.t,a)*pow(tabletop.h,b);
double bot =0;

std::list<table>::iterator it;
//Find the p^a*h^b for all the other fiesible points
for (it = tablebot.begin(); it != tablebot.end(); ++it){
    bot+=pow(it->t,a)*pow(it->h,b);
    
    }




return top/bot;


}





std::vector<std::list<table>>  UpdateTrails(std::vector<std::list<table>> table1, std::vector <int> points, double area,std::map <int,std::vector<ant>> tables,
double par){
 
//UpdateTrails will find the nodes that more than 1 ant went through and update them, and then it will reduce the pheoromone by ro,its done in optimizerpolygon

std::vector<std::list<table>>  ret=table1;
 int prev=0;
for ( auto v1= points.begin();v1!=points.end();++v1){


  std::list<table> :: iterator pos;

std::list<table> :: iterator it;


 if(v1!=points.begin())
{

   for(auto it1 = ret[prev].begin(); it1 != ret[prev].end(); ++it1) {
        
        if(it1->poly==v1[0]){
        if(it1->hasbranch){

       if(tables[it1->poly].size()>0){

          for(auto x=tables[it1->poly].begin();x!=tables[it1->poly].end();++x){



             if(x[0].father==prev){
             std::list<table>::iterator it2 = ret[x[0].father].begin();
              advance(it2, x[0].pos);
              it2->t+=(par*area)*tables[it1->poly].size();
             }

          }
        
       }

        }
        else{

        it1->t+=par*area;

        }
        }
    
      }
     


}
prev=v1[0];

}


 
   return ret;


}
//See if a point lies inside the polygon or not to find the fiesible points
int check_inside_Ant(Point pt, Point *pgn_begin, Point *pgn_end, Kernel traits)
{ 
  switch(CGAL::bounded_side_2(pgn_begin, pgn_end,pt, traits)) {
    case CGAL::ON_BOUNDED_SIDE :
       return 0;

      break;
    case CGAL::ON_BOUNDARY:
        return 1;
      break;
    case CGAL::ON_UNBOUNDED_SIDE:
    return 2;
      break;
  }

}

std::vector<Segment_2> CheckPolAnt(Polygon_2 ,Point ,int );


//Generate all the triangles that can be made in the given set of points
std::vector<Polygon_2> Generate3(std::vector<Point> list){
Polygon_2 poly;
int flag=0;
std::vector<Polygon_2> res;
    Point points[3] ;

for(auto v1=list.begin();v1!=list.end();++v1){


  for(auto v2=list.begin();v2!=list.end();++v2){

    if(v2[0]==v1[0])
    continue;

    for(auto v3=list.begin();v3!=list.end();++v3){
      if(v2[0]==v3[0])
    continue;
    if(v3[0]==v1[0])
    continue;

    poly.push_back(v1[0]);

    poly.push_back(v2[0]);

    poly.push_back(v3[0]);

    int flag=0;
int j=0;

  for (auto vi = poly.vertices_begin(); vi != poly.vertices_end(); ++vi,++j)
    {
       points[j]=vi[0];
     }
    for(auto v4=list.begin();v4!=list.end();++v4){
    
    if(check_inside_Ant(v4[0],points,points+poly.size(),Kernel())==0)
    { 
       flag=1;
    }
    
    }
    if(flag==0)
  res.push_back(poly);
  poly.clear();
    }


}

return res;

}


}



//Generate a list of x-agons

std::vector<Polygon_2> GenerateX(Polygon_2 space,
std::vector<Point> list){
Polygon_2 check;
std::vector<Polygon_2> temp;
std::vector <Segment_2> segs;
Polygon_2 hull;
  int i=0;
  int pos=0;
  int j=0;


    Point points[space.size()] ;

     for (auto vi = space.vertices_begin(); vi != space.vertices_end(); ++vi,++j)
    {
       points[j]=vi[0];
     }

int k=0;
for (auto v1=list.begin();v1!=list.end();++v1,++k)
{       
  
  
  

if(check_inside_Ant(v1[0],points,points+space.size(),Kernel())==2){

      segs=CheckPolAnt(space,v1[0],0);
    

     for(auto s=segs.begin();s!=segs.end();++s){

    int i=0;
          for(auto v3=space.edges_begin();v3!=space.edges_end();++v3,i++)
          if(v3[0]==s[0]){
            pos=i+1;
            break;
          }
       check=space;


       check.insert(check.vertices_begin()+pos,v1[0]);   
       int flag=0;
             Point points2[check.size()] ;
        int y=0;
        for (auto vi = check.vertices_begin(); vi != check.vertices_end(); ++vi,++y)
    {
       points2[y]=vi[0];
     }
       for (auto v2=list.begin();v2!=list.end();++v2)
{    
if(check_inside_Ant(v2[0],points2,points2+check.size(),Kernel())==0)
{flag=1;
}
}if(flag==0){
       temp.push_back(check);
       Pointpoped[convert(check)]=k;
}
     check.clear();


     }

     }


     
}






return temp;
}

std::string convert(Polygon_2 lis)
{

   std::string ret;
    for( auto v=lis.begin();v!=lis.end();++v){
      
      ret.append(std::to_string(int(v[0][0]+v[0][1])));

      

    }
return ret;

}


//add 1 node to the graph
std::vector<std::list<table>> add_edge(std::vector<std::list<table>> adj_list, int v,table t, int * val) { 

std::vector<std::list<table>> ret=adj_list;
if(val!=nullptr)
*val=ret[v].size();
ret[v].push_back(t);
return ret;
}


// return if the given polygon has already been created elsewhere
bool isin( std::vector<Polygon_2> map,Polygon_2 poly){


  for(auto v1=map.begin();v1!=map.end();++v1)
  if(v1[0]==poly)
  return 1;


  return 0;
}

//Depending on Max or Min algorythm ,return either 1 or 1/area^2 so that updatetrails changes the pheromone accordingly
double MaxOrMin(int area,int mode)
{
  if(mode==0)
  return 1;
  else
  return 1/area*area; 
  
  }
  
  
Polygon_2 Ant::optimalPolygon(){

std::vector<Polygon_2> space;
std::vector<Polygon_2> temp;
std::vector<Polygon_2> adder;
Polygon_2 next;
std::vector<Point> test;
Polygon_2 check;
test=this->list;
int max=-1;
int min=INT_MAX;
int min1=INT_MAX;
int max1=-1;

int pos=0;
double r=argFlags.ro;
std::map<std::string, int> enumvals;
std::map<int,std::vector<Polygon_2>> polymap;
std::vector <std::vector<Polygon_2>> map;
std::vector<std::list<table>> table1;
table table2;
std::vector <int> num;
std::map <int,std::vector<ant>> tables;
std::srand(time(0));
Polygon_2 BestFor1Ant;
int mode=argFlags.optimizationType; //0 for max 1 for min 
int dupes=0;
  int elitismpos=0;
    int elitismk=0;
Polygon_2 BestForCircle;
 space= Generate3(test);
 int AreaOfAllTriangles=0;
 for(auto v=space.begin();v!=space.end();++v){
 enumvals[convert(v[0])]=pos;
  table2.h=v[0].area();
AreaOfAllTriangles+=abs(v[0].area());
  table2.poly=pos;
  table1.resize(table1.size()+1);
  table1=add_edge(table1,pos,table2,nullptr);
  
  pos++;

 }
AreaOfAllTriangles=AreaOfAllTriangles/space.size();

map.push_back(space);
std::list<table> tablebot;
  int K=list.size()/4;
   int C=argFlags.L;
   std::vector <int> paths[K];

   ph=2;
   int elitism=argFlags.elitism;
   std::cout<<pos<<std::endl;
for(int c=0;c<C;c++){
for(int k=0;k<K;k++){
  temp.clear();
  num.clear();
  adder.clear();
int triangle=0;
double prob=0;


for (auto t1=space.begin();t1!=space.end();++t1,++triangle){

if(mode==1)
{

prob=AreaOfAllTriangles/t1[0].area();
prob/=100;
 int var=rand()%2;

 if(var<prob)
{
  next=t1[0];


break;
}
}
else{
prob=t1[0].area()/AreaOfAllTriangles;
prob/=100;
 int var=rand()%2;

 if(var<prob)
{
  next=t1[0];


break;
}
}
 if(triangle==space.size()-1)
{
  next=t1[0];

break;
}
}
tablebot.clear();

std::vector<Point> test1=test;

for(int i=3;i<=test.size();i++){


 num.push_back(enumvals[convert(next)]);
 if(i==test1.size())
 break;

if(polymap.find(enumvals[convert(next)])!=polymap.end()){

temp=polymap[enumvals[convert(next)]];

}else{

 temp= GenerateX(next,test1);
 polymap[enumvals[convert(next)]]=temp;

}


 for (auto t1=temp.begin();t1!=temp.end();++t1){
  if(!t1[0].is_simple()){

std::cout<<"a"<<std::endl;
  }
  if(map.size()<=i-2){
  
  adder.push_back(t1[0]);
  enumvals[convert(t1[0])]=pos;
  pos++;
  table2.poly=enumvals[convert(t1[0])];
  if(mode==0)
  table2.h=t1[0].area()-next.area() ;
  else
    table2.h=1/(t1[0].area()-next.area()) ;

  table1.resize(table1.size()+1);
  table1=add_edge(table1,enumvals[convert(next)],table2,nullptr);
  tablebot.push_back(table2);
  }
  else
 if(!isin(map[i-2],t1[0])){
  enumvals[convert(t1[0])]=pos;
  pos++;
  table2.poly=enumvals[convert(t1[0])];
   if(mode==0)
  table2.h=t1[0].area()-next.area() ;
  else
    table2.h=1/(t1[0].area()-next.area()) ;
    table1.resize(table1.size()+1);

  table1=add_edge(table1,enumvals[convert(next)],table2,nullptr);

  tablebot.push_back(table2);
  adder.push_back(t1[0]);

 }else{
dupes++;
  table2.poly=enumvals[convert(t1[0])];
    table1.resize(table1.size()+1);
    table2.hasbranch=1;
          ant antt;
        antt.father=enumvals[convert(next)];
         if(mode==0)
  table2.h=t1[0].area()-next.area() ;
  else
    table2.h=1/(t1[0].area()-next.area()) ;

  table1=add_edge(table1,enumvals[convert(next)],table2,&antt.pos);
    tables[enumvals[convert(t1[0])]].push_back(antt);
  tablebot.push_back(table2);
 }
 table2.hasbranch=0;
  
 }
 
 map.push_back(adder);

 adder.clear();

int t=0;
 for (auto t1=temp.begin();t1!=temp.end();++t1,++t){
std::list<table>::iterator it=tablebot.begin();
advance(it,t);
double prob=ProbFunction(argFlags.alpha,argFlags.beta,*it,tablebot);
 int var=rand()%2;
 if(var<prob)
{
  next=t1[0];


break;
}
 if(t==temp.size()-1)
{
  next=t1[0];

break;
}
}
if(temp.size()!=0)
test1.erase(test1.begin()+Pointpoped[convert(next)]);
}
 



  
  paths[k]=num;
num.clear();

  for(auto it1 = table1.begin(); it1 != table1.end(); ++it1) {

        
        std::list<table>::iterator it;
for (it = it1[0].begin(); it != it1[0].end(); ++it){
     it->t*=(1-r);
    
    }

        }
if(mode==0){
if(max<abs(next.area())){

 max=abs(next.area());
 BestFor1Ant=next;
  elitismk=k;
}
}else{
  
if(min>abs(next.area())){

 min=abs(next.area());
 BestFor1Ant=next;
  elitismk=k;
}
}
}
if(mode==0){
if(max1<abs(BestFor1Ant.area())){

 max1=abs(BestFor1Ant.area());
 BestForCircle=BestFor1Ant;
 elitismpos=elitismk;
}
}
else{
if(min1>abs(BestFor1Ant.area())){

 min1=abs(BestFor1Ant.area());
 BestForCircle=BestFor1Ant;
 elitismpos=elitismk;
}
}

if(elitism==0){
  for( int t=0;t<K;t++)
  table1=UpdateTrails(table1,paths[t],space[0].area(),tables,MaxOrMin(space[0].area(),mode));
}
  else{
  table1=UpdateTrails(table1,paths[elitismpos],space[0].area(),tables,MaxOrMin(space[0].area(),mode));


  }
}

  Polygon_2 poly;


  poly=BestForCircle;
  std::cout<<poly.area() <<std::endl;
      std::ofstream os("test.wkt");

  return poly;




}


//find the poss edges that can be replaced
std::vector<Segment_2> CheckPolAnt(Polygon_2 poly,Point p,int pos){

  
  std::vector<Segment_2> res;
  double x=0;
  double y=0;
  bool test;

  for (auto vi = poly.edges_begin(); vi != poly.edges_end(); ++vi){


    
    test=isReplaceable(p,vi[0],poly);
    
    if(test==true){
 
      res.push_back(vi[0]);
    }

    }

  
  return res;
}