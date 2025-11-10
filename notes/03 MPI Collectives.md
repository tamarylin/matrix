- performed by all the processors 

- synchronization calls
- data movement across all 
- collective computation 


#### Synchronization 
`MPI_Barrier(MPI_COMM_WORLD);`

- blocking call 
#### `MPI_Allreduce()`
- `MPI_Reduce() + MPI_Bcast()`
- unlike `MPI_Reduce` : computed data is returned to all processes not just the root

```C
int MPI_Allreduce(
    const void *sendbuf,
    void *recvbuf,
    int count,
    MPI_Datatype datatype,
    MPI_Op op,
    MPI_Comm comm
);
```

##### `op`
- operation to be performed on the sent data
- can be pre-defined or user-defined 

#### Broadcast 
- sends data from root to all processes 

```C
MPI_Bcast(
    void* buffer,           // Data to be sent/received
    int count,              // Number of elements in buffer
    MPI_Datatype datatype,  // Data type of elements
    int root,               // Rank of sending process
    MPI_Comm communicator   // Communicator
)
```
- **root**: Rank of the process broadcasting the data
- **comm**: Communicator defining the group of processes involved
#### Scatter 
- distributes data from a single root to all the processors 
- divides an input data array into equal chunks 
```C
MPI_Scatter(
    void* send_data,           // Array on root process
    int send_count,            // Elements to send to each process
    MPI_Datatype send_datatype, // Data type of elements
    
    void* recv_data,           // Receive buffer on each process
    int recv_count,            // Elements each process receives
    MPI_Datatype recv_datatype, // Data type of received elements
    
    int root,                  // Rank of sending process
    MPI_Comm communicator      // Communicator containing all processes
)
```

#### Gather
- collects data from all processes to a root process

```C
MPI_Gather(
    void* sendbuf,           // Send buffer on each process
    int sendcount,           // Elements each process sends
    MPI_Datatype sendtype,   // Data type of send elements
    void* recvbuf,           // Receive buffer (significant only at root)
    int recvcount,           // Elements received from each process
    MPI_Datatype recvtype,   // Data type of received elements
    int root,                // Rank of receiving process
    MPI_Comm comm            // Communicator
)
```

### memory allocation 
- memory must be allocated before the function is called 
- all buffers must be pre-allocated 
- collective routines are blocking operations 
- 