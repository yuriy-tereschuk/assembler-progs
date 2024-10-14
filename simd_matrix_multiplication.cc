/*
 * Debian Linux, GCC, x86_64
 *
 * Compile: g++ -larmadillo -mavx2 simd_matrix_multiplication.cc
 */

#include <cstdlib>
#include <iostream>
#include <chrono>

#include <armadillo>

#include <immintrin.h>

// 2 -> 4-> 8 -> 16 -> 32 -> 64 ....
#define _M 256
#define _K 256
#define _N 256

#define MATRIX_A_SIZE (_M * _K) // cols * rows
#define MATRIX_B_SIZE (_K * _N) // matrix_b_rows = matrix_a_cols
#define MATRIX_C_SIZE (_M * _N) // = matrix_a_rows * matrix_b_cols

extern "C" {
  extern void matrix_mul_asm(const uint16_t* matrix_a, const uint16_t* matrix_b, uint32_t* matrix_c);
}


/*
 0 1 2   0 1 2   15 18 21
 3 4 5 + 3 4 5 = 42 54 66 
 6 7 8   6 7 8   69 90 111
*/
void matrix_mul(const uint16_t* matrix_a, const uint16_t* matrix_b, uint32_t* matrix_c)
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

void simd_matrix_mul(const uint16_t* matrix_a, const uint16_t* matrix_b, uint32_t* matrix_c)
{
  uint32_t prod;
  uint32_t* tmp = (uint32_t*) aligned_alloc(32, 32);
  for (int i = 0; i < _M; i++)
  {
    for (int j = 0; j < _N; j++)
    {
      prod = 0;
      for (int k = 0; k < _N; k+=16)
      {
        const __m256i vec_a = _mm256_load_si256(reinterpret_cast<const __m256i*>(matrix_a + (i * k)));
        const __m256i vec_b = _mm256_load_si256(reinterpret_cast<const __m256i*>(matrix_b + (j * k)));
        const __m256i vec_c = _mm256_madd_epi16(vec_a, vec_b);
        const __m256i S1 = _mm256_hadd_epi32(vec_c, _mm256_setzero_si256());
        const __m256i S2 = _mm256_hadd_epi32(S1, _mm256_setzero_si256());

        _mm256_store_si256((__m256i*) tmp, S2); // furious fast, at least on Haswell.
        // _mm256_stream_si256((__m256i*) tmp, S2); // increadable slow operation on Haswell arch!
        prod += *((uint32_t*) tmp);
        prod += *((uint32_t*) tmp + 4);
 
      }
      _mm_stream_si32((int*) matrix_c, (int) prod);
      matrix_c++;
    }
  }
}

template<class T>
void test(T* matrix)
{
  int cols = _M;

  for (int i = 0; i < MATRIX_C_SIZE; i++)
  {
    if (i < cols)
    {
      std::cout << (int) matrix[i] << "  ";
    }
    else
    {
      std::cout << ";" << std::endl;
      std::cout << (int) matrix[i] << "  ";
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
template<class T>
void init(T* matrix, int value)
{
  for (int i = 1; i <= MATRIX_A_SIZE; i++)
  {
    *matrix++ = value;
  }
}

typedef arma::Mat<uint16_t> umat16_t;
typedef arma::Mat<uint32_t> umat32_t;
typedef arma::Col<uint16_t> uvec16_t;

int main(int argv, char** argc)
{
  uint16_t* matrix_a = (uint16_t*) aligned_alloc(32, sizeof(uint16_t) * MATRIX_A_SIZE);
  uint16_t* matrix_b = (uint16_t*) aligned_alloc(32, sizeof(uint16_t) * MATRIX_B_SIZE);
  uint32_t* matrix_c = (uint32_t*) aligned_alloc(32, sizeof(uint32_t) * MATRIX_C_SIZE);

  init(matrix_a, 2);
  init(matrix_b, 2);

  auto begin = std::chrono::steady_clock().now();
  matrix_mul(matrix_a, matrix_b, matrix_c);
  auto end = std::chrono::steady_clock().now();
  auto duration = end - begin;
  std::cout << "Matrix calculation takes: " << duration.count() << "ns." << std::endl;
  //test(matrix_c);

  init(matrix_c, 0);
  
  umat16_t A(matrix_a, _M, _K, false, true), 
           B(matrix_b, _K, _N, false, true);
  umat32_t C(matrix_c, _M, _N, false, true);
  begin = std::chrono::steady_clock().now();
  C = arma::conv_to<umat32_t>::from(A * B);
  end = std::chrono::steady_clock().now();
  duration = end - begin;
  std::cout << "Armadillo Matrix calculation takes: " << duration.count() << "ns." << std::endl;
  //test(matrix_c);

  uvec16_t Vb  = arma::vectorise(B);
  uint16_t* pVb = Vb.memptr();
  init(matrix_c, 0);

  begin = std::chrono::steady_clock().now();
  simd_matrix_mul(matrix_a, matrix_b, matrix_c);
  end = std::chrono::steady_clock().now();
  duration = end - begin;
  std::cout << "SIMD Matrix calculation takes: " << duration.count() << "ns." << std::endl;
  test(matrix_c);

  return EXIT_SUCCESS;
}

