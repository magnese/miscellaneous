// Converter from Gmsh mesh file format to Alberta mesh file format
// Author: Marco Agnese

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

enum FlagType{garbageFlag,numVerticesFlag,verticesFlag,numElementsFlag,elementsFlag};

int main(int argc,char** argv)
{
  // input information
  std::string inputFileName;
  if(argc>1)
  {
    inputFileName=argv[1];
    std::cout<<"Mehs file name : "<<inputFileName<<std::endl;
  }
  else
  {
    std::cout<<"Mehs file name : ";
    std::cin>>inputFileName;
  }
  unsigned int worldDim;
  if(argc>2)
  {
    worldDim=static_cast<unsigned int>(strtod(argv[2],NULL));
    std::cout<<"World dimension : "<<worldDim<<std::endl;
  }
  else
  {
    std::cout<<"World dimension : ";
    std::cin>>worldDim;
  }
  unsigned int gridDim;
  if(argc>3)
  {
    gridDim=static_cast<unsigned int>(strtod(argv[3],NULL));
    std::cout<<"Grid dimension : "<<gridDim<<std::endl;
  }
  else
  {
    std::cout<<"Grid dimension : ";
    std::cin>>gridDim;
  }
  std::cout<<std::endl;

  // read file
  bool inputFileOpened(false);
  std::size_t numVertices(0);
  std::vector<std::vector<double>> vertices(0);
  std::size_t verticesCounter(0);
  std::size_t numElements(0);
  std::vector<std::vector<unsigned int>> elements(0);
  std::size_t elementsCounter(0);
  std::ifstream inputFile(inputFileName);
  if(inputFile.is_open())
  {
    inputFileOpened=true;
    FlagType flag(garbageFlag);
    for(std::string line;std::getline(inputFile,line);)
    {
      if(line=="$Nodes")
        flag=numVerticesFlag;
      else if(line=="$EndNodes")
        flag=garbageFlag;
      else if(line=="$Elements")
        flag=numElementsFlag;
      else if(line=="$EndElements")
      {
        flag=garbageFlag;
        elements.resize(elementsCounter);
        numElements=elementsCounter;
      }
      else
      {
        if(flag==verticesFlag)
        {
          unsigned int garbage;
          std::istringstream iss(line);
          iss>>garbage; // discard node id
          iss>>vertices[verticesCounter][0]>>vertices[verticesCounter][1]>>vertices[verticesCounter][2];
          ++verticesCounter;
        }
        if(flag==numVerticesFlag)
        {
          std::istringstream iss(line);
          iss>>numVertices;
          vertices.resize(numVertices,std::vector<double>(3,0.0));
          flag=verticesFlag;
        }
        if(flag==elementsFlag)
        {
          unsigned int garbage;
          std::istringstream iss(line);
          iss>>garbage>>garbage; // discard element id and element type
          if(garbage==2)
          {
            iss>>garbage>>garbage>>garbage; // discard number of flags (=2) and flags
            iss>>elements[elementsCounter][0]>>elements[elementsCounter][1]>>elements[elementsCounter][2];
            ++elementsCounter;
          }
        }
        if(flag==numElementsFlag)
        {
          std::istringstream iss(line);
          iss>>numElements;
          elements.resize(numElements,std::vector<unsigned int>(3,0));
          flag=elementsFlag;
        }
      }
    }
    inputFile.close();
  }
  else
    std::cout<<"Impossible to open file "<<inputFileName<<"."<<std::endl;

  // write output
  std::string outputFileName(inputFileName);
  outputFileName.resize(outputFileName.size()-4);
  outputFileName+=".apmc";
  std::ofstream outputFile(outputFileName);
  if(outputFile.is_open()&&inputFileOpened)
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
      outputFile<<"\t"<<(*it)[0]-1<<" "<<(*it)[1]-1<<" "<<(*it)[2]-1<<std::endl;
    outputFile.close();
    std::cout<<"Mesh converted in file "<<outputFileName<<"."<<std::endl;
  }
  else
    std::cout<<"Impossible to create file "<<outputFileName<<"."<<std::endl;

  return 0;
}
