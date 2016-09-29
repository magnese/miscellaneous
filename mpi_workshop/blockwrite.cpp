#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>

int main(int argc,char **argv)
{

  MPI_Comm comm;
  int nprocs,procno;
  MPI_Init(&argc,&argv);
  comm = MPI_COMM_WORLD;
  MPI_Comm_size(comm,&nprocs);
  MPI_Comm_rank(comm,&procno);

  if(nprocs<2)
  {
    std::cout<<"Need at least 2 procs"<<std::endl;
    MPI_Abort(comm,0);
  }

  // Each process has a buffer of length `nwords' containing consecutive integers
  int nwords = 3;
  std::vector<int> output_data(nwords);
  for(int iw=0; iw<nwords; ++iw)
    output_data[iw] = procno*nwords+iw+1;

  MPI_File mpifile;

  // Open a file for writing, if it doesn't exist yet it needs to be created
  MPI_File_open(comm,"blockwrite.dat",MPI_MODE_CREATE|MPI_MODE_WRONLY,MPI_INFO_NULL,&mpifile);

  // Compute an offset (in bytes!) so that processes write do disjoint parts of the file.
  int nwriters = 2;
  if(procno<nwriters)
  {
    MPI_Offset offset =(nwords*sizeof(int))*procno;
    MPI_File_write_at(mpifile,offset,output_data.data(),nwords,MPI_INT,MPI_STATUS_IGNORE);
  }

  MPI_File_close(&mpifile);

  // Check correctness of the output file
  if(procno==0)
  {
    std::ifstream file("blockwrite.dat",std::ios::binary);
    unsigned int location = 0;
    for(int ip=0; ip<nwriters; ++ip)
      for(int iw=0; iw<nwords; ++iw)
      {
        unsigned int fromfile;
        file.read(reinterpret_cast<char*>(&fromfile),sizeof(fromfile));
        if(file.eof())
        {
          std::cout<<"Premature end of file"<<std::endl;
          break;
        }
        if(fromfile!=location+1)
          std::cout<<"Error "<<location<<":"<<fromfile<<std::endl;
        ++location;
      }
    std::cout<<"Finished"<<std::endl;
  }

  MPI_Finalize();
  return 0;
}
