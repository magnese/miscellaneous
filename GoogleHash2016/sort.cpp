#include <thread>
#include <vector>
#include <tuple>

#include "functions.hh"

std::vector<unsigned int> SortVector(const std::vector< unsigned int >& values)
{
  std::vector<std::tuple<unsigned int, unsigned int>> indicesWithWeights;
  for (unsigned int j = 0; j < values.size(); ++j)
    indicesWithWeights.push_back(std::tuple<unsigned int, unsigned int>(values[j], j));
  ApplyQuickSort(indicesWithWeights, 0, indicesWithWeights.size() - 1);
  std::vector<unsigned int> indices;
  for (unsigned int j = 0; j < indicesWithWeights.size(); ++j)
    indices.push_back(std::get<1>(indicesWithWeights[j]));
  return indices;
}

long long Partition(std::vector<std::tuple<unsigned int, unsigned int>>& in, long long lo, long long hi)
{
  // take last element to be the pivot
  auto pivot = std::get<0>(in[hi]);
  long long ind = lo;
  for (long long j = lo; j < hi; ++j)
    if (std::get<0>(in[j]) <= pivot)
    {
      std::swap(in[j], in[ind]);
      ++ind;
    }
  std::swap(in[ind], in[hi]);
  return ind;
}

void ApplyQuickSort(std::vector<std::tuple<unsigned int, unsigned int>>& in, long long lo, long long hi)
{
  if (lo < hi) // otherwise no sorting makes sense, probably throw an exception?
  {
    long long pivotInd = Partition(in, lo, hi);
    // now that pivotInd is in the correct place, partition either side
    ApplyQuickSort(in, lo, pivotInd - 1);
    ApplyQuickSort(in, pivotInd + 1, hi);
  }
}

void ApplyQuickSortMT(std::vector<std::tuple<unsigned int, unsigned int>>& in, long long lo, long long hi)
{
  if (lo < hi) // otherwise no sorting makes sense, probably throw an exception?
  {
    long long pivotInd = Partition(in, lo, hi);
    // now that pivotInd is in the correct place, partition either side
    std::thread t1(ApplyQuickSort, std::ref(in), lo, pivotInd - 1);
    std::thread t2(ApplyQuickSort, std::ref(in), pivotInd + 1, hi);
    t1.join();
    t2.join();
  }
}
