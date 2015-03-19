#include <vector>
#include <iostream>

template<class T>
void print(const T& vec,const std::string& name)
{
  std::cout<<name<<" = { ";
  for(auto& i:vec)
    std::cout<<i<<" ";
  std::cout<<"}"<<std::endl;
}

void convert(const int& N_, const int& M_,const std::vector<int>& Ap,const std::vector<int>& Ai,const std::vector<double>& Ax)
{
  // count the number of nonzeros per column
  std::vector<int> colstart_(M_+1,0);
  int Nnz_(0);
  int count(0);
  for(int i=0;i!=N_;++i)
  {
    Nnz_+=(Ap[i+1]-Ap[i]);
    while(count<Nnz_)
    {
      ++(colstart_[Ai[count]+1]);
      ++count;
    }
  }

  // compute the starting positions
  std::vector<int> tempPos(M_,0);
  for(int i=1;i!=(M_+1);++i)
  {
    colstart_[i]+=colstart_[i-1];
    tempPos[i-1]=colstart_[i-1];
  }

  // fill the values and the index vector
  std::vector<double> values_(Nnz_);
  std::vector<int> rowindex_(Nnz_);
  count=0;
  for(int i=0;i!=N_;++i)
  {
    int localCount(0);
    while(localCount<(Ap[i+1]-Ap[i]))
    {
      values_[tempPos[Ai[count]]]=Ax[count];
      rowindex_[tempPos[Ai[count]]]=i;
      ++(tempPos[Ai[count]]);
      ++localCount;
      ++count;
    }
  }

  // output
  print(colstart_,"Ap'");
  print(rowindex_,"Ai'");
  print(values_,"Ax'");
}


int main()
{
  int N(4);
  int M(4);
  std::vector<int> Ap={0,3,4,6,8};
  std::vector<int> Ai={0,2,3,2,0,1,0,1};
  std::vector<double> Ax={1.,2.,3.,4.,1.,5.,2.,3.};

  print(Ap,"Ap");
  print(Ai,"Ai");
  print(Ax,"Ax");
  std::cout<<std::endl;

  convert(N,M,Ap,Ai,Ax);

  return 0;
}
