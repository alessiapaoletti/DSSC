#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

void print_usage( int * a, int N, int nthreads ) {
  int tid, i;
  for( tid = 0; tid < nthreads; ++tid ) {
    fprintf( stdout, "%d: ", tid );
    for( i = 0; i < N; ++i ) {
      if( a[ i ] == tid) fprintf( stdout, "*" );
      else fprintf( stdout, " ");
    }
    printf("\n");
  }
}

int main(int argc, char const *argv[]) {

  int nthreads = argc>=2 ? atoi(argv[1]) : 4; //default 4 threads

  const int N = 250;
  int a[N];

  int thread_id = 0;
  printf("Sequential:\n");
  for (int i = 0; i < N; ++i) {
    a[i] = thread_id;
  }
  print_usage(a,N,nthreads); //visualize the result

  printf("\nStatic :\n");
  #pragma omp parallel for schedule(static)
    for(int i=0; i<N; i++)
      a[i]=omp_get_thread_num();
  print_usage(a,N,nthreads);

  printf("\nStatic, chunck 1 :\n");
  #pragma omp parallel for schedule(static,1)
    for(int i=0; i<N; i++)
      a[i]=omp_get_thread_num();
  print_usage(a,N,nthreads);

  printf("\nStatic, chunck 10 :\n");
  #pragma omp parallel for schedule(static,10)
    for(int i=0; i<N; i++)
      a[i]=omp_get_thread_num();
  print_usage(a,N,nthreads);

  printf("\nDynamic:\n");
  #pragma omp parallel for schedule(dynamic)
    for(int i=0; i<N; i++)
      a[i]=omp_get_thread_num();
  print_usage(a,N,nthreads);

  printf("\nDynamic, chunck 1:\n");
  #pragma omp parallel for schedule(dynamic,1)
    for(int i=0; i<N; i++)
      a[i]=omp_get_thread_num();
  print_usage(a,N,nthreads);

  printf("\nDynamic, chunck 10:\n");
  #pragma omp parallel for schedule(dynamic,10)
    for(int i=0; i<N; i++)
      a[i]=omp_get_thread_num();
  print_usage(a,N,nthreads);

  return 0;
}
