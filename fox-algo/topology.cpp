#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


int main(){
  int rank, size;
  MPI_Init(NULL, NULL);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  
    
  int ndim = 2;
  int grid_dim[2] = {0,0};
  int periods[2] = {0,1};
  int reorder = 1;
  MPI_Comm comm_2d;
  
  // tries to balance the grid ass much as possible 
  MPI_Dims_create(size, ndim, grid_dim);
  
  if(rank==0){
    std::cout << "grid determined : " << grid_dim[0] << " " << grid_dim[1] << " \n";
  }
  
  int ierr = MPI_Cart_create(MPI_COMM_WORLD, ndim, grid_dim, periods, reorder, &comm_2d);
  
  if(ierr != MPI_SUCCESS){
    std::cout << "error\n";
    return 1;
  }
  

  
  if(rank==0){
    std::cout << "successfully created 2D Cartesian topology.\n";
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  
  int cart_rank;
  int coords[2];
  
  MPI_Comm_rank(comm_2d, &cart_rank);
  MPI_Cart_coords(comm_2d, cart_rank, ndim, coords);
  
  // std::cout << "World_Rank : " << rank << " 2D_Rank : " << cart_rank << " -> Coordinates : (" << coords[0] << ", " << coords[1] << ") \n";
  
  MPI_Comm row_comm;
  MPI_Comm col_comm;
  
  int dim_remain[2] = {1,0}; // row
  MPI_Cart_sub(comm_2d, dim_remain, &row_comm);
  dim_remain[2] = {0,1};
  MPI_Cart_sub(comm_2d, dim_remain, &col_comm);
  
  // topology for communications 
  
  // 1) broadcast along rows
  int row_rank; 
  MPI_Comm_rank(row_comm, &row_rank);
  
  int row_root = 0;
  int transmit = 100;
  MPI_Bcast(&transmit, 1, MPI_INT, row_root, row_comm);
  
  // 2) cycling among cols 
  
  
  MPI_Finalize();
  return 0;
}
