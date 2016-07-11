#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <tuple>

int main(int argc,char** argv)
{

  const std::string user(getenv("USER"));
  const std::string pwd(getenv("PWD"));

  if(argc<3)
  {
    std::cout<<"qsubb v0.4"<<std::endl;
    std::cout<<"Description:\t qsubb creates a script and submit the job to qsub."<<std::endl;
    std::cout<<"Usage:\t\t qsubb program_name output_name [node_number](01 default)"<<std::endl;
    std::cout<<"\t\t Move to the folder where the program is saved."<<std::endl;
    std::cout<<"\t\t The output will be saved in /scratchcompXX/"<<user<<"/output_name/ where XX is the node_number."<<std::endl;
    std::cout<<"Author:\t\t Marco Agnese (m.agnese13@imperial.ac.uk)."<<std::endl;
    return 0;
  }

  const std::string program_name(argv[1]);
  const std::string output_name(argv[2]);
  const std::string output_node(argc>3?argv[3]:"01");

  std::ofstream file(program_name+"_qsub.sh");
  if(!file.is_open())
  {
    std::cout<<"ERROR: Impossible to create the file "<<program_name<<"_qsub.sh ."<<std::endl;
    return 1;
  }

  file<<"#! /bin/bash"<<std::endl;
  file<<"#PBS -N "<<program_name<<std::endl;
  file<<"#PBS -m abe"<<std::endl;
  file<<"#PBS -q standard"<<std::endl;
  file<<"#PBS -o /scratchcomp"<<output_node<<"/"<<user<<"/"<<output_name<<"/"<<output_name<<".out"<<std::endl;
  file<<"#PBS -e /scratchcomp"<<output_node<<"/"<<user<<"/"<<output_name<<"/"<<output_name<<".err"<<std::endl;
  file<<"mkdir -p /scratchcomp"<<output_node<<"/"<<user<<std::endl;
  file<<"mkdir -p /scratchcomp"<<output_node<<"/"<<user<<"/"<<output_name<<std::endl;
  file<<"cd /scratchcomp"<<output_node<<"/"<<user<<"/"<<output_name<<std::endl;
  file<<"export OPENBLAS_NUM_THREADS=1"<<std::endl;
  file<<pwd<<"/"<<program_name<<std::endl;

  const std::string command_chmod("chmod 755 "+program_name+"_qsub.sh");
  std::ignore=system(command_chmod.c_str());

  const std::string command_qsub("qsub "+program_name+"_qsub.sh");
  std::ignore=system(command_qsub.c_str());

  return 0;
}
