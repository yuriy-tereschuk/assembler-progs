
#include <cstdlib>
#include <iostream>
#include <chrono>

#define COLUMNS 64
#define ROWS    64

#define MATRIX_A_SIZE (COLUMNS * ROWS) // cols * rows
#define MATRIX_B_SIZE (COLUMNS * ROWS) // matrix_b_rows = matrix_a_cols
#define MATRIX_C_SIZE (COLUMNS * ROWS) // = matrix_a_rows * matrix_b_cols

/*
 0 1 2   0 1 2   15 18 21
 3 4 5 + 3 4 5 = 42 54 66 
 6 7 8   6 7 8   69 90 111
*/
void matrix_mul(const int* matrix_a, const int* matrix_b, int* matrix_c)
{
  int prod_id = 0;
  int prod = 0;

  for (int a = 0; a < MATRIX_A_SIZE; a+=ROWS)
  {
    for (int b = 0; b < COLUMNS; b++)
    {
      for (int i = a, j = b; i < a + ROWS; i++, j+=COLUMNS)
      {
        prod += matrix_a[i] * matrix_b[j];
      }
      matrix_c[prod_id++] = prod;
      prod = 0;
    }
  }
}

void test(int* matrix)
{
  int cols = COLUMNS;

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
      cols += COLUMNS;
    }
  }

  std::cout << ";" << std::endl;
}

/*
 0 1 2
 3 4 5
 6 7 8
*/
void init(int* matrix)
{
  for (int i = 1; i <= MATRIX_A_SIZE; i++)
  {
    *matrix++ = i;
  }
}

int main(int argv, char** argc)
{
  int* matrix_a = new int[sizeof(int) * MATRIX_A_SIZE];
  int* matrix_b = new int[sizeof(int) * MATRIX_B_SIZE];
  int* matrix_c = new int[sizeof(int) * MATRIX_C_SIZE];

  init(matrix_a);
  init(matrix_b);

  auto begin = std::chrono::steady_clock().now();
  matrix_mul(matrix_a, matrix_b, matrix_c);
  auto end = std::chrono::steady_clock().now();
  auto duration = end - begin;
  std::cout << "Matrix calculation takes: " << duration.count() << "ns." << std::endl;

  test(matrix_c);

  return EXIT_SUCCESS;
}


