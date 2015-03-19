#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

int main(int argc,char** argv)
{

  std::string user(getenv("USER"));
  std::string pwd(getenv("PWD"));

  if(argc<3)
  {
    std::cout<<"qsubb v0.2"<<std::endl;
    std::cout<<"Description:\t qsubb creates a script and submit the job to qsub."<<std::endl;
    std::cout<<"Usage:\t\t qsubb program_name output_name [node_number](01 default)"<<std::endl;
    std::cout<<"\t\t Move to the folder where the program is saved."<<std::endl;
    std::cout<<"\t\t The output will be saved in /scratchcompXX/"<<user<<"/output_name/ where XX is the node_number."<<std::endl;
    std::cout<<"Author:\t\t Marco Agnese (m.agnese13@imperial.ac.uk)."<<std::endl;
  }
  else
  {
    std::string program_name(argv[1]);
    std::string output_name(argv[2]);
    std::string output_node("01");

    if(argc>3)
      output_node=argv[3];

    const std::string qsub_script_name(program_name+"_qsub.sh");
    std::ofstream qsub_script(qsub_script_name);
    if(qsub_script.is_open())
    {
      qsub_script<<"#! /bin/bash"<<std::endl;
      qsub_script<<"#PBS -N "<<program_name<<std::endl;
      qsub_script<<"#PBS -m abe"<<std::endl;
      qsub_script<<"#PBS -q ubuntu_standard"<<std::endl;
      qsub_script<<"#PBS -o /scratchcomp"<<output_node<<"/"<<user<<"/"<<output_name<<"/"<<output_name<<".out"<<std::endl;
      qsub_script<<"#PBS -e /scratchcomp"<<output_node<<"/"<<user<<"/"<<output_name<<"/"<<output_name<<".err"<<std::endl;
      qsub_script<<"mkdir -p /scratchcomp"<<output_node<<"/"<<user<<std::endl;
      qsub_script<<"mkdir -p /scratchcomp"<<output_node<<"/"<<user<<"/"<<output_name<<std::endl;
      qsub_script<<"cd /scratchcomp"<<output_node<<"/"<<user<<"/"<<output_name<<std::endl;
      qsub_script<<pwd<<"/"<<program_name<<std::endl;
      qsub_script.close();
    }
    else
    {
      std::cout<<"ERROR: Impossible to create the file "<<qsub_script_name<<" ."<<std::endl;
      return 1;
    }

    std::string command_chmod="chmod 755 "+qsub_script_name;
    system(command_chmod.c_str());

    std::string command_qsub="qsub "+qsub_script_name;
    system(command_qsub.c_str());
  }

  return 0;
}
