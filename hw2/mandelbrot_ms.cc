/**
 *  \file mandelbrot_ms.cc
 *
 *  \brief Implement your parallel mandelbrot set in this file.
 */

#include <iostream>
#include <cstdlib>

#include "render.hh"
#include <mpi.h>
#include <math.h>
#include <assert.h>


using namespace std;

int
mandelbrot(double x, double y) {
  int maxit = 511;
  double cx = x;
  double cy = y;
  double newx, newy;

  int it = 0;
  for (it = 0; it < maxit && (x*x + y*y) < 4; ++it) {
    newx = x*x - y*y + cx;
    newy = 2*x*y + cy;
    x = newx;
    y = newy;
  }
  return it;
}

void
try_once(int width, int height){
  int rank, size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Status stat;
  int *data, row = 0;
  if (rank == 0){
    //int flag = 0;
    int *job_assignment = (int*) malloc(sizeof(int) * size);
    //int *flag = (int*) malloc(sizeof(int) * size);
    MPI_Status *stat_list = (MPI_Status*) malloc(sizeof(MPI_Status) * size);  
    // vector<MPI_Request> req(rank, )
    MPI_Request *req_list = (MPI_Request*) malloc(sizeof(MPI_Request) * size);
    //MPI_Request *send_req = (MPI_Request*) malloc(sizeof(MPI_Request) * size);

    //memset(flag, 0, size);
    // memset(req, MPI_Request, size);
    // memset(stat, MPI_Status, size);
    int curr = 0;
    data = (int*) malloc(sizeof(int) * height);
    for (int i = 1; i < size; i++){
      job_assignment[i] = curr;
      MPI_Send(job_assignment + i, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      MPI_Irecv(data+i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, req + i);
      curr++;
    }
    // while(true){
    //   for (i = 0; i < size; i++){
    //     MPI_Test(req + i, flag + i, stat + i);
    //     if (flag[i] == 1){
    //       curr += 1;
    //       if (curr >= size + height){
    //         break;
    //       }
          
    //       if (curr < height){
    //         //stat[i] = MPI_Status;
    //         //req[i] = MPI_Request;
    //         flag[i] = 0;
    //         MPI_Isend(&curr, 1, MPI_INT, i, 0, MPI_COMM_WORLD, send_req + i);
    //         MPI_Irecv(data+i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, req + i);
    //       }
    //       else{
    //         MPI_Isend(&curr, 1, MPI_INT, i, 0, MPI_COMM_WORLD, send_req + i);
    //       }
    //     }
    //   }
    // }

    // for (i = 0; i < height; i++){
    //   std::cout << data[i] << std::endl;
    // }
    MPI_Waitall(size-1, req_list+1, stat_list+1);
    
  
  }
  else{

    
      MPI_Recv(&row, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat); 
      // if (row >= height){
      //   break;
      // }
      row = -row;
      MPI_Send(&row, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    
  }
  //MPI_Barrier(MPI_COMM_WORLD);
  //MPI_Waitall(MPI_COMM_WORLD);

  if (rank == 0){
    for (int i = 1; i < size; i++){
      std::cout << data[i] << std::endl;
    }
  }

}


int
main (int argc, char* argv[])
{
  int height, width, trial;
  if (argc == 4) {
    height = atoi (argv[1]);
    width = atoi (argv[2]);
    trial = atoi (argv[3]);
    assert (height > 0 && width > 0 && trial > 0);
  } else {
    fprintf (stderr, "usage: %s <height> <width>\n", argv[0]);
    fprintf (stderr, "where <height> and <width> are the dimensions of the image.\n");
    return -1;
  }
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Barrier (MPI_COMM_WORLD);
  double start_time = MPI_Wtime();
  for (int i = 0; i < trial; i++){
    try_once(width, height);
    MPI_Barrier (MPI_COMM_WORLD);
  }
  if(rank == 0){
    std::cout << (MPI_Wtime() - start_time) / trial << std::endl;
  }
  MPI_Finalize();
  return 0; 
  

}

/* eof */