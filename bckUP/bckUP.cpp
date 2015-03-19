#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

const std::string Time()
{
  time_t now(time(0));
  struct tm tstruct;
  char buf[80];
  tstruct=*localtime(&now);
  strftime(buf,sizeof(buf),"%d-%m-%Y-%X",&tstruct);
  return buf;
}

int main(int argc,char** argv)
{
  std::string user(getenv("USER"));
  std::string home(getenv("HOME"));

  std::cout<<"bckUP v0.1"<<std::endl;
  std::cout<<"Description: bckUP create a tar archive of your home folder in /scratch/"<<user<<"/backups"<<std::endl;
  std::cout<<"Optional: providing your computer name (e.g. ./bckUP computername), bckUP will also copy the tar archve in ~/backups/ of your local machine"<<std::endl;
  std::cout<<"Author:\t\t Marco Agnese (m.agnese13@imperial.ac.uk)."<<std::endl<<std::endl;

  std::string fileName("backup_"+Time()+".tar.gz");
  std::string archiveFullPath("/scratch/"+user+"/backups/"+fileName);

  std::string commandMkdir("mkdir -p /scratch/"+user);
  system(commandMkdir.c_str());
  commandMkdir+="/backups";
  system(commandMkdir.c_str());

  std::string commandArchive("tar cf "+archiveFullPath+" ~/*");
  system(commandArchive.c_str());

  std::cout<<"Your home folder has been backed-up in "<<archiveFullPath<<std::endl<<std::endl;

  if(argc>1)
  {
    std::string computerName(argv[1]);
    std::cout<<"Prompt the password of your local machine to start the copy"<<std::endl;
    std::string commandScp("scp "+archiveFullPath+" "+user+"@"+computerName+".ma.ic.ac.uk:~/backups/"+fileName);
    system(commandScp.c_str());
    std::cout<<"Your home folder has been backed-up in "<<computerName<<".ma.ic.ac.uk:~/backups/"<<fileName<<std::endl<<std::endl;
  }

  return 0;
}

