#pragma once
#include <concepts>
#include <type_traits>

namespace smns
{
   namespace concepts
   {
      namespace math
      {
         template<typename number_t>
         concept arithmetic = std::is_arithmetic_v<number_t>;
      }
   }
}