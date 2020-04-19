/* test modification */

/**
 *  \file mergesort.cc
 *
 *  \brief Implement your mergesort in this file.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "sort.hh"
#include <vector>
#include <cstring>


void merge(keytype* A, int N, keytype* tmp) {
   int i = 0;
   int j = N/2;
   int ti = 0;

   while (i < N/2 && j < N) {
      if (A[i] < A[j]) {
         tmp[ti] = A[i];
         ti++; 
         i++;
      } 
      else {
         tmp[ti] = A[j];
         ti++; 
         j++;
      }
   }
   while (i < N/2) { /* finish up lower half */
      tmp[ti] = A[i];
      ti++; 
      i++;
   }
   while (j < N) { /* finish up upper half */
      tmp[ti] = A[j];
      ti++; 
      j++;
   }
   memcpy(A, tmp, N*sizeof(keytype));

} // end of merge()

void mergeSort(keytype* A, int N, keytype* tmp)
{
   if (N < 2) return;
   mergeSort(A, N/2, tmp);
   mergeSort(A+(N/2), N-(N/2), tmp);
    /* merge sorted halves into sorted list */
   merge(A, N, tmp);
}


void mySort (int N, keytype* A)
{
  /* Lucky you, you get to start from scratch */
   keytype* tmp = newKeys(N);
   mergeSort(A, N, tmp);
}

/* eof */
