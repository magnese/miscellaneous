#include <iostream>
#include <cmath>
#include <random>
#include <mpi.h>

int main()
{
  MPI_Comm comm = MPI_COMM_WORLD;
  int nprocs, procno;

  MPI_Init(0,0);

  // Compute communicator rank and size
  MPI_Comm_rank(comm,&procno);
  MPI_Comm_size(comm,&nprocs);

  // Compute the random number
  std::random_device rd;
  std::default_random_engine gen(rd());
  std::uniform_real_distribution<float> dis(0,1);
  float myrandom(dis(gen));
  std::cout<<"Process "<<procno<<" has random value "<<myrandom<<std::endl;

  // Compute the maximum random value on process zero
  float sendbuf=myrandom;
  float recvbuf;
  MPI_Reduce(&sendbuf,&recvbuf,1,MPI_FLOAT,MPI_MAX,0,comm);
  float maxrand=recvbuf;
  if(procno==0)
    std::cout<<"The maximum number is "<<maxrand<<std::endl;

  // Compute the sum of the values, everywhere and scale your number by the sum
  MPI_Allreduce(&sendbuf,&recvbuf,1, MPI_FLOAT,MPI_SUM,comm);
  float sumrandom=recvbuf;
  float scaled_random = myrandom / sumrandom;
  sendbuf=scaled_random;

  // Check that the sum of scales values is 1
  MPI_Allreduce(&sendbuf,&recvbuf,1, MPI_FLOAT,MPI_SUM,comm);
  sumrandom=recvbuf;
  if(abs(sumrandom-1.)>1.e-14)
    std::cout<<"Suspicious sum "<<sumrandom<<" on process "<<procno<<std::endl;

  if(procno==0)
    std::cout<<"Finished"<<std::endl;;

  MPI_Finalize();
  return 0;
}
