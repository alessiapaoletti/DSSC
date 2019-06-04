 /*
 module load cudatoolkit
qsub -q gpu -l nodes=1:ppn=1,walltime=00:20:00 -I
nvcc matrixTranspose.cu
*/


#include <stdio.h>
#define DIM 32 

__global__ void transposeNaive(double *odata, const double *idata,int BLOCK_ROWS)
{
  int x = blockIdx.x * DIM + threadIdx.x;
  int y = blockIdx.y * DIM + threadIdx.y;
  int width = gridDim.x * DIM;

  for (int j = 0; j < DIM; j+= BLOCK_ROWS)
    odata[x*width + (y+j)] = idata[(y+j)*width + x];
}

__global__ void transposeFast(double *odata, double *idata, int size_x, int size_y, int BLOCK_ROWS)
{
  __shared__ double tile[DIM][DIM];

  int xIndex = blockIdx.x * DIM + threadIdx.x;
  int yIndex = blockIdx.y * DIM + threadIdx.y;
  int index_in = xIndex + (yIndex) * size_x;

  xIndex = blockIdx.y * DIM + threadIdx.x;
  yIndex = blockIdx.x * DIM + threadIdx.y;
  int index_out = xIndex + (yIndex)* size_y;

  for (int i = 0; i < DIM; i+=BLOCK_ROWS) {
    tile[threadIdx.y+i][threadIdx.x] = idata[index_in+i*size_x];
  }
  __syncthreads();

  for (int  i = 0; i < DIM; i+=BLOCK_ROWS) {
    odata[index_out+i*size_y] = tile[threadIdx.x][threadIdx.y+i];
  }
}

int main(int argc, char const *argv[]) {

  const int size_x = 8192;
  const int size_y = 8192;

  int BLOCK_ROWS = argc>=2 ? atoi(argv[1]) : 2; // default case: 2 --> 64 threads

  //execution configuration parameters
  dim3 grid(size_x/DIM, size_y/DIM);
  dim3 block (DIM, BLOCK_ROWS);

  //size of memory required to store the matrix
  const int mem_size = sizeof(double) * size_x*size_y;

  //allocate host memory
  double *h_idata = (double*) malloc(mem_size);
  double *h_odata = (double*) malloc(mem_size);

  //allocate device memory
  double *d_idata;
  double *d_odata;
  cudaMalloc((void**) &d_idata, mem_size);
  cudaMalloc((void**) &d_odata, mem_size);

  // objects to timing
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);

  //initialize host data
  for (int i = 0; i < (size_x*size_y); i++)
    h_idata[i] = (double) i;

  //copy host data to device
  cudaMemcpy(d_idata, h_idata, mem_size, cudaMemcpyHostToDevice);

  printf("\nMatrix size: %dx%d, block: %dx%d, nthreads: %d\n",size_x,size_y, DIM, BLOCK_ROWS, BLOCK_ROWS*DIM );

  /****** Naive transpose ******/
  cudaEventRecord(start, 0);
  transposeNaive<<<grid,block>>>(d_idata, d_odata, BLOCK_ROWS);
  cudaEventRecord(stop, 0);
  cudaEventSynchronize(stop);
  float elapsTimeNaive;
  cudaEventElapsedTime(&elapsTimeNaive, start, stop);
  cudaMemcpy(h_odata, d_odata, mem_size, cudaMemcpyDeviceToHost);

  //the bandwidth is twice the size of the matrix divided by the time execution
  float bandNaive = (2 * mem_size) / elapsTimeNaive/1e6;
  printf("Naive bandwidth = %f, time = %f\n",bandNaive,elapsTimeNaive );

  /****** Fast transpose ******/
  cudaEventRecord(start, 0);
  transposeFast<<<grid,block>>>(d_idata, d_odata, size_x,size_y,BLOCK_ROWS);
  cudaEventRecord(stop, 0);
  cudaEventSynchronize(stop);
  float elapsTimeFast;
  cudaEventElapsedTime(&elapsTimeFast, start, stop);
  cudaMemcpy(h_odata, d_odata, mem_size, cudaMemcpyDeviceToHost);

  //the bandwidth is twice the size of the matrix divided by the time execution
  float bandFast = (2 * mem_size) / elapsTimeFast/1e6;
  printf("Fast bandwidth = %f, time = %f\n",bandFast,elapsTimeFast );

  //free memory
  free(h_idata);
  free(h_odata);
  cudaFree(d_idata);
  cudaFree(d_odata);

  return 0;
}
