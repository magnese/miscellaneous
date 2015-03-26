#! /bin/sh

# update
sudo apt-get update
sudo apt-get -y dist-upgrade
sudo apt-get -y autoremove

# install packages
sudo apt-get install -y vim
sudo apt-get install -y git
sudo apt-get install -y qtcreator
sudo apt-get install -y synaptic
sudo apt-get install -y gimp
sudo apt-get install -y cmake
sudo apt-get install -y vlc
sudo apt-get install -y paraview
sudo apt-get install -y ssh
sudo apt-get install -y kile
sudo apt-get install -y gmsh
sudo apt-get install -y mercurial

# install chrome
cd /tmp
wget https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb
sudo dpkg -i google-chrome-stable_current_amd64.deb
rm google-chrome-stable_current_amd64.deb

# install java
sudo add-apt-repository -y ppa:webupd8team/java
sudo apt-get update
echo oracle-java8-installer shared/accepted-oracle-license-v1-1 select true | sudo /usr/bin/debconf-set-selections
sudo apt-get install oracle-java8-installer
sudo apt-get install oracle-java8-set-default

# configure vim
cd $HOME
echo set tabstop=2 > .vimrc
echo set shiftwidth=2 >> .vimrc
echo set expandtab >> .vimrc
echo "autocmd BufWritePre * :%s/\\s\\+$//e" >> .vimrc
