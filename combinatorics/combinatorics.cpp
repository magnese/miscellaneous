#include <iostream>
#include <vector>

#include "combinatorics.hh"

int main()
{
  // fill vector
  std::vector<short unsigned int> v(5,0);
  short unsigned int val(1);
  for(auto& entry:v)
  {
    entry=val;
    ++val;
  }

  // permutations
  std::cout<<"Permuations :"<<std::endl;
  permutations(v);

  // combinations
  std::cout<<"Combinations :"<<std::endl;
  combinations(v,3);

  return 0;
}
