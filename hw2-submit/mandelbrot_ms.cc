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

// Run the whole computation once
void
try_once(int width, int height, int if_render){
  int rank, size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0){ // Master process

    int i;
    int curr = 0; // The next job (row number) to assign

    int *flag = (int*) malloc(sizeof(int) * size);
    int *data = (int*) malloc(sizeof(int) * height * width);  // Space to receive and store the final computation result
    int *job_assignment = (int*) malloc(sizeof(int) * size);  // Index i store the row number that was assigned to rank i

    // A list of MPI_Status for each rank 
    MPI_Status *stat_list = (MPI_Status*) malloc(sizeof(MPI_Status) * size);  

    // A list of MPI_Request for each rank, use when receive partial result from slave proc
    MPI_Request *recv_req = (MPI_Request*) malloc(sizeof(MPI_Request) * size);

    // A list of MPI_Request for each rank, use when assign job to slave proc
    MPI_Request *send_req = (MPI_Request*) malloc(sizeof(MPI_Request) * size);

    if(job_assignment == NULL || flag == NULL || stat_list == NULL || recv_req == NULL || send_req == NULL ){
      std::cout<<"null " << rank << std::endl;
    }

    // Initialization
    for (i = 0; i < size; i++){
      flag[i] = 1;
      job_assignment[i] = -2;
    }


    while(curr < height){ // while still have job to assign
      for (i = 1; i < size; i++){
        if (job_assignment[i] != -2){
          MPI_Test(recv_req + i, flag + i, stat_list + i);  // If slave i was assigned a job, check if it return the result 
        }
        if (flag[i] == 1){  // If slave i return the result
          flag[i] = 0;
          job_assignment[i] = curr; // Assign new job to slave i
          MPI_Isend(job_assignment + i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, send_req + i);  // Send the row number of new job to slave i
          MPI_Irecv(data + curr * width, width, MPI_INT, i, 0, MPI_COMM_WORLD, recv_req + i); // An unblocking receive to receive the result from slave i
          curr++;
          if (curr == height){
            break;
          }
        }

      }
    }

    // No more job to assign. Then wait for all slave to return result of their last job, and reply with a termination signal
    int rem = size - 1; // Do this to all (size - 1) slaves
    while (rem > 0){
      for (int i = 1; i < size; i++){
        if (job_assignment[i] == -1){
          continue;
        }
        MPI_Test(recv_req + i, flag + i, stat_list + i); 
        if (flag[i] == 1){  // If slave i return the result
          flag[i] = 0;
          job_assignment[i] = -1; // Mark it with -1
          MPI_Isend(job_assignment + i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, send_req + i);  // Send termination signal (-1) to slave i

          rem--;
        }
      }
    }
    if(if_render){
      gil::rgb8_image_t img(height, width);
      auto img_view = gil::view(img);
      for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
          img_view(j, i) = render(data[i * width + j] / 512.0);
        }
      }
      gil::png_write_view("mandelbrot.png", const_view(img));
    }
    MPI_Waitall(size-1, send_req+1, stat_list+1);

  }

  else{ // All slave processes
    double minX = -2.1;
    double maxX = 0.7;
    double minY = -1.25;
    double maxY = 1.25;
    
    double it = (maxY - minY)/height;
    double jt = (maxX - minX)/width;
    double x, y;

    MPI_Status status;

    // Current row number to calculate
    int row;

    // local buffer to store the result of a row of image
    int* row_data = (int*) malloc(sizeof(int) * width);
    

    while (true){
      MPI_Recv(&row, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status); 

      // If receive the termination signal, break the loop
      if (row == -1){
        break;
      }

      x = minX;
      y = minY + row * it;

      for (int j = 0; j < width; ++j) {
        row_data[j] = mandelbrot(x, y);
        x += jt;
      }

      MPI_Send(row_data, width, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    
  }

}


int
main (int argc, char* argv[])
{
  int start, end, trial, if_render;
  if (argc == 5) {
    start = atoi (argv[1]);
    end = atoi (argv[2]);
    trial = atoi (argv[3]);
    if_render = atoi(argv[4]);
    assert (start > 0 && end >= start && trial > 0);
  } else {
    fprintf (stderr, "usage: %s <height> <width>\n", argv[0]);
    fprintf (stderr, "where <height> and <width> are the dimensions of the image.\n");
    return -1;
  }
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if(rank == 0){
    std::cout<<"Master/slave model - " << size << " processes" <<std::endl;
  }  
  MPI_Barrier (MPI_COMM_WORLD);
  double start_time;
  for (int image_sz = start; image_sz <= end; image_sz = image_sz * 2){
    MPI_Barrier (MPI_COMM_WORLD);
    start_time = MPI_Wtime();
    for (int i = 0; i < trial; i++){
      try_once(image_sz, image_sz, if_render);
      MPI_Barrier (MPI_COMM_WORLD);
    }
    if(rank == 0){
      std::cout<<image_sz<<" * "<<image_sz<<" | "<<(MPI_Wtime() - start_time) / trial <<" s"<< std::endl;
    }
  }
  if(rank == 0){
    std::cout << std::endl;
  }
  MPI_Finalize();
  return 0; 
  

}

/* eof */