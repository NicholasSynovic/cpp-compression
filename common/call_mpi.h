#ifndef __my_mpi_header_h
#define __my_mpi_header_h

#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

#define CALL_MPI( _cmd_ ) \
{ \
   /* Execute the function and capture the return int. */ \
   auto __mpi_cmd_ret = (_cmd_); \
   /* Check the error flag. Print if something went wrong and then abort. */ \
   if (__mpi_cmd_ret != MPI_SUCCESS) \
   { \
      int  __mpi_error_str_length = 0; \
      char __mpi_error_str[MPI_MAX_ERROR_STRING]; \
      MPI_Error_string(__mpi_cmd_ret, __mpi_error_str, &__mpi_error_str_length); \
      fprintf(stderr,"MPI Error caught: errcode=%d errstring=%s\n", __mpi_cmd_ret, __mpi_error_str); \
      MPI_Abort( MPI_COMM_WORLD, __mpi_cmd_ret ); \
      exit(__mpi_cmd_ret); \
   } \
}
 
#endif
