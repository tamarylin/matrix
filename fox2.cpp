#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <iostream> // Added for std::cout

// Global matrix dimension (N x N)
#define N 4
// Process grid dimension (P x P)
#define P 2

// Local matrix dimension (each process holds a block of size LOCAL_DIM x LOCAL_DIM)
// This is N/P. Your original code used 'P', which only worked
// because N=4 and P=2, making N/P == P. This is now robust.
#define LOCAL_DIM (N / P)

/**
 * @brief Performs local matrix multiplication: C = C + A * B
 * @param A Matrix A
 * @param B Matrix B
 * @param C Matrix C (result is accumulated here)
 * @param p The dimension of the local matrices (LOCAL_DIM)
 */
void local_matrix_multiply(float* A, float* B, float* C, int p);


int main() {
    MPI_Init(NULL, NULL);
    
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size != P * P) {
        if (world_rank == 0) {
            fprintf(stderr, "Error: Number of processes must be %d (P*P)\n", P * P);
        }
        MPI_Finalize();
        return 1;
    }

    // 1. --- Create the 2D Cartesian Grid ---
    int ndims = 2;
    int periods[2] = {1, 1}; // Both dimensions are periodic (torus)
    int reorder = 0;
    int grid_dim[2] = {P, P}; // Explicitly set grid dimensions
    
    MPI_Comm grid_comm;
    MPI_Cart_create(MPI_COMM_WORLD, ndims, grid_dim, periods, reorder, &grid_comm);
    
    
    // 2. --- Get coords and setup row/col communicators ---
    int my_coords[2];
    int grid_rank;
    MPI_Comm_rank(grid_comm, &grid_rank);
    MPI_Cart_coords(grid_comm, grid_rank, 2, my_coords);
    
    // Row communicator
    int row_dim[2] = {0, 1}; // Keep row (dim 0) false, vary col (dim 1)
    MPI_Comm row_comm;
    MPI_Cart_sub(grid_comm, row_dim, &row_comm);

    // Column communicator
    int col_dim[2] = {1, 0}; // Vary row (dim 0), keep col (dim 1) false
    MPI_Comm col_comm;
    MPI_Cart_sub(grid_comm, col_dim, &col_comm);
    
    // 3. --- Setup local matrices ---
    // Note: Local dimension is LOCAL_DIM (which is N/P)
    float* home_A = (float*)malloc((LOCAL_DIM * LOCAL_DIM) * sizeof(float));
    float* bcast_A = (float*)malloc((LOCAL_DIM * LOCAL_DIM) * sizeof(float));
    
    float* send_B = (float*)malloc((LOCAL_DIM * LOCAL_DIM) * sizeof(float));
    float* recv_B = (float*)malloc((LOCAL_DIM * LOCAL_DIM) * sizeof(float)); // This holds the initial local B

    float* local_C = (float*)malloc((LOCAL_DIM * LOCAL_DIM) * sizeof(float));
    for (int i = 0; i < LOCAL_DIM * LOCAL_DIM; i++) {
        local_C[i] = 0;
    }
    
    // 4. --- Initialize Input Matrices (A and B) ---
    // We are multiplying two 4x4 identity matrices
    // Only diagonal processes (P0,0 and P1,1) hold non-zero blocks
    if (my_coords[0] == my_coords[1]) {
        for (int i = 0; i < LOCAL_DIM; i++) {
            for (int j = 0; j < LOCAL_DIM; j++) {
                if (i == j) {
                    home_A[i * LOCAL_DIM + j] = 1;
                    recv_B[i * LOCAL_DIM + j] = 1; // This is the initial local B block
                } else {
                    home_A[i * LOCAL_DIM + j] = 0;
                    recv_B[i * LOCAL_DIM + j] = 0;
                }
            }
        }
    } else {
        // Off-diagonal processes hold zero blocks
        for (int i = 0; i < LOCAL_DIM * LOCAL_DIM; i++) {
            home_A[i] = 0;
            recv_B[i] = 0;
        }
    }

    MPI_Barrier(grid_comm);

    // 5. --- Main Fox Algorithm Loop ---
    // The algorithm has P stages
    for (int k = 0; k < P; k++) {
        
        // --- a) Broadcast A block ---
        // In stage k, the process on the k-th (wrapped) diagonal broadcasts.
        // For row 'my_coords[0]', the broadcasting column is (my_coords[0] + k) % P
        int broadcast_root_col = (my_coords[0] + k) % P;

        // The root's rank *in the row communicator* is just its column index
        int broadcast_root_rank = broadcast_root_col;

        // The root process copies its local A to the broadcast buffer
        if (my_coords[1] == broadcast_root_col) {
            for (int i = 0; i < LOCAL_DIM * LOCAL_DIM; i++) {
                bcast_A[i] = home_A[i];
            }
        }

        // Broadcast bcast_A to all processes in the same row
        MPI_Bcast(bcast_A, LOCAL_DIM * LOCAL_DIM, MPI_FLOAT, broadcast_root_rank, row_comm);
        
        // --- b) Local multiplication ---
        // C_local = C_local + A_bcast * B_local (B_local is in recv_B)
        local_matrix_multiply(bcast_A, recv_B, local_C, LOCAL_DIM);
        
        // --- c) Shift B blocks UP by one ---
        // First, copy current B block (recv_B) to send buffer (send_B)
        for (int i = 0; i < LOCAL_DIM * LOCAL_DIM; i++) {
            send_B[i] = recv_B[i];
        }
        
        int rank_source; // Process we receive from (below us)
        int rank_dest;   // Process we send to (above us)
        
        // Shift UP by 1 (displacement -1).
        // 'rank_source' is the rank of the process *below* (displacement +1).
        // 'rank_dest' is the rank of the process *above* (displacement -1).
        // This is confusing, so let's use the correct displacement: -1
        MPI_Cart_shift(col_comm, 0, -1, &rank_source, &rank_dest);
        
        MPI_Status status;
        // Use MPI_Sendrecv to simultaneously send our B up and receive from B below.
        // This is non-blocking and prevents deadlock.
        MPI_Sendrecv(send_B, LOCAL_DIM * LOCAL_DIM, MPI_FLOAT, rank_dest, k,
                     recv_B, LOCAL_DIM * LOCAL_DIM, MPI_FLOAT, rank_source, k,
                     col_comm, &status);
    }
    
    // 6. --- Gather the result on root (rank 0) ---
    float* big_C = NULL;
    
    if (grid_rank == 0) {
        big_C = (float*)malloc(sizeof(float) * N * N);
    }
    
    MPI_Barrier(grid_comm);
    // Gather all 'local_C' blocks into 'big_C' on rank 0
    MPI_Gather(local_C, LOCAL_DIM * LOCAL_DIM, MPI_FLOAT,
               big_C, LOCAL_DIM * LOCAL_DIM, MPI_FLOAT, 0, grid_comm);
    
    // 7. --- Print the result from root ---
    if (grid_rank == 0) {
        std::cout << "Result C matrix (N x N):\n";
        for (int i = 0; i < N; i++) { // global row
            for (int j = 0; j < N; j++) { // global col
                
                // --- This is the key logic to fix printing ---
                // Map global (i, j) to the 1D gathered buffer
                
                // 1. Which process block is (i, j) in?
                int proc_row = i / LOCAL_DIM;
                int proc_col = j / LOCAL_DIM;
                // 2. What is the rank of that process in grid_comm?
                int proc_rank = proc_row * P + proc_col;
                
                // 3. What is the local (i, j) within that block?
                int local_i = i % LOCAL_DIM;
                int local_j = j % LOCAL_DIM;
                int local_idx = local_i * LOCAL_DIM + local_j;
                
                // 4. Final index in the 1D 'big_C' buffer
                // (Start of the block) + (offset within the block)
                int global_idx = (proc_rank * LOCAL_DIM * LOCAL_DIM) + local_idx;
                
                std::cout << big_C[global_idx] << " ";
            }
            std::cout << "\n";
        }
    }
    
    // 8. --- Cleanup ---
    MPI_Finalize();
    
    free(local_C);
    free(send_B);
    free(recv_B);
    free(home_A);
    free(bcast_A);
    if (grid_rank == 0) {
        free(big_C);
    }
    
    return 0;
}

/**
 * @brief Performs local matrix multiplication: C = C + A * B
 * All matrices are 1D arrays representing p x p matrices.
 */
void local_matrix_multiply(float* A, float* B, float* C, int p) {
    for (int i = 0; i < p; i++) {
        for (int j = 0; j < p; j++) {
            float element = 0.0;
            for (int k = 0; k < p; k++) {
                // C[i][j] += A[i][k] * B[k][j]
                // A[i][k] -> A[i*p + k]
                // B[k][j] -> B[k*p + j]
                element += A[i * p + k] * B[k * p + j]; // Your indexing here was correct!
            }
            C[i * p + j] += element;
        }
    }
    return;
}
