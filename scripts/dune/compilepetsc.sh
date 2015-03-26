#This script configure and install PETSc
#Author: Marco Agnese (m.agnese13@imperial.ac.uk)

PATH_PETSC_SOURCE=/usr/local_machine/petsc-3.5.0-source
PATH_PETSC_INSTALL=/usr/local_machine/petsc-3.5.0

echo "Installing PETSC in directory : "
echo $PATH_PETSC_INSTALL

cd $PATH_PETSC_SOURCE
mkdir -p $PATH_PETSC_INSTALL

./configure --prefix=$PATH_PETSC_INSTALL --download-superlu_dist --download-hypre --with-metis-include=/usr/local_machine/parmetis-4.0.3/include --with-metis-lib="-L/usr/local_machine/parmetis-4.0.3/lib -lmetis" --with-parmetis-include=/usr/local_machine/parmetis-4.0.3/include --with-parmetis-lib="-L/usr/local_machine/parmetis-4.0.3/lib -lparmetis -lmetis" PETSC_DIR=$PATH_PETSC_SOURCE PETSC_ARCH=linux-gnu-c --with-cc=mpicc --with-cxx=mpicxx --with-fc=mpif90 --with-c2html=0

make PETSC_DIR=$PATH_PETSC_SOURCE PETSC_ARCH=linux-gnu-c

make install
