#ifndef PERMUTATIONS_HH
#define PERMUTATIONS_HH

#include <iostream>
#include <utility>

template<typename T>
void permutation(T v,std::size_t i=0)
{
  const auto size(v.size());
  if(i==size)
  {
    for(const auto& value: v)
      std::cout<<value<<" ";
    std::cout<<std::endl;
  }
  else
  {
    for(auto j=i;j!=size;++j)
    {
      std::swap(v[i],v[j]);
      permutation(v,i+1);
      std::swap(v[i],v[j]);
    }
  }
}

#endif // PERMUTATIONS_HH
