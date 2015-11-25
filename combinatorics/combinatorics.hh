#ifndef COMBINATORICS_HH
#define COMBINATORICS_HH

#include <algorithm>
#include <iostream>
#include <utility>

template<typename T>
void permutations(T v,std::size_t l=0)
{
  const auto size(v.size());
  if(l==size)
  {
    for(const auto& value:v)
      std::cout<<value<<" ";
    std::cout<<std::endl;
  }
  else
  {
    for(auto i=l;i!=size;++i)
    {
      std::swap(v[l],v[i]);
      permutations(v,l+1);
      std::swap(v[l],v[i]);
    }
  }
}

template<typename T>
void combinations(T v,std::size_t n,std::size_t l=0)
{
  if(l==n)
  {
    for(std::size_t i=0;i!=n;++i)
      std::cout<<v[i]<<" ";
    std::cout<<std::endl;
  }
  else
  {
    const auto size(v.size());
    for(auto i=l;i!=size;++i)
    {
      auto vnew(v);
      std::copy_n(v.begin()+i,size-i,vnew.begin()+l);
      combinations(vnew,n,l+1);
    }
  }
}

#endif // COMBINATORICS_HH
