#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// point type
struct PointType
{
  PointType():
    x(0.0),y(0.0),z(0.0),idx(1)
  {}
  double x;
  double y;
  double z;
  std::size_t idx;
};

std::ostream& operator<<(std::ostream& os, const PointType& point)
{
  return os<<"Point("<<point.idx<<") = {"<<point.x<<", "<<point.y<<", "<<point.z<<", cli};";
}

// line type
struct LineType
{
  LineType():
    idxPoint1(1),idxPoint2(2),idx(1)
  {}
  std::size_t idxPoint1;
  std::size_t idxPoint2;
  std::size_t idx;
};

std::ostream& operator<<(std::ostream& os, const LineType& line)
{
  return os<<"Line("<<line.idx<<") = {"<<line.idxPoint1<<" ,"<<line.idxPoint2<<"};";
}

// create geometry
template<typename T>
void createGeometry(const double& R, const std::size_t& numPoints,T& ofs,const double& alpha)
{
  if(numPoints>1)
  {
    // create outer points
    std::vector<PointType> outerPoints(numPoints);
    const double angle(2.0*M_PI/static_cast<double>(numPoints));
    std::size_t pointIdx(1);
    for(auto& point:outerPoints)
    {
      point.idx=pointIdx;
      point.x=R*cos(static_cast<double>(pointIdx%numPoints)*angle);
      point.y=R*sin(static_cast<double>(pointIdx%numPoints)*angle);
      ++pointIdx;
    }

    // create outer lines
    std::vector<LineType> outerLines(numPoints);
    std::size_t lineIdx(1);
    for(auto& line:outerLines)
    {
      line.idxPoint1=lineIdx;
      line.idxPoint2=lineIdx%numPoints+1;
      line.idx=lineIdx;
      ++lineIdx;
    }

    // compute sizes of the triangles of the outer region
    const double base(pow(pow(outerPoints[0].x-outerPoints[1].x,2)+pow(outerPoints[0].y-outerPoints[1].y,2),0.5));
    const double height(pow(3.0,0.5)*base/2.0);

    // create inner points
    std::vector<PointType> innerPoints(numPoints);
    const double angleOffset(angle/2.0);
    const double r(R-height*alpha);
    for(auto& point:innerPoints)
    {
      point.idx=pointIdx;
      point.x=r*cos(static_cast<double>(pointIdx%numPoints)*angle+angleOffset);
      point.y=r*sin(static_cast<double>(pointIdx%numPoints)*angle+angleOffset);
      ++pointIdx;
    }

    // create inner lines
    std::vector<LineType> innerLines(numPoints);
    for(auto& line:innerLines)
    {
      line.idxPoint1=lineIdx;
      line.idxPoint2=lineIdx%numPoints+1+numPoints;
      line.idx=lineIdx;
      ++lineIdx;
    }

    // create zig-zag lines
    std::vector<LineType> zigLines(numPoints);
    for(auto& line:zigLines)
    {
      line.idxPoint1=lineIdx-2*numPoints;
      line.idxPoint2=lineIdx-numPoints;
      line.idx=lineIdx;
      ++lineIdx;
    }
    std::vector<LineType> zagLines(numPoints);
    for(auto& line:zagLines)
    {
      line.idxPoint1=lineIdx-2*numPoints;
      line.idxPoint2=lineIdx%numPoints+1;
      line.idx=lineIdx;
      ++lineIdx;
    }

    // dump characteristic lenght
    ofs<<"cli = "<<base<<";"<<std::endl;

    // dump points
    for(const auto& point:outerPoints)
      ofs<<point<<std::endl;
    for(const auto& point:innerPoints)
      ofs<<point<<std::endl;

    // dump lines
    for(const auto& line:outerLines)
      ofs<<line<<std::endl;
    for(const auto& line:innerLines)
      ofs<<line<<std::endl;
    for(const auto& line:zigLines)
      ofs<<line<<std::endl;
    for(const auto& line:zagLines)
      ofs<<line<<std::endl;

    // dump inner surface
    std::size_t loopIdx(1);
    ofs<<"Line Loop("<<loopIdx<<") = {";
    bool isFirst(true);
    for(auto& line:innerLines)
      if(isFirst)
      {
        ofs<<line.idx;
        isFirst=false;
      }
      else
        ofs<<", "<<line.idx;
    ofs<<"};"<<std::endl;
    ofs<<"Plane Surface("<<loopIdx<<") = {"<<loopIdx<<"};"<<std::endl;
    ofs<<"Physical Surface(1) = {"<<loopIdx<<"};"<<std::endl;
    ++loopIdx;

    // dump outer surface
    for(std::size_t i=0;i!=numPoints;++i)
    {
      ofs<<"Line Loop("<<loopIdx<<") = {"<<zagLines[i].idx<<", "<<zigLines[(i+1)%numPoints].idx<<", -"<<innerLines[i].idx<<"};"<<std::endl;
      ofs<<"Plane Surface("<<loopIdx<<") = {"<<loopIdx<<"};"<<std::endl;
      ++loopIdx;
      ofs<<"Line Loop("<<loopIdx<<") = {"<<outerLines[i].idx<<", -"<<zigLines[i].idx<<", -"<<zagLines[i].idx<<"};"<<std::endl;
      ofs<<"Plane Surface("<<loopIdx<<") = {"<<loopIdx<<"};"<<std::endl;
      ++loopIdx;
    }
    ofs<<"Physical Surface(2) = {";
    for(std::size_t i=2;i!=loopIdx;++i)
      ofs<<i<<", ";
    ofs<<loopIdx<<"};"<<std::endl;

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
  double alpha(1.0);
  if(argc>1)
    numPoints=static_cast<std::size_t>(strtod(argv[1],NULL));
  if(argc>2)
    filename=argv[2];
  if(argc>3)
    R=strtod(argv[3],NULL);
  if(argc>4)
    alpha=strtod(argv[4],NULL);

  // output parameters
  std::cout<<"Number of points: "<<numPoints<<std::endl;
  std::cout<<"File name : "<<filename<<std::endl;
  std::cout<<"Outer radious: "<<R<<std::endl;
  std::cout<<"Dilatation factor for annulus width: "<<alpha<<std::endl;

  // create geoemtry and dump it
  std::ofstream ofs(filename);
  createGeometry(R,numPoints,ofs,alpha);
  ofs.close();
  return 0;
}
