/*
 * Let processor 0 have an array x of length 10P, where P is the number of
 * processors. Elements 0 , P , 2P , . . . , 9P should go to processor zero,
 * 1 , P + 1 , 2P + 1 , . . . to processor 1, et cetera. Code this as a sequence of
 * send/recv calls, using a vector datatype for the send, and a contiguous buffer
 * for the receive.
 *
 * For simplicity, skip the send to/from zero. What is the most elegant solution if
 * you want to include that case?
 *
 * For testing, define the array as x [ i ] = i.
 */

#include <iostream>
#include <vector>
#include <mpi.h>

int main(int argc,char **argv)
{
  MPI_Comm comm;
	int nprocs,procno;

  MPI_Init(&argc,&argv);
  comm = MPI_COMM_WORLD;
  MPI_Comm_rank(comm,&procno);
  MPI_Comm_size(comm,&nprocs);

  if(nprocs<2)
  {
    std::cout<<"Need at least 2 procs"<<std::endl;
    MPI_Abort(comm,0);
  }

  int sender(0),localsize(10);

  if(procno==sender)
  {
    int ndata(localsize*nprocs);
    std::vector<int> data(ndata);
    for(int i=0; i<ndata; ++i)
      data[i] = i;

    MPI_Datatype scattertype;
    int count(localsize),stride(nprocs),blocklength(1);
    MPI_Type_vector(count,blocklength,stride,MPI_INT,&scattertype);
    MPI_Type_commit(&scattertype);

    for(int sendto=0; sendto<nprocs; ++sendto)
      if(sendto!=procno)
        MPI_Send(data.data()+sendto,1,scattertype,sendto,0,comm);

    MPI_Type_free(&scattertype);
  }
  else
  {
    std::vector<int> mydata(localsize);
    MPI_Recv(mydata.data(),localsize,MPI_INT,sender,0,comm,MPI_STATUS_IGNORE);
    for(int i=0; i<localsize; ++i)
      if(mydata[i]%nprocs!=procno)
	      std::cout<<"["<<procno<<"] received element="<<mydata[i]<<", should be "<<i*nprocs+procno<<std::endl;
  }

  if(procno==0)
    std::cout<<"Finished"<<std::endl;

  MPI_Finalize();
  return 0;
}

