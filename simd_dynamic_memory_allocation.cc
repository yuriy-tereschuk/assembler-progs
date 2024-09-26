/**
 * GCC i686
 * GLibc
 *
 * SIMD instaction set usage to copy buffers in chunks of aligned memory in 16 bytes.
 * i686-linux-gnu-g++ simd_dynamic_memory_allocation.cc -mavx512vl -mavx512f --static
 *
 */

#include <iostream>
#include <immintrin.h>

#define ARRAY_SIZE 100000000
#define ALIGN_BNDR 16 // dytes

using namespace std;


void init(__m32* mem_a, __m32* mem_b)
{
  for(int i = 0; i < ARRAY_SIZE; i++)
  {
    *mem_a = (__m32) i;
    *mem_b = (__m32) 0;
    mem_a++;
  }
}

void test(__m32* mem_a, __m32* mem_b)
{
  int* p_mid_a = ((int*) mem_a) + ARRAY_SIZE / 2;
  int* p_mid_b = ((int*) mem_b) + ARRAY_SIZE / 2;
  int* p_end_a = ((int*) mem_a) + ARRAY_SIZE - 3;
  int* p_end_b = ((int*) mem_b) + ARRAY_SIZE - 3;

  printf("SRC Buffer Values: %x:%d; %x:%d; %x:%d | %x:%d; %x:%d; %x:%d | %x:%d; %x:%d; %x:%d \n", 
 /* at the begin of array */  mem_a, *mem_a, (mem_a+1), *(mem_a+1), (mem_a+2), *(mem_a+2),
 /* at the middle of array */ p_mid_a, *(p_mid_a), (p_mid_a+1), *(p_mid_a+1), (p_mid_a+2), *(p_mid_a+2),
 /* at the end of array */    p_end_a, *(p_end_a), (p_end_a+1), *(p_end_a+1), (p_end_a+2), *(p_end_a+2));
  printf("DST Buffer Values: %x:%d; %x:%d; %x:%d | %x:%d; %x:%d; %x:%d | %x:%d; %x:%d; %x:%d \n", 
 /* at the begin of array */    mem_b, *mem_b,     (mem_b+1),   *(mem_b+1),   (mem_b+2),   *(mem_b+2),
 /* at the middle of array */ p_mid_b, *(p_mid_b), (p_mid_b+1), *(p_mid_b+1), (p_mid_b+2), *(p_mid_b+2),
 /* at the end of array */    p_end_b, *(p_end_b), (p_end_b+1), *(p_end_b+1), (p_end_b+2), *(p_end_b+2));
}

void copy(__m32* mem_a, __m32* mem_b)
{
  __m128i* p_i = (__m128i*) mem_b;
  for (int i = 0; i < ARRAY_SIZE; i+=4)
  {
    *p_i =_mm_load_epi32((__m128i*) (mem_a+i));

    p_i++;
  }
}

int main(int argc, char** argv)
{
  __m32* mem_a = (__m32*) aligned_alloc(ALIGN_BNDR, ARRAY_SIZE * sizeof(int)); // aligned integer to 16 bytes boundary
  __m32* mem_b = (__m32*) aligned_alloc(ALIGN_BNDR, ARRAY_SIZE * sizeof(int)); // aligned integer to 16 bytes boundary

  init(mem_a, mem_b);
  test(mem_a, mem_b);

  copy(mem_a, mem_b);

  test(mem_a, mem_b);

  free(mem_b);
  free(mem_a);

  return EXIT_SUCCESS;
}
