#ifndef PERMUTATIONS_HH
#define PERMUTATIONS_HH

#include <iostream>
#include <utility>

template<typename T>
void printVector(const T& v)
{
  for(const auto& value: v)
    std::cout<<value<<" ";
  std::cout<<std::endl;
}

template<typename T>
void permutation(T v,std::size_t i)
{
  const std::size_t size(v.size());
  if(i==size)
    printVector(v);
  else
  {
    for(std::size_t j=i;j!=size;++j)
    {
      std::swap(v[i],v[j]);
      permutation(v,i+1);
      std::swap(v[i],v[j]);
    }
  }
}

#endif // PERMUTATIONS_HH
