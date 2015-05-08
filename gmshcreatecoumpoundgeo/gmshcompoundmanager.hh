#ifndef GMSHCOMPOUNDMANAGER_HH
#define GMSHCOMPOUNDMANAGER_HH

#include <string>
#include <array>

#include "GModel.h"

class GMSHCompoundManager
{
  public:
  GMSHCompoundManager(const std::string& domainFileName,const std::string& interfaceFileName,const std::string& holeFileName):
    gmodelptrs_(),hashole_(false)
  {
    // load domain
    domain()=new GModel();
    domain()->setFactory("Gmsh");
    domain()->readGEO(domainFileName);
    // load interface
    interface()=new GModel();
    interface()->setFactory("Gmsh");
    interface()->readMSH(interfaceFileName);
    // load hole (if present)
    hole()=new GModel();
    hole()->setFactory("Gmsh");
    if(holeFileName!="")
    {
      hole()->readGEO(holeFileName);
      hashole_=true;
    }
    // create compound
    createCompound();
  }

  ~GMSHCompoundManager()
  {
    for(auto& model:gmodelptrs_)
      delete model;
  }

  inline GModel*& domain()
  {
    return gmodelptrs_[0];
  }
  inline GModel*& interface()
  {
    return gmodelptrs_[1];
  }
  inline GModel*& hole()
  {
    return gmodelptrs_[2];
  }
  inline GModel*& compound()
  {
    return gmodelptrs_[3];
  }

  inline bool& hasHole()
  {
    return hashole_;
  }

  void createCompound()
  {
    if(compound()!=nullptr)
      delete gmodelptrs_[3];
    compound()=new GModel();
    compound()->setFactory("Gmsh");

    // add domain to compound gmodel
    std::vector<GEdge*> domainEdges(0);
    addGModelToCompound(*domain(),domainEdges);

    // add interface to compound gmodel
    std::vector<GEdge*> interfaceEdges(0);
    addMeshToCompound(*interface(),interfaceEdges);

    // add hole to compound gmodel (if present)
    std::vector<GEdge*> holeEdges(0);
    if(hashole_)
      addGModelToCompound(*hole(),holeEdges);

    // add line loops and faces to compound gmodel
    std::vector<std::vector<GEdge*> > lineLoop(1,domainEdges);
    compound()->addPlanarFace(lineLoop);
  }

  private:
  std::array<GModel*,4> gmodelptrs_;
  bool hashole_;

  void addGModelToCompound(GModel& model,std::vector<GEdge*>& edges)
  {
    unsigned int vtxInsertionCounter(0);
    std::vector<GVertex*> vtxInsertedPtr(0);
    std::vector<GVertex*> vtxPtr(2,nullptr);
    std::vector<int> vtxInsertionMap(model.getNumVertices()+1,-1);
    for(typename GModel::eiter it=model.firstEdge();it!=model.lastEdge();++it)
    {
      // get edge physical ID
      unsigned int physicalID(((*it)->getPhysicalEntities())[0]);
      // add first vertex
      vtxPtr[0]=(*it)->getBeginVertex();
      if(vtxInsertionMap[vtxPtr[0]->tag()]==-1)
      {
        vtxInsertedPtr.push_back(compound()->addVertex(vtxPtr[0]->x(),vtxPtr[0]->y(),vtxPtr[0]->z(),
                                                        vtxPtr[0]->prescribedMeshSizeAtVertex()));
        vtxInsertionMap[vtxPtr[0]->tag()]=vtxInsertionCounter;
        ++vtxInsertionCounter;
      }
      vtxPtr[0]=vtxInsertedPtr[vtxInsertionMap[vtxPtr[0]->tag()]];
      // add last vertex
      vtxPtr[1]=(*it)->getEndVertex();
      if(vtxInsertionMap[vtxPtr[1]->tag()]==-1)
      {
        vtxInsertedPtr.push_back(compound()->addVertex(vtxPtr[1]->x(),vtxPtr[1]->y(),vtxPtr[1]->z(),
                                 vtxPtr[1]->prescribedMeshSizeAtVertex()));
        vtxInsertionMap[vtxPtr[1]->tag()]=vtxInsertionCounter;
        ++vtxInsertionCounter;
      }
      vtxPtr[1]=vtxInsertedPtr[vtxInsertionMap[vtxPtr[1]->tag()]];
      // add edge
      edges.push_back(compound()->addLine(vtxPtr[0],vtxPtr[1]));
      (*edges.rbegin())->addPhysicalEntity(physicalID);
    }
  }

  void addMeshToCompound(GModel& model,std::vector<GEdge*>& edges)
  {
    model.indexMeshVertices(true,0,true); // index all the mesh vertices in a continuous sequence starting at 1
    unsigned int vtxInsertionCounter(0);
    std::vector<GVertex*> vtxInsertedPtr(0);
    std::vector<unsigned int> vtxInsertionMap(model.getMaxVertexNumber()+1,0);
    constexpr double charlenght(1000);

    // add all vertices
    for(unsigned int i=1;i!=vtxInsertionMap.size();++i)
    {
      MVertex* vtxPtr(model.getMeshVertexByTag(i));
      if(vtxPtr!=nullptr)
      {
        if(vtxPtr->getIndex()>(-1))
        {
          vtxInsertedPtr.push_back(compound()->addVertex(vtxPtr->x(),vtxPtr->y(),vtxPtr->z(),charlenght));
          vtxInsertionMap[i]=vtxInsertionCounter;
          ++vtxInsertionCounter;
        }
      }
    }
  }
};

#endif //GMSHCOMPOUNDMANAGER_HH
