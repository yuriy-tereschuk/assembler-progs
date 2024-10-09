/*
 * Debian Linux, GCC, x86_64
 *
 * Compile: g++ -larmadillo simd_matrix_multiplication.cc
 */

#include <cstdlib>
#include <iostream>
#include <chrono>

#include <armadillo>

#define _M 10
#define _K 10
#define _N 10

#define MATRIX_A_SIZE (_M * _K) // cols * rows
#define MATRIX_B_SIZE (_K * _N) // matrix_b_rows = matrix_a_cols
#define MATRIX_C_SIZE (_M * _N) // = matrix_a_rows * matrix_b_cols

extern "C" {
  extern void matrix_mul_asm(const long long int* matrix_a, const long long int* matrix_b, long long int* matrix_c);
}


/*
 0 1 2   0 1 2   15 18 21
 3 4 5 + 3 4 5 = 42 54 66 
 6 7 8   6 7 8   69 90 111
*/
void matrix_mul(const long long int* matrix_a, const long long int* matrix_b, long long int* matrix_c)
{
  int prod_id = 0;
  int prod = 0;

  for (int a = 0; a < MATRIX_A_SIZE; a+=_K)
  {
    for (int b = 0; b < _M; b++)
    {
      for (int i = a, j = b; i < a + _K; i++, j+=_M)
      {
        prod += matrix_a[i] * matrix_b[j];
      }
      matrix_c[prod_id++] = prod;
      prod = 0;
    }
  }
}

void test(long long int* matrix)
{
  int cols = _M;

  for (int i = 0; i < MATRIX_C_SIZE; i++)
  {
    if (i < cols)
    {
      std::cout << matrix[i] << "  ";
    }
    else
    {
      std::cout << ";" << std::endl;
      std::cout << matrix[i] << "  ";
      cols += _M;
    }
  }

  std::cout << ";" << std::endl;
}

/*
 0 1 2
 3 4 5
 6 7 8
*/
void init(long long int* matrix)
{
  for (int i = 1; i <= MATRIX_A_SIZE; i++)
  {
    *matrix++ = i;
  }
}

int main(int argv, char** argc)
{
  long long int* matrix_a = new long long int[sizeof(long long int) * MATRIX_A_SIZE];
  long long int* matrix_b = new long long int[sizeof(long long int) * MATRIX_B_SIZE];
  long long int* matrix_c = new long long int[sizeof(long long int) * MATRIX_C_SIZE];

  init(matrix_a);
  init(matrix_b);

  auto begin = std::chrono::steady_clock().now();
  matrix_mul(matrix_a, matrix_b, matrix_c);
  auto end = std::chrono::steady_clock().now();
  auto duration = end - begin;
  std::cout << "Matrix calculation takes: " << duration.count() << "ns." << std::endl;
  test(matrix_c);

  init(matrix_c);
  
  arma::imat A(matrix_a, _M, _K, false, true), B(matrix_b, _K, _N, false, true), C(matrix_c, _M, _N, false, false);
  begin = std::chrono::steady_clock().now();
  C = A * B;
  end = std::chrono::steady_clock().now();
  duration = end - begin;
  std::cout << "Armadillo Matrix calculation takes: " << duration.count() << "ns." << std::endl;
  C.print("C:");

  return EXIT_SUCCESS;
}


