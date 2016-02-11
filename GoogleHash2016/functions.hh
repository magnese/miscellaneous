#ifndef FUNCTIONS_HH
#define FUNCTIONS_HH

#include <array>
#include <list>
#include <math.h>
#include <vector>
#include <tuple>

enum DroneCommand {LOAD, UNLOAD, WAIT, DELIVER};

extern unsigned int totalCommands;
extern std::vector<std::vector<unsigned int>> orders;
extern std::vector<std::vector<unsigned int>> warehouses;

void OutputCommands(const std::string& filename);
void GenerateCommands();
std::vector<unsigned int> generate_weights(std::vector<std::vector<unsigned int>>& orders,
                                           std::vector<std::vector<unsigned int>>& warehouses,
                                           std::vector<unsigned int>& product_weights);

std::vector<unsigned int> SortVector(const std::vector< unsigned int >& values);
long long Partition(std::vector<std::tuple<unsigned int, unsigned int>>& in, long long lo, long long hi);
void ApplyQuickSort(std::vector<std::tuple<unsigned int, unsigned int>>& in, long long lo, long long hi);
void ApplyQuickSortMT(std::vector<std::tuple<unsigned int, unsigned int>>& in, long long lo, long long hi);

using namespace std;

struct Warehouse
{
  Warehouse(const vector<unsigned int>& warehouseData) :x(warehouseData[0]), y(warehouseData[1]) {
    auto first = warehouseData.begin() + 2;
    auto last = warehouseData.end();
    currentStock = vector<unsigned int>(first, last);
  }
  void Reserve(unsigned int prodIdx, unsigned int count) {
    currentStock[prodIdx] -= count;
  }
  unsigned int x, y;
  vector<unsigned int> currentStock;
};

struct Drone
{
  Drone():currentTime(0), X(warehouses[0][0]),Y(warehouses[0][1]) {};
  ~Drone(){};
  bool isAvailable(unsigned int currentT) { return(currentT >= currentTime); };
  void AddCommand(DroneCommand comm, unsigned int val1, unsigned int val2, unsigned int val3) {
    commands.push_back({ static_cast<unsigned int>(comm),val1,val2,val3 });
    ++totalCommands;
    double x_dist;
    double y_dist;
    unsigned int elapsedTime;
    switch (comm) {
    case DroneCommand::DELIVER:
      x_dist = static_cast< double >(X) - static_cast< double >(orders[val1][1]); // get X
      y_dist = static_cast< double >(Y) - static_cast< double >(orders[val1][2]); // get Y
      elapsedTime = static_cast< unsigned int >(sqrt(x_dist * x_dist + y_dist * y_dist) + 0.99999999) + 1;
      currentTime += elapsedTime;
      break;
    case DroneCommand::LOAD:
      // go to warehouse val1
      x_dist = static_cast< double >(X) - static_cast< double >(warehouses[val1][0]);
      y_dist = static_cast< double >(Y) - static_cast< double >(warehouses[val1][1]);
      elapsedTime = static_cast< unsigned int >(sqrt(x_dist * x_dist + y_dist * y_dist) + 0.99999999) + 1;
      currentTime += elapsedTime;
      break;
    case DroneCommand::UNLOAD:
      break;
    case DroneCommand::WAIT:
      break;
    }
  }
  list<array<unsigned int, 4>> commands;
  unsigned int currentTime;
  unsigned int X, Y;
};

#endif
