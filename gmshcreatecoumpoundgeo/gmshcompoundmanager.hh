#ifndef GMSHCOMPOUNDMANAGER_HH
#define GMSHCOMPOUNDMANAGER_HH

#include <string>
#include <array>
#include <vector>
#include <list>
#include <map>
#include <functional>

#include "Gmsh.h"
#include "GModel.h"
#include "MLine.h"
#include "MTriangle.h"

// mesh algorithms type
enum GmshAlgorithmType {automatic=2,delaunay=5,frontal=6,meshadapt=1};

// fixed charlength policy
struct FixedCharlength
{
  template<typename... Args>
  inline FixedCharlength(const Args&... )
  {}

  template<typename... Args>
  inline double operator()(const GVertex& vtx,const Args&...) const
  {
    return vtx.prescribedMeshSizeAtVertex();
  }
};

// base class
template<unsigned int dim,typename CharlengthPolicyImp,typename Imp>
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
    if(interfacefilename_.find(".geo")!=std::string::npos)
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
    imp().createCompoundGeo(FixedCharlength());
    compound()->mesh(worlddim);
  }

  protected:
  typedef Imp Implementation;
  typedef CharlengthPolicyImp CharlengthPolicyType;

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
template<unsigned int dim,typename CharlengthPolicyType=FixedCharlength>
class GMSHCompoundManager;

