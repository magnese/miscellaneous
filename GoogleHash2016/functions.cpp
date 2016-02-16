#include <algorithm>
#include <fstream>
#include <iostream>
#include <array>
#include <limits>
#include <list>
#include <map>
#include <math.h>
#include <vector>

#include "functions.hh"

void OutputCommands(const std::vector<Drone>& drones,const std::string& filename,unsigned int totalCommands)
{
  std::map<unsigned int, char> CommandToOutput;
  CommandToOutput[DroneCommand::LOAD]='L';
  CommandToOutput[DroneCommand::UNLOAD]='U';
  CommandToOutput[DroneCommand::WAIT]='W';
  CommandToOutput[DroneCommand::DELIVER]='D';

  std::ofstream file(filename);
  file<<totalCommands<<std::endl;
  for(unsigned int droneIndex=0;droneIndex!=drones.size();++droneIndex)
  {
    for(const auto& command:drones[droneIndex].commands)
    {
      file<<droneIndex<<" ";
      file<<CommandToOutput[command[0]]<<" ";
      file<<command[1]<<" ";
      if (command[0]!=DroneCommand::WAIT)
      {
        file<<command[2]<<" ";
        file<<command[3]<<" ";
      }
      file<<std::endl;
    }
  }
}

unsigned int distance(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1)
{
  double dx(static_cast<double>(x1)-static_cast<double>(x0));
  double dy(static_cast<double>(y1)-static_cast<double>(y0));
  return static_cast<unsigned int>(sqrt(pow(dx,2)+pow(dy,2))+0.99999999);
}

void sortOrders(std::list<std::vector<unsigned int>>& warehousesNeeded,std::list<std::vector<unsigned int>>& orders,
                const std::vector<std::vector<unsigned int>>& warehouses,const std::vector<unsigned int>& productWeights)
{
  warehousesNeeded.clear();

  // calculate score for each order
  for(auto& order:orders)
  {
    // clear order score
    order[3]=0;

    // create warehouse needed
    std::vector<unsigned int> warehouseNeeded(productWeights.size()+1,std::numeric_limits<unsigned int>::max());

    // calculate the distance from order i to each warehouse
    std::vector<std::array<unsigned int,2>> warehouse_distances(warehouses.size(),{0,0});
    for(unsigned int warehouse_idx= 0;warehouse_idx!=warehouses.size();++warehouse_idx)
    {
      warehouse_distances[warehouse_idx][0]=warehouse_idx;
      warehouse_distances[warehouse_idx][1]=distance(order[1],order[2],warehouses[warehouse_idx][0],warehouses[warehouse_idx][1]);
    }

    // sort warehouse distances
    std::sort(warehouse_distances.begin(),warehouse_distances.end(),
      [](const std::array<unsigned int,2>& a,const std::array<unsigned int,2>& b){return a[1]<b[1];});

    for(unsigned int prod_idx=0;prod_idx!=productWeights.size();++prod_idx)
    {
      // does the order need product prod_idx?
      if(order[prod_idx+4]>0)
      {
        // find nearest warehouse with enough product prod_idx
        const auto desired=order[prod_idx+4];
        bool found(false);
        unsigned int warehouseIdx;
        unsigned int distance;
        for(const auto& entry:warehouse_distances)
        {
          warehouseIdx=entry[0];
          // is there enough stock in the chosen warehouse?
          if(warehouses[warehouseIdx][prod_idx+2]>=desired)
          {
            distance=entry[1];
            found=true;
            break;
          }
        }
        if(!found)
        {
          order[3]=std::numeric_limits<unsigned int>::max();
          continue;
        }
        if(order[3]!=std::numeric_limits<unsigned int>::max())
          order[3]+=distance;
        warehouseNeeded[prod_idx+1]=warehouseIdx;
      }
    }
    // assign the same score so it will be sorted as orders
    warehouseNeeded[0]=order[3];
    warehousesNeeded.emplace_back(std::move(warehouseNeeded));
  }

  // sort orders by score
  orders.sort([](const std::vector<unsigned int>& a,const std::vector<unsigned int>& b){return a[3]<b[3];});
  // sort warehouses needed by score
  warehousesNeeded.sort([](const std::vector<unsigned int>& a,const std::vector<unsigned int>& b){return a[0]<b[0];});
}

bool ApplyNextOrder(Drone& drone,std::list<std::vector<unsigned int>>& orders,std::vector<std::vector<unsigned int>>& warehouses,
                    const std::vector<unsigned int>& productWeights,unsigned int& totalCommands,unsigned int maxPayload)
{
  // find next order
  std::list<std::vector<unsigned int>> warehousesNeeded;
  sortOrders(warehousesNeeded,orders,warehouses,productWeights);

  if(orders.size()==0)
    return false;

  auto& order(orders.front());
  auto& warehouseNeeded(warehousesNeeded.front());

  // loop over products
  for (unsigned int prodIdx=0;prodIdx!=productWeights.size();++prodIdx)
  {
    // do I need this product?
    if (order[prodIdx + 4] > 0)
    {
      auto desired=order[prodIdx+4];
      warehouses[warehouseNeeded[prodIdx+1]][prodIdx+2]-=desired;
      // how many can I carry at a time?
      unsigned int trips;
      if((productWeights[prodIdx]*desired)%maxPayload)
        trips=(productWeights[prodIdx]*desired)/maxPayload+1;
      else
        trips=(productWeights[prodIdx]*desired)/maxPayload;
      unsigned int maxPerTrip=maxPayload/productWeights[prodIdx];
      unsigned int remaining=desired;
      for (unsigned int trip=0;trip!=trips;++trip)
      {
        drone.AddCommand(DroneCommand::LOAD,warehouseNeeded[prodIdx+1],prodIdx,std::min(remaining,maxPerTrip),totalCommands);
        drone.AddCommand(DroneCommand::DELIVER,order[0],prodIdx,std::min(remaining,maxPerTrip),totalCommands);
      }
    }
  }
  orders.pop_front();

  return true;
}
