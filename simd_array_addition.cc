#include <iostream>
#include <cstring>
#include <chrono>

#define ARRAY_SIZE 100000000
#define VEKTOR_LEN 16

using namespace std;
using namespace std::chrono;

// GCC SIMD instructions set implementation.
typedef unsigned int v4si __attribute__ ((vector_size((VEKTOR_LEN)))); // 4 elements of int type at onese

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
  v4si aa, bb, cc;
  
  for (int i = 0; i < (ARRAY_SIZE/VEKTOR_LEN); i++)
  {
    memcpy(&aa, (a + i), VEKTOR_LEN);
    memcpy(&bb, (b + i), VEKTOR_LEN);

    cc = aa + bb;

    memcpy((c + i), &cc, VEKTOR_LEN);
  }
}

int main(int argc, char** argv)
{
  // define arrays
  unsigned int* a = new unsigned int[ARRAY_SIZE];
  unsigned int* b = new unsigned int[ARRAY_SIZE];
  unsigned int* c = new unsigned int[ARRAY_SIZE];

  std::cout << "Init arrays..." << std::endl;
  for (unsigned int i = 0; i < ARRAY_SIZE; i++)
  {
    *(a + i) = i;
    *(b + i) = i;
    *(c + i) = 0;
  }

  std::cout << "Sum of scalars..." << std::endl;
  auto timeStart = std::chrono::steady_clock().now();

  scalar_sum(a, b, c);

  auto timeEnd = std::chrono::steady_clock().now();
  auto timeDiff = timeEnd - timeStart;
  std::cout << "Sum of scalars takes: " << timeDiff.count() << "ns" << std::endl;

  std::cout << "Sum of vectors..." << std::endl;
  
  timeStart = std::chrono::steady_clock().now();
  
  vector_sum(a, b, c);
  timeEnd = std::chrono::steady_clock().now();
  timeDiff = timeEnd - timeStart;

  std::cout << "Sum of scalars takes: " << timeDiff.count() << "ns" << std::endl;
  std::cout << "Exit." << std::endl;

  return EXIT_SUCCESS;
}
