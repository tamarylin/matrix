#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


#define N 4
#define P 2

// setting up the grid
/*typedef struct{
  // 2D communicator
  MPI_Comm grid_2d; 
  
  // 1D communicator 
  MPI_Comm grid_row;
  MPI_Comm grid_col;
  
  // determining Cartesian coordinates 
  int my_row;
  int my_col;
  int ndims;
}ProcessGrid;*/


 void local_matrix_multiply(float* A, float* B, float* C, int p);


int main(){
  MPI_Init(NULL, NULL);
  
  int world_rank, world_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  
// creating the grid 
  int ndims = 2;
  int periods[2] = {1,1}; // need the column to be cyclic
  int reorder = 0;
  
  // creating dimension on each axis
  int grid_dim[2] = {0,0};
  MPI_Dims_create(world_size, 2, grid_dim);
  
  // creating the 2D plane
  MPI_Comm grid_comm;
  MPI_Cart_create(MPI_COMM_WORLD, ndims, grid_dim, periods, reorder, &grid_comm);
  
  
  // setup row and column communicators 
  int my_coords[2]= {0,0};
  int grid_rank;
  
  MPI_Comm_rank(grid_comm, &grid_rank);
  MPI_Cart_coords(grid_comm, grid_rank, 2, my_coords);
  
  int row_dim[2] = {1,0}; // for row
  MPI_Comm row_comm;
  MPI_Cart_sub(grid_comm, row_dim, &row_comm);

  MPI_Comm col_comm;
  
  int col_dim[2] = {0,1}; 
  MPI_Cart_sub(grid_comm, col_dim, &col_comm);
  
// setup local matrices 
  float* home_A = (float*)malloc((P*P)*sizeof(float));
  float* bcast_A = (float*)malloc((P*P)*sizeof(float));
  
  float* send_B = (float*)malloc((P*P)*sizeof(float));
  float* recv_B = (float*)malloc((P*P)*sizeof(float));

  float* local_C = (float*)malloc((P*P)*sizeof(float));
  
  
  // zero initialize C
  for(int i=0; i<P*P; i++){
    local_C[i] = 0;
  }
  
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
 
  
  MPI_Barrier(MPI_COMM_WORLD);
  

  for(int i=0; i<4; i++){
    for(int j=0; j<4; j++){
    int grid_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &grid_rank);
      if(i==j){
        home_A[i*P+j] = grid_rank;
        recv_B[i*P+j] = grid_rank;
      }
      else{
       home_A[i*P+j] = recv_B[i*P+j] = 0;
      }
    }
  }
  
  // iteration over fox algo
  
  
  MPI_Barrier(MPI_COMM_WORLD);
  // print your own matrix 
  int row_rank, col_rank;
  MPI_Comm_rank(row_comm, &row_rank);
  MPI_Comm_rank(col_comm, &col_rank);
  
  
  
  // trivial case 
  /*for(int i=0; i<P*P; i++){
    bcast_A[i] = home_A[i];
  }*/
  
for(int k=0; k<P; k++){

// broadcast row
  if(row_rank == k){
    // construct broadcast A
    for(int i=0; i<P*P; i++){
      bcast_A[i] = home_A[i];
    }
  }

   // collective call to broadcast
   MPI_Bcast(bcast_A, P*P, MPI_FLOAT, k, row_comm);          
   
   local_matrix_multiply(bcast_A, recv_B, local_C, P);

// switch up columns
    // copy data from recv -> send
    for(int i=0; i<P*P; i++){
      send_B[i] = recv_B[i];
    }
  
    int col_size;
    MPI_Comm_size(col_comm, &col_size);
    int up = (col_rank-1+col_size)%col_size;
    int down = (col_rank+1)%col_size;
      
    //MPI_Cart_shift(col_comm, 0, 1, &col_sender_rank, &col_recver_rank);    
    
    MPI_Status status;
    MPI_Sendrecv(send_B, P*P, MPI_FLOAT, down, 0,
                  recv_B, P*P, MPI_FLOAT, up, 0, 
                    col_comm, &status);

    
    //MPI_Barrier(grid_comm);
        
  }
  
  
  // printing the result
  float* big_C = NULL;
  MPI_Comm_rank(grid_comm, &grid_rank);
  
  if(grid_rank==0){
    big_C = (float*)malloc(sizeof(float)*N*N); 
  }
  
  MPI_Barrier(grid_comm);
  MPI_Gather(local_C, P*P, MPI_FLOAT, big_C, P*P, MPI_FLOAT, 0, grid_comm);
  
  if(grid_rank==0){
    for(int i=0; i<N; i++){
      for(int j=0; j<N; j++){
        std::cout << big_C[i*N+j] << " ";  
      }
      std::cout << "\n";
    }
  }
  
  

  
  
  MPI_Finalize();
  
  free(local_C);
  free(send_B);
  free(recv_B);
  free(home_A);
  free(bcast_A);
  free(big_C);

  
  return 0; 
}


void local_matrix_multiply(float* A, float* B, float* C, int p){
  
  for(int i=0; i<p; i++){
    for(int j=0; j<p; j++){
      float element = 0.0;
      for(int k=0; k<p; k++){
        element += A[i*p+k]*B[p*k+j];
      }
      C[i*p+j] += element;
    }
  }
  return;
}
