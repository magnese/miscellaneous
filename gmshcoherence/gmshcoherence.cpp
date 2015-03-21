#include <iostream>
#include <string>
#include <vector>

#include "Gmsh.h"
#include "GModel.h"
#include "MTetrahedron.h"

#define DEBUG_STATUS 0

enum GmshAlgorithmType {automatic=2,delaunay=5,frontal=6,meshadapt=1};

int main(int argc,char* argv[])
{
  GmshInitialize(argc, argv);

  GmshSetOption("General","Terminal",1.);
  #if DEBUG_STATUS
  GmshSetOption("General","Verbosity",99.);
  #endif
  GmshSetOption("Mesh","Algorithm",static_cast<double>(automatic));

  GModel* gmodelptr_=new GModel();
  gmodelptr_->setFactory("Gmsh");

  std::string fileNameInput;
  if(argc>1)
    fileNameInput=argv[1];
  else fileNameInput="geometry.geo";
    gmodelptr_->readGEO(fileNameInput);

  #if DEBUG_STATUS
  for(typename GModel::viter it=gmodelptr_->firstVertex();it!=gmodelptr_->lastVertex();++it)
    std::cout<<(*it)->x()<<" "<<(*it)->y()<<" "<<(*it)->z()<<std::endl;
  #endif

  int dimgrid(3);
  gmodelptr_->mesh(dimgrid);
  //gmodelptr_->readMSH(fileName);

  #if DEBUG_STATUS
  std::string fileNameOutput("geometry.msh");
  gmodelptr_->writeMSH(fileNameOutput,2.2,false,false);
  #endif

  gmodelptr_->indexMeshVertices(true,0,true); // index all the mesh vertices in a continuous sequence starting at 1

  #if DEBUG_STATUS
  std::cout<<"MaxVertexNumber: "<<gmodelptr_->getMaxVertexNumber()<<std::endl;
  #endif

  MVertex* vtxPtr;
  int iEnd(0);
  int contNullPtr(0);
  int contNegative(0);

  iEnd=gmodelptr_->getMaxVertexNumber();
  //iEnd=gmodelptr_->getNumMeshVertices();

   std::vector<bool> isVtxInserted(static_cast<unsigned>(iEnd+1),false);

  #if DEBUG_STATUS
  std::cout<<"Nodes:"<<std::endl;
  #endif

  for(int i=1;i!=(iEnd+1);++i)
  {
    vtxPtr=gmodelptr_->getMeshVertexByTag(i);
    if(vtxPtr==nullptr)
    {
      ++contNullPtr;
      #if DEBUG_STATUS
        std::cout<<i<<": ERROR nullptr for the vertex!"<<std::endl;
      #endif
    }
    else
    {
      if(vtxPtr->getIndex()>(-1))
      {
        isVtxInserted[i]=true;
        #if DEBUG_STATUS
        std::cout<<i<<": ("<<vtxPtr->x()<<", "<<vtxPtr->y()<<", "<<vtxPtr->z()<<") "<<std::endl;
        #endif
      }
      else
      {
        ++contNegative;
        #if DEBUG_STATUS
        std::cout<<i<<": ERROR getIndex is negative!"<<std::endl;
        #endif
      }
    }
  }

  #if DEBUG_STATUS
  std::cout<<"MaxVertexNumber="<<gmodelptr_->getMaxVertexNumber()<<", NumMeshVertices="<<gmodelptr_->getNumMeshVertices()<<", ContNullPtr="<<contNullPtr<<", ContNegative "<<contNegative<<std::endl;
  #endif

  if((gmodelptr_->getMaxVertexNumber()-gmodelptr_->getNumMeshVertices()-contNullPtr)!=0)
    std::cout<<"ERROR: no consistency!"<<std::endl;

  MTetrahedron* simplexPtr;
  GModel::riter it(gmodelptr_->firstRegion());
  #if DEBUG_STATUS
  std::cout<<"Number of elements: "<<(*it)->tetrahedra.size()<<std::endl;
  #endif
  for(unsigned int i=0;i!=(*it)->tetrahedra.size();++i)
  {
    simplexPtr=(*it)->tetrahedra[i];
    for(int j=0;j<=dimgrid;++j)
    {
      if(!(isVtxInserted[simplexPtr->getVertex(j)->getNum()]))
        std::cout<<"ERROR: vertex "<<simplexPtr->getVertex(j)->getNum()<<" not inserted!"<<std::endl;
    }
  }

  delete gmodelptr_;
  GmshFinalize();

  return 0;
}

