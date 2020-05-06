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


int
main (int argc, char* argv[])
{
  /* Lucky you, you get to write MPI code */
  double minX = -2.1;
  double maxX = 0.7;
  double minY = -1.25;
  double maxY = 1.25;
  
  int height, width;
  if (argc == 3) {
    height = atoi (argv[1]);
    width = atoi (argv[2]);
    assert (height > 0 && width > 0);
  } else {
    fprintf (stderr, "usage: %s <height> <width>\n", argv[0]);
    fprintf (stderr, "where <height> and <width> are the dimensions of the image.\n");
    return -1;
  }

  double it = (maxY - minY)/height;
  double jt = (maxX - minX)/width;
  double x, y;

  int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int* data = NULL;
  if(rank == 0){

    data = (int*) malloc(sizeof(int) * width * height);
    assert(data != NULL);
  }
  int num_rows = floor(height / float(size));

  int parsz = width * size * num_rows;
  int bufsz = width * num_rows;

  int *buf = (int*) malloc(sizeof(int) * bufsz);

  y = minY + rank * num_rows * it;
  for (int i = 0; i < num_rows; ++i) {
    x = minX;
    for (int j = 0; j < width; ++j) {
      buf[i * width + j] = mandelbrot(x, y);
      //img_view(j, i) = render(mandelbrot(x, y)/512.0);
      x += jt;
    }
    y += it;
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Gather(buf, bufsz, MPI_INT, data, parsz, MPI_INT, 0, MPI_COMM_WORLD);
  // MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
  //           void *recvbuf, int width * height, MPI_Datatype recvtype, int root, MPI_Comm comm)
  if (rank == 0){
    gil::rgb8_image_t img(height, width);
    auto img_view = gil::view(img);
    for(int i = 0; i < num_rows * size; i++){
      for(int j = 0; j < width; j++){
        img_view(j, i) = render(data[i * width + j] / 512.0);
      }
    }
    gil::png_write_view("mandelbrot.png", const_view(img));

  }

  MPI_Finalize();
  return 0; 
  

}

/* eof */
