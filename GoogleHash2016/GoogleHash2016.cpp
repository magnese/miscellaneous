/* Google hash code 2016 */

#include <array>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "functions.hh"

int main(int argc, char** argv)
{
  // create variables
  std::vector<std::vector<unsigned int>> warehouses;
  std::list<std::vector<unsigned int>> orders;
  std::vector<unsigned int> productWeights;
  unsigned int numRows;
  unsigned int numColumns;
  unsigned int numDrones;
  unsigned int numTurns;
  unsigned int maxPayload;
  unsigned int numProducts;
  unsigned int numWarehouses;
  unsigned int numOrders;
  unsigned int numItems;

  // read from file
  std::string filename("busy_day.in");
  if(argc>1)
    filename=argv[1];
  std::ifstream inputFile(filename);
  if(inputFile.is_open())
  {
    std::cout<<"Read from file "<<filename<<std::endl;
    // read rows, columns, drones, turns and maxpayload
    inputFile>>numRows>>numColumns>>numDrones>>numTurns>>maxPayload;
    // read number different product types
    inputFile>>numProducts;
    // read products weigths
    productWeights.resize(numProducts,0);
    for(auto& productWeight:productWeights)
      inputFile>>productWeight;
    // read number warehouses
    inputFile>>numWarehouses;
    // read warehouses data
    warehouses.resize(numWarehouses);
    for(auto& warehouse:warehouses)
    {
      warehouse.resize(numProducts+2,0);
      // read position werahouse
      inputFile>>warehouse[0]>>warehouse[1];
      // read product quantity
      for(std::size_t i=2;i!=warehouse.size();++i)
        inputFile>>warehouse[i];
    }
    // read number of orders
    inputFile>>numOrders;
    // read orders data
    for(unsigned int i=0;i!=numOrders;++i)
    {
      unsigned int offset(4);
      std::vector<unsigned int> order(numProducts+offset,0);
      // set order index
      order[0]=i;
      // read position werahouse
      inputFile>>order[1]>>order[2];
      // set order weight
      order[3]=0;
      // read number of items
      inputFile>>numItems;
      for(unsigned int i=offset; i!=(numItems+offset);++i)
      {
        unsigned int item;
        inputFile>>item;
        ++(order[item+offset]);
      }
      orders.emplace_back(std::move(order));
    }
    inputFile.close();
  }
  else
  {
    std::cout<<"Impossibile to read from file "<<filename<<std::endl;
    return 1;
  }

  // create variables
  unsigned int totalCommands(0);
  std::vector<Drone> droneSwarm(numDrones,Drone(orders,warehouses));

  // generate commands
  bool success(true);
  for(unsigned int t=0;(t!=numTurns)&&success;++t)
  {
    for(auto droneIt=droneSwarm.begin();(droneIt!=droneSwarm.end())&&success;++droneIt)
      if(droneIt->isAvailable(t))
        success = ApplyNextOrder(*droneIt,orders,warehouses,productWeights,totalCommands,maxPayload);
    std::cout<<"t = "<<t<<std::endl;
  }

  // dump on file
  OutputCommands(droneSwarm,"solution.out",totalCommands);

  return 0;
}
