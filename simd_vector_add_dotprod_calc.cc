/**
 * GCC, i686, CPU Vector instructions set AVX.
 * To compile use i686-linux-gnu-g++ compiler with -mavx, -mfma option.
 * For successful linkage i686 under x86_64 use --static option.
 * #> fasm simd_vector_add_dotprod_calc.asm
 * #> i686-linux-gnu-g++ simd_vector_add_dotprod_calc.cc -mavx -mfma -static simd_vector_add_dotprod_calc.o
 */

#include <immintrin.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <chrono>

#define ARRAY_SIZE 100000000
#define ALIGN_BNDR 32

extern "C" {
  extern void vector_add_asm(int array_size, __m256* mem_a, __m256* mem_b, __m256* mem_c);
  extern float dot_product_asm(int array_size, __m256* mem_a, __m256* mem_b);
}

void init(__m32* mem, float mask)
{
//  __m32* p_v;
  float tmp = mask;
  for (int i = 0; i < ARRAY_SIZE; i++)
  {
/*  without reinterpret_cast  
    p_v = (__m32*) &tmp;
    *mem = (__m32) *p_v;
*/

    *mem = *reinterpret_cast<__m32*>(&tmp);
    mem++;
  }
}

void test(__m32* mem, const char* prefix)
{
  float* p_mid = ((float*) mem) + ARRAY_SIZE / 2;
  float* p_end = ((float*) mem) + ARRAY_SIZE - 3;

  printf("%s: %x:%2.2f; %x:%2.2f; %x:%2.2f | %x:%2.2f; %x:%2.2f; %x:%2.2f | %x:%2.2f; %x:%2.2f; %x:%2.2f \n", prefix,
  /* at the begin of array  */ mem, *((float*) mem), (mem+1), *((float*) mem+1), (mem+2), *((float*)mem+2),
  /* at the middle of array */ p_mid, *(p_mid), (p_mid+1), *(p_mid+1), (p_mid+2), *(p_mid+2),
  /* at the end of array    */ p_end, *(p_end), (p_end+1), *(p_end+1), (p_end+2), *(p_end+2));
}

void scalar_add(__m32* mem_a, __m32* mem_b, __m32* mem_c)
{
  for (int i = 0; i < ARRAY_SIZE; i++)
  {
    *(float*)(mem_c + i) = *(float*)(mem_a + i) + *(float*)(mem_b + i);
  }
}

void vector_add(__m256* mem_a, __m256* mem_b, __m256* mem_c)
{                                       
  int vector_size = ARRAY_SIZE * sizeof(__m32); // exact size of array of float type entries

  for (int i = 0; i < vector_size; i += sizeof(__m256)) // base type of each element in vector 8th times less then memory addressed by SIMD instruction,
  {                                                     // so reduce the amount of memory chunks appropriatelly.
    *(mem_c++) = _mm256_add_ps(*(mem_a++), *(mem_b++));
  }
}

float dot_product(__m256* mem_a, __m256* mem_b, __m256* mem_c)
{
  float dotprod = 0.0;
  __m256 result = _mm256_setzero_ps();

  int vector_size = ARRAY_SIZE * sizeof(__m32); // exact size of array of float type entries

  for (int i = 0; i < vector_size; i += sizeof(__m256)) // see coments in lines 62:63
  {
    result = _mm256_fmadd_ps(*(mem_a++), *(mem_b++), result); 
  }

  __m32* entry = (__m32*) &result;
  for (int i = 0; i < (sizeof(__m256)/sizeof(float)); i++) // four bytes for each entry
  {
    dotprod += *reinterpret_cast<float*>(entry++);
  }

  return dotprod;
}

int main(int argc, char** argv)
{
  __m32 *mem_a, *mem_b, *mem_c; 
  posix_memalign((void**) &mem_a, ALIGN_BNDR, ARRAY_SIZE * sizeof(__m32));
  posix_memalign((void**) &mem_b, ALIGN_BNDR, ARRAY_SIZE * sizeof(__m32));
  posix_memalign((void**) &mem_c, ALIGN_BNDR, ARRAY_SIZE * sizeof(__m32));

  init(mem_a, 2.0f);
  init(mem_b, 2.0f);
  init(mem_c, 0.0f);

  test(mem_a, "Vector A: ");
  test(mem_b, "Vector B: ");
  test(mem_c, "Vector C: ");

  auto begin = std::chrono::steady_clock().now();
  scalar_add(mem_a, mem_b, mem_c);
  auto end = std::chrono::steady_clock().now();
  auto duration = end - begin;
  test(mem_c, "Scalar C[ADD]");
  std::cout << "Scalar add takes: " << duration.count() << "ns." << std::endl;

  init(mem_c, 0.0f);
  begin = std::chrono::steady_clock().now();
  vector_add((__m256*) mem_a, (__m256*) mem_b, (__m256*) mem_c);
  end = std::chrono::steady_clock().now();
  duration = end - begin;
  test(mem_c, "Vector C[ADD]:");
  std::cout << "Vector add takes: " << duration.count() << "ns." << std::endl;

  init(mem_c, 0.0f);
  begin = std::chrono::steady_clock().now();
  float result = dot_product((__m256*) mem_a, (__m256*) mem_b, (__m256*) mem_c);
  end = std::chrono::steady_clock().now();
  duration = end - begin;
  std::cout << "Dot prod = " << result << std::endl;
  std::cout << "Dot product count takes: " << duration.count() << "ns." << std::endl;

  std::cout << "====== Assembler =====" << std::endl;
  init(mem_c, 0.0f);
  begin = std::chrono::steady_clock().now();
  vector_add_asm(ARRAY_SIZE, (__m256*) mem_a, (__m256*) mem_b, (__m256*) mem_c);
  end = std::chrono::steady_clock().now();
  test(mem_c, "Assembly vector add: ");
  duration = end - begin;
  std::cout << "Assembly vector add takes: " << duration.count() << "ns." << std::endl;

  init(mem_c, 0.0f);
  begin = std::chrono::steady_clock().now();
  result = dot_product_asm(ARRAY_SIZE, (__m256*) mem_a, (__m256*) mem_b);
  end = std::chrono::steady_clock().now();
  duration = end - begin;
  std::cout << "Asemmbly dot prod = " << result << std::endl;
  std::cout << "Assembly dot product count takes: " << duration.count() << "ns." << std::endl;

  return EXIT_SUCCESS;
}
