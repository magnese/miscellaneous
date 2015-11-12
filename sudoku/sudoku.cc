#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

template<typename T>
void init(T& s)
{
  for(auto& row:s)
    for(auto& column:row)
      if(column[0]==0)
        for(auto& entry:column)
          entry=0;
}

template<typename T>
void finalize(T& s)
{
  for(auto& row:s)
    for(auto& entry:row)
      if(entry[0]==0)
      {
        int cont(0);
        for(int i=1;i!=entry.size();++i)
          if(entry[i]==0)
          {
            entry[0]=i;
            ++cont;
          }
        if(cont>1)
          entry[0]=0;
      }
}

template<typename T>
void fillRows(T& s,bool& changed)
{
  const auto size(s.size());
  for(auto row=0;row!=size;++row)
    for(auto col=0;col!=size;++col)
    {
      const auto pos(s[row][col][0]);
      if(pos!=0)
        for(auto i=0;i!=size;++i)
          if(s[row][i][pos]==0)
          {
            s[row][i][pos]=1;
            changed=true;
          }
    }
}

template<typename T>
void fillColumns(T& s,bool& changed)
{
  const auto size(s.size());
  for(auto col=0;col!=size;++col)
    for(auto row=0;row!=size;++row)
    {
      const auto pos(s[row][col][0]);
      if(pos!=0)
        for(auto i=0;i!=size;++i)
          if(s[i][col][pos]==0)
          {
            s[i][col][pos]=1;
            changed=true;
          }
    }
}

template<typename T>
void fillSquares(T& s,bool& changed)
{
  const auto size(static_cast<int>(sqrt(s.size())));
  for(auto srow=0;srow!=size;++srow)
    for(auto scol=0;scol!=size;++scol)
      for(auto row=srow*size;row!=(srow+1)*size;++row)
        for(auto col=scol*size;col!=(scol+1)*size;++col)
        {
          const auto pos(s[row][col][0]);
          if(pos!=0)
            for(auto i=srow*size;i!=(srow+1)*size;++i)
              for(auto j=scol*size;j!=(scol+1)*size;++j)
                if(s[i][j][pos]==0)
                {
                  s[i][j][pos]=1;
                  changed=true;
                }
        }
}

template<typename T>
void print(const T& s)
{
  const auto size(s.size());
  const auto squareSize(static_cast<int>(sqrt(size)));
  for(auto row=0;row!=size;++row)
  {
    if(row%squareSize==0)
      std::cout<<std::endl;
    for(auto col=0;col!=size;++col)
    {
      if(col%squareSize==0)
        std::cout<<" ";
      std::cout<<s[row][col][0]<<" ";
    }
    std::cout<<std::endl;
  }
  std::cout<<std::endl;
}

int main(int argc,char** argv)
{
  //create sudoku structure
  constexpr int size(9);
  std::array<std::array<std::array<int,size+1>,size>,size> sudoku;

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
    return 1;
  }

  return 0;
}
