#include <iostream>
#include <random>
#include <array>
#include <algorithm>
#include <utility>

int main()
{
  // simulation parameters
  constexpr int numSimulations(100000);
  constexpr double stickLength(1.0);
  constexpr int numBreakingPoints(2);

  // create uniform distribution
  std::default_random_engine generator(std::random_device{}());
  std::uniform_real_distribution<double> distribution(0.0,stickLength);

  // breaking points
  std::array<double,numBreakingPoints+2> pts;
  pts.front()=0.0;
  pts.back()=stickLength;
  double averageMaxLength(0.0);

  // compute average max length of the pieces
  for(auto i=0;i!=numSimulations;++i)
  {
    for(auto i=1;i!=pts.size()-1;++i)
      pts[i]=distribution(generator);
    std::sort(pts.begin()+1,pts.end()-1);
    double maxLength(0.0);
    for(auto i=1;i!=pts.size();++i)
      maxLength=std::max(maxLength,pts[i]-pts[i-1]);
    averageMaxLength+=maxLength;
  }
  averageMaxLength/=static_cast<double>(numSimulations);

  // output result
  std::cout<<"Breaking the stick of length "<<stickLength<<" in "<<numBreakingPoints<<" originates a piece of maximum expected average length of "<<averageMaxLength<<"."<<std::endl;
  return 0;
}
