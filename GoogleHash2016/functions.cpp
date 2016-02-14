#include <vector>
#include <array>
#include <math.h>
#include <list>
#include <map>
#include <fstream>
#include <iostream>
#include "functions.hh"

using namespace std;

extern vector<vector<unsigned int>> orders;
extern vector<vector<unsigned int>> warehouses;
extern vector<unsigned int> productWeights;
extern unsigned int numDrones;
extern unsigned int numTurns;
extern unsigned int numWarehouses;
extern unsigned int maxPayload;
extern unsigned int score;
unsigned int totalCommands;

void OutputCommands(const std::vector<Drone>& drones,const std::string& filename)
{
  std::map<unsigned int, char> CommandToOutput;
  CommandToOutput[DroneCommand::LOAD] = 'L';
  CommandToOutput[DroneCommand::UNLOAD] = 'U';
  CommandToOutput[DroneCommand::WAIT] = 'W';
  CommandToOutput[DroneCommand::DELIVER] = 'D';

  std::ofstream file(filename);
  file << totalCommands << std::endl;
  for (unsigned int droneIndex = 0; droneIndex < drones.size(); ++droneIndex)
  {
    // I am looking at drone elem. Elem is a list of commands to this drone.
    for (const auto& command : drones[droneIndex].commands)
    {
      file << droneIndex << " ";
      file << CommandToOutput[command[0]] << " ";
      file << command[1] << " ";
      if (command[0] != DroneCommand::WAIT)
      {
        file << command[2] << " ";
        file << command[3] << " ";
      }
      file << std::endl;
    }
  }
}

bool generate_weights(vector< unsigned int >& orders_scores,
  vector< vector< unsigned int > >& orders_nearest_warehouses,
  const vector< vector< unsigned int > > & orders,
  const vector< vector< unsigned int > > & warehouses,
  const vector< unsigned int > & product_weights) {
  orders_scores.clear();
  orders_scores.resize(orders.size());

  orders_nearest_warehouses = vector< vector< unsigned int > >(orders.size(), vector< unsigned int >(productWeights.size(), -1));
  // calculate score for each order
  for (unsigned int i = 0; i < orders.size(); i++) {
    if (orders[i][0] == 1)
      continue;
    // calculate the distance from order i to each warehouse
    vector< unsigned int > warehouse_distances = vector< unsigned int >(warehouses.size());
    for (unsigned int warehouse_idx = 0; warehouse_idx < warehouses.size(); warehouse_idx++) {
      double x_dist = static_cast< double >(orders[i][1]) - static_cast< double >(warehouses[warehouse_idx][0]);
      double y_dist = static_cast< double >(orders[i][2]) - static_cast< double >(warehouses[warehouse_idx][1]);
      warehouse_distances[warehouse_idx] = static_cast< unsigned int >(sqrt(x_dist * x_dist + y_dist * y_dist) + 0.99999999);
    }
    // sort the warehouse_distances - I need the index of the nearest warehouse, then the second-nearest warehouse, …
    vector< unsigned int > sorted_warehouse_indices = SortVector(warehouse_distances);

    for (unsigned int prod_idx = 0; prod_idx < product_weights.size(); prod_idx++) {
      // does the order need product prod_idx?
      if (orders[i][prod_idx + 3] > 0) {
        // find nearest warehouse with enough product prod_idx
        unsigned int desired = orders[i][prod_idx + 3];
        bool found = false;
        unsigned int warehouse_idx = 0;
        unsigned int chosen_warehouse;
        while ((!found) && warehouse_idx < numWarehouses) { // TODO: TERMINATOR
          chosen_warehouse = sorted_warehouse_indices[warehouse_idx];
          // is there enough stock in the chosen warehouse?
          if (warehouses[chosen_warehouse][prod_idx+2] >= desired) {
            found = true;
          }
          else {
            warehouse_idx++;
          }
        }
        if (!found){
          orders_scores[i] = -1;
          continue;
        }
        orders_scores[i] += warehouse_distances[chosen_warehouse];
        orders_nearest_warehouses[i][prod_idx] = chosen_warehouse;
      }
    }
  }
  return true;
}

bool ApplyNextOrder(Drone& drone) {
  //////////////////////////////////////
  // Retrieve next order
  vector<unsigned int> orderWeights;
  vector< vector< unsigned int > > warehousesNeeded;
  //vector<unsigned int> warehousesNeeded;
  bool success = generate_weights(orderWeights, warehousesNeeded, orders, warehouses, productWeights);
  //////////////////////////////////////
  if (success){
    //////////////////////////////////////
    // sort the weights
    vector<tuple<unsigned int, unsigned int>> indicesWithWeights;
    for (unsigned int j = 0; j < orderWeights.size(); ++j)
      indicesWithWeights.push_back(tuple<unsigned int, unsigned int>(orderWeights[j], j));
    // ok now let's sort this!
    ApplyQuickSortMT(indicesWithWeights, 0, orderWeights.size() - 1);
    //////////////////////////////////////
    unsigned int index = 0;
    while (index < indicesWithWeights.size() && orders[get<1>(indicesWithWeights[index])][0] != 0)
      ++index; // find the next one
    if (index == indicesWithWeights.size())
      return false;
    unsigned int currentOrder = get<1>(indicesWithWeights[index]);
    // ok now that's my current order, let's do something with it.
    for (unsigned int prodIdx = 0; prodIdx < productWeights.size(); ++prodIdx) {
      // do I want this one?
      if (orders[currentOrder][prodIdx + 3] > 0) // Yes I want this one. Start queueing.
      {
        unsigned int desired = orders[currentOrder][prodIdx + 3];

        warehouses[warehousesNeeded[currentOrder][prodIdx]][prodIdx+2]-=desired;
        // how many can I carry at a time?
        unsigned int trips;
        if((productWeights[prodIdx] * desired) % maxPayload)
          trips = (productWeights[prodIdx] * desired) / maxPayload + 1;
        else
          trips = (productWeights[prodIdx]*desired) / maxPayload;
        unsigned int maxPerTrip = maxPayload / productWeights[prodIdx];
        unsigned int remaining = desired;
        for (unsigned int trip = 0; trip < trips; ++trip)
        {
          drone.AddCommand(DroneCommand::LOAD, warehousesNeeded[currentOrder][prodIdx], prodIdx, min(remaining,maxPerTrip));
          drone.AddCommand(DroneCommand::DELIVER, currentOrder, prodIdx, min(remaining, maxPerTrip));
        }
      }
    }
    if (drone.currentTime < numTurns)
      score += (100 * (numTurns - drone.currentTime)) / numTurns; // add a score for this delivery
    orders[currentOrder][0] = 1;
  }
  return true;
}
