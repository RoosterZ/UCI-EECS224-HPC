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
#include <iostream>


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

// Serial merge function
// Merge N1 elements start from A1 and N2 elements start from A2
// Save the merge result in (N1+N2) locations start form tmp
void merge(keytype* A1, keytype* A2, int N1, int N2, keytype* tmp) {
   int i = 0;
   int j = 0;
   int ti = 0;

   while (i < N1 && j < N2) {
      if (A1[i] < A2[j]) {
         tmp[ti] = A1[i];
         ti++; 
         i++;
      } 
      else {
         tmp[ti] = A2[j];
         ti++; 
         j++;
      }
   }
   while (i < N1) {
      tmp[ti] = A1[i];
      ti++; 
      i++;
   }
   while (j < N2) {
      tmp[ti] = A2[j];
      ti++; 
      j++;
   }

}

// Parallel merge function
// Merge a elements start from A and b elements start from B
// Save the merge result in (a+b) locations start from tmp
void Pmerge(keytype* A, keytype* B, int a, int b, keytype* tmp){
    if (a+b < 3000){     // If input is too small, use serial merge
        merge(A, B, a, b, tmp);
    }
    else{    
        // Break A at index midA, two parts of A have size a1 and a2
        // Break B at index midB, two parts of B have size b1 and b2
        int midA = a / 2;
        int midB = binarySearch(B, b, A[midA]);
        int a1 = midA, a2 = a - midA, b1 = midB, b2 = b - midB;

        if (midB == b - 1 && B[midB] <= A[midA]){ //Deal with binary search corner case
            b1 = b;
            b2 = 0;
        }

        // Parallelize work on two smaller merge problems
        #pragma omp task
        Pmerge(A, B, a1, b1, tmp);
        #pragma omp task
        Pmerge(A+a1, B+b1, a2, b2, tmp+a1+b1);
        #pragma omp taskwait

   }

}

void mergeSort(keytype* A, int N, keytype* tmp)
{
   if (N < 2) return;
   if (N > 20000){
      #pragma omp task
      mergeSort(A, N/2, tmp);
      #pragma omp task
      mergeSort(A+(N/2), N-(N/2), tmp+(N/2));
      #pragma omp taskwait
   }
   else{
       mergeSort(A, N/2, tmp);
       mergeSort(A+(N/2), N-(N/2), tmp+(N/2));

   }

   Pmerge(A, A+(N/2), N/2, N-N/2, tmp);

   memcpy(A, tmp, N * sizeof(keytype));
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
