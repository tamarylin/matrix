#### `MPI_Dims_create()`
```C++

```


#### `MPI_Cart_create()`
- creates a communicator for a certain cartesian topology 
- 
```C++
int MPI_Cart_create(
		MPI_Comm old_communicator, // process to be used
        int dimension_number,  // dimension of grid
	    const int* dimensions,  // process_per_dimension
        const int* periods,  // periodic / non-periodic
        int reorder,  // rank preserve of parent or not
        MPI_Comm* new_communicator);  // 
```



#### `MPI_Cart_coords()`
```C++
int MPI_Cart_coords(
		MPI_Comm communicator, // new_cart_comm
        int rank, // rank in new_cart_comm
        int dimension_number, // dimension of coords
        int* coords); // pointer n-dim array
```

#### `MPI_Cart_get()`
- retrieves info about a cartesian system on having a communicator passed to it
- 
```C++
int MPI_Cart_get(MPI_Comm communicator,
        int dimension_number,
        int* dimensions,
        int* periods,
        int* coords);
```

#### `MPI_Cart_shift()`
```C++
int MPI_Cart_shift(
		MPI_Comm communicator,
        int direction,
        int displacement,
        int* source,
        int* destination);
```
