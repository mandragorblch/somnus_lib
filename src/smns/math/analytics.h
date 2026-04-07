#pragma once
#include <functional>
#include "../defs.h"
#include <concepts>

namespace smns::analytic {
  using real = ::smns::defs::real;
  using complex = ::smns::defs::complex;

  enum class SOLVER_TYPE : uint8_t {
    REAL,
    COMPLEX
  };

template<typename func_t>
concept implicit_function =
requires(func_t f, real x, real y)
{
    { f(x, y) } -> std::convertible_to<real>;
};

template<typename func_t>
concept explicit_real_function =
requires(func_t f, real x)
{
    { f(x) } -> std::convertible_to<real>;
};

template<typename func_t>
concept explicit_complex_function =
requires(func_t f, complex x)
{
    { f(x) } -> std::convertible_to<complex>;
};

template<implicit_function func_t>
bool is_inside(const func_t& f, real x, real y) {
  return f(x, y) < 0;
}

template<explicit_real_function func_t>
real f_prime_real(
    const func_t& f, real x,
    const real dx = std::sqrt(std::numeric_limits<real>::epsilon())) { 
    return (f(x + dx) - f(x - dx)) / (2 * dx);
}

template<explicit_complex_function func_t>
real f_prime_complex(
    const func_t& f, real x,
    const real dx = std::sqrt(std::numeric_limits<real>::epsilon())) {
  std::complex x_idx{x, dx};
  auto res = f(x_idx);
  return std::imag(res) / dx;
}

//derivative of implicit function
template<implicit_function func_t>
real f_prime_x_real(
    const func_t& f, real x, real y,
    const real dx = std::sqrt(std::numeric_limits<real>::epsilon())) { 
    return (f(x + dx, y) - f(x - dx, y)) / (2 * dx);
}

//derivative of implicit function
template<implicit_function func_t>
real f_prime_y_real(const func_t& f, real x, real y,
    const real dy = std::sqrt(std::numeric_limits<real>::epsilon())) {

    return (f(x, y + dy) - f(x, y - dy)) / (2 * dy);
}
//TODO complex version of derivatives of implicit function



//f(x) = g(x)
template<explicit_complex_function func_t>
real newton_solver_complex(
    const func_t& f,
    const func_t& g, const real guess,
    const size_t iteration_max = 100,
    const real precision = std::sqrt(std::numeric_limits<real>::epsilon())) {
  auto h = [&](auto x) { return f(x) - g(x); };
  real x = guess;

  for (size_t it = 0; it < iteration_max; ++it) {
    real fx = std::real(h(x));


    auto dfx = f_prime_complex(h, x);

    if (std::abs(dfx) < precision) break;

    real step = fx / dfx;
    x -= step;

    if (std::abs(step) < precision) break;
  }

  return x;
}

//f(x) = 0
template<explicit_complex_function func_t>
real newton_solver_complex(
    const func_t& f, const real guess,
    const real iteration_max = 100,
    const real precision = std::sqrt(std::numeric_limits<real>::epsilon())) {
   real x = guess;

  for (uint32_t it = 0; it < iteration_max; ++it) {
    real fx = std::real(f(x));
    real dfx = f_prime_complex(f, x);

    if (std::abs(dfx) < precision) break;

    real step = fx / dfx;
    x -= step;

    if (std::abs(step) < precision) break;
  }

  return x;
}

//f(x, const y) = 0
template<implicit_function func_t>
real newton_solver_const_y(
    const func_t& f, const real guess_x, const real y,
  const real iteration_max = 100,
    const real precision = std::sqrt(std::numeric_limits<real>::epsilon())) {
   real x = guess_x;

  for (uint32_t it = 0; it < iteration_max; ++it) {
    real fxy = f(x, y);
    real dfx = f_prime_x_real(f, x, y);

    if (std::abs(dfx) < precision) break;

    real step = fxy / dfx;
    x -= step;

    if (std::abs(step) < precision) break;
  }

  return x;
}

//f(const x, y) = 0
template<implicit_function func_t>
real newton_solver_const_x(
    const func_t& f, const real x, const real guess_y,
  const real iteration_max = 100,
    const real precision = std::sqrt(std::numeric_limits<real>::epsilon())) {
   real y = guess_y;

  for (uint32_t it = 0; it < iteration_max; ++it) {
    real fxy = f(x, y);
    real dfy = f_prime_y_real(f, x, y);

    if (std::abs(dfy) < precision) break;

    real step = fxy / dfy;
    y -= step;

    if (std::abs(step) < precision) break;
  }

  return y;
}

template<implicit_function func_t>
real sdf_from_implicit(
    const func_t& f, real x, real y,
    const real eps = std::sqrt(std::numeric_limits<real>::epsilon())) {

    auto fx = f_prime_x_real(f, x, y);
    auto fy = f_prime_y_real(f, x, y);
    auto grad = std::sqrt(fx*fx + fy*fy);
    if (std::abs(grad) <= eps) return 0; // avoid division by very small values
    return f(x,y) / grad;
}
}  // namespace analytic

