#ifndef FUNCTIONS_HH
#define FUNCTIONS_HH

#include <array>
#include <list>
#include <vector>

enum DroneCommand {LOAD, UNLOAD, WAIT, DELIVER};

extern unsigned int totalCommands;
extern std::vector<std::vector<unsigned int>> orders;
extern std::vector<std::vector<unsigned int>> warehouses;
struct Drone;

bool ApplyNextOrder(Drone& drone);
void OutputCommands(const std::vector<Drone>& drones,const std::string& filename);
unsigned int distance(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1);


struct Drone
{
  Drone():
    currentTime(0), X(warehouses[0][0]),Y(warehouses[0][1])
  {};

  bool isAvailable(unsigned int currentT)
  {
    return(currentT >= currentTime);
  };
  void AddCommand(DroneCommand comm, unsigned int val1, unsigned int val2, unsigned int val3)
  {
    commands.push_back({static_cast<unsigned int>(comm),val1,val2,val3});
    ++totalCommands;
    switch (comm)
    {
      case DroneCommand::DELIVER:
        currentTime += distance(X,Y,orders[val1][1],orders[val1][2]) + 1;
        break;
      case DroneCommand::LOAD:
        currentTime += distance(X,Y,warehouses[val1][0],warehouses[val1][1]) + 1;
        break;
      case DroneCommand::UNLOAD:
        break;
      case DroneCommand::WAIT:
        break;
    }
  }
  std::list<std::array<unsigned int, 4>> commands;
  unsigned int currentTime;
  unsigned int X, Y;
};

#endif
