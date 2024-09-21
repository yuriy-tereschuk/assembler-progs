/* SIMD Basic operation.
 * Application implements three types of array entries addition.
 * - simple loop iteration
 * - GCC vector optimisation
 * - pure assembly implementation.
 *
 * As assembly is implemented for i386 ABI, the CPP entry point
 * needs to be build for x86 as well.
 * To build application perform next steps.
 * - compile assembly implementation as described in assembly file.
 * - compile without linking CPP: 
 *   #> i686-linux-gnu-g++ -c simd_array_addition.cc -o simd_array_addition_cc.o
 * - link object files:
 *   #> i686-linux-gnu-g++ --static simd_array_addition_cc.o simd_array_addition_asm.o
 */

#include <iostream>
#include <cstring>
#include <chrono>

#define ARRAY_SIZE 100000000
#define VEKTOR_LEN 16

extern "C" {
  extern void asm_vector_add(unsigned int* a, unsigned int* b, unsigned int* c, int buffer_size);
}

using namespace std;
using namespace std::chrono;

// GCC SIMD instructions set implementation.
typedef int v4si __attribute__ ((vector_size((VEKTOR_LEN)))); // 4 elements of int type at onese

/**
The simple loop iteration generated: 

.text:0000000000001192 loc_1192:                               ; CODE XREF: scalar_sum(uint *,uint *,uint *)+62↓j
.text:0000000000001192                 mov     eax, [rbp+var_4]
.text:0000000000001195                 lea     rdx, ds:0[rax*4]
.text:000000000000119D                 mov     rax, [rbp+var_18]
.text:00000000000011A1                 add     rax, rdx
.text:00000000000011A4                 mov     ecx, [rax]
.text:00000000000011A6                 mov     eax, [rbp+var_4]
.text:00000000000011A9                 lea     rdx, ds:0[rax*4]
.text:00000000000011B1                 mov     rax, [rbp+var_20]
.text:00000000000011B5                 add     rax, rdx
.text:00000000000011B8                 mov     edx, [rax]
.text:00000000000011BA                 mov     eax, [rbp+var_4]
.text:00000000000011BD                 lea     rsi, ds:0[rax*4]
.text:00000000000011C5                 mov     rax, [rbp+var_28]
.text:00000000000011C9                 add     rax, rsi
.text:00000000000011CC                 add     edx, ecx
.text:00000000000011CE                 mov     [rax], edx
.text:00000000000011D0                 add     [rbp+var_4], 1
.text:00000000000011D4
.text:00000000000011D4 loc_11D4:                               ; CODE XREF: scalar_sum(uint *,uint *,uint *)+17↑j
.text:00000000000011D4                 cmp     [rbp+var_4], 5F5E0FFh
.text:00000000000011DB                 jbe     short loc_1192

*/
void scalar_sum(unsigned int* a, unsigned int* b, unsigned int* c)
{
  for (unsigned int i = 0; i < ARRAY_SIZE; i++)
  {
    c[i] = a[i] + b[i];
  }
}


/**
 The compiler constructs instructions to operate on vectors:
...
movdqa  xmm1, [rbp+var_20]
movdqa  xmm0, [rbp+var_30]
paddd   xmm0, xmm1
movaps  [rbp+var_40], xmm0
.....
*/
void vector_sum(unsigned int* a, unsigned int* b, unsigned int* c)
{
  v4si *p_aa, *p_bb, *p_cc;

  p_aa = (v4si*) a;
  p_bb = (v4si*) b;
  p_cc = (v4si*) c;

  for (int i = 0; i < ARRAY_SIZE; i+=4) // step on size of vector base 
  {
    /* Have no idea why compiler placed address from heap into general purpose register 
     * and then make a copy of data from memory behind address in that register.
     * Just waste a time...
     * From compiled code:
     * mov     eax, [ebp+var_10]
     * movdqa  xmm0, xmmword ptr ds:(_GLOBAL_OFFSET_TABLE_ - 820DFF4h)[eax]
     */
    *p_cc = __builtin_ia32_paddd128(*p_aa, *p_bb);

    p_aa++;
    p_bb++;
    p_cc++;
  }
}

void init(unsigned int* a, unsigned int* b, unsigned int* c)
{
  std::cout << "Init arrays..." << std::endl;
  for (unsigned int i = 0; i < ARRAY_SIZE; i++)
  {
    *(a + i) = i;
    *(b + i) = i;
    *(c + i) = 0;
  }
}

void test(unsigned int* c)
{
  cout << "Test destination array values: ";
  for (int i = 0; i < ARRAY_SIZE; i++)
  {
    if ((i < 5) || (i > 59999997 && i < 60000003) || (i > (ARRAY_SIZE - 5)))
    {
      std::cout << *(c + i) << "; ";
    }
  }
  cout << endl;
}


int main(int argc, char** argv)
{
  // define arrays
  unsigned int* a = new unsigned int[ARRAY_SIZE];
  unsigned int* b = new unsigned int[ARRAY_SIZE];
  unsigned int* c = new unsigned int[ARRAY_SIZE];

  init(a, b, c);
  test(c);

  std::cout << "Sum of scalars..." << std::endl;
  auto timeStart = std::chrono::steady_clock().now();

  scalar_sum(a, b, c);

  auto timeEnd = std::chrono::steady_clock().now();
  auto timeDiff = timeEnd - timeStart;
  std::cout << "Sum of scalars takes: " << timeDiff.count() << "ns" << std::endl;

  test(c);
  init(a, b, c);
  test(c);

  std::cout << "Sum of vectors..." << std::endl;
  
  timeStart = std::chrono::steady_clock().now();
  
  vector_sum(a, b, c);
  timeEnd = std::chrono::steady_clock().now();
  timeDiff = timeEnd - timeStart;

  std::cout << "Sum of vectors takes: " << timeDiff.count() << "ns" << std::endl;

  test(c);
  init(a, b, c);
  test(c);

  std::cout << "Sum of vectors in assembly..." << std::endl;
  
  timeStart = std::chrono::steady_clock().now();
  
  asm_vector_add(a, b, c, ARRAY_SIZE/4);

  timeEnd = std::chrono::steady_clock().now();
  timeDiff = timeEnd - timeStart;

  std::cout << "Sum of vectors in assembly takes: " << timeDiff.count() << "ns" << std::endl;

  test(c);

  std::cout << "Exit." << std::endl;

  return EXIT_SUCCESS;
}

