#include "stdio.h"
#include "stdlib.h"
#include "mpi.h"

#define N 1024


int main(){
  float *A; 
  float *B; 
  float *C;
  float *row;
  float *partial;
  
  int rank, size;
  


// set up environment 
  MPI_Init(NULL, NULL);
  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  int block = N/size;
  
// allocate memory for send and recv buffers   

  partial = (float*)malloc(block*N*sizeof(float));
  row = (float*)malloc(block*N*sizeof(float));
  
  if(rank==0){
  A = (float*)malloc(N*N*sizeof(float));
  B = (float*)malloc(N*N*sizeof(float));
  C = (float*)malloc(N*N*sizeof(float));  
  }
  else{
  A = NULL;
  B = (float*)malloc(N*N*sizeof(float));
  C = NULL;
  }
  
// master process 
  if(rank==0){
       
    // initialize A, B, C    
    for(int i=0; i<N; i++){
      for(int j=0; j<N; j++){
        /*A[i*N+j] = rand()/(float)RAND_MAX;
        B[i*N+j] = rand()/(float)RAND_MAX;
        C[i*N+j] = 0.0f;*/

      if(i==j){
        A[i*N+j] = 1;
        B[i*N+j] = 1;
        }
      else{
        A[i*N+j] = 0;
        B[i*N+j] = 0;
        }

        
      }
    }
  }
  
    // initialize row & partial 
    /*for(int i=0; i<block; i++){
      for(int j=0; j<N; j++){
        row[i*N+j] = partial[i*N+j] = 0.0f;
      }
    }
    */
  
  // where MPI computation really starts 
  MPI_Barrier(MPI_COMM_WORLD);
  double start_time = MPI_Wtime();
    
// row distribution     
    // broadcast B 
    MPI_Bcast(B, N*N, MPI_FLOAT, 0, MPI_COMM_WORLD);
    // scatter A (row wise)
    MPI_Scatter(A, block*N, MPI_FLOAT, row, block*N, MPI_FLOAT, 0, MPI_COMM_WORLD);  
  


  
// compute on all processes 
MPI_Barrier(MPI_COMM_WORLD);
  double comp_start = MPI_Wtime();
  
 for(int i=0; i<block; i++){
   for(int j=0; j<N; j++){
     
     float element = 0.0f;
     
     for(int k=0; k<N; k++){
       element += row[i*N+k]*B[k*N+j];
     }
     
     partial[i*N+j] = element; 
   }
 }
 
 MPI_Barrier(MPI_COMM_WORLD);
 double comp_end = MPI_Wtime();
 double comp_time = comp_end - comp_start;

 MPI_Gather(partial, block*N, MPI_FLOAT, C, block*N, MPI_FLOAT, 0, MPI_COMM_WORLD);
 
 MPI_Barrier(MPI_COMM_WORLD);
 double end_time = MPI_Wtime();
 double local_time = end_time - start_time;
  

 double agg_time = 0.0;
 double comp_agg_time = 0.0;

 MPI_Reduce(&local_time, &agg_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
 MPI_Reduce(&comp_time, &comp_agg_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

 // outputing the result 
  if(rank==0){
  agg_time /= (float)size;
  comp_agg_time /= (float)size;
    printf("matrix dimensions  : %d\n", N);
    printf("total time elapsed : %f\n" , agg_time);
    printf("computation time elapsed : %f\n" , comp_agg_time);
    printf("communication time elapsed : %f\n" , agg_time-comp_agg_time);
    
    double n = (2.0)*N*N*N;
    printf("GLOPS : %.6f\n", (n)/(agg_time*1e9));
    printf("communication overhead : %.6f\n", ((agg_time-comp_agg_time)*100)/(agg_time));
  } 
  
  

  
  
  
 
 MPI_Finalize();
  free(partial);
  free(row);
  free(A);
  free(B);
  free(C);
  return 0;
}

