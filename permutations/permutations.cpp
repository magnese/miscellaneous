#include <vector>

#include "permutations.hh"

int main()
{
  std::vector<short unsigned int> v(3,0);
  short unsigned int val(1);
  for(auto& entry:v)
  {
    entry=val;
    ++val;
  }
  permutation(v);

  return 0;
}
