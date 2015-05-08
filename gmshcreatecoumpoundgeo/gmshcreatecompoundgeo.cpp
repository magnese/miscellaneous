#include <iostream>
#include <string>

#include "Gmsh.h"

#include "gmshcompoundmanager.hh"

#define DEBUG_STATUS 1

enum GmshAlgorithmType {automatic=2,delaunay=5,frontal=6,meshadapt=1};

int main(int argc,char** argv)
{
  // init gmsh
  GmshInitialize(argc, argv);
  GmshSetOption("General","Terminal",1.);
  #if DEBUG_STATUS
  GmshSetOption("General","Verbosity",99.);
  #endif
  GmshSetOption("Mesh","Algorithm",static_cast<double>(automatic));

  // get geometries file names
  std::string domainFileName;
  std::string interfaceFileName;
  std::string holeFileName;
  if(argc<3)
  {
    std::cout<<"Program usage: "<<std::endl;
    std::cout<<"./gmshreader domain.geo interface.geo [hole.geo]"<<std::endl;
    return 1;
  }
  else
  {
    domainFileName=argv[1];
    std::cout<<"Domain: "<<domainFileName<<std::endl;
    interfaceFileName=argv[2];
    std::cout<<"Interface: "<<interfaceFileName<<std::endl;
    if(argc>3)
    {
      holeFileName=argv[3];
      std::cout<<"Hole: "<<holeFileName<<std::endl;
    }
  }

  // create gmodels
  GMSHCompoundManager compoundManager(domainFileName,interfaceFileName,holeFileName);

  // dump compound geo file
  std::string compoundFileName("compound.geo");
  compoundManager.compound()->writeGEO(compoundFileName,true,false);

  // finalize gmsh and return
  GmshFinalize();
  return 0;
}
