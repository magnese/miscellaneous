#ifndef SUDOKU_HH
#define SUDOKU_HH

#include <cmath>
#include <iostream>

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
  const auto size(s.size());
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
  const auto size(s.size());
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
  const auto size(s.size());
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
  const auto size(s.size());
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
  const auto size(s.size());
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
  const auto size(static_cast<int>(sqrt(s.size())));
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

template<typename T>
void print(const T& s,std::ostream& out=std::cout)
{
  const auto size(s.size());
  const auto squareSize(static_cast<int>(sqrt(size)));
  for(std::size_t row=0;row!=size;++row)
  {
    if(row%squareSize==0)
      out<<std::endl;
    for(std::size_t col=0;col!=size;++col)
    {
      if(col%squareSize==0)
        out<<" ";
      out<<s[row][col][0]<<" ";
    }
    out<<std::endl;
  }
  out<<std::endl;
}

#endif
