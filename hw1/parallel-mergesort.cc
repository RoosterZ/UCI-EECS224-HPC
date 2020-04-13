/* test modification */

/**
 *  \file mergesort.cc
 *
 *  \brief Implement your mergesort in this file.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "sort.hh"

// Merges two subarrays of arr[]. 
// First subarray is arr[l..m] 
// Second subarray is arr[m+1..r] 
void merge(keytype* A, int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 =  r - m; 
  
    /* create temp arrays */
    // int L[n1], R[n2]; 
    keytype* L = newKeys(n1);
    keytype* R = newKeys(n2);
    
  
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


void merge1(keytype* A, int p, int q, int r)
{
    // std::vector<keytype> l(A.begin() + p, a.begin() + q + 1);
    // std::vector<keytype> ri(a.begin() + q + 1, a.begin() + r + 1);
    

    int n1 = q - p + 1;
    int n2 = r - q;
    keytype l[n1];
    keytype ri[n2];

    int i, j; i = j = 0;

    while (i < n1 && j < n2) {
        if (l[i] < ri[j]) {
            A[p++] = l[i++];
        } else {
            A[p++] = ri[j++];
        }
    }

    while (i < n1) {
        A[p++] = l[i++];
    }

    while (j < n2) {
        A[p++] = ri[j++];
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
        mergeSort(A, l, m); 
        mergeSort(A, m+1, r); 
  
        //merge(A, l, m, r);
        merge1(A, l, m, r); 
    } 
} 


void mySort (int N, keytype* A)
{
  /* Lucky you, you get to start from scratch */
  mergeSort(A, 0, N-1);
  
}

/* eof */
