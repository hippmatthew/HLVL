#include "src/linalg/include/vec.hpp"

#include <cmath>
#include <stdexcept>

namespace la {

template<unsigned int N, typename T>
vec<N, T>::vec(std::initializer_list<T> list) {
  if (list.size() != N)
    throw std::runtime_error("hlvl: initializer list must be same size as vec");

  unsigned int i = 0;
  for (auto itr = list.begin(); itr != list.end(); ++itr)
    data[i++] = *itr;
}

template<unsigned int N, typename T>
template<typename U, typename>
vec<N, T>::vec(const vec<N, U>& v) {
  for (int i = 0; i < N; ++i)
    data[i] = static_cast<T>(v[i]);
}

template<unsigned int N, typename T>
vec<N, T>& vec<N, T>::operator = (std::initializer_list<T> list) {
  if (list.size() != N)
    throw std::runtime_error("hlvl: initializer list must be same size as vec");

  unsigned int i = 0;
  for (auto itr = list.begin(); itr != list.end(); ++itr)
    data[i++] = *itr;

  return *this;
}

template<unsigned int N, typename T>
template<typename U, typename>
vec<N, T>& vec<N, T>::operator = (const vec<N, U>& v) {
  for (int i = 0; i < N; ++i)
    data[i] = v[i];

  return *this;
}

template<unsigned int N, typename T>
T& vec<N, T>::operator [] (unsigned int index) {
  if (index > N - 1)
    throw std::runtime_error("hlvl: vec index out of bounds");

  return data[index];
}

template <unsigned int N, typename T>
const T& vec<N, T>::operator [] (unsigned int index) const {
  if (index > N - 1)
    throw std::runtime_error("hlvl: vec index out of bounds");

  return data[index];
}

template<unsigned int N, typename T>
bool vec<N, T>::operator == (const vec& rhs) const {
  for (unsigned int i = 0; i < N; ++i)
    if (data[i] != rhs.data[i]) return false;

  return true;
}

template<unsigned int N, typename T>
double vec<N, T>::operator * (const vec& rhs) const {
  double sum = 0;
  for (unsigned int i = 0; i < N; ++i)
    sum += data[i] * rhs[i];

  return sum;
}

template<unsigned int N, typename T>
vec<N, T> vec<N, T>::operator / (double rhs) const {
  vec res = vec::zero();
  for (int i = 0; i < N; ++i)
    res[i] = data[i] / rhs;

  return res;
}

template<unsigned int N, typename T>
vec<N, T> vec<N, T>::zero() {
  return vec<N, T>();
}

template <unsigned int N, typename T>
double vec<N, T>::magnitude() const {
  return sqrt(*this * *this);
}

template <unsigned int N, typename T>
vec<N, T> vec<N, T>::normalized() const {
  return *this / magnitude();
}

template <unsigned int N, typename T>
vec<3, T> vec<N, T>::cross(const vec<N, T>& rhs) const {
  if (N != 3)
    throw std::runtime_error("hlvl: cross product is only available for vec<3>");

  return vec<3, T>{
    data[1] * rhs.data[2] - data[2] * rhs.data[1],
    data[2] * rhs.data[0] - data[0] * rhs.data[2],
    data[0] * rhs.data[1] - data[1] * rhs.data[0]
  };
}

} // namespace la

template class la::vec<2, float>;
template class la::vec<3, float>;
template class la::vec<4, float>;
template class la::vec<2, double>;
template class la::vec<3, double>;
template class la::vec<4, double>;
template class la::vec<2, int>;
template class la::vec<3, int>;
template class la::vec<4, int>;
template class la::vec<2, unsigned int>;
template class la::vec<3, unsigned int>;
template class la::vec<4, unsigned int>;