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

#define PAR_TH 1000

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

void merge2(keytype* A1, keytype* A2, int N1, int N2, keytype* tmp) {
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
   while (i < N1) { /* finish up lower half */
      tmp[ti] = A1[i];
      ti++; 
      i++;
   }
   while (j < N2) { /* finish up upper half */
      tmp[ti] = A2[j];
      ti++; 
      j++;
   }
   //memcpy(dest, tmp, (N1 + N2)*sizeof(keytype));

} // end of merge()



// void Pmerge2(keytype* A, keytype* B, int a, int b, keytype* tmp){
//    if (a+b < PAR_TH){
//       merge2(A, B, a, b, tmp);
//    }
//    else{

//       int midA = a / 2;
//       int midB = binarySearch(B, b, A[midA]);
//       int a1 = midA, a2 = a - midA, b1 = midB, b2 = b - midB;
//       if (midB == b - 1 && B[midB] <= A[midA]){
//          b1 = b;
//          b2 = 0;
//       }
//       #pragma omp task
//       Pmerge2(A, B, a1, b1, tmp);
//       #pragma omp task
//       Pmerge2(A+a1, B+b1, a2, b2, tmp+a1+b1);
//       #pragma omp taskwait
//       //memcpy(A, tmp, sizeof(keytype)*(a+b));
//    }

// }

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
