#ifndef GMSHCOMPOUNDMANAGER_HH
#define GMSHCOMPOUNDMANAGER_HH

#include <string>
#include <array>
#include <vector>
#include <list>
#include <map>

#include "Gmsh.h"
#include "GModel.h"
#include "MLine.h"
#include "MTriangle.h"

// mesh algorithms type
enum GmshAlgorithmType {automatic=2,delaunay=5,frontal=6,meshadapt=1};

// base class
template<unsigned int dim,typename Imp>
class GMSHCompoundManagerBase
{
  public:
  inline void create()
  {
    // load domain
    domain()=new GModel();
    domain()->setFactory("Gmsh");
    domain()->readGEO(domainfilename_);
    // load interface
    interface()=new GModel();
    interface()->setFactory("Gmsh");
    std::size_t found(interfacefilename_.find(".geo"));
    if(found!=std::string::npos)
      interface()->readGEO(interfacefilename_);
    else
    {
      interface()->readMSH(interfacefilename_);
      imp().interfaceMesh2GModel(interface());
    }
    // load hole (if present)
    hole()=new GModel();
    hole()->setFactory("Gmsh");
    if(holefilename_!="")
    {
      hole()->readGEO(holefilename_);
      hashole_=true;
    }
    imp().createCompoundGeo();
    compound()->mesh(worlddim);
  }

  protected:
  typedef Imp Implementation;

  GMSHCompoundManagerBase(int argc,char** argv,const std::string& domainFileName,const std::string& interfaceFileName,
                          const std::string& holeFileName,const GmshAlgorithmType& algorithm,const bool& verbosity):
    domainfilename_(domainFileName),interfacefilename_(interfaceFileName),holefilename_(holeFileName),gmodelptrs_(),hashole_(false)
  {
    // init gmsh
    GmshInitialize(argc,argv);
    GmshSetOption("General","Terminal",1.);
    if(verbosity)
      GmshSetOption("General","Verbosity",99.);
    GmshSetOption("Mesh","Algorithm",static_cast<double>(algorithm));
  }

  ~GMSHCompoundManagerBase()
  {
    for(auto& model:gmodelptrs_)
      delete model;
    GmshFinalize();
  }

  inline Implementation& imp()
  {
    return static_cast<Implementation&>(*this);
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

  public:
  inline void writeInterfaceGeo(const std::string& fileName="interface.geo")
  {
    interface()->writeGEO(fileName,true,false);
  }
  inline void writeInterfaceMsh(const std::string& fileName="interface.msh")
  {
    interface()->writeMSH(fileName,2.2,false,false);
  }
  inline void writeCompoundGeo(const std::string& fileName="compound.geo")
  {
    compound()->writeGEO(fileName,true,false);
  }
  inline void writeCompoundMsh(const std::string& fileName="compound.msh")
  {
    compound()->writeMSH(fileName,2.2,false,false);
  }

  static constexpr unsigned int worlddim=dim;

  private:
  const std::string& domainfilename_;
  const std::string& interfacefilename_;
  const std::string& holefilename_;
  std::array<GModel*,4> gmodelptrs_;
  bool hashole_;
};

// different specialization according to the dimension
template<unsigned int dim>
class GMSHCompoundManager;

// specialization for worlddim = 2
template<>
class GMSHCompoundManager<2>:public GMSHCompoundManagerBase<2,GMSHCompoundManager<2>>
{

  friend GMSHCompoundManagerBase<2,GMSHCompoundManager<2>>;

  public:
  template<typename... Args>
  inline GMSHCompoundManager(Args... args):GMSHCompoundManagerBase(args...)
  {}

  private:
  void createCompoundGeo()
  {
    if(compound()!=nullptr)
      delete compound();
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
    if(hasHole())
      addGModelToCompound(hole(),holeEdges);
    // add line loops and faces to compound gmodel
    std::vector<std::vector<GEdge*>> outerLineLoop({domainEdges,interfaceEdges});
    (compound()->addPlanarFace(outerLineLoop))->addPhysicalEntity(2);
    std::vector<std::vector<GEdge*>> innerLineLoop({interfaceEdges});
    if(hasHole())
      innerLineLoop.push_back(holeEdges);
    (compound()->addPlanarFace(innerLineLoop))->addPhysicalEntity(1);
  }

  void addGModelToCompound(GModel*& model,std::vector<GEdge*>& edges)
  {
    unsigned int vtxCounter(0);
    std::vector<GVertex*> vertices(0);
    std::array<GVertex*,worlddim> vtxPtr;
    std::vector<int> verticesMap(model->getNumVertices()+1,-1);
    for(typename GModel::eiter it=model->firstEdge();it!=model->lastEdge();++it)
    {
      // get edge physical ID
      const int physicalID(((*it)->getPhysicalEntities())[0]);
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
      edges.back()->addPhysicalEntity(physicalID);
    }
  }

