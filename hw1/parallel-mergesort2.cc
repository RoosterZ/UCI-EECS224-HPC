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

}



void Pmerge2(keytype* A, keytype* B, int a, int b, keytype* tmp){
   if (a+b < 2000){
   //if (true){
      merge2(A, B, a, b, tmp);
   }
   else{

      int midA = a / 2;
      int midB = binarySearch(B, b, A[midA]);
      int a1 = midA, a2 = a - midA, b1 = midB, b2 = b - midB;
      if (midB == b - 1 && B[midB] <= A[midA]){
         b1 = b;
         b2 = 0;
      }
      #pragma omp task
      Pmerge2(A, B, a1, b1, tmp);
      Pmerge2(A+a1, B+b1, a2, b2, tmp+a1+b1);
      #pragma omp taskwait
      //memcpy(A, tmp, (a+b)*sizeof(keytype));
   }

}

void mergeSort(keytype* A, int N, keytype* tmp)
{
   if (N < 2) return;
   if (N > 1000){
   //if (false){
      #pragma omp task
      mergeSort(A, N/2, tmp);
      //#pragma omp task
      mergeSort(A+(N/2), N-(N/2), tmp+(N/2));
      /* merge sorted halves into sorted list */
      #pragma omp taskwait
   }
   else{
      //sequentialSort(N, A);
       mergeSort(A, N/2, tmp);
       mergeSort(A+(N/2), N-(N/2), tmp+(N/2));

   }

   Pmerge2(A, A+(N/2), N/2, N-N/2, tmp);

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
   // for(int i = 0; i < N; i++){
   //    std::cout << A[i];
   //    std::cout << std::endl;
   // }
}

/* eof */
