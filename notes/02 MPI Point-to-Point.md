- `message passing interface` :  standardized and portable message-passing system
- used in message passing parallel programming model , typically in a distributed memory system


- multiple process -> each assigned a rank and address space 
##### run command 
`mpirun mpiexec`
`-np 4`

- scheduler figures out itself 
```bash
mpicc pp.c : compile
mpirun -np 6 ./a.out : run
```


#### basic calls 

|                                        |                                      |
| -------------------------------------- | ------------------------------------ |
| `MPI_Comm_Size(communticator, &p)`     | total number of concurrent processes |
| `MPI_Comm_Rank(communicator, &myRank)` | rank of the current process          |
| `MPI_COMM_WORLD`                       | a communicator                       |
|                                        |                                      |




##### Send and Receive 


##### send
`int MPI_Send(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);`

###### Parameters:

- `buf`: Pointer to the buffer containing the data to send.    
- `count`: Number of elements in the buffer to send (not bytes).    
- `datatype`: Type of data elements, e.g., MPI_INT, MPI_DOUBLE.    
- `dest`: Rank (ID) of the destination process within the communicator.    
- `tag`: Message tag, an arbitrary integer used to identify the message type.
- `comm`: The communicator handle that defines the group of processes communicating (often MPI_COMM_WORLD)
###### `tag`
- used to route different incoming and outgoing messages 

- tag matching : `communicator , user_tag , source_rank`
##### Recv

```C
int MPI_Recv(
   void         *buf,
   int          count,
   MPI_Datatype datatype,
   int          source,
   int          tag,
   MPI_Comm     comm,
   MPI_Status   *status
);
```


- note : `count` here is the maximum size of data we can receive, 
  because buffer pointer has limited memory 
- when not waiting for any particular source, `MPI_ANY_SOURCE`
- when not specifying tag, `MPI_ANY_TAG`


###### MPI_Status
```C
typedef struct {
   int MPI_SOURCE; // rank of source 
   int MPI_TAG; // tag of recevied message
   int MPI_ERROR; // 
   int MPI_internal[5];
} MPI_Status;

```

- use `MPI_sTATUS_IGNORE`: when we do not require status object 
###### 
```C
int buffer[100];
MPI_Status status;

// Receive a message into a buffer that can hold up to 100 integers

MPI_Recv(buffer, 100, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,
	&status);

// Determine how many integers were actually received
int actual_count;
MPI_Get_count(&status, MPI_INT, &actual_count);

printf("Received %d integers from process %d with tag %d\n", 
	actual_count, status.MPI_SOURCE, status.MPI_TAG);

// Process only the received elements
for (int i = 0; i < actual_count; i++) {
    printf("Element %d: %d\n", i, buffer[i]);
}
```

- tells us how much data  was actually transferred 
#### Broadcast 
- one rank passes data to all the other data 
- multiple calls creates bottlenecks in the network, network queue 
##### recursive doubling 
```C
for(itr=1; ; itr*=2){
  if(rank>itr){
  // add bound checking 
  // already have the data
  // Send to rank+itr
  }
  else if(rank < ctr){
  // recevive from rank-itr  
  MPI_recv(...);
  }
}
```

#### Inside MPI send 
- interconnection network : maintain internal queues 
- requests fills up the queue 
- rate of queue filling depends on the bandwidth of the link 

- sometimes, `queue < buffer_size`
- `MPI_Send` is a blocking call  ; control will not come back to the user until the buffer has been emptied out 

###### non-blocking call 
- `MPI_Isend();`
- `MPI_Irecv();`

```C
int MPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);

int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
```

###### `MPI_Test()`
`int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status)`

```C
MPI_Request req; int flag; 
MPI_Isend(&data, count, MPI_INT, dest, tag, MPI_COMM_WORLD, &req); 
// Poll for completion while doing other work 
while(!flag){
	MPI_Test(&req, &flag, MPI_STATUS_IGNORE);
	// Do other useful computation here 
	}
```

- request : Pointer to the `MPI_Request` handle from a prior non-blocking operation (`MPI_Isend` or `MPI_Irecv`)
- flag : Pointer to an integer that returns as 1 (true) if the operation is complete, or 0 (false) if still pending
- status : Pointer to an `MPI_Status` object containing information about the completed operation (source, tag, data size, etc.)
###### `MPI_Wait()`
- unlike `MPI_Test()`, its is blocking 
###### `MPI_Testany()`
- when multiple requests posted
- we check for if anyone of them has completed 
###### `MPI_Waitall()`

##### internal working of send and receive
- data is transferred using buffers (designated memory locations)
- sender packs data into send buffer, receiver ...

`process A --> process B`

1) send buffer creation : by providing pointers to memory location, no. of elements, data type
2) MPI_Send call : send buffer data is copied into an internal MPI buffer (queue) for transmission 
3) routing : MPI sends the message
4) MPI_Recv call : specifies where the buffer is to stored , which process and message to expect

- `MPI_Recv` : always blocking 
###### eager and rendezvous protocol 
- if message size is smaller than the internal MPI buffer, then MPI_Send call is non-blocking
  it will returns without waiting for reception after that data is copied
- if message size exceeds the queue size, MPI_Send call is blocking 
