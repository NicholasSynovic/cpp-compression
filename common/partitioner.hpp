#ifndef __partitioner_hpp
#define __partitioner_hpp

#include <vector>

// HW3: This is a useful function for partitioning a 1d iteration
// space uniforming w/o any overlap.
void partition_range (const size_t global_start, const size_t global_end,
                      const size_t num_partitions, const size_t rank,
                      size_t* local_start, size_t* local_end)
{
   // Total length of the iteration space.
   const size_t global_length = global_end - global_start;

   // Simple per-partition size ignoring remainder.
   const size_t chunk_size = global_length / num_partitions;

   // And now the remainder.
   const size_t remainder = global_length - chunk_size * num_partitions;

   // We want to spreader the remainder around evening to the 1st few ranks.
   // ... add one to the simple chunk size for all ranks < remainder.
   if (rank < remainder)
   {
      *local_start = global_start + rank * chunk_size + rank;
      *local_end   = *local_start + chunk_size + 1;
   }
   else
   {
      *local_start = global_start + rank * chunk_size + remainder;
      *local_end   = *local_start + chunk_size;
   }
}

// Another version of this function that returns all N partitions
// in an array of length N+1. Range for rank q is range[q+1]-range[q];
std::vector<size_t>
partition_range ( const size_t global_start, const size_t global_end,
                  const size_t num_partitions )
{
   std::vector<size_t> ranges(num_partitions+1);

   ranges[0] = global_start;
   for (size_t rank = 0; rank < num_partitions; rank++)
   {
      size_t lo, hi;
      partition_range( global_start, global_end, num_partitions, rank, &lo, &hi );
      ranges[rank+1] = hi;
   }

   return ranges;
}

// Last version of this function that returns all N partitions
// in an array of length N+1. Range for rank q is range[q+1]-range[q];
std::vector<size_t>
partition_range ( const size_t nelems, const size_t num_partitions )
{
   return partition_range ( 0, nelems, num_partitions );
}

#endif
