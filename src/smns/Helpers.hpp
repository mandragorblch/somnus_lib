#pragma once
#include <algorithm>
#include <map>
#include <list>

extern uint64_t counter;

namespace smns {
  namespace types {
    struct average {
      uint64_t calls = 0;
      uint64_t total_sum = 0;

      average& add(uint64_t val) {
        ++calls;
        total_sum += val;
        return *this;
      }

      average& operator+=(uint64_t val) {
        return add(val);
      }

      long double res() {
        return static_cast<long double>(total_sum) / calls;
      } 
    };
  
    
  }

  namespace algos {
    template <typename T>
    requires(std::totally_ordered<T>)
    T find_median_5(const std::array<T, 5>& arr) {
      // a, b, c, d, e
      T a = arr[0];
      T b = arr[1];
      T c = arr[2];
      T d = arr[3];
      T e = arr[4];

      if (a > b) std::swap(a, b);  // {a, b}
      if (c > d) std::swap(c, d);  // {c, d}
      if (a > c) {
        std::swap(a, c);
        std::swap(b, d);
      }  //{a, b} {c, d} with a <= c <= d, hence a - not median
  #ifdef LOG
      print_arr(arr);
  #endif
      if (b > e) {
        std::swap(b, e);
      }  //{b, e} {c, d}
  #ifdef LOG
      print_arr(arr);
  #endif

      if (b > c) {
        std::swap(c, b);
        std::swap(d, e);
      }  //{b, e} {c, d} with b <= c <= d, hence b - not median

  #ifdef LOG
      print_arr(arr);
  #endif

      if (c > e) {
        std::swap(c, e);
      }

  #ifdef LOG
      print_arr(arr);
  #endif

      return c;
    }

    //fast median finder for numbers
    template <typename T>
    requires(std::is_arithmetic_v<T>)
    T find_median_3(T a, T b, T c) {
      if (a > b) std::swap(a, b);  // {a, b}
      if (b > c) std::swap(b, c);  // {b, c} => a ? b
      if (a > b) std::swap(a, b);  // {a, b, c}

      return b;
    }

    //fast median index finder for elements arr[a], arr[b], arr[c]
    template <typename T>
    size_t find_median_index_3(T& arr, size_t a, size_t b, size_t c) {
      if (arr[a] > arr[b]) std::swap(a, b);  // {a, b}
      if (arr[b] > arr[c]) std::swap(b, c);  // {b, c} => a ? b
      if (arr[a] > arr[b]) std::swap(a, b);  // {a, b, c}

      return b;
    }

    //sorts 3 given numbers
    template <typename T>
    requires(std::is_arithmetic_v<T>)
    void sort_3(T& a, T& b, T& c) {
      if (a > b) std::swap(a, b);  // {a, b}
      ++counter;
      if (b > c) std::swap(b, c);  // {b, c} => a ? b
      ++counter;
      if (a > b) std::swap(a, b);  // {a, b, c}
      ++counter;
    }

    //calculate amount of digits of a number with a lil error, but fast (O(1) timecomplexity)
    template <typename number_t, typename return_t = uint16_t>
      requires (std::is_integral_v<number_t>)
    return_t digits_fast(number_t num) {
      if(num == 0) return 1ull;

      if constexpr (std::is_signed_v<number_t>) {
        if (num == std::numeric_limits<number_t>::min())
          return std::numeric_limits<number_t>::digits10 + 1 + 1;//+ 1 for digits and + 1 for sign
        // //branchless flex
        //bool sign = num < 0;
        //num *= 1 - static_cast<number_t>(sign) * 2;
        //if(num == std::numeric_limits<number_t>::max()) return std::numeric_limits<number_t>::digits10 + 1 + sign;
        //return std::ceil(std::log10(num + 1)) + sign;

        if(num < 0) {
          if(num == std::numeric_limits<number_t>::max()) return std::numeric_limits<number_t>::digits10 + 1 + 1;//+ 1 for digits and + 1 for sign
          return std::ceil(std::log10(num + 1)) + 1;//+ 1 for sign
        }

        if(num == std::numeric_limits<number_t>::max()) return std::numeric_limits<number_t>::digits10 + 1;
        return std::ceil(std::log10(num + 1));
      } else {
        if(num == std::numeric_limits<number_t>::max()) return std::numeric_limits<number_t>::digits10 + 1;
        return std::ceil(std::log10(num + 1));
      }
    }

    // calculate exact amount of digits of a number (O(log10(num)) timecomplexity)
    template <typename number_t, typename return_t = uint16_t>
      requires(std::is_integral_v<number_t>)
    return_t digits(number_t num) {
      if (num == 0) return 1u;

      return_t ans = 0;
      if constexpr (std::is_signed_v<number_t>) {
        if (num == std::numeric_limits<number_t>::min())
          return std::numeric_limits<number_t>::digits10 + 1 + 1;//+ 1 for digits and + 1 for sign
        if (num < 0) ++ans, num = -num;
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

    //print all elements in range [begin, end)
    template <std::forward_iterator iter_t>
    void print_range(iter_t begin, iter_t end) {
      if (begin == end) return;
      std::cout << '{' << begin;
      for (auto curr = begin; ++curr != end; ) std::cout << ", " << *curr;
      std::cout << '}' << std::endl;
    }
  }
}