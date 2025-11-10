## MPI Code 
- we try to distribute the workload by paralyzing multiply-add operations 

#### Master Slave Paradigms
- matrix multiplication is distributed over rows 
- `AB = [r1 r2 ... rn][B] = [r1 r2 ... rn][B]`

##### Master 
- input the matrices in master process (Rank 0 : `A,B`)
- broadcast A to each process
- divide the into columns and scatter the data to each process (slaves) (process i : `col_i`)
- gather back the data into the master process (rank 0)




## Profiling 
- we need to profile both communication pattern and computational performance 

#### MPI performance metrics 
1) load balance (`LBE`) : measure of workload distribution
2) communication efficiency (`ComE`) : Serialization Efficiency (time taken by MPI routines) & Transfer Efficiency (time taken by the network for data transfers)
3) computation efficiency : time taken after increasing processor count 

4) MPI imbalance : average idle time
5) parallel efficiency : `LBE*ComE`


