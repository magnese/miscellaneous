#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

// coordinate type
struct CoordinateType
{
  CoordinateType(const double value):
    x(value),y(value),z(value),idx(1)
  {}
  double x;
  double y;
  double z;
  std::size_t idx;
};

std::ostream& operator<<(std::ostream& os, const CoordinateType& coord)
{
  os<<"Point("<<coord.idx<<") = {"<<coord.x<<", "<<coord.y<<", "<<coord.z<<", cli};";
}

// print line
template<typename T>
void printLine(const std::size_t& idx,const std::size_t& idx1,const std::size_t& idx2,T& ofs)
{
   ofs<<"Line("<<idx<<") = {"<<idx1<<" ,"<<idx2<<"};"<<std::endl;
}

// create zig-zag circle geometry
template<typename T>
void createGeometry(const double& R, const std::size_t& numPoints,T& ofs)
{
  if(numPoints>1)
  {
    // create outer points
    std::vector<CoordinateType> outerPoints(numPoints,0.0);
    const double angle(2.0*M_PI/static_cast<double>(numPoints));
    std::size_t pointIdx(1);
    for(auto& point:outerPoints)
    {
      point.idx=pointIdx;
      point.x=R*cos(static_cast<double>(pointIdx%numPoints)*angle);
      point.y=R*sin(static_cast<double>(pointIdx%numPoints)*angle);
      ++pointIdx;
    }

    // compute sizes of the triangles of the outer region
    const double base(pow(pow(outerPoints[0].x-outerPoints[1].x,2)+pow(outerPoints[0].y-outerPoints[1].y,2),0.5));
    const double height(pow(3.0,0.5)*base/2.0);

    // create inner points
    std::vector<CoordinateType> innerPoints(numPoints,0.0);
    const double angleOffset(angle/2.0);
    const double r(R-height);
    for(auto& point:innerPoints)
    {
      point.idx=pointIdx;
      point.x=r*cos(static_cast<double>(pointIdx%numPoints)*angle+angleOffset);
      point.y=r*sin(static_cast<double>(pointIdx%numPoints)*angle+angleOffset);
      ++pointIdx;
    }

    // dump characteristic lenght
    ofs<<"cli = "<<base<<";"<<std::endl;

    // dump points into the stream
    for(const auto& point:outerPoints)
      ofs<<point<<std::endl;
    for(const auto& point:innerPoints)
      ofs<<point<<std::endl;

    // dump circles lines
    std::size_t lineIdx(1);
    for(std::size_t i=1;i!=numPoints+1;++i,++lineIdx)
      printLine(lineIdx,i,i%numPoints+1,ofs);
    for(std::size_t i=1;i!=numPoints+1;++i,++lineIdx)
      printLine(lineIdx,i+numPoints,i%numPoints+1+numPoints,ofs);

    // dump zig-zag lines
    for(std::size_t i=1;i!=numPoints+1;++i,++lineIdx)
    {
      printLine(lineIdx,i,i+numPoints,ofs);
      ++lineIdx;
      printLine(lineIdx,i+numPoints,i%numPoints+1,ofs);
    }

    // create inner surface
    std::size_t loopIdx(1);
    ofs<<"Line Loop("<<loopIdx<<") = {";
    for(std::size_t i=numPoints+1;i!=2*numPoints;++i)
      ofs<<i<<", ";
    ofs<<2*numPoints<<"};"<<std::endl;
    ofs<<"Plane Surface("<<loopIdx<<") = {"<<loopIdx<<"};"<<std::endl;
    ofs<<"Physical Surface(1) = {"<<loopIdx<<"};"<<std::endl;

    // create outer surface
    ++loopIdx;
    for(std::size_t i=0;i!=numPoints;++i,++loopIdx)
    {
      ofs<<"Line Loop("<<loopIdx<<") = {"<<i+1<<", -"<<2*numPoints+i*2+1<<", -"<<2*numPoints+i*2+2<<"};"<<std::endl;
      ofs<<"Plane Surface("<<loopIdx<<") = {"<<loopIdx<<"};"<<std::endl;
      ++loopIdx;
      ofs<<"Line Loop("<<loopIdx<<") = {"<<numPoints+i+1<<", -"<<2*numPoints+i*2+2<<", -"<<2*numPoints+(i*2+2)%(2*numPoints)+1<<"};"<<std::endl;
      ofs<<"Plane Surface("<<loopIdx<<") = {"<<loopIdx<<"};"<<std::endl;
    }
    ofs<<"Physical Surface(2) = {";
    for(std::size_t i=0;i!=(2*numPoints-1);++i)
      ofs<<i+2<<", ";
    ofs<<2*numPoints+1<<"};"<<std::endl;
  }
  else
    std::cout<<"ERROR: you need to put at least 2 points!"<<std::endl;
}

// main
int main(int argc,char** argv)
{
  // set parameters
  std::size_t numPoints(100);
  std::string filename("output.geo");
  double R(1.0);
  if(argc>1)
    numPoints=static_cast<std::size_t>(strtod(argv[1],NULL));
  if(argc>2)
    filename=argv[2];
  if(argc>3)
    R=strtod(argv[3],NULL);

  // output parameters
  std::cout<<"Number of points: "<<numPoints<<std::endl;
  std::cout<<"File name : "<<filename<<std::endl;
  std::cout<<"Outer radious: "<<R<<std::endl;

  // create geoemtry and dump it
  std::ofstream ofs(filename);
  createGeometry(R,numPoints,ofs);
  ofs.close();
  return 0;
}
