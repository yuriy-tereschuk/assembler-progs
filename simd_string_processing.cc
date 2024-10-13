/*
 * GCC, x86_64
 * 
 *
 */
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <chrono>


#include <immintrin.h>

int get_occurrences_count(const char* base_string, const char* substring, int base_size, int sub_size)
{
  int occurrences = 0;
  bool is_ok = true;

  for (int i = 0; i < base_size; i++)
  {
    for (int j = 0; j < sub_size; j++)
    {
      if (base_string[i+j] != substring[j])
      {
        is_ok = false;
        break;
      }
    }

    if (is_ok)
    {
      ++occurrences;
      i += (sub_size - 1);
    }
    else
    {
      is_ok = true;
    }
  }

  return occurrences;
}

/*
 * Slow on small amount of data.
 */
int _simd_get_occurrences_count(const char* base_string, const char* substring, size_t base_size, size_t sub_size)
{
  int occurrences = 0;
  const __m256i first_substring_char = _mm256_set1_epi8(substring[0]);
  const __m256i last_substring_char = _mm256_set1_epi8(substring[sub_size - 1]);

  for (size_t i = 0; i < base_size; i += 32) {
  
    const __m256i lookup_from_begin = _mm256_load_si256(reinterpret_cast<const __m256i*>(base_string + i));
    // unalligned mem load slow down 
    const __m256i lookup_after_shift = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(base_string + i + sub_size -1));

    const __m256i eq_first = _mm256_cmpeq_epi8(first_substring_char, lookup_from_begin);
    const __m256i eq_last = _mm256_cmpeq_epi8(last_substring_char, lookup_after_shift);
    const __m256i all_mask = _mm256_and_si256(eq_first, eq_last);

    uint32_t mask = _mm256_movemask_epi8(all_mask); // compact byte mask into bit mask
    uint32_t pos = 0;
    uint8_t tmp = 0;

    while (mask != 0)
    {
      tmp = __builtin_ctz(mask); //GCC compiler specific function, returns count of trailing zero bits after right most set bit.
      pos = tmp > pos ? tmp : pos; 

      if (memcmp(base_string + i + pos, substring, sub_size ) == 0)
      {
        occurrences++;
        pos += sub_size;
      }
      mask = mask & (mask - 1); // clear rigth most set bit
    }
  }

  return occurrences;
}


int main(int argv, char** argc)
{
  std::string search_where = "000000000100011010000000000000001111000010000001101";
  std::string search_what = "1111";
 
  for (int i  = 10; i > 0; i--)
  {
    search_where.append(search_where);
  }

  char* _search_where;
  const char* _search_what = search_what.c_str();

  posix_memalign((void**) &_search_where, 32, search_where.size() * sizeof(char));

  std::copy(search_where.begin(), search_where.end(), _search_where);
   
  auto begin = std::chrono::steady_clock().now();
  int occurrences = get_occurrences_count(_search_where, _search_what, search_where.size(), search_what.size());
  auto end = std::chrono::steady_clock().now();
  auto diff = end - begin;

  std::cout << "Occurrences count: " << occurrences << "; In time: " << diff.count() << "ns." << std::endl;

  begin = std::chrono::steady_clock().now();
  occurrences = _simd_get_occurrences_count(_search_where, _search_what, search_where.size(), search_what.size());
  end = std::chrono::steady_clock().now();
  diff = end - begin;
  
  std::cout << "SIMD, Occurrences count: " << occurrences << "; In time: " << diff.count() << "ns." << std::endl;

  delete[] _search_where;

  return EXIT_SUCCESS;
}
