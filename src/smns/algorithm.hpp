#pragma once
#include <type_traits>
#include <concepts>
#include <ranges>
#include <cstdint>
#include <cmath>
#include "concepts.hpp"
#include "math/constants.hpp"

namespace smns
{
  namespace math
  {
    namespace algorithm
    {
      template <std::random_access_iterator iterator_t>
        requires std::totally_ordered<std::iter_value_t<iterator_t>>
      auto find_median_5(iterator_t iterator) {
        // a, b, c, d, e
        auto a = iterator[0];
        auto b = iterator[1];
        auto c = iterator[2];
        auto d = iterator[3];
        auto e = iterator[4];

        if (a > b) std::swap(a, b);  // {a, b}
        if (c > d) std::swap(c, d);  // {c, d}
        if (a > c) {
          std::swap(a, c);
          std::swap(b, d);
        }  //{a, b} {c, d} with a <= c <= d, hence a - not median

        if (b > e) {
          std::swap(b, e);
        }  //{b, e} {c, d}

        if (b > c) {
          std::swap(c, b);
          std::swap(d, e);
        }  //{b, e} {c, d} with b <= c <= d, hence b - not median

        if (c > e) {
          std::swap(c, e);
        }

        return c;
      }

      //fast median finder for numbers
      template <concepts::math::arithmetic number_t>
        requires std::totally_ordered<number_t>
      auto find_median_3(number_t a, number_t b, number_t c) {
        if (a > b) std::swap(a, b);  // {a, b}
        if (b > c) std::swap(b, c);  // {b, c} => a ? b
        if (a > b) std::swap(a, b);  // {a, b, c}

        return b;
      }

      //fast median index finder for elements arr[a], arr[b], arr[c]
      template <std::forward_iterator iterator_t>
        requires std::totally_ordered<std::iter_value_t<iterator_t>>
      iterator_t find_median_index_3(iterator_t it1, iterator_t it2, iterator_t it3) {
        if (*it1 > *it2) std::swap(it1, it2);  // {it1, it2}
        if (*it2 > *it3) std::swap(it2, it3);  // {it2, it3} => it1 ? it2
        if (*it1 > *it2) std::swap(it1, it2);  // {it1, it2, it3}

        return it2;
      }

      //sorts 3 given numbers
      template <concepts::math::arithmetic number_t>
        requires std::totally_ordered<number_t>
      void sort_3(number_t& a, number_t& b, number_t& c) {
        if (a > b) std::swap(a, b);  // {a, b}
        if (b > c) std::swap(b, c);  // {b, c} => a ? b
        if (a > b) std::swap(a, b);  // {a, b, c}
      }

      //calculate amount of digits of a number with a lil error, but fast (O(1) timecomplexity)
      template <std::integral number_t>
      auto digits_fast(number_t number) {
        if constexpr (std::is_unsigned_v<number_t>) {
          //approximating log10_2 as 1233/4096, where devision by 4096 can be replaced as >> 12 
          //constexpr long double log10_2 = 0.3010299956639811952L;
          size_t digits = (std::bit_width(number) * 1233) >> 12;
          //fixing understimation
//(std::bit_width(512) and std::bit_width(1023) yield the same result)
          return digits + (number >= powers_of_10_lookup[digits]);
        } else {
          using unumber_t = std::make_unsigned_t<number_t>;
          //-1 if number < 0, 0 otherwise
          number_t mask = number >> (sizeof(number_t) * 8 - 1);
          //abs(), but avoiding overflow
          unumber_t unumber = (number + mask) ^ mask;
          size_t digits = (std::bit_width(unumber) * 1233) >> 12;
          return digits + (unumber >= powers_of_10_lookup[digits]) - mask;
        }
      }

      // calculate exact amount of digits of a number (O(log10(num)) timecomplexity)
      template <typename number_t, typename return_t = uint16_t>
        requires(std::is_integral_v<number_t>)
      return_t digits(number_t num) {
        return_t ans = 0;
        if constexpr (std::is_signed_v<number_t>) {
          if (num == std::numeric_limits<number_t>::min())
            return std::numeric_limits<number_t>::digits10 + 1 + 1;//+ 1 because digits10 doesn't count most significant digit and + 1 for sign
          if (num < 0) {
            ++ans;
            num = -num;
          }
        }

        do ++ans;
        while (num /= 10);

        return ans;
      }

      //calculate amount of digits of all elems in range [begin, end)
      template <std::forward_iterator iter_t>
        requires std::integral<std::iter_value_t<iter_t>>
      uint64_t sum_of_digits(iter_t begin, iter_t end) {
        uint64_t ans = 0;
        auto curr = begin;
        while (curr != end) ans += digits_exact(*(curr++));
        return ans;
      }
    }// namespace math
  //print all elements in range [begin, end)
    template <std::forward_iterator iter_t>
    void print_range(iter_t begin, iter_t end) {
      if (begin == end) return;
      std::cout << '{' << begin;
      for (auto curr = begin; ++curr != end; ) std::cout << ", " << *curr;
      std::cout << '}' << std::endl;
    }
  }//namespace algorithm

}//namepsace smns
