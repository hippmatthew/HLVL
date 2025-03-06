#pragma once

#include "src/linalg/include/vec.hpp"

#include <initializer_list>
#include <type_traits>

namespace la {

template <unsigned int N, typename T = float>
class alignas(
  (N == 2) * 2 * sizeof(T) + (N == 3 || N == 4) * 4 * sizeof(T)
) mat {
  static_assert(N > 1 && N < 5, "hlvl: mat size can only be 2, 3, or 4");
  static_assert(
    std::is_same<float, T>::value         ||
    std::is_same<double, T>::value        ||
    std::is_same<int, T>::value           ||
    std::is_same<unsigned int, T>::value,
    "hlvl: mat type must be either float, double, int, or unsigned int"
  );

  public:
    mat() = default;
    mat(const mat&) = default;
    mat(mat&&) = default;
    mat(std::initializer_list<vec<N, T>>);

    template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    mat(const mat<N, U>&);

    ~mat() = default;

    mat& operator = (const mat&) = default;
    mat& operator = (mat&&) = default;
    mat& operator = (std::initializer_list<vec<N, T>>);

    template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    mat& operator = (const mat<N, U>&);

    vec<N, T>& operator [] (unsigned int);
    const vec<N, T>& operator [] (unsigned int) const;

    bool operator == (const mat&) const;

    mat operator * (const mat&) const;
    vec<N, T> operator * (const vec<N, T>&) const;
    mat operator / (double) const;

    static mat identity();

    mat transpose() const;

  private:
    vec<N, T> data[N] = { vec<N, T>::zero() };
};

} // namespace la

template <unsigned int N, typename T>
la::mat<N, T> operator * (double lhs, const la::mat<N, T>& rhs) {
  la::mat<N, T> res;
  for (int i = 0; i < N; ++i)
    res[i] = lhs * rhs[i];

  return res;
}