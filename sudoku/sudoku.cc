#include <fstream>
#include <iostream>
#include <string>

#include "sudoku.hh"

int main(int argc,char** argv)
{
  //create sudoku structure
  Sudoku sudoku;

  // read file containing sudoku to solve
  std::string filename("sudoku.dat");
  if(argc>1)
    filename=argv[1];
  std::ifstream inputFile(filename);
  if(inputFile.is_open())
  {
    std::cout<<"Sudoku read from file "<<filename<<std::endl;
    inputFile>>sudoku;
    std::cout<<sudoku<<std::endl;
  }
  else
  {
    std::cout<<"Impossibile to read sudoku from file "<<filename<<std::endl;
    return 1;
  }

  // solve
  solve(sudoku);

  // write solved sudoku
  filename="solved_"+filename;
  std::ofstream outputFile(filename);
  if(outputFile.is_open())
  {
    outputFile<<sudoku;
    std::cout<<"Sudoku solution written on file "<<filename<<std::endl;
    std::cout<<sudoku<<std::endl;
  }
  else
  {
    std::cout<<"Impossibile to write sudoku solution on file "<<filename<<std::endl;
    std::cout<<sudoku<<std::endl;
    return 1;
  }

  return 0;
}
