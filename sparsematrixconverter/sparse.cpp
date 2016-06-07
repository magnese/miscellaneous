#include <vector>
#include <iostream>

struct SparseRowMatrix
{
  typedef std::size_t size_type;
  typedef double value_type;

  SparseRowMatrix() = default;

  SparseRowMatrix(size_type N_,size_type M_,std::vector<size_type>&& colstart_,std::vector<size_type>&& rowindex_,
                  std::vector<value_type>&& values_):
  N(N_),M(M_),colstart(colstart_),rowindex(rowindex_),values(values_)
  {}

  void print() const
  {
    for(size_type i=0;i!=N;++i)
    {
      std::vector<value_type> row(M,0.0);
      for(size_type j=colstart[i];j!=colstart[i+1];++j)
        row[rowindex[j]]=values[j];
      for(const auto& value:row)
        std::cout<<value<<" ";
      std::cout<<std::endl;
    }
    std::cout<<std::endl;
  }

  void printAsVectors() const
  {
    std::cout<<"colstart = { ";
    for(const auto& value:colstart)
      std::cout<<value<<" ";
    std::cout<<"}"<<std::endl;
    std::cout<<"rowindex = { ";
    for(const auto& value:rowindex)
      std::cout<<value<<" ";
    std::cout<<"}"<<std::endl;
    std::cout<<"values = { ";
    for(const auto& value:values)
      std::cout<<value<<" ";
    std::cout<<"}"<<std::endl;
    std::cout<<std::endl;
  }

  size_type N;
  size_type M;
  std::vector<size_type> colstart;
  std::vector<size_type> rowindex;
  std::vector<value_type> values;
};

SparseRowMatrix transpose(const SparseRowMatrix& matrix)
{
  // create transpose matrix
  SparseRowMatrix matrixT;
  matrixT.N=matrix.N;
  matrixT.M=matrix.M;

  // count the number of nonzeros per column
  matrixT.colstart.resize(matrix.M+1,0);
  typedef typename SparseRowMatrix::size_type size_type;
  size_type Nnz(0);
  size_type count(0);
  for(size_type i=0;i!=matrix.N;++i)
  {
    Nnz+=(matrix.colstart[i+1]-matrix.colstart[i]);
    while(count<Nnz)
    {
      ++(matrixT.colstart[matrix.rowindex[count]+1]);
      ++count;
    }
  }

  // compute the starting positions
  std::vector<size_type> tempPos(matrix.M,0);
  for(size_type i=1;i!=(matrix.M+1);++i)
  {
    matrixT.colstart[i]+=matrixT.colstart[i-1];
    tempPos[i-1]=matrixT.colstart[i-1];
  }

  // fill the values and the index vector
  matrixT.values.resize(Nnz,0);
  matrixT.rowindex.resize(Nnz,0);
  count=0;
  for(size_type i=0;i!=matrix.N;++i)
  {
    size_type localCount(0);
    while(localCount<(matrix.colstart[i+1]-matrix.colstart[i]))
    {
      matrixT.values[tempPos[matrix.rowindex[count]]]=matrix.values[count];
      matrixT.rowindex[tempPos[matrix.rowindex[count]]]=i;
      ++(tempPos[matrix.rowindex[count]]);
      ++localCount;
      ++count;
    }
  }

  return matrixT;
}

int main()
{
  std::cout<<"Matrix :"<<std::endl;
  const SparseRowMatrix matrix(4,4,{0,3,4,6,8},{0,2,3,2,0,1,0,1},{1.,2.,3.,4.,1.,5.,2.,3.});
  matrix.print();
  matrix.printAsVectors();

  std::cout<<"Matrix^T :"<<std::endl;
  const auto matrixT=transpose(matrix);
  matrixT.print();
  matrixT.printAsVectors();

  return 0;
}
