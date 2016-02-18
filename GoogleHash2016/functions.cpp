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

void outputCommands(const std::vector<Drone>& drones,const std::string& filename,unsigned int totalCommands)
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

void clearOrders(std::list<std::vector<unsigned int>>& orders)
{
  // clear empty orders
  for(auto it=orders.begin();it!=orders.end();++it)
  {
    bool isEmpty(true);
    for(unsigned int i=4;i!=it->size();++i)
      if((*it)[i]>0)
      {
        isEmpty=false;
        break;
      }
    if(isEmpty)
      it=orders.erase(it);
  }
}

void sortOrders(std::vector<unsigned int>& firstOrderWarehouse,std::list<std::vector<unsigned int>>& orders,
                const std::vector<std::vector<unsigned int>>& warehouses,const std::vector<unsigned int>& productWeights,
                unsigned int maxPayload)
{
  // calculate score for each order
  std::list<std::vector<unsigned int>> warehousesNeeded;
  for(auto& order:orders)
  {
    // clear order score
    order[3]=0;

    // create warehouse needed
    std::vector<unsigned int> warehouseNeeded(productWeights.size()+1,std::numeric_limits<unsigned int>::max());

    // calculate the distance from order i to each warehouse
    std::vector<std::array<unsigned int,2>> warehouseDistances(warehouses.size(),{0,0});
    for(unsigned int warehouseIdx=0;warehouseIdx!=warehouses.size();++warehouseIdx)
    {
      warehouseDistances[warehouseIdx][0]=warehouseIdx;
      warehouseDistances[warehouseIdx][1]=distance(order[1],order[2],warehouses[warehouseIdx][0],warehouses[warehouseIdx][1]);
    }

    // sort warehouse distances
    std::sort(warehouseDistances.begin(),warehouseDistances.end(),
      [](const std::array<unsigned int,2>& a,const std::array<unsigned int,2>& b){return a[1]<b[1];});

    for(unsigned int prodIdx=0;prodIdx!=productWeights.size();++prodIdx)
    {
      // does the order need product prodIdx?
      if(order[prodIdx+4]>0)
      {
        // find nearest warehouse with enough product prodIdx
        const auto desired(order[prodIdx+4]);
        bool found(false);
        unsigned int warehouseIdx;
        unsigned int score;
        for(const auto& entry:warehouseDistances)
        {
          warehouseIdx=entry[0];
          // is there enough stock in the chosen warehouse?
          if(warehouses[warehouseIdx][prodIdx+2]>=desired)
          {
            // score is distance*(2*trips-1) since you need to come back to retrive the stuff
            unsigned int trips;
            if((productWeights[prodIdx]*desired)%maxPayload)
              trips=(productWeights[prodIdx]*desired)/maxPayload+1;
            else
              trips=(productWeights[prodIdx]*desired)/maxPayload;
            score=entry[1]*(2*trips-1);
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
          order[3]+=score;
        warehouseNeeded[prodIdx+1]=warehouseIdx;
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
  firstOrderWarehouse=std::move(warehousesNeeded.front());
}

void applyNextOrder(Drone& drone,std::list<std::vector<unsigned int>>& orders,std::vector<std::vector<unsigned int>>& warehouses,
                    const std::vector<unsigned int>& productWeights,unsigned int& totalCommands,unsigned int maxPayload,
                    const std::vector<unsigned int>& firstOrderWerahouse)
{
  // find closer warehouse which has some needed product
  unsigned int minDistance(std::numeric_limits<unsigned int>::max());
  unsigned int warehouseIdx(0);
  auto& order(orders.front());
  for(unsigned int prodIdx=0;prodIdx!=productWeights.size();++prodIdx)
  {
    // do I need this product?
    if(order[prodIdx+4]>0)
    {
      const auto idx=firstOrderWerahouse[prodIdx+1];
      const auto dist=distance(drone.X,drone.Y,warehouses[idx][0],warehouses[idx][1]);
      if(dist<minDistance)
      {
        warehouseIdx=idx;
        minDistance=dist;
      }
    }
  }

  auto residualPayload(maxPayload);

  // list of commands to execute
  std::list<std::array<unsigned int,3>> loadCommands;
  std::list<std::array<unsigned int,3>> deliverCommands;

  // loop over orders
  for(auto& order:orders)
  {
    // loop over products
    for(unsigned int prodIdx=0;prodIdx!=productWeights.size();++prodIdx)
    {
      // do I need this product?
      if((order[prodIdx+4]>0)&&(residualPayload>=productWeights[prodIdx]))
      {
        // do I have this product?
        if(warehouses[warehouseIdx][prodIdx+2]>=order[prodIdx+4])
        {
          // compute number of trips needed
          auto desired(order[prodIdx+4]);
          unsigned int trips;
          if((productWeights[prodIdx]*desired)%residualPayload)
            trips=(productWeights[prodIdx]*desired)/residualPayload+1;
          else
            trips=(productWeights[prodIdx]*desired)/residualPayload;
          // reserve maximum loadable quantities
          if(trips>1)
            desired=residualPayload/productWeights[prodIdx];
          residualPayload-=desired*productWeights[prodIdx];
          warehouses[warehouseIdx][prodIdx+2]-=desired;
          order[prodIdx+4]-=desired;
          // store commands
          loadCommands.push_back({warehouseIdx,prodIdx,desired});
          deliverCommands.push_back({order[0],prodIdx,desired});
        }
      }
    }
  }

  // add commands to drone
  for(const auto& command:loadCommands)
    drone.AddLoadCommand(command[0],command[1],command[2],totalCommands);
  for(const auto& command:deliverCommands)
    drone.AddDeliverCommand(command[0],command[1],command[2],totalCommands);
}
