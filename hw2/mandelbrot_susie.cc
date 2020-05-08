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

void
try_once(int width, int height){
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
  if(rank == 0){
    data = (int*) malloc(sizeof(int) * width * height);
    assert(data != NULL);
  }
  int num_rows = floor(height / float(size));

  int parsz = width * size * num_rows;
  int bufsz = width * num_rows;

  int *buf = (int*) malloc(sizeof(int) * bufsz);
  int i, j;
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

  if (rank == 0){
    gil::rgb8_image_t img(height, width);
    auto img_view = gil::view(img);

    int k;
    i = 0;
    j = 0;
    for(k = 0; k < parsz; k++){
      // if(i >= size * num_rows){
      //   std::cout<<i<<"  "<<j<<std::endl;
      // }
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

    // for(int i = 0; i < num_rows * size; i++){
    //   for(int j = 0; j < width; j++){
    //     img_view(j, i) = render(data[j * width + i] / 512.0);
    //   }
    // }
    gil::png_write_view("mandelbrot.png", const_view(img));

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
