- solving one large problem, with high throughput 
- single processor : single stream , single processor 
- concurrent parts , multiple processor 
- huge memory requirement 
- processors have hit upper latency due to heat dissipation and power consumption issues 

- numerically intensive simulations 
- database and information systems 
- AI \ ML \ DL

- shared memory and distributed memory parallel programming

#### Parallel Architectures & Programming Model 
##### Shared Memory 
- common memory unit accessible to multiple processors 
- there is a high speed bus , scheduling problem
- scalability upper bound

- assumes one huge global address space 
- all the task is visible to all others


##### Distributed Memory 
- each processor has its own memory unit 
- synchronization problem (network)
- bottleneck is a network instead of a bus 

- `message passing` : each task gets its own private address space , for intercommunication , message passing
- explicit `send <-> receive` 
##### Hybrid
- multiple nodes , each with multiple CPUs
- locally on each nodes -> shared memory 
  cross-node -> distributed memory 

- within nodes -> shared memory model (`OpenMP`)
  cross nodes -> `MPI`(message passing interface)library

  

