/**
 *  \file mandelbrot_joe.cc
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
  double minX = -2.1;
  double maxX = 0.7;
  double minY = -1.25;
  double maxY = 1.25;
  
  double it = (maxY - minY)/height;
  double jt = (maxX - minX)/width;
  double x, y;

  int* data = NULL;
  int rank, size;
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &size);

  // rank0 malloc memory space for receiving computation result from all processes
  if(rank == 0){
    data = (int*) malloc(sizeof(int) * width * height);
    assert(data != NULL);
  }

  int num_rows = floor(height / float(size)); // # of rows that each processes should calculate

  int parsz = width * size * num_rows;
  int bufsz = width * num_rows;

  int *buf = (int*) malloc(sizeof(int) * bufsz);  // local buffer at each process, store partial computation result
  int i, j;

  // Every process calculate its own part of the image
  y = minY + rank * it;
  for (i = 0; i < num_rows; ++i) {
    x = minX;
    for (j = 0; j < width; ++j) {
      buf[i * width + j] = mandelbrot(x, y);
      x += jt;
    }
    y += it * size;
  }

  MPI_Gather(buf, bufsz, MPI_INT, data, bufsz, MPI_INT, 0, MPI_COMM_WORLD);
  free(buf);
  
  // rank0 render the image if flag is set
  if (rank == 0){
    if (if_render){
      gil::rgb8_image_t img(height, width);
      auto img_view = gil::view(img);

      int k;
      i = 0;
      j = 0;
      for(k = 0; k < parsz; k++){
        img_view(j, i) = render(data[k] / 512.0);
        j++;
        if(j >= width){
          j = 0;
          i = i + size;
          if (i >= size * num_rows){
            i = (i % size) + 1;
          }
        }
      }
      gil::png_write_view("mandelbrot.png", const_view(img));
    }
    free(data);
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
    if_render = atoi (argv[4]);
    assert (start > 0 && end >= start && trial > 0 && (if_render == 0 or if_render == 1));
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
    std::cout<<"Susie's apporach - " << size << " processes" <<std::endl;
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