  void interfaceMesh2GModel(GModel*& model)
  {
    // create new gmodel
    GModel* newGModel(new GModel());
    newGModel->setFactory("Gmsh");
    // index all the mesh vertices in a continuous sequence starting at 1
    model->indexMeshVertices(true,0,true);
    unsigned int vtxCounter(0);
    std::vector<GVertex*> vertices(0);
    std::vector<int> verticesMap(model->getMaxVertexNumber()+1,0);
    constexpr double charlenght(1000);
    // add vertices
    for(std::size_t i=1;i!=verticesMap.size();++i)
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
    std::array<unsigned int,worlddim> posVtx;
    constexpr int physicalID(1);
    for(typename GModel::eiter edgeIt=model->firstEdge();edgeIt!=model->lastEdge();++edgeIt)
    {
      for(std::size_t i=0;i!=(*edgeIt)->lines.size();++i)
      {
        MLine* linePtr((*edgeIt)->lines[i]);
        posVtx[0]=verticesMap[linePtr->getVertex(0)->getNum()];
        posVtx[1]=verticesMap[linePtr->getVertex(1)->getNum()];
        (newGModel->addLine(vertices[posVtx[0]],vertices[posVtx[1]]))->addPhysicalEntity(physicalID);
      }
    }
    // free old mesh model and assign new gmodel
    delete model;
    model=newGModel;
  }
};

// specialization for worlddim = 3
template<>
class GMSHCompoundManager<3>:public GMSHCompoundManagerBase<3,GMSHCompoundManager<3>>
{
  friend GMSHCompoundManagerBase<3,GMSHCompoundManager<3>>;

  public:
  template<typename... Args>
  inline GMSHCompoundManager(Args... args):GMSHCompoundManagerBase(args...)
  {}

  private:
  void createCompoundGeo()
  {
    if(compound()!=nullptr)
      delete compound();
    compound()=new GModel();
    compound()->setFactory("Gmsh");
    // add domain to compound gmodel
    std::vector<GFace*> domainFaces(0);
    addGModelToCompound(domain(),domainFaces);
    // add interface to compound gmodel
    std::vector<GFace*> interfaceFaces(0);
    addGModelToCompound(interface(),interfaceFaces);
    // add hole to compound gmodel (if present)
    std::vector<GFace*> holeFaces(0);
    if(hasHole())
      addGModelToCompound(hole(),holeFaces);
    // add surface loops and volumes to compound gmodel
    std::vector<std::vector<GFace*>> outerSurfaceLoop({domainFaces,interfaceFaces});
    (compound()->addVolume(outerSurfaceLoop))->addPhysicalEntity(2);
    std::vector<std::vector<GFace*>> innerSurfaceLoop({interfaceFaces});
    if(hasHole())
      innerSurfaceLoop.push_back(holeFaces);
    (compound()->addVolume(innerSurfaceLoop))->addPhysicalEntity(1);
  }

  void addGModelToCompound(GModel*& model,std::vector<GFace*>& faces)
  {
    std::vector<GVertex*> vertices(0);
    std::array<GVertex*,2> vtxPtr({nullptr,nullptr});
    std::vector<int> verticesMap(model->getNumVertices()+1,-1);
    unsigned int vtxCounter(0);
    std::map<int,GEdge*> edgesMap;
    // loop over faces
    for(typename GModel::fiter faceIt=model->firstFace();faceIt!=model->lastFace();++faceIt)
    {
      // get face physical ID
      const int physicalID(((*faceIt)->getPhysicalEntities())[0]);
      std::list<GEdge*> edgesList((*faceIt)->edges());
      std::vector<GEdge*> edges(edgesList.size(),0);
      unsigned int edgeCounter(0);
      // loop over edges
      for(auto& edge:edgesList)
      {
        typename std::map<int,GEdge*>::iterator edgeMapIt(edgesMap.find(edge->tag()));
        if(edgeMapIt==edgesMap.end())
        {
          // add first vertex
          vtxPtr[0]=edge->getBeginVertex();
          if(verticesMap[vtxPtr[0]->tag()]==-1)
          {
            vertices.push_back(compound()->addVertex(vtxPtr[0]->x(),vtxPtr[0]->y(),vtxPtr[0]->z(),vtxPtr[0]->prescribedMeshSizeAtVertex()));
            verticesMap[vtxPtr[0]->tag()]=vtxCounter;
            ++vtxCounter;
          }
          vtxPtr[0]=vertices[verticesMap[vtxPtr[0]->tag()]];
          // add last vertex
          vtxPtr[1]=edge->getEndVertex();
          if(verticesMap[vtxPtr[1]->tag()]==-1)
          {
            vertices.push_back(compound()->addVertex(vtxPtr[1]->x(),vtxPtr[1]->y(),vtxPtr[1]->z(),vtxPtr[1]->prescribedMeshSizeAtVertex()));
            verticesMap[vtxPtr[1]->tag()]=vtxCounter;
            ++vtxCounter;
          }
          vtxPtr[1]=vertices[verticesMap[vtxPtr[1]->tag()]];
          // add edge
          edges[edgeCounter]=compound()->addLine(vtxPtr[0],vtxPtr[1]);
          edgesMap.insert(std::pair<int,GEdge*>(edge->tag(),edges[edgeCounter]));
        }
        else
          edges[edgeCounter]=edgeMapIt->second;
        ++edgeCounter;
      }
      // add lineloop
      std::vector<std::vector<GEdge*>> lineLoop({edges});
      faces.push_back(compound()->addPlanarFace(lineLoop));
      faces.back()->addPhysicalEntity(physicalID);
    }
  }

