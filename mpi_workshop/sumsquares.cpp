#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>

int main(int argc,char **argv)
{
  MPI_Comm comm = MPI_COMM_WORLD;
  int nprocs, procno;

  MPI_Init(&argc,&argv);

  MPI_Comm_size(comm,&nprocs);
  MPI_Comm_rank(comm,&procno);

  int nlocal,nglobal;
  if(procno==0)
  {
    if(argc>1)
      nlocal = atoi(argv[1]);
    else
      nlocal = 10000;
  }
  MPI_Bcast(&nlocal,1,MPI_INTEGER,0,comm);
  nglobal = nprocs*nlocal;

  // Allocate local data. We use doubles rather than integers (why?)
  std::vector<double> local_squares(nlocal);

  // Set your local values
  for(int i=0; i<nlocal; ++i)
    local_squares[i]=std::pow(i+1+nlocal*procno,2);

  // Do the local summation
  double local_sum(0);
  for(auto&& entry:local_squares)
    local_sum+=entry;

  // Do the global summation
  double global_sum;
  MPI_Reduce(&local_sum,&global_sum,1,MPI_DOUBLE,MPI_SUM,0,comm);

  if(procno==0)
    std::cout<<"Global sum: "<<global_sum<<", should be "<<(2.*nglobal*nglobal*nglobal + 3.*nglobal*nglobal + nglobal)/6.<<std::endl;

  MPI_Finalize();
  return 0;
}

