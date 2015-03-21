// Create a 2D mesh in Alberta file format from a geometry file in Gmsh file format
// Author: Marco Agnese

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "Gmsh.h"
#include "GModel.h"
#include "MTriangle.h"

#define DEBUG_STATUS 1

enum GmshAlgorithmType{automatic=2,delaunay=5,frontal=6,meshadapt=1};

int main(int argc,char** argv)
{
  // input information
  std::string inputFileName;
  std::cout<<"Geometry file name : ";
  std::cin>>inputFileName;
  unsigned int worldDim;
  std::cout<<"World dimension : ";
  std::cin>>worldDim;
  unsigned int gridDim;
  std::cout<<"Grid dimension : ";
  std::cin>>gridDim;
  std::cout<<std::endl;

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
        ++verticesCounter;
      }
    }
  }
  numVertices=verticesCounter;
  vertices.resize(verticesCounter);

  // extracting elements
  MTriangle* simplexPtr;
  std::size_t numElements(0);
  std::vector<std::vector<unsigned int>> elements(0);
  std::size_t elementsCounter(0);
  for(GModel::fiter it=gmodelptr_->firstFace();it!=gmodelptr_->lastFace();++it)
  {
    numElements=(*it)->triangles.size();
    elements.resize(numElements+elementsCounter,std::vector<unsigned int>(3,0));
    for(std::size_t i=0;i!=numElements;++i)
    {
      simplexPtr=(*it)->triangles[i];
      for(std::size_t j=0;j!=(gridDim+1);++j)
        elements[elementsCounter][j]=simplexPtr->getVertex(j)->getNum()-1;
      ++elementsCounter;
    }
  }
  numElements=elementsCounter;

  // free gmsh memory
  delete gmodelptr_;
  GmshFinalize();

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
    for(std::vector<std::vector<double>>::iterator it=vertices.begin();it!=vertices.end();++it)
      outputFile<<"\t"<<(*it)[0]<<" "<<(*it)[1]<<" "<<(*it)[2]<<std::endl;
    outputFile<<std::endl;
    outputFile<<"element vertices:"<<std::endl;
    for(std::vector<std::vector<unsigned int>>::iterator it=elements.begin();it!=elements.end();++it)
      outputFile<<"\t"<<(*it)[0]<<" "<<(*it)[1]<<" "<<(*it)[2]<<std::endl;
    outputFile.close();
    std::cout<<"Mesh created in file "<<outputFileName<<"."<<std::endl;
  }
  else
    std::cout<<"Impossible to create file "<<outputFileName<<"."<<std::endl;

  return 0;
}
