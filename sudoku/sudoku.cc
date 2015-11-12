#include <array>
#include <fstream>
#include <iostream>
#include <string>

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
  for(auto row=0;row!=9;++row)
    for(auto col=0;col!=9;++col)
    {
      const auto pos(s[row][col][0]);
      if(pos!=0)
        for(auto i=0;i!=9;++i)
          s[row][i][pos]=1;
    }
}

template<typename T>
void checkColumns(T& s)
{
  for(auto col=0;col!=9;++col)
    for(auto row=0;row!=9;++row)
    {
      const auto pos(s[row][col][0]);
      if(pos!=0)
        for(auto i=0;i!=9;++i)
          s[i][col][pos]=1;
    }
}

template<typename T>
void checkSquares(T& s)
{
  for(auto srow=0;srow!=3;++srow)
    for(auto scol=0;scol!=3;++scol)
      for(auto row=srow*3;row!=(srow+1)*3;++row)
        for(auto col=scol*3;col!=(scol+1)*3;++col)
        {
          const auto pos(s[row][col][0]);
          if(pos!=0)
            for(auto i=srow*3;i!=(srow+1)*3;++i)
              for(auto j=scol*3;j!=(scol+1)*3;++j)
                s[i][j][pos]=1;
        }
}

int main(int argc,char** argv)
{
  //create sudoku structure
  std::array<std::array<std::array<int,10>,9>,9> sudoku;

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
  }
  else
  {
    std::cout<<"Impossibile to read sudoku from file "<<filename<<std::endl;
    return 1;
  }

  // solve sudoku
  bool changed(true);
  while(changed)
  {
    clear(sudoku);
    checkRows(sudoku);
    checkColumns(sudoku);
    checkSquares(sudoku);
    changed=fill(sudoku);
  }

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
