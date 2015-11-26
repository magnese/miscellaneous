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
void combinations(T pool,std::size_t n,T comb={})
{
  if(n==0)
  {
    for(const auto& value:comb)
      std::cout<<value<<" ";
    std::cout<<std::endl;
  }
  else
  {
    for(auto it=pool.begin();it!=(pool.end()-n+1);++it)
    {
      comb.push_back(*it);
      T newpool(pool.end()-it-1);
      std::copy(it+1,pool.end(),newpool.begin());
      combinations(newpool,n-1,comb);
      comb.pop_back();
    }
  }
}

#endif // COMBINATORICS_HH
