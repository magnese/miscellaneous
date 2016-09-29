#include <iostream>
#include <cmath>
#include <mpi.h>

#define VERSION 0

int main()
{
  MPI_Comm comm = MPI_COMM_WORLD;
  int nprocs, procno;
  int bignum = 2000000111;

  MPI_Init(0,0);

  // Exercise:
  // -- Parallelize the do loop so that each processor
  //    tries different candidate numbers.
  // -- If a processors finds a factor, print it to the screen.
  MPI_Comm_rank(comm,&procno);
  MPI_Comm_size(comm,&nprocs);

  #if VERSION == 0
  int start(((std::sqrt(bignum)+1)/nprocs)*procno+1);
  int end(((std::sqrt(bignum)+1)/nprocs)*(procno+1));
  int increment(2);
  #else
  int start(2*procno+3);
  int end(std::sqrt(bignum)+1);
  int increment(nprocs*2);
  #endif

  for(int myfactor=start;myfactor<end;myfactor+=increment)
    if(bignum%myfactor==0)
      std::cout<<"Processor "<<procno<<" found factor "<<myfactor<<std::endl;

  MPI_Finalize();
  return 0;
}
