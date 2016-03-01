#include <array>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>

template<typename M,typename S=std::ostream>
void print(const M& mapping,S& s=std::cout)
{
  for(const auto& entry:mapping)
  {
    for(const auto& value:entry)
      s<<value<<" ";
    s<<std::endl;
  }
}

int main()
{
  // allocate vector
  std::array<unsigned int,3> size{5,5,5};
  std::vector<std::array<unsigned int,3>> mapping(size[0]*size[1]*size[2],{0,0,0});
  // compute continuous mapping
  for(unsigned int k=0;k!=size[2];++k)
    for(unsigned int j=0;j!=size[1];++j)
      for(unsigned int i=0;i!=size[0];++i)
      {
        auto pos(k*size[0]*size[1]);
        if(k%2==0)
        {
          pos+=((j%2==0)?(j*size[0]+i):((j+1)*size[0]-i-1));
        }
        else
        {
          pos+=size[0]*size[1]-1;
          pos-=((j%2==0)?(j*size[0]+i):((j+1)*size[0]-i-1));
        }
        if(pos>=mapping.size())
          throw;
        mapping[pos]={i,j,k};
      }
  // dump path
  std::ofstream file("path.dat");
  print(mapping,file);
  system("./plot.sh");
  return 0;
}