  void interfaceMesh2GModel(GModel*& model)
  {
    // create new gmodel
    GModel* newGModel(new GModel());
    newGModel->setFactory("Gmsh");
    // index all the mesh vertices in a continuous sequence starting at 1
    model->indexMeshVertices(true,0,true);
    unsigned int vtxCounter(0);
    std::vector<GVertex*> vertices(0);
    typedef std::list<GEdge*> EdgeList;
    std::vector<std::pair<int,EdgeList>> verticesMap(model->getMaxVertexNumber()+1,std::make_pair(0,EdgeList()));
    constexpr double charlenght(1000);
    // add vertices
    for(std::size_t i=1;i!=verticesMap.size();++i)
    {
      MVertex* vtxPtr(model->getMeshVertexByTag(i));
      if(vtxPtr!=nullptr)
      {
        if(vtxPtr->getIndex()>(-1))
        {
          vertices.push_back(newGModel->addVertex(vtxPtr->x(),vtxPtr->y(),vtxPtr->z(),charlenght));
          verticesMap[i].first=vtxCounter;
          ++vtxCounter;
        }
      }
    }
    // add simplices
    std::array<unsigned int,worlddim> idVtx;
    constexpr int physicalID(1);
    std::vector<GEdge*> simplexEdges(worlddim,nullptr);
    std::map<int,GEdge*> edgesMap;
    for(typename GModel::fiter faceIt=model->firstFace();faceIt!=model->lastFace();++faceIt)
    {
      for(std::size_t i=0;i!=(*faceIt)->triangles.size();++i)
      {
        MTriangle* simplexPtr((*faceIt)->triangles[i]);
        // loop over edges
        for(std::size_t l=0;l!=worlddim;++l)
        {
          // get index of the 2 vertices
          for(std::size_t k=0;k!=2;++k)
            idVtx[k]=simplexPtr->getVertex((l+k)%worlddim)->getNum();
          // check if the edge has already been inserted
          typedef typename EdgeList::iterator EdgeListIter;
          EdgeListIter edge0It(verticesMap[idVtx[0]].second.begin());
          const EdgeListIter edge0ItEnd(verticesMap[idVtx[0]].second.end());
          const EdgeListIter edge1ItEnd(verticesMap[idVtx[1]].second.end());
          bool found(false);
          std::cout<<"Entering"<<std::endl;
          while((edge0It!=edge0ItEnd)&&(!found))
          {
            for(EdgeListIter edge1It=verticesMap[idVtx[1]].second.begin();(edge1It!=edge1ItEnd)&&(!found);++edge1It)
              if((*edge0It)->tag()==(*edge1It)->tag())
                found=true;
            if(!found)
              ++edge0It;
          }
          std::cout<<"Leaving"<<std::endl;
          // use the already inserted edge or create the new one
          if(found)
            simplexEdges[l]=*edge0It;
          else
          {
            simplexEdges[l]=newGModel->addLine(vertices[verticesMap[idVtx[0]].first],vertices[verticesMap[idVtx[1]].first]);
            verticesMap[idVtx[0]].second.push_back(simplexEdges[l]);
            verticesMap[idVtx[1]].second.push_back(simplexEdges[l]);
          }
        }
        std::vector<std::vector<GEdge*>> edgeLoop({simplexEdges});
        (newGModel->addPlanarFace(edgeLoop))->addPhysicalEntity(physicalID);
      }
    }
    // free old mesh model and assign new gmodel
    delete model;
    model=newGModel;
  }
};

#endif //GMSHCOMPOUNDMANAGER_HH
