// Create a 3D mesh in Alberta file format from a geometry file in Gmsh file format
// Author: Marco Agnese

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "Gmsh.h"
#include "GModel.h"
#include "MTetrahedron.h"
#include "MTriangle.h"

#define DEBUG_STATUS 0

enum GmshAlgorithmType{automatic=2,delaunay=5,frontal=6,meshadapt=1};

// given a tetrahedron and a triangle, return the position of the triangle in the tetrahedron if present otherwise retrun -1
int findPosition(std::vector<unsigned int>& tetrahedron,std::vector<unsigned int>& triangle)
{
  std::vector<unsigned int> vtxPresent(4,0);
  for(std::size_t i=0;i!=3;++i)
  {
    std::vector<unsigned int>::iterator it(std::find(tetrahedron.begin(),tetrahedron.end(),triangle[i]));
    if(it!=tetrahedron.end())
      vtxPresent[it-tetrahedron.begin()]=1;
  }
  int position(-1);
  unsigned int counter(0);
  for(std::size_t i=0;i!=4;++i)
  {
    if(vtxPresent[i]==0) position=i;
    counter+=vtxPresent[i];
  }
  if(counter!=3) position=-1;
  return position;
}

int main(int argc,char** argv)
{
  // input information
  std::string inputFileName;
  if (argc!=2)
  {
    std::cout<<"Geometry file name : ";
    std::cin>>inputFileName;
  }
  else
    inputFileName=argv[1];

  constexpr unsigned int worldDim(3);
  constexpr unsigned int gridDim(3);

  // create mesh
  GmshInitialize(argc, argv);
  GmshSetOption("General","Terminal",1.);
  #if DEBUG_STATUS
  GmshSetOption("General","Verbosity",99.);
  #endif
  GmshSetOption("Mesh","Algorithm",static_cast<double>(automatic));

  GModel* gmodelptr_=new GModel();
  gmodelptr_->setFactory("Gmsh");
  gmodelptr_->readGEO(inputFileName);
  gmodelptr_->mesh(gridDim);
  #if DEBUG_STATUS
  gmodelptr_->writeMSH("debug.msh",2.2,false,false);
  #endif

  // extracting vertices
  gmodelptr_->indexMeshVertices(true,0,true); // indexing all the mesh vertices in a continuous sequence starting at 1
  MVertex* vtxPtr;
  std::size_t numVertices(gmodelptr_->getMaxVertexNumber());
  std::size_t verticesCounter(0);
  std::vector<int> vtxMap(numVertices,-1);
  std::vector<std::vector<double>> vertices(numVertices,std::vector<double>(3,0.0));
  for(std::size_t i=0;i!=numVertices;++i)
  {
    vtxPtr=gmodelptr_->getMeshVertexByTag(i+1); // offset since gmsh numbering start from 1
    if(vtxPtr!=nullptr)
    {
      if(vtxPtr->getIndex()>-1)
      {
        vertices[verticesCounter][0]=vtxPtr->x();
        vertices[verticesCounter][1]=vtxPtr->y();
        vertices[verticesCounter][2]=vtxPtr->z();
        vtxMap[i]=verticesCounter;
        ++verticesCounter;
      }
    }
  }
  numVertices=verticesCounter;
  vertices.resize(verticesCounter);

  // extracting elements
  std::size_t numElements(0);
  std::vector<std::vector<unsigned int>> elements(0);
  std::size_t elementsCounter(0);
  for(GModel::riter it=gmodelptr_->firstRegion();it!=gmodelptr_->lastRegion();++it)
  {
    numElements=(*it)->tetrahedra.size();
    elements.resize(elementsCounter+numElements,std::vector<unsigned int>(gridDim+1,0));
    for(std::size_t i=0;i!=numElements;++i)
    {
      MTetrahedron* simplexPtr((*it)->tetrahedra[i]);
      bool isReversed((*it)->tetrahedra[i]->getVolumeSign()>-1?false:true);
      if(isReversed)
        std::cout<<"WANRING: tetrahedra "<<elementsCounter<<" is reversed! Please swap the vertices!"<<std::endl;
      for(std::size_t j=0;j!=(gridDim+1);++j)
        elements[elementsCounter][j]=vtxMap[simplexPtr->getVertex(j)->getNum()-1];
      ++elementsCounter;
    }
  }
  numElements=elementsCounter;

  // extracting boundaries
  std::size_t numBoundaries(0);
  std::vector<std::vector<unsigned int>> boundaries(0);
  std::size_t boundariesCounter(0);
  for(GModel::fiter it=gmodelptr_->firstFace();it!=gmodelptr_->lastFace();++it)
  {
    numBoundaries=(*it)->triangles.size();
    boundaries.resize(boundariesCounter+numBoundaries,std::vector<unsigned int>(gridDim,0));
    for(std::size_t i=0;i!=numBoundaries;++i)
    {
      MTriangle* simplexPtr((*it)->triangles[i]);
      for(std::size_t j=0;j!=gridDim;++j)
        boundaries[boundariesCounter][j]=vtxMap[simplexPtr->getVertex(j)->getNum()-1];
      ++boundariesCounter;
    }
  }
  numBoundaries=boundariesCounter;

  // free gmsh memory
  delete gmodelptr_;
  GmshFinalize();

  // compute boundaries in alberta format
  std::vector<std::vector<unsigned int>> elementBoundaries(numElements,std::vector<unsigned int>(gridDim+1,0));
  for(std::vector<unsigned int>& triangle:boundaries)
  {
    std::size_t i(0);
    bool notFound(true);
    while(notFound)
    {
      int position(findPosition(elements[i],triangle));
      if(position!=-1)
      {
        elementBoundaries[i][position]=1;
        notFound=false;
      }
      ++i;
    }
  }

  // write output
  std::string outputFileName(inputFileName);
  outputFileName.resize(outputFileName.size()-4);
  outputFileName+=".apmc";
  std::ofstream outputFile(outputFileName);
  if(outputFile.is_open())
  {
    outputFile<<"DIM:\t\t\t\t\t"<<gridDim<<std::endl;
    outputFile<<"DIM_OF_WORLD: "<<worldDim<<std::endl<<std::endl;
    outputFile<<"number of vertices: "<<numVertices<<std::endl;
    outputFile<<"number of elements: "<<numElements<<std::endl<<std::endl;
    outputFile<<"vertex coordinates:"<<std::endl;
    for(std::vector<double>& coor:vertices)
      outputFile<<"\t"<<coor[0]<<" "<<coor[1]<<" "<<coor[2]<<std::endl;
    outputFile<<std::endl;
    outputFile<<"element vertices:"<<std::endl;
    for(std::vector<unsigned int>& element:elements)
    {
      outputFile<<"\t";
      for(unsigned int& vtx:element)
        outputFile<<vtx<<" ";
      outputFile<<std::endl;
    }
    outputFile<<std::endl;
    outputFile<<"element boundaries:"<<std::endl;
    for(std::vector<unsigned int>& elementBoundary:elementBoundaries)
    {
      outputFile<<"\t";
      for(unsigned int& value:elementBoundary)
        outputFile<<value<<" ";
      outputFile<<std::endl;
    }
    outputFile.close();
    std::cout<<"Mesh created in file "<<outputFileName<<"."<<std::endl;
  }
  else
    std::cout<<"Impossible to create file "<<outputFileName<<"."<<std::endl;

  return 0;
}
