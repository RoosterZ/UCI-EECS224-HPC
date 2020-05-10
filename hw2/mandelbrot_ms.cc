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

  if (rank == 0){

    int i, curr = 0;

    int *flag = (int*) malloc(sizeof(int) * size);
    int *data = (int*) malloc(sizeof(int) * height * width);
    int *job_assignment = (int*) malloc(sizeof(int) * size);
    MPI_Status *stat_list = (MPI_Status*) malloc(sizeof(MPI_Status) * size);  
    MPI_Request *recv_req = (MPI_Request*) malloc(sizeof(MPI_Request) * size);
    MPI_Request *send_req = (MPI_Request*) malloc(sizeof(MPI_Request) * size);

    if(job_assignment == NULL || flag == NULL || stat_list == NULL || recv_req == NULL || send_req == NULL ){
      std::cout<<"null " << rank << std::endl;
    }

    for (i = 0; i < size; i++){
      flag[i] = 1;
      job_assignment[i] = -2;
    }
    while(curr < height){
      for (i = 1; i < size; i++){
        if (job_assignment[i] != -2){
          MPI_Test(recv_req + i, flag + i, stat_list + i);
        }
        if (flag[i] == 1){
          flag[i] = 0;
          job_assignment[i] = curr;
          std::cout<<"assign " << curr << "to" << i <<std::endl;
          MPI_Isend(job_assignment + i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, send_req + i);
          MPI_Irecv(data + curr * width, width, MPI_INT, i, 0, MPI_COMM_WORLD, recv_req + i);
          curr++;
          if (curr == height){
            break;
          }
        }

      }
    }

    int rem = size - 1;
    while (rem > 0){
      for (int i = 1; i < size; i++){
        if (job_assignment[i] == -1){
          continue;
        }
        MPI_Test(recv_req + i, flag + i, stat_list + i);
        if (flag[i] == 1){
          flag[i] = 0;
          job_assignment[i] = -1;
          MPI_Isend(job_assignment + i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, send_req + i);
          //MPI_Irecv(data + curr, 1, MPI_INT, i, 0, MPI_COMM_WORLD, recv_req + i);
          rem--;
        }
      }
    }
    for (int i = 0; i < height * width; i++){
        std::cout << data[i] << std::endl;
    } 

    // gil::rgb8_image_t img(height, width);
    // auto img_view = gil::view(img);
    // for(int i = 0; i < height; i++){
    //   for(int j = 0; j < width; j++){
    //     img_view(j, i) = render(data[i * width + j] / 512.0);
    //   }
    // }
    // gil::png_write_view("mandelbrot.png", const_view(img));

    MPI_Waitall(size-1, send_req+1, stat_list+1);



  }
  else{
    double minX = -2.1;
    double maxX = 0.7;
    double minY = -1.25;
    double maxY = 1.25;
    
    double it = (maxY - minY)/height;
    double jt = (maxX - minX)/width;
    double x, y;

    MPI_Status status;
    int row;
    int* row_data = (int*) malloc(sizeof(int) * width);
    while (true){
      MPI_Recv(&row, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status); 
      if (row == -1){
        break;
      }

      x = minX;
      y = minY + row * it;
      // for (int i = 0; i < num_rows; ++i) {
      //   x = minX;
      for (int j = 0; j < width; ++j) {
        row_data[j] = mandelbrot(x, y);
        x += jt;
      }
      // for (int i = 0; i < width; i++){
      //   row_data[i] = row;
      // }
      //row = -row;
      MPI_Send(row_data, width, MPI_INT, 0, 0, MPI_COMM_WORLD);
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