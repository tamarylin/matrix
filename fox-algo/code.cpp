#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

using namespace std;


typedef struct{
 MPI_Comm comm_2d;
 MPI_Comm row_comm;
 MPI_Comm col_comm;
 
 int my_row;
 int my_col;
 int dim;
 

}ProcessGrid; 

int main(){

  
  MPI_Init(NULL, NULL);

 
  MPI_Finalize();
  
  
  return 0;
}


