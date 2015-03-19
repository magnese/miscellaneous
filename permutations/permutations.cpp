#include <vector>

#include "permutations.hh"

int main()
{
  typedef std::vector<short unsigned int> VectorType;
  constexpr std::size_t n(3);
  VectorType v(n,0);
  for(std::size_t i=0;i!=n;++i)
    v[i]=i+1;
  permutation(v,0);

  return 0;
}
