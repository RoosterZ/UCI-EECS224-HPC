#include <stdlib.h>
#include <stdio.h>

#include "cuda_utils.h"
#include "timer.c"

typedef float dtype;

#define BLOCK_DIM_Y 8
#define PATCH_DIM 32

__global__ 
void matTrans(dtype* AT, dtype* A, int N)  {
	/* Fill your code here */
	//const unsigned int scratch_dim = blockDim.x;
	
	__shared__ dtype scratch[PATCH_DIM][PATCH_DIM+1];
	int x = blockIdx.x * PATCH_DIM + threadIdx.x;
	int y = blockIdx.y * PATCH_DIM + threadIdx.y;

	 int i;
	// //int dim = gridDim.x * blockDim.x;
	for (i = 0; i < PATCH_DIM; i += BLOCK_DIM_Y){
		scratch[i + threadIdx.y][threadIdx.x] = A[(y+i) * N + x]; 
	}
	// for (i = 0; i < PATCH_DIM; i += BLOCK_DIM_Y){
	// 	AT[(y+i) * N + x] = A[(y+i) * N + x]; 
	// }	

	__syncthreads();

	x = PATCH_DIM * blockIdx.y + threadIdx.x;
	y = blockIdx.x * PATCH_DIM + threadIdx.y;
 
	for (i = 0; i < PATCH_DIM; i += BLOCK_DIM_Y){
		AT[(y+i) * N + x] = scratch[threadIdx.x][i + threadIdx.y];
	}
    

}


void
parseArg (int argc, char** argv, int* N)
{
	if(argc == 2) {
		*N = atoi (argv[1]);
		assert (*N > 0);
	} else {
		fprintf (stderr, "usage: %s <N>\n", argv[0]);
		exit (EXIT_FAILURE);
	}
}

void
initArr (dtype* in, int N)
{
	int i;

	for(i = 0; i < N; i++) {
		in[i] = (dtype) rand () / RAND_MAX;
	}
}

void
cpuTranspose (dtype* A, dtype* AT, int N)
{
	int i, j;

	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			AT[j * N + i] = A[i * N + j];
		}
	}
}

int
cmpArr (dtype* a, dtype* b, int N)
{
	int cnt, i;

	cnt = 0;
	for(i = 0; i < N; i++) {
		if(abs(a[i] - b[i]) > 1e-6) cnt++;
	}

	return cnt;
}

void
gpuTranspose (dtype* A, dtype* AT, int N)
{
	cudaEvent_t startEvent, stopEvent;
	checkCuda( cudaEventCreate(&startEvent) );
	checkCuda( cudaEventCreate(&stopEvent) );
	float ms;

	dtype *d_idata, *d_odata;
	CUDA_CHECK_ERROR (cudaMalloc (&d_idata, N * N * sizeof (dtype)));
	CUDA_CHECK_ERROR (cudaMalloc (&d_odata, N * N * sizeof (dtype)));
	CUDA_CHECK_ERROR (cudaMemcpy (d_idata, A, N * N * sizeof (dtype), 
	cudaMemcpyHostToDevice));

	// int block_x, block_y, grid_x, grid_y;
	// getNumBlocksAndThreads(N, block_x, block_y, grid_x, grid_y);
	// dim3 gb(grid_x, grid_y, 1);
	// dim3 tb(block_x, block_y, 1);
	dim3 gb(32, 32, 1);
	dim3 tb(32, 8, 1);
	matTrans <<<gb, tb>>> (d_odata, d_idata, N);

	struct stopwatch_t* timer = NULL;
  	long double t_gpu;
	
  	/* Setup timers */
  	stopwatch_init ();
  	timer = stopwatch_create ();
  
	stopwatch_start (timer);
	CUDA_CHECK_ERROR( cudaEventRecord(startEvent, 0));  
	matTrans <<<gb, tb>>> (d_odata, d_idata, N);
	/* run your kernel here */
	CUDA_CHECK_ERROR( cudaEventRecord(stopEvent, 0) );
	CUDA_CHECK_ERROR( cudaEventSynchronize(stopEvent) );
	CUDA_CHECK_ERROR( cudaEventElapsedTime(&ms, startEvent, stopEvent) );

  	cudaThreadSynchronize ();
  	t_gpu = stopwatch_stop (timer);
  	fprintf (stderr, "GPU transpose: %Lg secs ==> %Lg billion elements/second\n",
           t_gpu, (N * N) / t_gpu * 1e-9 );
	fprintf (stdout, "GPU transpose: %Lg secs ==> %Lg billion elements/second\n",
	t_gpu, (N * N) / t_gpu * 1e-9 );


	// double bw = (N * N * sizeof(dtype)) / (t_gpu * 1e9);
	// fprintf (stdout, "Effective bandwidth: %.2lf GB/s\n", bw);
	double bw = (N * N * sizeof(dtype)) / (ms * 1e6);
	fprintf (stdout, "Effective bandwidth: %.2lf GB/s\n", bw);

	CUDA_CHECK_ERROR (cudaMemcpy (AT, d_odata, N * N * sizeof (dtype), 
	cudaMemcpyDeviceToHost));

}

int 
main(int argc, char** argv)
{
  /* variables */
	dtype *A, *ATgpu, *ATcpu;
  int err;

	int N;

  struct stopwatch_t* timer = NULL;
  long double t_cpu;

	N = -1;
	parseArg (argc, argv, &N);

  /* input and output matrices on host */
  /* output */
  ATcpu = (dtype*) malloc (N * N * sizeof (dtype));
  ATgpu = (dtype*) malloc (N * N * sizeof (dtype));

  /* input */
  A = (dtype*) malloc (N * N * sizeof (dtype));

	initArr (A, N * N);

	/* GPU transpose kernel */
	gpuTranspose (A, ATgpu, N);

  /* Setup timers */
  stopwatch_init ();
  timer = stopwatch_create ();

	stopwatch_start (timer);
  /* compute reference array */
	cpuTranspose (A, ATcpu, N);
  t_cpu = stopwatch_stop (timer);
  fprintf (stderr, "Time to execute CPU transpose kernel: %Lg secs\n",
           t_cpu);

  /* check correctness */
	err = cmpArr (ATgpu, ATcpu, N * N);
	if(err) {
		fprintf (stderr, "Transpose failed: %d\n", err);
	} else {
		fprintf (stderr, "Transpose successful\n");
	}

	free (A);
	free (ATgpu);
	free (ATcpu);

  return 0;
}