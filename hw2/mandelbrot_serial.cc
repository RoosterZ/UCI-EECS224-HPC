/**
 *  \file mandelbrot_serial.cc
 *  \brief HW 2: Mandelbrot set serial code
 */


#include <iostream>
#include <cstdlib>
#include <mpi.h>

#include "render.hh"

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
try_once(int width, int height, int if_render){
  double minX = -2.1;
  double maxX = 0.7;
  double minY = -1.25;
  double maxY = 1.25;

  double it = (maxY - minY)/height;
  double jt = (maxX - minX)/width;
  double x, y;

  int* data = (int*) malloc(sizeof(int) * width * height);


  y = minY;
  for (int i = 0; i < height; ++i) {
    x = minX;
    for (int j = 0; j < width; ++j) {
      data[i * width + j] = mandelbrot(x, y);
      x += jt;
    }
    y += it;
  }

  if (if_render == 1){
    gil::rgb8_image_t img(height, width);
    auto img_view = gil::view(img);
    for(int i = 0; i < height; i++){
      for(int j = 0; j < width; j++){
        img_view(j, i) = render(data[i * width + j] / 512.0);
      }
    }
    gil::png_write_view("mandelbrot.png", const_view(img));
  }

}

int
main(int argc, char* argv[]) {
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
  MPI_Init(&argc, &argv);
  std::cout<<"Serial - " << std::endl;
  double start_time;
  start_time = MPI_Wtime();
  for (int image_sz = start; image_sz <= end; image_sz = image_sz * 2){
    start_time = MPI_Wtime();
    for (int i = 0; i < trial; i++){
      try_once(image_sz, image_sz);
    }
    std::cout<<image_sz<<" * "<<image_sz<<" | "<<(MPI_Wtime() - start_time) / trial <<" s"<< std::endl;
  }
  MPI_Finalize();
  return 0;
}

/* eof */
