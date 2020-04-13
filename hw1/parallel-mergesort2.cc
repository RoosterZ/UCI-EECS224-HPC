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


void merge(keytype* X, int n, keytype* tmp) {
   int i = 0;
   int j = n/2;
   int ti = 0;

   while (i<n/2 && j<n) {
      if (X[i] < X[j]) {
         tmp[ti] = X[i];
         ti++; i++;
      } else {
         tmp[ti] = X[j];
         ti++; j++;
      }
   }
   while (i<n/2) { /* finish up lower half */
      tmp[ti] = X[i];
      ti++; i++;
   }
      while (j<n) { /* finish up upper half */
         tmp[ti] = X[j];
         ti++; j++;
   }
   memcpy(X, tmp, n*sizeof(keytype));

} // end of merge()

void mergeSort(keytype* X, int n, keytype* tmp)
{
   if (n < 2) return;

   #pragma omp task firstprivate (X, n, tmp)
   mergesort(X, n/2, tmp);

   #pragma omp task firstprivate (X, n, tmp)
   mergesort(X+(n/2), n-(n/2), tmp);
 
   #pragma omp taskwait

    /* merge sorted halves into sorted list */
   merge(X, n, tmp);
}



void mySort (int N, keytype* A)
{
  /* Lucky you, you get to start from scratch */
    keytype* tmp = newKeys(N);
    #pragma omp parallel
    {
        #pragma omp single
        mergeSort(A, N, tmp);
    }
}

/* eof */
