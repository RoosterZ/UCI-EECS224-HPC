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
// Merges two subarrays of arr[]. 
// First subarray is arr[l..m] 
// Second subarray is arr[m+1..r] 
void merge(keytype* A, int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 =  r - m; 
  

    // keytype L[n1];
    // keytype R[n2];

    std::vector<keytype> L(n1);
    std::vector<keytype> R(n2);
    
  
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++) 
        L[i] = A[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = A[m + 1+ j]; 
  
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray 
    j = 0; // Initial index of second subarray 
    k = l; // Initial index of merged subarray 
    while (i < n1 && j < n2) 
    { 
        if (L[i] <= R[j]) 
        { 
            A[k] = L[i]; 
            i++; 
        } 
        else
        { 
            A[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
  
    /* Copy the remaining elements of L[], if there 
       are any */
    while (i < n1) 
    { 
        A[k] = L[i]; 
        i++; 
        k++; 
    } 
  
    /* Copy the remaining elements of R[], if there 
       are any */
    while (j < n2) 
    { 
        A[k] = R[j]; 
        j++; 
        k++; 
    } 
} 

  
/* l is for left index and r is right index of the 
   sub-array of arr to be sorted */
void mergeSort(keytype* A, int l, int r) 
{ 
    if (l < r) 
    { 
        // Same as (l+r)/2, but avoids overflow for 
        // large l and h 
        int m = l+(r-l)/2; 
  
        // Sort first and second halves 
        #pragma omp taskgroup
        {
        #pragma omp task shared(A)
        mergeSort(A, l, m); 
        mergeSort(A, m+1, r); 
        }
        // #pragma omp taskwait
        merge(A, l, m, r); 
    } 
} 


void mySort (int N, keytype* A)
{
  /* Lucky you, you get to start from scratch */
  #pragma omp parallel
  #pragma omp single nowait
  mergeSort(A, 0, N-1);
  
}

/* eof */
