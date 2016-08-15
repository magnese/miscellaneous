#ifndef SUDOKU_HH
#define SUDOKU_HH

#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

// Sudoku structure
class Sudoku:public std::array<std::array<std::array<unsigned int,10>,9>,9>
{};

// Read
std::istream& operator>>(std::istream& is,Sudoku& sudoku)
{
	for(auto& row:sudoku)
    for(auto& entry:row)
      is>>entry[0];
  return is;
}

// Print to file
std::ofstream& operator<<(std::ofstream& os,const Sudoku& sudoku)
{
	for(const auto& row:sudoku)
  {
    for(const auto& entry:row)
      os<<entry[0]<<" ";
    os<<std::endl;
  }
  return os;
}

// Print
std::ostream& operator<<(std::ostream& os,const Sudoku& sudoku)
{
  constexpr auto size(sudoku.size());
  const auto squareSize(static_cast<std::size_t>(sqrt(size)));
  for(std::size_t row=0;row!=size;++row)
  {
    if(row%squareSize==0)
      os<<std::endl;
    for(std::size_t col=0;col!=size;++col)
    {
      if(col%squareSize==0)
        os<<" ";
      os<<sudoku[row][col][0]<<" ";
    }
    os<<std::endl;
  }
  return os;
}

// Square indices map
std::pair<std::size_t,std::size_t> squareMap(std::size_t fakeRow,std::size_t fakeCol)
{
	return std::pair<std::size_t,std::size_t>(fakeRow-fakeRow%3+fakeCol/3,fakeRow%3*3+fakeCol%3);
}

// Check if is consistent
bool isConsistent(const Sudoku& sudoku)
{
  constexpr auto size(sudoku.size());

  // check rows
  for(std::size_t i=0;i!=size;++i)
  {
    std::vector<std::size_t> mask(size+1,0);
    for(std::size_t j=0;j!=size;++j)
      if(sudoku[i][j][0]>0)
        if((++(mask[sudoku[i][j][0]]))>1)
          return false;
  }

  // check columns
  for(std::size_t i=0;i!=size;++i)
  {
    std::vector<std::size_t> mask(size+1,0);
    for(std::size_t j=0;j!=size;++j)
      if(sudoku[j][i][0]>0)
        if((++(mask[sudoku[j][i][0]]))>1)
          return false;
  }

	// check squares
  for(std::size_t i=0;i!=size;++i)
  {
    std::vector<std::size_t> mask(size+1,0);
    for(std::size_t j=0;j!=size;++j)
		{
			const auto indices(squareMap(i,j));
      if(sudoku[indices.first][indices.second][0]>0)
        if((++(mask[sudoku[indices.first][indices.second][0]]))>1)
          return false;
		}
  }

  return true;
}

// Recursive solve
void recursiveSolve(Sudoku sudoku,std::size_t row=0,std::size_t col=0)
{
  if(!isConsistent(sudoku))
    return;

  constexpr auto size(sudoku.size());
  bool found(false);
  while((row<size)&&(!found))
  {
    while((col<size)&&(!found))
    {
      if(sudoku[row][col][0]==0)
        found=true;
      else
        ++col;
    }
    if(!found)
    {
      col=0;
      ++row;
    }
  }

  if(!found)
  {
    std::cout<<sudoku<<std::endl;
    std::cout<<"SOLVED!"<<std::endl;
    return;
  }

  for(std::size_t val=1;val!=(size+1);++val)
  {
    sudoku[row][col][0]=val;
    recursiveSolve(sudoku,row,col);
  }
}

