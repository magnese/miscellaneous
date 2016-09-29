#include <iostream>
#include <mpi.h>

int main()
{
  MPI_Comm comm = MPI_COMM_WORLD;
  int nprocs, procno;

  MPI_Init(0,0);

  // Let each processor print out a message like "Process procno out of nprocs"
  MPI_Comm_rank(comm,&procno);
  MPI_Comm_size(comm,&nprocs);
  std::cout<<"Process "<<procno<<" out of "<<nprocs<<std::endl;

  // Let only processs zero print out "There are in total nprocs processes"
  if(procno==0)
    std::cout<<"There are in total "<<nprocs<<" processes"<<std::endl;

  MPI_Finalize();
  return 0;
}
