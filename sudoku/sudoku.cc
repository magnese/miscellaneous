#include <array>
#include <fstream>
#include <iostream>
#include <string>

#include "sudoku.hh"

int main(int argc,char** argv)
{
  //create sudoku structure
  constexpr unsigned int size(9);
  std::array<std::array<std::array<unsigned int,size+1>,size>,size> sudoku;

  // read file containing sudoku to solve
  std::string filename("sudoku.dat");
  if(argc>1)
    filename=argv[1];
  std::ifstream inputFile(filename);
  if(inputFile.is_open())
  {
    std::cout<<"Sudoku read from file "<<filename<<std::endl;
    for(auto& row:sudoku)
      for(auto& entry:row)
        inputFile>>entry[0];
    print(sudoku);
  }
  else
  {
    std::cout<<"Impossibile to read sudoku from file "<<filename<<std::endl;
    return 1;
  }

  // init sudoku
  init(sudoku);

  // solve sudoku
  bool changed;
  do
  {
    changed=false;
    fillRows(sudoku,changed);
    fillColumns(sudoku,changed);
    fillSquares(sudoku,changed);
    finalize(sudoku);
  }
  while(changed);

  // write solved sudoku
  filename="solved_"+filename;
  std::ofstream outputFile(filename);
  if(outputFile.is_open())
  {
    for(const auto& row:sudoku)
    {
      for(const auto& entry:row)
        outputFile<<entry[0]<<" ";
      outputFile<<std::endl;
    }
    std::cout<<"Sudoku solution written on file "<<filename<<std::endl;
    print(sudoku);
  }
  else
  {
    std::cout<<"Impossibile to write sudoku solution on file "<<filename<<std::endl;
    print(sudoku);
    return 1;
  }

  return 0;
}
