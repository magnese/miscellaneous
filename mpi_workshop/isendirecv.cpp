#include <iostream>
#include <mpi.h>

int main()
{

  MPI_Comm comm = MPI_COMM_WORLD;
  int nprocs, procno;

  MPI_Init(0,0);

  MPI_Comm_size(comm,&nprocs);
  MPI_Comm_rank(comm,&procno);

  int mydata(1),leftdata(0),rightdata(0);
  int sendto,recvfrom;
  MPI_Request requests[4];
  int firstproc(0),lastproc(nprocs-1);

  // Set `sendto' and `recvfrom' twice once to get data from the left and from the right

  // First specify sending to the right
  if(procno!=firstproc)
    recvfrom = procno-1;
  else
    recvfrom = MPI_PROC_NULL;
  if(procno!=lastproc)
    sendto = procno+1;
  else
    sendto = MPI_PROC_NULL;
  MPI_Isend(&mydata,1,MPI_INT,sendto,0,comm,&(requests[0]));
  MPI_Irecv(&leftdata,1,MPI_INT,recvfrom,0,comm,&(requests[1]));

  // Then sending to the left
  if(procno!=lastproc)
    recvfrom = procno+1;
  else
    recvfrom = MPI_PROC_NULL;
  if(procno!=firstproc)
    sendto = procno-1;
  else
    sendto = MPI_PROC_NULL;
  MPI_Isend(&mydata,1,MPI_INT,sendto,0,comm,&(requests[2]));
  MPI_Irecv(&rightdata,1,MPI_INT,recvfrom,0,comm,&(requests[3]));

  // Now make sure all Isend/Irecv operations are completed
  MPI_Waitall(4,requests,MPI_STATUSES_IGNORE);

  // Check correctness
  mydata = mydata+leftdata+rightdata;
  if(procno==0||procno==nprocs-1)
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
