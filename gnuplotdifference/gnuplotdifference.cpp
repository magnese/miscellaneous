#include <array>
#include <fstream>
#include <list>
#include <vector>

// read file containing gnuplot function
std::list<std::array<double,2>> readGnuplotFunction(std::string filename)
{
  std::ifstream file(filename);
  std::list<std::array<double,2>> f;
  while(file.good())
  {
    std::array<double,2> row;
    file>>row[0]>>row[1];
    f.emplace_back(std::move(row));
  }
  f.pop_back();
  return f;
}

int main()
{
  // read files
  auto f1=readGnuplotFunction("data1.dat");
  auto f2=readGnuplotFunction("data2.dat");
  // compute difference
  std::vector<std::array<double,2>> diff(f1.size(),{0.0,0.0});
  auto f1It(f1.begin());
  auto f2It(f2.begin());
  for(auto diffIt=diff.begin();diffIt!=diff.end();++diffIt,++f1It,++f2It)
    (*diffIt)={(*f1It)[0],(*f1It)[1]-(*f2It)[1]};
  // dump difference on file
  std::ofstream file("diff.dat");
  for(const auto& row:diff)
    file<<row[0]<<" "<<row[1]<<std::endl;
  return 0;
}
