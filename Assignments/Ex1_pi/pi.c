#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

double function(double x) {
  double p = pow(x,2);
  return 1/(1+p);
}

double serial_approx(double h, double N ){
  double res=0.0;
  int i;
  for (i = 0; i < N; i++) {
    res += h*function(h*i+h/2);
  }
  return res*4;
}

double local_sum(double local_a, double local_b, int local_n, double h ){
  double local_result = (function(local_a)+function(local_b))/2.0;
  int i;
  for (i = 1; i < local_n-1; i++) {
    double x_i= local_a + i *h;
    local_result += function(x_i);
  }
  local_result = h*local_result;
  return local_result;
}

int main() {
  double N = 100000000;
  double h = 1/N;

  /*Serial */
  double tstart_serial = omp_get_wtime();
  double pi_serial  = serial_approx(h,N);
  double duration_serial = omp_get_wtime() -tstart_serial;
  printf("Serial: res = %lf, time = %lf \n",pi_serial,duration_serial);

  /*Reduction */
  double tstart_red = omp_get_wtime();
  double global_result_red = 0.0;
  #pragma omp parallel reduction (+:global_result_red)
  {
    int tid = omp_get_thread_num();
    int nthreads = omp_get_num_threads();
    int local_n = N /nthreads;
    double local_a = tid*local_n *h;
    double local_b = local_a +local_n *h;

    global_result_red += local_sum(local_a,local_b, local_n,h);
  }
  global_result_red = global_result_red *4;
  double duration_red = omp_get_wtime() -tstart_red;
  printf("Reduction: res = %lf, time = %lf \n", global_result_red, duration_red );

  /* Atomic */
  double tstart_atom = omp_get_wtime();
  double global_result_atom = 0.0;
  #pragma omp parallel
  {
    int tid = omp_get_thread_num();
    int nthreads = omp_get_num_threads();
    int local_n = N /nthreads;
    double local_a = tid*local_n *h;
    double local_b = local_a +local_n *h;
    double local_result_atom = local_sum(local_a,local_b,local_n,h);
    #pragma omp atomic
      global_result_atom += local_result_atom;
  }
  global_result_atom = global_result_atom *4;
  double duration_atom = omp_get_wtime()-tstart_atom;
  printf("Atomic: res = %lf, time = %lf \n", global_result_atom, duration_atom );

  /*Critical section */
  double tstart_crit = omp_get_wtime();
  double global_result_crit = 0.0;
  #pragma omp parallel
  {
    int tid = omp_get_thread_num();
    int nthreads = omp_get_num_threads();
    int local_n = N /nthreads;
    double local_a = tid*local_n *h;
    double local_b = local_a +local_n *h;
    double local_result_crit = local_sum(local_a,local_b,local_n,h);
    #pragma omp critical
      //global_result_crit += local_sum(local_a,local_b,local_n,h);
      global_result_crit += local_result_crit;
  }
  global_result_crit = global_result_crit *4;
  double duration_crit = omp_get_wtime() -tstart_crit;
  printf("Critical: res = %lf, time = %lf \n", global_result_crit, duration_crit );
}
