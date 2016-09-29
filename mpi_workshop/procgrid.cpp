/*
 * Organize your processes in a grid, and make subcommunicators for the rows and columns. For
 * this compute the row and column number of each process.
 * In the row and column communicator, compute the rank.
 */

#include <cmath>
#include <iostream>
#include <mpi.h>

int main()
{
  MPI_Comm comm = MPI_COMM_WORLD;
  int nprocs, procno;

  // data specifically for this program
  int nrows,ncols,row_no,col_no,row_rank,col_rank;
  MPI_Comm row_comm,col_comm;

  MPI_Init(0,0);

  MPI_Comm_size(comm,&nprocs);
  MPI_Comm_rank(comm,&procno);

  // Try to arrange the processors in a grid
  for(nrows=sqrt(nprocs+1); nrows>=1; --nrows)
    if(nprocs%nrows==0)
      break;
  if(nrows==1)
  {
    if(procno==0)
      std::cout<<"Number of processes is prime"<<std::endl;
    MPI_Abort(comm,0);
  }
  ncols = nprocs/nrows;

  // Find the coordinates of this process
  row_no = procno/ncols;
  col_no = procno - ncols*row_no;

  // Make a `col_comm' communicator with all processes in this column
  MPI_Comm_split(comm,col_no,procno,&col_comm);

  // Make a `row_comm' communicator with all processes in this row
  MPI_Comm_split(comm,row_no,procno,&row_comm);

  // Now find `row_rank': the number I am in my row by using MPI_Comm_rank
  MPI_Comm_rank(row_comm,&row_rank);

  // also find `col_rank': the number I am in my column by using MPI_Comm_rank
  MPI_Comm_rank(col_comm,&col_rank);

  // Now check that the rank in the row equals the column number
  if(row_rank!=col_no)
    std::cout<<"["<<procno<<"="<<row_no<<","<<col_no<<"] wrong row rank "<<row_rank<<std::endl;

  // Now check that the rank in the column equals the row number
  if(col_rank!=row_no)
		std::cout<<"["<<procno<<"="<<row_no<<","<<col_no<<"] wrong col rank "<<col_rank<<std::endl;

  if(procno==0)
    std::cout<<"Finished"<<std::endl;

  MPI_Finalize();
  return 0;
}
