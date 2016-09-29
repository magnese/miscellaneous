#include <iostream>
#include <mpi.h>

int main(int argc,char **argv)
{
  std::cout<<"Start"<<std::endl;

  MPI_Init(&argc,&argv);

  std::cout<<"Hello world!"<<std::endl;

  char name[MPI_MAX_PROCESSOR_NAME];
  int resultlen;
  MPI_Get_processor_name(name,&resultlen);

  for(int i=0;i<resultlen;++i)
    std::cout<<name[i];
  std::cout<<std::endl;

  MPI_Finalize();

  std::cout<<"End"<<std::endl;

  return 0;
}
