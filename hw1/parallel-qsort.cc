#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "sort.hh"
#include <omp.h>
#include <cstring>
#include <iostream>

/**
 *   Given a pivot value, this routine partitions a given input array
 *   into two sets: the set A_le, which consists of all elements less
 *   than or equal to the pivot, and the set A_gt, which consists of
 *   all elements strictly greater than the pivot.
 *      
 *   This routine overwrites the original input array with the
 *   partitioned output. It also returns the index n_le such that
 *   (A[0:(k-1)] == A_le) and (A[k:(N-1)] == A_gt).
 */
int partition (keytype pivot, int N, keytype* A)
{
  int k = 0;
  for (int i = 0; i < N; ++i) {
    /* Invariant:
     * - A[0:(k-1)] <= pivot; and
     * - A[k:(i-1)] > pivot
     */
    const int ai = A[i];
    if (ai <= pivot) {
      /* Swap A[i] and A[k] */
      int ak = A[k];
      A[k++] = ai;
      A[i] = ak;
    }
  }
  return k;
}

int partition2 (keytype pivot, int N, keytype* A){
  //keytype tmp[N];
  keytype *tmp = new keytype[N];
  int *leq = new int[N]();
  int *gt = new int[N]();
  if (tmp == NULL || leq == NULL || gt == NULL){
    std::cout<<"null pointer"<<std::endl;
    return 1;
  }

  int i;
  //std::cout<<"partition"<<level<<"|"<<omp_get_num_threads()<<std::endl;
  #pragma omp taskloop shared (A, N, leq, gt, pivot) private(i) num_tasks(8)
  //#pragma omp taskloop
  for (i = 0; i < N; i++){
    //std::cout<<omp_get_thread_num();
    if (A[i] <= pivot){
      leq[i] = 1;
    }
    else{
      gt[i] = 1;
    }
  }
  //std::cout<<std::endl<<"-----------"<<std::endl;
  // for (i = 1; i < N; i++){
  //   leq[i] = leq[i-1] + leq[i];
  //   gt[i] = gt[i-1] + gt[i];
  // }

  int *leq_psum = new int[N]();
  int *gt_psum = new int[N]();
  int scan_leq = 0, scan_gt = 0;
  #pragma omp simd reduction(inscan, +:(scan_leq, scan_gt))
  for (i = 0; i < N; i++){
    scan_leq += leq[i];
    scan_gt += gt[i];
    #pragma omp scan inclusive(scan_leq, scan_gt)
    {
      leq_psum[i] = scan_leq;
      gt_psum[i] = scan_gt;
    }

  }



  // #pragma omp taskloop shared(A, N, leq, gt, pivot) private(i) num_tasks(8)
  // //#pragma omp for
  // for (i = 0; i < N; i++){
  //   if (A[i] <= pivot){
  //     tmp[leq[i]-1] = A[i];
  //   }
  //   else{
  //     tmp[N-gt[i]] = A[i];
  //   }
  // }

  #pragma omp taskloop shared(A, N, leq, gt, pivot) private(i) num_tasks(8)
  //#pragma omp for
  for (i = 0; i < N; i++){
    if (A[i] <= pivot){
      tmp[leq_psum[i]-1] = A[i];
    }
    else{
      tmp[N-gt_psum[i]] = A[i];
    }
  }

  memcpy(A, tmp, N * sizeof(keytype));
  int rval = N - gt[N-1];
  if (leq != NULL)  delete [] leq;
  if (gt != NULL) delete [] gt;
  if (tmp != NULL)  delete [] tmp;
  if (leq_psum != NULL)  delete [] leq_psum;
  if (gt_psum != NULL)  delete [] gt_psum;  
  return rval;
}

void quickSort (int N, keytype* A)
{
  const int G = 1024; /* base case size, a tuning parameter */
  if (N < G)
    sequentialSort (N, A);
  else {
    // Choose pivot at random
    keytype pivot = A[rand () % N];

    // Partition around the pivot. Upon completion, n_less, n_equal,
    // and n_greater should each be the number of keys less than,
    // equal to, or greater than the pivot, respectively. Moreover, the array
    int n_le = partition2 (pivot, N, A);
    #pragma omp task
    quickSort (n_le, A);
    #pragma omp task
    quickSort (N-n_le, A + n_le);
    //#pragma omp taskwait
  }
}

void mySort (int N, keytype* A)
{
  #pragma omp parallel
  {
    #pragma omp single
    quickSort (N, A);
  }
}

/* eof */



#include<iostream>
02
#include<math.h>
03
#include<string.h>
04
#include<chrono>
05
#define N 16
06
using namespace std;
07
int main(){
08
        int a[N], serial_scan[N], naive_scan[N], simd_scan[N], scan_a;
09
        for(int i = 0; i < N; i++){
10
                a[i] = i;
11
                serial_scan[i] = 0;
12
                simd_scan[i] = 0;
13
        }
14
        //Serial Scan with "+" operator
15
        auto start = std::chrono::system_clock::now();
16
        scan_a = 0;
17
        for(int i = 0; i < N; i++){
18
                scan_a += a[i];
19
                serial_scan[i] = scan_a;
20
        }
21
        auto stop = std::chrono::system_clock::now();
22
        std::cout<<"Serial Scan Output:\n";
23
        for(int i = 0; i < N; i++)
24
                std::cout<<serial_scan[i]<<"\t";
25
        std::cout<<"\n";
26
        std::chrono::duration<double> elapsed_seconds = stop-start;
27
        std::cout<<"Time taken in seconds is "<<elapsed_seconds.count()<<"\n";
28
        //SIMD Scan with "+" operator
29
        start = std::chrono::system_clock::now();
30
        scan_a = 0;
31
        #pragma omp simd reduction(inscan, +:scan_a)
32
        for(int i = 0; i < N; i++){
33
                scan_a += a[i];
34
                #pragma omp scan inclusive(scan_a)
35
                simd_scan[i] = scan_a;
36
        }
37
        stop = std::chrono::system_clock::now();
38
        std::cout<<"SIMD Scan Output:\n";
39
        for(int i = 0; i < N; i++)
40
                std::cout<<simd_scan[i]<<"\t";
41
        std::cout<<"\n";
42
        elapsed_seconds = stop - start;
43
        std::cout<<"Time taken in seconds is "<<elapsed_seconds.count()<<"\n";
44
        return 0;
45
}
