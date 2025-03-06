#pragma once

#include <initializer_list>
#include <type_traits>

namespace la {

template<unsigned int N, typename T = float>
class alignas(
  (N == 2) * 2 * sizeof(T) + (N == 3 || N == 4) * 4 * sizeof(T)
) vec {
  static_assert(N > 1 && N < 5, "hlvl: vec size must be 2, 3, or 4");
  static_assert(
    std::is_same<float, T>::value         ||
    std::is_same<double, T>::value        ||
    std::is_same<int, T>::value           ||
    std::is_same<unsigned int, T>::value,
    "hlvl: vec type must be either float, double, int, or unsigned int"
  );

  public:
    vec() = default;
    vec(const vec&) = default;
    vec(vec&&) = default;
    vec(std::initializer_list<T>);

    template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    vec(const vec<N, U>&);

    ~vec() = default;

    vec& operator = (const vec&) = default;
    vec& operator = (vec&&) = default;
    vec& operator = (std::initializer_list<T>);

    template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    vec& operator = (const vec<N, U>&);

    T& operator [] (unsigned int);
    const T& operator [] (unsigned int) const;

    bool operator == (const vec&) const;

    double operator * (const vec&) const;
    vec operator / (double) const;

    static vec zero();

    double magnitude() const;
    vec<N, T> normalized() const;
    vec<3, T> cross(const vec&) const;

  private:
    T data[N] = { 0 };
};

} // namespace la

template<unsigned int N, typename T>
la::vec<N, T> operator * (double lhs, const la::vec<N, T>& rhs) {
  la::vec<N, T> res = rhs;
  for (int i = 0; i < N; ++i)
    res[i] *= lhs;

  return res;
}