template<typename T>
void finalize(T& s)
{
  for(auto& row:s)
    for(auto& entry:row)
      if(entry[0]==0)
      {
        std::size_t cont(0);
        for(std::size_t i=1;i!=entry.size();++i)
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
void fillUniquesRows(T& s,bool& changed)
{
  constexpr auto size(s.size());
  for(std::size_t row=0;row!=size;++row)
    for(std::size_t entry=1;entry!=size+1;++entry)
    {
      std::size_t cont=0;
      std::size_t colPos=0;
      for(std::size_t col=0;(col!=size)&&(cont<2);++col)
        if((s[row][col][0]==0)&&(s[row][col][entry]==0))
        {
          if(cont==0)
          {
            s[row][col][0]=entry;
            ++cont;
            colPos=col;
          }
          else
          {
            s[row][colPos][0]=0;
            ++cont;
          }
        }
      if(cont==1)
        changed=true;
    }
}

template<typename T>
void fillRows(T& s,bool& changed)
{
  constexpr auto size(s.size());
  for(std::size_t row=0;row!=size;++row)
    for(std::size_t col=0;col!=size;++col)
    {
      const auto pos(s[row][col][0]);
      if(pos!=0)
        for(std::size_t i=0;i!=size;++i)
          if(s[row][i][pos]==0)
          {
            s[row][i][pos]=1;
            changed=true;
          }
    }
  fillUniquesRows(s,changed);
}

template<typename T>
void fillUniquesColumns(T& s,bool& changed)
{
  constexpr auto size(s.size());
  for(std::size_t col=0;col!=size;++col)
    for(std::size_t entry=1;entry!=size+1;++entry)
    {
      std::size_t cont=0;
      std::size_t rowPos=0;
      for(std::size_t row=0;(row!=size)&&(cont<2);++row)
        if((s[row][col][0]==0)&&(s[row][col][entry]==0))
        {
          if(cont==0)
          {
            s[row][col][0]=entry;
            ++cont;
            rowPos=row;
          }
          else
          {
            s[rowPos][col][0]=0;
            ++cont;
          }
        }
      if(cont==1)
        changed=true;
    }
}

template<typename T>
void fillColumns(T& s,bool& changed)
{
  constexpr auto size(s.size());
  for(std::size_t col=0;col!=size;++col)
    for(std::size_t row=0;row!=size;++row)
    {
      const auto pos(s[row][col][0]);
      if(pos!=0)
        for(std::size_t i=0;i!=size;++i)
          if(s[i][col][pos]==0)
          {
            s[i][col][pos]=1;
            changed=true;
          }
    }
  fillUniquesColumns(s,changed);
}

template<typename T>
void fillUniquesSquares(T& s,bool& changed)
{
  constexpr auto size(s.size());
  const auto squareSize(static_cast<int>(sqrt(size)));
  for(std::size_t srow=0;srow!=squareSize;++srow)
    for(std::size_t scol=0;scol!=squareSize;++scol)
      for(std::size_t entry=1;entry!=size+1;++entry)
      {
        std::size_t cont=0;
        std::size_t rowPos=0;
        std::size_t colPos=0;
        for(auto row=srow*squareSize;(row!=(srow+1)*squareSize)&&(cont<2);++row)
          for(auto col=scol*squareSize;(col!=(scol+1)*squareSize)&&(cont<2);++col)
            if((s[row][col][0]==0)&&(s[row][col][entry]==0))
            {
              if(cont==0)
              {
                s[row][col][0]=entry;
                ++cont;
                rowPos=row;
                colPos=col;
              }
              else
              {
                s[rowPos][colPos][0]=0;
                ++cont;
              }
            }
        if(cont==1)
          changed=true;
      }
}

template<typename T>
void fillSquares(T& s,bool& changed)
{
  constexpr auto size(static_cast<int>(sqrt(s.size())));
  for(std::size_t srow=0;srow!=size;++srow)
    for(std::size_t scol=0;scol!=size;++scol)
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
  fillUniquesSquares(s,changed);
}

void solve(Sudoku& sudoku)
{
  // init sudoku
  for(auto& row:sudoku)
    for(auto& column:row)
      if(column[0]==0)
        for(auto& entry:column)
          entry=0;

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
}

#endif
