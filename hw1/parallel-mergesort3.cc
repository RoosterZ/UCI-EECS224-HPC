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

#define PAR_TH 4000

int binarySearch(keytype* A, int N, int target) 
{ 
   
   int l = 0, r = N - 1;
   int mid = 0;
   while (r > l){
      mid = l + (r - l) / 2;
      if (A[mid] <= target){
         l = mid + 1;
      }
      else {
         r = mid;
      }
   }
   return l;
} 



keytype* merge(keytype* A, keytype* B, int a, int b) {
   
   keytype* rval = newKeys(a+b);

   int i = 0;
   int j = 0;
   int ti = 0;

   while (i < a && j < b) {
      if (A[i] < B[j]) {
         rval[ti] = A[i];
         ti++; 
         i++;
      } 
      else {
         rval[ti] = B[j];
         ti++; 
         j++;
      }
   }
   while (i < a) { /* finish up lower half */
      rval[ti] = A[i];
      ti++; 
      i++;
   }
   while (j < b) { /* finish up upper half */
      rval[ti] = B[j];
      ti++; 
      j++;
   }
   return rval;

} // end of merge()


void mergeSort(keytype* A, int N)
{
    if (N < 2) return;
    if (N > PAR_TH){
      #pragma omp task
      mergeSort(A, N/2);
      #pragma omp task
      mergeSort(A+(N/2), N-(N/2));
      /* merge sorted halves into sorted list */
      #pragma omp taskwait
   }
   else{
      mergeSort(A, N/2);
      mergeSort(A+(N/2), N-(N/2));
   }

   keytype* tmp = merge(A, A+(N/2), N/2, N-(N/2));
   memcpy(A, tmp, sizeof(keytype) * N);
}


void mySort (int N, keytype* A)
{
  /* Lucky you, you get to start from scratch */
    #pragma omp parallel
    {
        #pragma omp single
        mergeSort(A, N);
    }
}

/* eof */
