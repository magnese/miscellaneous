#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>

template<typename T>
void clear(T& s)
{
  for(auto& row:s)
    for(auto& column:row)
      if(column[0]==0)
        for(auto& entry:column)
          entry=0;
}

template<typename T>
bool fill(T& s)
{
  bool changed(false);
  for(auto& row:s)
    for(auto& entry:row)
      if(entry[0]==0)
      {
        int cont(0);
        for(int i=1;i!=10;++i)
          if(entry[i]==0)
          {
            entry[0]=i;
            ++cont;
          }
        if(cont>1)
          entry[0]=0;
        else
          changed=true;
      }
  return changed;
}

template<typename T>
void checkRows(T& s)
{
  for(auto i=0;i!=9;++i)
  {
    std::vector<int> temp(10,0);
    for(auto j=0;j!=9;++j)
      temp[s[i][j][0]]=1;
    for(auto j=0;j!=9;++j)
      for(auto k=1;k!=10;++k)
        s[i][j][k]+=temp[k];
  }
}

template<typename T>
void checkColumns(T& s)
{
  for(auto i=0;i!=9;++i)
  {
    std::vector<int> temp(10,0);
    for(auto j=0;j!=9;++j)
      temp[s[j][i][0]]=1;
    for(auto j=0;j!=9;++j)
      for(auto k=1;k!=10;++k)
        s[j][i][k]+=temp[k];
  }
}

template<typename T>
void checkSquares(T& s)
{}

int main()
{
  //create sudoku structure
  std::array<std::array<std::array<int,10>,9>,9> sudoku;

  // read file containing sudoku to solve
  std::string filename;
  std::cout<<"Sudoku filename: ";
  std::cin>>filename;
  std::ifstream inputFile(filename);
  if(inputFile.is_open())
    for(auto& row:sudoku)
      for(auto& entry:row)
        inputFile>>entry[0];
  else
  {
    std::cout<<"Impossibile to read sudoku from file "<<filename<<std::endl;
    return 1;
  }

  // solve sudoku
  do
  {
    clear(sudoku);
    checkRows(sudoku);
    checkColumns(sudoku);
    checkSquares(sudoku);
   }
   while(fill(sudoku));

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
  }
  else
  {
    std::cout<<"Impossibile to write sudoku solution on file "<<filename<<std::endl;
    return 1;
  }

  return 0;
}