// specialization for worlddim = 2
template<typename CharlengthPolicyType>
class GMSHCompoundManager<2,CharlengthPolicyType>:
  public GMSHCompoundManagerBase<2,CharlengthPolicyType,GMSHCompoundManager<2,CharlengthPolicyType>>
{

  friend GMSHCompoundManagerBase<2,CharlengthPolicyType,GMSHCompoundManager<2,CharlengthPolicyType>>;

  typedef GMSHCompoundManagerBase<2,CharlengthPolicyType,GMSHCompoundManager<2,CharlengthPolicyType>> BaseType;
  using BaseType::compound;
  using BaseType::interface;
  using BaseType::domain;
  using BaseType::hole;
  using BaseType::hasHole;
  using BaseType::worlddim;

  public:
  template<typename... Args>
  inline GMSHCompoundManager(Args... args):BaseType(args...)
  {}

  private:
  template<typename... Args>
  void createCompoundGeo(const Args&... args)
  {
    if(compound()!=nullptr)
      delete compound();
    compound()=new GModel();
    compound()->setFactory("Gmsh");
    // add domain to compound gmodel
    std::vector<GEdge*> domainEdges(0);
    addGModelToCompound(domain(),domainEdges,args...);
    // add interface to compound gmodel
    std::vector<GEdge*> interfaceEdges(0);
    addGModelToCompound(interface(),interfaceEdges,FixedCharlength());
    // add hole to compound gmodel (if present)
    std::vector<GEdge*> holeEdges(0);
    if(hasHole())
      addGModelToCompound(hole(),holeEdges,args...);
    // add line loops and faces to compound gmodel
    std::vector<std::vector<GEdge*>> outerLineLoop({domainEdges,interfaceEdges});
    (compound()->addPlanarFace(outerLineLoop))->addPhysicalEntity(2);
    std::vector<std::vector<GEdge*>> innerLineLoop({interfaceEdges});
    if(hasHole())
      innerLineLoop.push_back(holeEdges);
    (compound()->addPlanarFace(innerLineLoop))->addPhysicalEntity(1);
  }

  void addGModelToCompound(GModel*& model,std::vector<GEdge*>& edges,const std::function<double(const GVertex&)>& charlength)
  {
    long int vtxCounter(0);
    std::vector<GVertex*> vertices(0);
    std::array<GVertex*,worlddim> vtxPtr;
    std::vector<long int> verticesMap(model->getNumVertices()+1,-1);
    for(auto it=model->firstEdge();it!=model->lastEdge();++it)
    {
      // get edge physical ID
      const auto physicalID(((*it)->getPhysicalEntities())[0]);
      // add first vertex
      vtxPtr[0]=(*it)->getBeginVertex();
      if(verticesMap[vtxPtr[0]->tag()]==-1)
      {
        vertices.push_back(compound()->addVertex(vtxPtr[0]->x(),vtxPtr[0]->y(),vtxPtr[0]->z(),charlength(*(vtxPtr[0]))));
        verticesMap[vtxPtr[0]->tag()]=vtxCounter;
        ++vtxCounter;
      }
      vtxPtr[0]=vertices[verticesMap[vtxPtr[0]->tag()]];
      // add last vertex
      vtxPtr[1]=(*it)->getEndVertex();
      if(verticesMap[vtxPtr[1]->tag()]==-1)
      {
        vertices.push_back(compound()->addVertex(vtxPtr[1]->x(),vtxPtr[1]->y(),vtxPtr[1]->z(),charlength(*(vtxPtr[1]))));
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
    long int vtxCounter(0);
    std::vector<GVertex*> vertices(0);
    std::vector<long int> verticesMap(model->getMaxVertexNumber()+1,0);
    constexpr double charlength(1000);
    // add vertices
    for(decltype(verticesMap.size()) i=1;i!=verticesMap.size();++i)
    {
      auto vtxPtr(model->getMeshVertexByTag(i));
      if(vtxPtr!=nullptr)
      {
        if(vtxPtr->getIndex()>(-1))
        {
          vertices.push_back(newGModel->addVertex(vtxPtr->x(),vtxPtr->y(),vtxPtr->z(),charlength));
          verticesMap[i]=vtxCounter;
          ++vtxCounter;
        }
      }
    }
    // add edges
    std::array<long int,worlddim> posVtx;
    constexpr int physicalID(1);
    for(auto edgeIt=model->firstEdge();edgeIt!=model->lastEdge();++edgeIt)
    {
      for(decltype((*edgeIt)->lines.size()) i=0;i!=(*edgeIt)->lines.size();++i)
      {
        auto linePtr((*edgeIt)->lines[i]);
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
template<typename CharlengthPolicyType>
class GMSHCompoundManager<3,CharlengthPolicyType>:
  public GMSHCompoundManagerBase<3,CharlengthPolicyType,GMSHCompoundManager<3,CharlengthPolicyType>>
{
  friend GMSHCompoundManagerBase<3,CharlengthPolicyType,GMSHCompoundManager<3,CharlengthPolicyType>>;

  typedef GMSHCompoundManagerBase<3,CharlengthPolicyType,GMSHCompoundManager<3,CharlengthPolicyType>> BaseType;
  using BaseType::compound;
  using BaseType::interface;
  using BaseType::domain;
  using BaseType::hole;
  using BaseType::hasHole;
  using BaseType::worlddim;

  public:
  template<typename... Args>
  inline GMSHCompoundManager(Args... args):BaseType(args...)
  {}

  private:
  template<typename... Args>
  void createCompoundGeo(const Args&... args)
  {
    if(compound()!=nullptr)
      delete compound();
    compound()=new GModel();
    compound()->setFactory("Gmsh");
    // add domain to compound gmodel
    std::vector<GFace*> domainFaces(0);
    addGModelToCompound(domain(),domainFaces,args...);
    // add interface to compound gmodel
    std::vector<GFace*> interfaceFaces(0);
    addGModelToCompound(interface(),interfaceFaces,FixedCharlength());
    // add hole to compound gmodel (if present)
    std::vector<GFace*> holeFaces(0);
    if(hasHole())
      addGModelToCompound(hole(),holeFaces,args...);
    // add surface loops and volumes to compound gmodel
    std::vector<std::vector<GFace*>> outerSurfaceLoop({domainFaces,interfaceFaces});
    (compound()->addVolume(outerSurfaceLoop))->addPhysicalEntity(2);
    std::vector<std::vector<GFace*>> innerSurfaceLoop({interfaceFaces});
    if(hasHole())
      innerSurfaceLoop.push_back(holeFaces);
    (compound()->addVolume(innerSurfaceLoop))->addPhysicalEntity(1);
  }

  void addGModelToCompound(GModel*& model,std::vector<GFace*>& faces,const std::function<double(const GVertex&)>& charlength)
  {
    std::vector<GVertex*> vertices(0);
    std::array<GVertex*,2> vtxPtr({nullptr,nullptr});
    std::vector<long int> verticesMap(model->getNumVertices()+1,-1);
    long int vtxCounter(0);
    std::map<long int,GEdge*> edgesMap;
    // loop over faces
    for(auto faceIt=model->firstFace();faceIt!=model->lastFace();++faceIt)
    {
      // get face physical ID
      const auto physicalID(((*faceIt)->getPhysicalEntities())[0]);
      auto edgesList((*faceIt)->edges());
      std::vector<GEdge*> edges(edgesList.size(),0);
      long int edgeCounter(0);
      // loop over edges
      for(auto& edge:edgesList)
      {
        auto edgeMapIt(edgesMap.find(edge->tag()));
        if(edgeMapIt==edgesMap.end())
        {
          // add first vertex
          vtxPtr[0]=edge->getBeginVertex();
          if(verticesMap[vtxPtr[0]->tag()]==-1)
          {
            vertices.push_back(compound()->addVertex(vtxPtr[0]->x(),vtxPtr[0]->y(),vtxPtr[0]->z(),charlength(*(vtxPtr[0]))));
            verticesMap[vtxPtr[0]->tag()]=vtxCounter;
            ++vtxCounter;
          }
          vtxPtr[0]=vertices[verticesMap[vtxPtr[0]->tag()]];
          // add last vertex
          vtxPtr[1]=edge->getEndVertex();
          if(verticesMap[vtxPtr[1]->tag()]==-1)
          {
            vertices.push_back(compound()->addVertex(vtxPtr[1]->x(),vtxPtr[1]->y(),vtxPtr[1]->z(),charlength(*(vtxPtr[1]))));
            verticesMap[vtxPtr[1]->tag()]=vtxCounter;
            ++vtxCounter;
          }
          vtxPtr[1]=vertices[verticesMap[vtxPtr[1]->tag()]];
          // add edge
          edges[edgeCounter]=compound()->addLine(vtxPtr[0],vtxPtr[1]);
          edgesMap.emplace(edge->tag(),edges[edgeCounter]);
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
    long int vtxCounter(0);
    std::vector<GVertex*> vertices(0);
    typedef std::list<GEdge*> EdgeList;
    std::vector<std::pair<long int,EdgeList>> verticesMap(model->getMaxVertexNumber()+1,std::make_pair(0,EdgeList()));
    constexpr double charlength(1000);
    // add vertices
    for(decltype(verticesMap.size()) i=1;i!=verticesMap.size();++i)
    {
      auto vtxPtr(model->getMeshVertexByTag(i));
      if(vtxPtr!=nullptr)
      {
        if(vtxPtr->getIndex()>(-1))
        {
          vertices.push_back(newGModel->addVertex(vtxPtr->x(),vtxPtr->y(),vtxPtr->z(),charlength));
          verticesMap[i].first=vtxCounter;
          ++vtxCounter;
        }
      }
    }
    // add simplices
    std::array<long int,worlddim> idVtx;
    constexpr int physicalID(1);
    std::vector<GEdge*> simplexEdges(worlddim,nullptr);
    std::map<long int,GEdge*> edgesMap;
    for(auto faceIt=model->firstFace();faceIt!=model->lastFace();++faceIt)
    {
      for(decltype((*faceIt)->triangles.size()) i=0;i!=(*faceIt)->triangles.size();++i)
      {
        auto simplexPtr((*faceIt)->triangles[i]);
        // loop over edges
        for(auto l=0;l!=worlddim;++l)
        {
          // get index of the 2 vertices
          for(auto k=0;k!=2;++k)
            idVtx[k]=simplexPtr->getVertex((l+k)%worlddim)->getNum();
          // check if the edge has already been inserted
          auto edge0It(verticesMap[idVtx[0]].second.begin());
          const auto edge0ItEnd(verticesMap[idVtx[0]].second.end());
          const auto edge1ItEnd(verticesMap[idVtx[1]].second.end());
          bool found(false);
          while((edge0It!=edge0ItEnd)&&(!found))
          {
            for(auto edge1It=verticesMap[idVtx[1]].second.begin();(edge1It!=edge1ItEnd)&&(!found);++edge1It)
              if((*edge0It)->tag()==(*edge1It)->tag())
                found=true;
            if(!found)
              ++edge0It;
          }
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
