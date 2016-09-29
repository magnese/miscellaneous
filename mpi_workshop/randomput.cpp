#include <iostream>
#include <random>
#include <mpi.h>

int main(int argc,char **argv)
{
  MPI_Comm comm = MPI_COMM_WORLD;
  int nprocs, procno;

  // Data specific for this program
  MPI_Win the_window;
  int my_number=1, my_sum = 0, window_data, other;

  MPI_Init(0,0);

  MPI_Comm_size(comm,&nprocs);
  MPI_Comm_rank(comm,&procno);

  if(nprocs<3)
  {
    std::cout<<"Need at least 3 procs"<<std::endl;
    MPI_Abort(comm,0);
  }

  // Create random number generator
  std::random_device rd;
  std::default_random_engine gen(rd());
  std::uniform_int_distribution<> dis(1,2);

  // Take the variable `window_data' and make it into a window of size 1 integer
  MPI_Win_create(&window_data,sizeof(int),sizeof(int),MPI_INFO_NULL,comm,&the_window);

  // Put 20 numbers from proc 0 into 1 or 2, depending on random chance
  for(int c=0; c<20; ++c)
  {
    // Compute `other' which is where to put the data
    other = dis(gen);

    // Receiving procs set their window to zero
    if(procno==1 || procno==2)
      window_data = 0;

    // Now we have an epoch where proc 0 puts data either in 1 or 2
    MPI_Win_fence(0,the_window);
    if(procno==0)
      MPI_Put(&my_number,1,MPI_INT,other,0,1,MPI_INT,the_window);
    MPI_Win_fence(0,the_window);

    // Receiving procs add incoming data (or zero) to a running sum
    my_sum += window_data;
  }

  // Now check that data receive on 1 & 2 together should be 20
  if(procno>0 && procno<3)
    std::cout<<"Sum on "<<procno<<": "<<my_sum<<std::endl;
  if(procno==0)
    std::cout<<"(sums on 1 & 2 together should be 20)"<<std::endl;

  MPI_Win_free(&the_window);

  MPI_Finalize();
  return 0;
}
