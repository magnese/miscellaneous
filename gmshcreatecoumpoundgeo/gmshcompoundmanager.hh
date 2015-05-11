#ifndef GMSHCOMPOUNDMANAGER_HH
#define GMSHCOMPOUNDMANAGER_HH

#include <string>
#include <array>
#include <vector>

#include "Gmsh.h"
#include "GModel.h"
#include "MLine.h"

// mesh algorithms type
enum GmshAlgorithmType {automatic=2,delaunay=5,frontal=6,meshadapt=1};

class GMSHCompoundManager
{
  public:
  GMSHCompoundManager(int argc,char** argv,const std::string& domainFileName,const std::string& interfaceFileName,
                      const std::string& holeFileName,const GmshAlgorithmType& algorithm=automatic,const bool& verbosity=false):
    gmodelptrs_(),hashole_(false)
  {
    // init gmsh
    GmshInitialize(argc,argv);
    GmshSetOption("General","Terminal",1.);
    if(verbosity)
      GmshSetOption("General","Verbosity",99.);
    GmshSetOption("Mesh","Algorithm",static_cast<double>(algorithm));
    // load domain
    domain()=new GModel();
    domain()->setFactory("Gmsh");
    domain()->readGEO(domainFileName);
    // load interface
    interface()=new GModel();
    interface()->setFactory("Gmsh");
    interface()->readMSH(interfaceFileName);
    convertMesh2GModel(interface());
    // load hole (if present)
    hole()=new GModel();
    hole()->setFactory("Gmsh");
    if(holeFileName!="")
    {
      hole()->readGEO(holeFileName);
      hashole_=true;
    }
  }

  ~GMSHCompoundManager()
  {
    for(auto& model:gmodelptrs_)
      delete model;
    // finalize gmsh
    GmshFinalize();
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

  void createCompoundGeo()
  {
    if(compound()!=nullptr)
      freeCompound();
    compound()=new GModel();
    compound()->setFactory("Gmsh");

    // add domain to compound gmodel
    std::vector<GEdge*> domainEdges(0);
    addGModelToCompound(domain(),domainEdges);

    // add interface to compound gmodel
    std::vector<GEdge*> interfaceEdges(0);
    addGModelToCompound(interface(),interfaceEdges);

    // add hole to compound gmodel (if present)
    std::vector<GEdge*> holeEdges(0);
    if(hashole_)
      addGModelToCompound(hole(),holeEdges);

    // add line loops and faces to compound gmodel
    std::vector<std::vector<GEdge*>> outerLineLoop({domainEdges,interfaceEdges});
    (compound()->addPlanarFace(outerLineLoop))->addPhysicalEntity(2);
    std::vector<std::vector<GEdge*>> innerLineLoop({interfaceEdges});
    if(hashole_)
      innerLineLoop.push_back(holeEdges);
    (compound()->addPlanarFace(innerLineLoop))->addPhysicalEntity(1);
  }

  inline void createCompoundMsh()
  {
    if(compound()==nullptr)
      createCompoundGeo();
    compound()->mesh(2);
  }

  inline void writeCompoundGeo(const std::string& fileName="compound.geo")
  {
    compound()->writeGEO(fileName,true,false);
  }

  inline void writeCompoundMsh(const std::string& fileName="compound.msh")
  {
    compound()->writeMSH(fileName,2.2,false,false);
  }

  inline void freeCompound()
  {
    delete compound();
  }

  private:
  std::array<GModel*,4> gmodelptrs_;
  bool hashole_;

  void addGModelToCompound(GModel*& model,std::vector<GEdge*>& edges)
  {
    unsigned int vtxCounter(0);
    std::vector<GVertex*> vertices(0);
    std::array<GVertex*,2> vtxPtr({nullptr,nullptr});
    std::vector<int> verticesMap(model->getNumVertices()+1,-1);
    for(typename GModel::eiter it=model->firstEdge();it!=model->lastEdge();++it)
    {
      // get edge physical ID
      unsigned int physicalID(((*it)->getPhysicalEntities())[0]);
      // add first vertex
      vtxPtr[0]=(*it)->getBeginVertex();
      if(verticesMap[vtxPtr[0]->tag()]==-1)
      {
        vertices.push_back(compound()->addVertex(vtxPtr[0]->x(),vtxPtr[0]->y(),vtxPtr[0]->z(),vtxPtr[0]->prescribedMeshSizeAtVertex()));
        verticesMap[vtxPtr[0]->tag()]=vtxCounter;
        ++vtxCounter;
      }
      vtxPtr[0]=vertices[verticesMap[vtxPtr[0]->tag()]];
      // add last vertex
      vtxPtr[1]=(*it)->getEndVertex();
      if(verticesMap[vtxPtr[1]->tag()]==-1)
      {
        vertices.push_back(compound()->addVertex(vtxPtr[1]->x(),vtxPtr[1]->y(),vtxPtr[1]->z(),vtxPtr[1]->prescribedMeshSizeAtVertex()));
        verticesMap[vtxPtr[1]->tag()]=vtxCounter;
        ++vtxCounter;
      }
      vtxPtr[1]=vertices[verticesMap[vtxPtr[1]->tag()]];
      // add edge
      edges.push_back(compound()->addLine(vtxPtr[0],vtxPtr[1]));
      (*edges.rbegin())->addPhysicalEntity(physicalID);
    }
  }

  void convertMesh2GModel(GModel*& model)
  {
    // create new gmodel
    GModel* newGModel(new GModel());
    newGModel->setFactory("Gmsh");

    // index all the mesh vertices in a continuous sequence starting at 1
    model->indexMeshVertices(true,0,true);
    unsigned int vtxCounter(0);
    std::vector<GVertex*> vertices(0);
    std::vector<unsigned int> verticesMap(model->getMaxVertexNumber()+1,0);
    constexpr double charlenght(1000);

    // add vertices
    for(unsigned int i=1;i!=verticesMap.size();++i)
    {
      MVertex* vtxPtr(model->getMeshVertexByTag(i));
      if(vtxPtr!=nullptr)
      {
        if(vtxPtr->getIndex()>(-1))
        {
          vertices.push_back(newGModel->addVertex(vtxPtr->x(),vtxPtr->y(),vtxPtr->z(),charlenght));
          verticesMap[i]=vtxCounter;
          ++vtxCounter;
        }
      }
    }

    // add edges
    std::array<unsigned int,2> posVtx({0,0});
    constexpr unsigned int physicalID(1);
    std::vector<GEdge*> edges(0);
    for(typename GModel::eiter edgeIt=model->firstEdge();edgeIt!=model->lastEdge();++edgeIt)
    {
      for(unsigned int i=0;i!=(*edgeIt)->lines.size();++i)
      {
        MLine* linePtr((*edgeIt)->lines[i]);
        posVtx[0]=verticesMap[linePtr->getVertex(0)->getNum()];
        posVtx[1]=verticesMap[linePtr->getVertex(1)->getNum()];
        edges.push_back(newGModel->addLine(vertices[posVtx[0]],vertices[posVtx[1]]));
        (*edges.rbegin())->addPhysicalEntity(physicalID);
      }
    }

    // free old mesh model and assign new gmodel
    delete model;
    model=newGModel;
  }
};

#endif //GMSHCOMPOUNDMANAGER_HH
