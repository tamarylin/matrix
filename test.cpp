#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>



int main(){
  MPI_Init(NULL, NULL);

// world rank and size
  int world_size, world_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  
// setting u parameters for 2-D system
  int ndims = 2;
  
  int dims[2] = {0,0};
  MPI_Dims_create(world_size, 2, dims);
  
  int periods[2] = {1,1};
  int reorder = 0;
  
  MPI_Comm grid_comm;
  MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &grid_comm);
  
  int grid_rank;
  MPI_Comm_rank(grid_comm, &grid_rank);
  
// get ranks on carts
  MPI_Comm row_comm;
  MPI_Comm col_comm;

  int col_remain[2] = {0,1};
  int row_remain[2] ={1,0};
  
  MPI_Cart_sub(grid_comm, row_remain, &row_comm);
  MPI_Cart_sub(grid_comm, col_remain, &col_comm);


  int row_rank;
  int col_rank;
  MPI_Comm_rank(row_comm, &row_rank);
  MPI_Comm_rank(col_comm, &col_rank);
  
  
// initialize data
  float* data = (float*)malloc(2*2*sizeof(float));

  int proc_coords[2] = {0,0};
  MPI_Cart_coords(grid_comm, grid_rank, 2, proc_coords);
  
    for(int i=0; i<2; i++){
      for(int j=0; j<2; j++){
        if(i==j)
         data[i*2+j] = grid_rank;
        else
         data[i*2+j] = 0;
      }
    }

  
  for(int i=0; i<2; i++){
    for(int j=0; j<2; j++){
      std::cout << data[i*2+j] << " ";
    }
    std::cout << "\n";
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  //
  
  float** database;
  
    database = (float**)malloc(4*sizeof(float*));
    
    for(int i=0; i<4; i++){
      database[i] = (float*)malloc(4*sizeof(float));
    }
      


  
  MPI_Send(data, 2*2, MPI_FLOAT, 0, 0, grid_comm);
  
  if(grid_rank==0){
  for(int i=0; i<4; i++){
    MPI_Status status;
    MPI_Recv(database[i], 2*2, MPI_FLOAT, i, 0, grid_comm, &status);
    }
  }
    
    
  if(grid_rank==0){
  float *cur1, *cur2;
  
  for(int i=0; i<4; i+=2){
    cur1 = database[i];
    cur2 = database[i+1];
    
    for(int j=0; j<8; j++){
      if(j%4<2){
        std::cout << *cur1 << " ";
        cur1++;
      }
      else{
        std::cout << *cur2 << " ";
        cur2++;
      }
      if(j%4==3){
        std::cout << "\n";
      }
    }
        
  }
  
  }                
  
  free(data);
  free(database);

  MPI_Finalize();

  return 0;
}
