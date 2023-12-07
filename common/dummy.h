#ifndef __common_dummy_h
#define __common_dummy_h

#if !defined(__cplusplus)
extern "C" {
#endif

void dummy_function( int N, void* ptr );

void dummy_function( int N, void* ptr1, void* ptr2 );

void dummy_function( int N, void* ptr1, void* ptr2, void *ptr3);

#if !defined(__cplusplus)
} // extern "C"
#endif

#endif
