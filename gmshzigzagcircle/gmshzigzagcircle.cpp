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
void createGeometry(const double& r,const double& R, const std::size_t& numPoints,T& ofs)
{
  if(numPoints>1)
  {
    // create points
    std::vector<CoordinateType> innerPoints(numPoints,0.0);
    std::vector<CoordinateType> outerPoints(numPoints,0.0);
    const double angle(2.0*M_PI/static_cast<double>(numPoints));
    const double angleOffset(angle/2.0);
    for(std::size_t i=0;i!=numPoints;++i)
    {
      outerPoints[i].idx=i+1;
      outerPoints[i].x=R*cos(static_cast<double>(i)*angle);
      outerPoints[i].y=R*sin(static_cast<double>(i)*angle);
      innerPoints[i].idx=i+1+numPoints;
      innerPoints[i].x=r*cos(static_cast<double>(i)*angle+angleOffset);
      innerPoints[i].y=r*sin(static_cast<double>(i)*angle+angleOffset);
    }

    // dump characteristic lenght
    ofs<<"cli = 0.1;"<<std::endl;

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
  }
  else
    std::cout<<"ERROR: you need to put at least 2 points!"<<std::endl;
}

// main
int main(int argc,char** argv)
{
  // set parameters
  std::size_t numPoints(100);
  std::string filename("zigzagcircle.geo");
  double r(0.95);
  double R(1.0);
  if(argc>1)
    numPoints=static_cast<std::size_t>(strtod(argv[1],NULL));
  if(argc>2)
    filename=argv[2];
  if(argc>3)
    r=strtod(argv[3],NULL);
  if(argc>4)
    R=strtod(argv[4],NULL);

  // output parameters
  std::cout<<"Number of points: "<<numPoints<<std::endl;
  std::cout<<"File name : "<<filename<<std::endl;
  std::cout<<"Inner radious: "<<r<<std::endl;
  std::cout<<"Outer radious: "<<R<<std::endl;

  // create geoemtry and dump it
  std::ofstream ofs(filename);
  createGeometry(r,R,numPoints,ofs);
  ofs.close();
  return 0;
}