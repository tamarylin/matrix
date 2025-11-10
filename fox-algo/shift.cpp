#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>




int main(){

  int size, rank;
  MPI_Init(NULL, NULL);
  
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
// create 2-d Cartesian system 
  int dims[2] = {0,0};
  int ndims = 2;
  MPI_Dims_create(size, ndims, dims);
  
  int periods[2] = {0,0};
  int reorder = false;
  
  MPI_Comm grid_2d;
  MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &grid_2d);
  
  
// find neighbors
  enum DIRECTIONS {UP, DOWN, LEFT, RIGHT};
  int neighbors[4] = {0,0,0,0};
  MPI_Cart_shift(grid_2d, 0, 1, &neighbors[UP], &neighbors[DOWN]);
  MPI_Cart_shift(grid_2d, 1, 1, &neighbors[LEFT], &neighbors[RIGHT]);
  
  std::string names[4] = {"up", "down", "left", "right"};
  
  

  
  
  int my_rank;
  MPI_Comm_rank(grid_2d, &my_rank);
  std::cout << "neighbors of " << my_rank << "\n";
  
  for(int i=0; i<4; i++){
  if(neighbors[i] != MPI_PROC_NULL)
    std::cout << names[i] << " : " << neighbors[i] << "\n";
  else 
    std::cout << "end\n";
  }
  std::cout << "- - - - - - - - -\n";

  
  
  MPI_Finalize();
  return 0;
}
