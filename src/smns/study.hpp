#pragma once
#include <algorithm>

namespace study {
  template <typename number_t, size_t sz>
  number_t find_nth_biggest(std::array<number_t, sz> & arr, size_t n) {
    for (size_t i = sz - 1; i >= sz - n; --i) {
      for (size_t j = 0; j < i; ++j) {
        if (arr[j] > arr[j + 1]) std::swap(arr[j], arr[j + 1]);
      }
    }
    return arr[sz - n];
  }

  // pivot - arr[end]. just swap your pivot with it. trust me, i'm a possum!
  template <typename number_t, size_t sz>
  size_t partition(std::array<number_t, sz> & arr, size_t begin = 0,
                   size_t end = sz - 1) {
    // two iterators method
    auto left_it = begin;
    auto right_it = end;

    while (left_it < right_it) {
      // left_it scans for the first element >= pivot (if we need such an order)
      // in the last iteration it'll go until left_it = right_it, which is the
      // first element >= pivot
      while (left_it < right_it && (arr[left_it] < arr[end])) ++left_it;
      // right_it scans for the first element < pivot
      // in the last iteration it'll go until right_ it = left_it which is the
      // last element >= pivot (due to the preceding while)
      while (left_it < right_it && (arr[right_it] >= arr[end])) --right_it;

      // swap appropriate elements. it's tempting to immediately advance both of
      // them, but it'll shift right_it from the last element >= pivot, which
      // will break the loop's logic
      std::swap(arr[left_it++], arr[right_it]);
    }
    // now left_it is the first element >= pivot or next to it (if it has been
    // swapped when right_it = left_it) so we check that left_it points to the
    // element >= pivot and then we can swap it with our pivot now we have all
    // elements less than pivot on the left from it, and all elements more than
    // or equal to pivot on the right, hence our pivot is in its final position
    std::swap(arr[left_it > right_it ? --left_it : left_it], arr[end]);
    return left_it;
  }

  template <typename number_t, size_t sz>
  number_t find_nth_biggest_fast(std::array<number_t, sz> & arr, size_t n,
                                 size_t begin = 0, size_t end = sz - 1) {
    auto curr = (end + begin) / 2;
    std::swap(arr[curr], arr[end]);
    curr = partition(arr, begin, end);
    if (curr == n)
      return arr[curr];
    else if (curr > n)
      return find_nth_biggest_fast(arr, n, begin, curr - 1);
    else
      return find_nth_biggest_fast(arr, n, curr + 1, end);
  }

  template <typename number_t, size_t sz>
  number_t find_nth_biggest_faster(std::array<number_t, sz> & arr, size_t n,
                                   size_t begin = 0, size_t end = sz - 1) {
    auto curr = (end + begin) / 2;
    smns::algos::sort_3(arr[begin], arr[curr], arr[end]);
    std::swap(arr[curr], arr[end]);
    curr = partition(arr, begin, end);
    if (curr == n)
      return arr[curr];
    else if (curr > n)
      return find_nth_biggest_fast(arr, n, begin, curr - 1);
    else
      return find_nth_biggest_fast(arr, n, curr + 1, end);
  }

  template <size_t sz>
  void insertion_sort(std::array<int, sz> & arr) {
    for (size_t i = 1; i < sz; ++i)
      for (size_t j = i; j >= 1; --j) {
        if (arr[j - 1] > arr[j])
          std::swap(arr[j - 1], arr[j]);
        else
          break;
      }
  }

  template <size_t sz>
  size_t lower_bound_linear(std::array<int, sz> & arr, int val) {
    size_t ans = 0;
    while (arr[ans] < val) ++ans;
    return ans;
  }

  template <size_t sz>
  size_t recursive_part(const std::array<int, sz>& arr, int val,
                        size_t begin = 0, size_t end = sz - 1) {
    if (begin == end) return begin;
    auto pivot =
        begin + (end - begin) /
                    2;  // prevent overflow (idk how u would get it, but yeah)
    if (arr[pivot] < val)
      return recursive_part(arr, val, pivot + 1,
                            end);  // excluding pointer, because we r not
                                   // interested in such element
    else
      return recursive_part(
          arr, val, begin,
          pivot);  // including pivot to converge to it if that's the first
                   // element that >= val (that's what we r lookin' for)
  }

  // begin and end must be valid and sorted range, or u will make the author
  // hiccup (pls, don't :p)
  template <size_t sz>
  size_t lower_bound_recursive(const std::array<int, sz>& arr, int val,
                               size_t begin = 0, size_t end = sz - 1) {
    if constexpr (sz == 0) return 0;
    if (begin > end) return begin;
    auto ans = recursive_part(arr, val, begin, end);
    return (ans == sz - 1 && arr[ans] < val) ? ans + 1 : ans;
  }

  //[begin, end) must be valid and sorted range, or u will make the author
  //hiccup (pls, don't :p)
  template <size_t sz>
  size_t lower_bound_iterative(const std::array<int, sz>& arr, int val,
                               size_t begin = 0, size_t end = sz) {
    while (begin != end) {
      auto pivot = begin + (end - begin) / 2;
      if (arr[pivot] < val)
        begin = pivot + 1;
      else
        end = pivot;
    }

    return begin;
  }

  template <size_t sz>
  void insertion_sort_bin_search(std::array<int, sz> & arr) {
    for (size_t curr_el_it = 1; curr_el_it < sz; ++curr_el_it) {
      auto curr_el = arr[curr_el_it];
      auto ins_pos = lower_bound_iterative(arr, curr_el, 0, curr_el_it);
      //good flex, but bad for understanding
      // std::rotate(arr.begin() + ins_pos, curr_el_it, std::next(curr_el_it));
      for (size_t i = curr_el_it; i > ins_pos; --i) {
        arr[i] = arr[i - 1];
      }
      arr[ins_pos] = curr_el;
      // print_arr(arr);
    }
  }

  template <size_t sz>
  void bubble_sort(std::array<int, sz> & arr) {
    for (size_t i = 1; i < sz; ++i)
      for (size_t j = 0; j < sz - i; ++j)
        if (arr[j] > arr[j + 1]) std::swap(arr[j], arr[j + 1]);
  }

   //go until it stops swapping
  template <size_t sz>
  void bubble_sort_with_flag(std::array<int, sz> & arr) {
    bool swapped = true;
    for (size_t i = 1; swapped; ++i) {
      swapped = false;
      for (size_t j = 0; j < sz - i; ++j) {
        if (arr[j] > arr[j + 1]) {
          std::swap(arr[j], arr[j + 1]);
          swapped = true;
        }
      }
    }
  }

  // i'm using the fact that we never go after last swapped (because we are choosing max element each time). may cut out some
  // elements, but on tests it's worse then with flag
  template <size_t sz>
  void bubble_sort_shortcut(std::array<int, sz> & arr) {
    size_t last_swapped_index = sz - 1;
    size_t curr_last_swapped_index;
    for (size_t i = 1; last_swapped_index > 0; ++i) {
      curr_last_swapped_index = 0;
      for (size_t j = 0; j < last_swapped_index; ++j) {
        if (arr[j] > arr[j + 1]) {
          std::swap(arr[j], arr[j + 1]);
          curr_last_swapped_index = j;
        }
      }
      last_swapped_index = curr_last_swapped_index;
    }
  }
}