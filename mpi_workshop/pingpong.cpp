#include <iostream>
#include <mpi.h>

constexpr int nexperiments=100;
constexpr int processA=0;
constexpr int processB=1;

int main()
{
  MPI_Comm comm = MPI_COMM_WORLD;
  int nprocs, procno;

  MPI_Init(0,0);

  MPI_Comm_size(comm,&nprocs);
  MPI_Comm_rank(comm,&procno);

  // Exercise:
  // -- set source and target processors two ways:
  //    close together and far apart
  // -- run the experiment both ways.
  double t, send[10000],recv[10000]; send[0] = 1.1;
  if(procno==processA)
	{
    t = MPI_Wtime();
    for(int n=0; n<nexperiments; ++n)
		{
      MPI_Send(send,1,MPI_DOUBLE,processB,0,comm);
      MPI_Recv(recv,1,MPI_DOUBLE,processB,0,comm,MPI_STATUS_IGNORE);
    }
    t = MPI_Wtime()-t;
    t /= nexperiments;
    std::cout<<"Time for pingpong: "<<t<<std::endl;
  }
  else if(procno==processB)
  {
    for(int n=0; n<nexperiments; ++n)
    {
      MPI_Recv(recv,1,MPI_DOUBLE,processA,0,comm,MPI_STATUS_IGNORE);
      MPI_Send(recv,1,MPI_DOUBLE,processA,0,comm);
    }
  }

  MPI_Finalize();
  return 0;
}
