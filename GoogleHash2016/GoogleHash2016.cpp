/* Google hash code 2016 */

#include <array>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "functions.hh"

std::vector<std::vector<unsigned int>> warehouses;
std::vector<std::vector<unsigned int>> orders;
std::vector<unsigned int> productWeights;
unsigned int numTurns;
unsigned int maxPayload;
unsigned int score;
unsigned int totalCommands;

int main(int argc, char** argv)
{
  // create variables
  unsigned int numRows;
  unsigned int numColumns;
  unsigned int numDrones;
  unsigned int numProducts;
  unsigned int numWarehouses;
  unsigned int numOrders;
  unsigned int numItems;

  // read from file
  std::string filename("busy_day.in");
  if (argc>1)
    filename = argv[1];
  std::ifstream inputFile(filename);
  if (inputFile.is_open())
  {
    std::cout << "Read from file " << filename << std::endl;
    // read rows, columns, drones, turns and maxpayload
    inputFile >> numRows >> numColumns >> numDrones >> numTurns >> maxPayload;
    // read number different product types
    inputFile >> numProducts;
    // read products weigths
    productWeights.resize(numProducts, 0);
    for (auto& productWeight:productWeights)
      inputFile >> productWeight;
    // read number warehouses
    inputFile >> numWarehouses;
    // read warehouses data
    warehouses.resize(numWarehouses);
    for (auto& warehouse : warehouses)
    {
      warehouse.resize(numProducts + 2, 0);
      // read position werahouse
      inputFile >> warehouse[0] >> warehouse[1];
      // read product quantity
      for (std::size_t i = 2; i != warehouse.size(); ++i)
        inputFile >> warehouse[i];
    }
    // read number of orders
    inputFile >> numOrders;
    // read orders data
    orders.resize(numOrders);
    for (auto& order : orders)
    {
      unsigned int offset(3);
      order.resize(numProducts + offset, 0);
      // read position werahouse
      inputFile >> order[1] >> order[2];
      // read number of items
      inputFile >> numItems;
      for (unsigned int i = offset; i != (numItems + offset); ++i)
      {
        unsigned int item;
        inputFile >> item;
        ++(order[item + offset]);
      }
    }
    inputFile.close();
  }
  else
  {
    std::cout << "Impossibile to read from file " << filename << std::endl;
    return 1;
  }

  // create variables
  score = 0;
  totalCommands = 0;
  std::vector<Drone> droneSwarm(numDrones,Drone(orders,warehouses));

  // generate commands
  bool success(true);
  for (unsigned int t = 0; (t < numTurns) && success; ++t)
  {
    for (auto droneIt=droneSwarm.begin();(droneIt!=droneSwarm.end()) && success; ++droneIt)
      if (droneIt->isAvailable(t))
        success = ApplyNextOrder(*droneIt);
    std::cout << t << std::endl;
  }

  // dump on file
  OutputCommands(droneSwarm,"solution.out");

  std::cout << score << std::endl;

  return 0;
}
