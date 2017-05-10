#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <tuple>

int main(int argc,char** argv)
{
  const std::string user(getenv("USER"));
  const std::string pwd(getenv("PWD"));

  if(argc<2)
  {
    std::cout<<"qsubb v0.5\n";
    std::cout<<"Description:\t qsubb creates a script and submit the job to qsub.\n";
    std::cout<<"Usage:\t\t qsubb program_name [output_name](program_name default) [queue](standard default) [node_number](01 default)\n";
    std::cout<<"\t\t Move to the folder where the program is saved.\n";
    std::cout<<"\t\t The output will be saved in /scratchcompX/"<<user<<"/output_name/ where X is the node_number.\n";
    std::cout<<"Author:\t\t Marco Agnese (m.agnese13@imperial.ac.uk).\n";
    return 0;
  }

  const std::string program_name(argv[1]);
  const std::string output_name(argc>2?argv[2]:program_name);
  const std::string queue(argc>3?argv[3]:"standard");
  const std::string output_node(argc>4?argv[4]:"01");

  const std::string script_name("submit.sh");
  std::ofstream file(script_name);
  if(!file.is_open())
  {
    std::cout<<"ERROR: Impossible to create the script "<<script_name<<".\n";
    return 1;
  }

  file<<"#PBS -N "<<output_name<<"\n";
  file<<"#PBS -m abe\n";
  file<<"#PBS -q "<<queue<<"\n";
  file<<"#PBS -o /scratchcomp"<<output_node<<"/"<<user<<"/"<<output_name<<"/"<<output_name<<".out\n";
  file<<"#PBS -e /scratchcomp"<<output_node<<"/"<<user<<"/"<<output_name<<"/"<<output_name<<".err\n";
  file<<"mkdir -p /scratchcomp"<<output_node<<"/"<<user<<"\n";
  file<<"mkdir -p /scratchcomp"<<output_node<<"/"<<user<<"/"<<output_name<<"\n";
  file<<"cd /scratchcomp"<<output_node<<"/"<<user<<"/"<<output_name<<"\n";
  file<<"export OPENBLAS_NUM_THREADS=1\n";
  file<<pwd<<"/"<<program_name<<"\n";


  const std::string command_qsub("qsub "+script_name);
  std::ignore=system(command_qsub.c_str());

  return 0;
}
