#pragma once
#include <cstdint>
#include "../concepts.hpp"

namespace smns::types::math
{
      //counts an average of passed values
      //value_t is a type of value stored
      //result_t is a type of result representation
      //counter_t is a counter type
      template< concepts::math::arithmetic value_t = uint64_t,
         concepts::math::arithmetic counter_t = size_t,
         concepts::math::arithmetic result_t = long double>
      struct average_t {
         value_t accumulated = 0;
         counter_t calls = 0;

         average_t() = default;

         template <concepts::math::arithmetic number_t>
         average_t(number_t initial_value) : accumulated(static_cast<number_t>(initial_value), calls(static_cast<counter_t>(1))) {}

         template <concepts::math::arithmetic number_t>
         average_t& add(number_t val) {
            accumulated += static_cast<value_t>(val);
            ++calls;
            return *this;
         }

         template <concepts::math::arithmetic number_t>
         average_t& operator+=(number_t val) {
            return add(val);
         }

         void reset() {
            calls = 0;
            accumulated = 0;
         }

         result_t result() const {
            return static_cast<result_t>(accumulated) / calls;
         }

         result_t operator()() const {
            return result();
         }
      };
   }