#include <iostream>
#include <mpi.h>

int main()
{
  MPI_Comm comm = MPI_COMM_WORLD;
  int nprocs, procno;

  MPI_Init(0,0);

  MPI_Comm_size(comm,&nprocs);
  MPI_Comm_rank(comm,&procno);

  int mydata=1,leftdata=0,rightdata=0;
  int sendto,recvfrom;
  int firstproc(0),lastproc(nprocs-1);

  // Set `sendto' and `recvfrom' twice once to get data from the left and from the right

  // First get left neighbour data
  if(procno!=firstproc)
    recvfrom = procno-1;
  else
    recvfrom = MPI_PROC_NULL;
  if(procno!=lastproc)
    sendto = procno+1;
  else
    sendto = MPI_PROC_NULL;
  MPI_Sendrecv(&mydata,1,MPI_INT,sendto,0,&leftdata,1,MPI_INT,recvfrom,0,comm,MPI_STATUS_IGNORE);

  // Then the right neighbour data
  if(procno!=lastproc)
    recvfrom = procno+1;
  else
    recvfrom = MPI_PROC_NULL;
  if(procno!=firstproc)
    sendto = procno-1;
  else
    sendto = MPI_PROC_NULL;
  MPI_Sendrecv(&mydata,1,MPI_INT,sendto,0,&rightdata,1,MPI_INT,recvfrom,0,comm,MPI_STATUS_IGNORE);

  // Check correctness
  mydata = mydata+leftdata+rightdata;
  if(procno==0 || procno==nprocs-1)
  {
    if(mydata!=2)
      std::cout<<"Data on proc "<<procno<<" should be 2, not "<<mydata<<std::endl;
  }
  else
  {
    if(mydata!=3)
      std::cout<<"Data on proc "<<procno<<" should be 3, not "<<mydata<<std::endl;
  }

  if(procno==0)
    std::cout<<"Finished"<<std::endl;

  MPI_Finalize();
  return 0;
}

