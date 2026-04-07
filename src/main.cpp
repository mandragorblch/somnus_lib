
#include <algorithm>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>
#include <bitset>

#include "smns/math/types/bigInt.hpp"
#include "smns/types/string.h"
#include "smns/study.hpp"

// #define LOG

uint64_t counter;

typedef long double real_t;

//using real_t = long double;

real_t f(real_t x) {
  return x * std::cbrt(4 - x);
}

real_t f_prime(real_t x) {
  constexpr real_t epsilon = 1e-12;
  return (f(x + epsilon) - f(x - epsilon)) / (epsilon * 2);
}

real_t L(real_t x) {
  return std::sqrt(1 + f_prime(x) * f_prime(x));
}

real_t simpson(real_t a, real_t b, uint64_t n) {
  n += (n & 1);
  real_t ans = 0;
  real_t dx = (b - a) / n;

  for(uint64_t i = 1; i < n; i += 2) {
    ans += L(a + i * dx);
  }
  ans *= 2;

  for (uint64_t i = 2; i < n; i += 2) {
    ans += L(a + i * dx);
  }
  ans *= 2;

  ans += L(a) + L(b);

  ans *= dx / 3;

  return ans;
}

uint64_t binomial(uint64_t k, uint64_t n) {
  return std::llround(1.0L / ((n + 1) * std::beta(n - k + 1, k + 1)));
}

template<typename T>
void print_arr(const T& arr) {
  std::cout << "\n{ ";
  if (arr.size()) {
    auto iter = arr.begin();
    std::cout << *iter;
    ++iter;
    for (; iter != arr.end(); ++iter) std::cout << ", " << *iter;
  }
  std::cout << " }\n";
}

void print_tree(std::vector<int>& arr) {
  std::cout << '\n';
  //ceil(log2(arr.size() + 1)), but 100% accurate
  size_t nearest_log2 = 0;
  {
    size_t sz = arr.size();
    while (sz) {
      sz /= 2;
      ++nearest_log2;
    }
  }
  size_t nearest_2_power = 1 << nearest_log2;
  size_t step = nearest_2_power;
  size_t last_layer_size = nearest_2_power / 2;
  std::string curr_layer;
  curr_layer.resize(2 * last_layer_size + 1);
  size_t curr_el_in_arr = 0;
  size_t elements_in_curr_layer = 1;
  for (size_t layer = 0; layer < nearest_log2; ++layer) {
    std::fill(curr_layer.begin(), curr_layer.end(), ' ');
    step /= 2;
    size_t curr_pos = step;
    for (size_t i = 0; i < elements_in_curr_layer && curr_el_in_arr < arr.size(); ++i, ++curr_el_in_arr) {
      curr_layer[curr_pos] = (arr[curr_el_in_arr] < 10 ? '0' + arr[curr_el_in_arr] : 'A' + arr[curr_el_in_arr] - 10);
      curr_pos += step * 2;
    }
    std::cout << curr_layer;
    for(size_t i = 0; i < step / 2; ++i) {
      std::cout << '\n';
    }
    elements_in_curr_layer *= 2;
  }
  std::cout << '\n';
}

void FixHeap(std::vector<int>& arr, size_t root_pos, int key, size_t right_bound) {
  size_t vacant = root_pos + 1;
  while (2 * vacant - 1 < right_bound) {
    //assuming
    size_t max_child_pos = 2 * vacant - 1;
    //fixing
    if (max_child_pos < right_bound && arr[max_child_pos + 1] > arr[max_child_pos]) {
      ++max_child_pos;
    }

    //if key became larger, we shoulda put it in prev vacant place
    //therefore it yields cool-ass tree :3
    if (key > arr[max_child_pos]) break;

    //shifting our vacant to the largest child
    //therefore it yields cool-ass tree :3
    arr[vacant - 1] = arr[max_child_pos];
    vacant = max_child_pos + 1;
  }
  arr[vacant - 1] = key;
  print_tree(arr);
}

void heapSort(std::vector<int>& arr) {
  //arr.size() / 2 because leftover is all leafs
  //and they r automaticaly being poked
  for (size_t i = arr.size() / 2; i-- > 0; ) {
    FixHeap(arr, i, arr[i], arr.size());
  }

  std::cout << "\nthe tree above is built\n";

  //extracting max element and then fixing our heap
  for (size_t i = arr.size() - 1; i > 0; --i) {
    auto max = arr[0];
    FixHeap(arr, 0, arr[i], i);
    arr[i] = max;
  }
}

int main() {
  
  std::cout << '\n';
  system("pause");
  {
    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::vector<int> arr((1 << 4) - 1);
    std::iota(arr.begin(), arr.end(), 1);
    for (size_t i = 0; i < arr.size(); ++i) {
      auto num = rng() % (i + 1);
      std::swap(arr[num], arr[i]);
    }
    print_arr(arr);
    print_tree(arr);

    heapSort(arr);

    print_tree(arr);

    std::cout << std::boolalpha << std::is_sorted(arr.begin(), arr.end()) << '\n';
    print_arr(arr);
    system("pause");
    return 0;
  }
  
  std::cout << '\n';
  system("pause");
}