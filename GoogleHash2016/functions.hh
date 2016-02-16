#ifndef FUNCTIONS_HH
#define FUNCTIONS_HH

#include <array>
#include <list>
#include <vector>

enum DroneCommand{LOAD, UNLOAD, WAIT, DELIVER};

unsigned int distance(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1);

struct Drone
{
  Drone(const std::list<std::vector<unsigned int>>& orders,const std::vector<std::vector<unsigned int>>& warehouses):
    orders_(orders),warehouses_(warehouses),currentTime(0),X(warehouses_[0][0]),Y(warehouses_[0][1])
  {};

  bool isAvailable(unsigned int currentT)
  {
    return currentT>=currentTime;
  };

  void AddLoadCommand(unsigned int val1,unsigned int val2,unsigned int val3,unsigned int& totalCommands)
  {
    commands.push_back({static_cast<unsigned int>(DroneCommand::LOAD),val1,val2,val3});
    ++totalCommands;
    currentTime+=distance(X,Y,warehouses_[val1][0],warehouses_[val1][1])+1;
    X=warehouses_[val1][0];
    Y=warehouses_[val1][1];
  }
  void AddDeliverCommand(unsigned int val1,unsigned int val2,unsigned int val3,unsigned int& totalCommands)
  {
    commands.push_back({static_cast<unsigned int>(DroneCommand::DELIVER),val1,val2,val3});
    ++totalCommands;
    bool found(false);
    auto orderIt=orders_.begin();
    while(!found)
    {
      if((*orderIt)[0]==val1)
        found=true;
      else
        ++orderIt;
    }
    currentTime+=distance(X,Y,(*orderIt)[1],(*orderIt)[2])+1;
    X=(*orderIt)[1];
    Y=(*orderIt)[2];
  }

  const std::list<std::vector<unsigned int>>& orders_;
  const std::vector<std::vector<unsigned int>>& warehouses_;
  std::list<std::array<unsigned int, 4>> commands;
  unsigned int currentTime;
  unsigned int X;
  unsigned int Y;
};

bool ApplyNextOrder(Drone& drone,std::list<std::vector<unsigned int>>& orders,std::vector<std::vector<unsigned int>>& warehouses,
                    const std::vector<unsigned int>& productWeights,unsigned int& totalCommands,unsigned int maxPayload);
void OutputCommands(const std::vector<Drone>& drones,const std::string& filename,unsigned int totalCommands);

#endif
