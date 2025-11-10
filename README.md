# matrix

## MPI Code  
- we try to distribute the workload by paralyzing multiply-add operations  
  
##### Master Slave Paradigms  
- matrix multiplication is distributed over rows  
- `AB = [r1 r2 ... rn][B] = [r1 r2 ... rn][B]`  
###### Master  
- input the matrices in master process (Rank 0 : `A,B`)  
- broadcast A to each process  
- divide the into columns and scatter the data to each process (slaves) (process i : `col_i`)  
- gather back the data into the master process (rank 0)  
  
  
  
## Profiling  
- we need to profile both communication pattern and computational performance  
##### MPI performance metrics  
1) load balance (`LBE`) : measure of workload distribution  
2) communication efficiency (`ComE`) : Serialization Efficiency (time taken by MPI routines) & Transfer Efficiency (time taken by the network for data transfers)  
3) computation efficiency : time taken after increasing processor count  
  
4) MPI imbalance : average idle time  
5) parallel efficiency : `LBE*ComE

##### Performance Analysis 

| dimensions | time elapsed (#2) | time elapsed (#4) |
| ---------- | ----------------- | ----------------- |
| 128        | `0.002357`        | `0.002245`        |
| 256        | `0.050543`        | `0.033461`        |
| 512        | `0.165898`        | `0.211654`        |
| 1024       | `2.636257`        | `2.054840`        |

##### calculating GLOPS and communication overheads 
- `GLOPS` : giga floating point operations 
- `FLOP` operations for matrix multiplication : `n*n*n` multiply-add operations 

- `GLOPS = FLOPS/(elasped * 10^9)`
#### updated performance matrix 
##### dimensions

| dimensions | computation time | communication time | GLOPS  | communication overhead |
| ---------- | ---------------- | ------------------ | ------ | ---------------------- |
| 128        | `0.008148`       | `0.000071`         | `0.51` | `0.853`                |
| 256        | `0.064483`       | `0.000295`         | `0.52` | `0.491`                |
| 512        | `0.525295`       | `0.001300`         | `0.51` | `0.168`                |
| 1024       | `6.032553`       | `0.004552`         | `0.35` | `0.076`                |
###### conclusions
1) 
2) GLOPS: decreases with increase in matrix size (why?)
3) communication overhead dominates for smaller matrices 
4) 

##### number of processors : 512

| number of processors | computation time | communication time | GLOPS  | communication overhead |
| -------------------- | ---------------- | ------------------ | ------ | ---------------------- |
| 1                    | `0.586937`       | `0.001246`         | `0.45` | `0.212`                |
| 2                    | `0.356732`       | `0.002124`         | `0.60` | `0.592`                |
| 4                    | `0.191183`       | `0.004392`         | `1.37` | `2.246`                |
| 8                    | `0.130497`       | `0.047329`         | `1.51` | `26.615`               |

### optimizations 
