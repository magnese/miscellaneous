#include <iostream>
#include <string>

#include "gmshcompoundmanager.hh"

int main(int argc,char** argv)
{
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
  // create gmodels and dump compund geo file
  constexpr int worlddim(3);
  GMSHCompoundManager<worlddim> compoundManager(argc,argv,domainFileName,interfaceFileName,holeFileName,automatic,true);
  compoundManager.createCompoundGeo();
  compoundManager.writeCompoundGeo();
  // create compund mesh and dump it
  compoundManager.createCompoundMsh();
  compoundManager.writeCompoundMsh();
  // return
  return 0;
}
