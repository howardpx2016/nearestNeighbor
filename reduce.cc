#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <ctime>
#include <iostream>
#include "get_time.h"

long long reduce(int* A, int n) {
  if (n == 1) {
    return A[0];
  }
  long long L, R;
  L = cilk_spawn reduce(A, n / 2);
  R = reduce(A + n / 2, n - n / 2);
  cilk_sync;
  return L + R;
}

int main(int argc, char* argv[]) {
  if (argc == 1) {
    fprintf(stderr, "Usage: %s [num_of_elements]\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  int n = atoi(argv[1]);
  int* A = new int[n];
  cilk_for(int i = 0; i < n; i++) { A[i] = i; }
  timer t;
  long long sum = reduce(A, n);
  t.stop();
  std::cout << "sum: " << sum << std::endl;
  std::cout << "elapsed time: " << t.get_total() << std::endl;
  delete[] A;
  return 0;
}

