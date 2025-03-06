#include "src/linalg/include/mat.hpp"

#include <stdexcept>

namespace la {

template <unsigned int N, typename T>
mat<N, T>::mat(std::initializer_list<vec<N, T>> list) {
  if (list.size() != N)
    throw std::runtime_error("hlvl: initializer list must be same size as mat");

  unsigned int index = 0;
  for (auto itr = list.begin(); itr != list.end(); ++itr)
    data[index++] = *itr;
}

template <unsigned int N, typename T>
template <typename U, typename>
mat<N, T>::mat(const mat<N, U>& m) {
  for (int i = 0; i < N; ++i)
    data[i] = m[i];
}

template <unsigned int N, typename T>
mat<N, T>& mat<N, T>::operator = (std::initializer_list<vec<N, T>> list) {
  if (list.size() != N)
    throw std::runtime_error("hlvl: initializer list must be same size as mat");

  unsigned int i = 0;
  for (auto itr = list.begin(); itr != list.end(); ++itr)
    data[i++] = *itr;

  return *this;
}

template <unsigned int N, typename T>
template <typename U, typename>
mat<N, T>& mat<N, T>::operator = (const mat<N, U>& m) {
  for (int i = 0; i < N; ++i)
    data[i] = m[i];

  return *this;
}

template <unsigned int N, typename T>
vec<N, T>& mat<N, T>::operator [] (unsigned int index) {
  if (index > N - 1)
    throw std::runtime_error("hlvl: mat index out of bounds");

  return data[index];
}

template <unsigned int N, typename T>
const vec<N, T>& mat<N, T>::operator [] (unsigned int index) const {
  if (index > N - 1)
    throw std::runtime_error("hlvl: mat index out of bounds");

  return data[index];
}

template <unsigned int N, typename T>
bool mat<N, T>::operator == (const mat& rhs) const {
  for (int i = 0; i < N; ++i)
    if (data[i] != rhs[i]) return false;

  return true;
}

template <unsigned int N, typename T>
mat<N, T> mat<N, T>::operator * (const mat& rhs) const {
  mat t = rhs.transpose();

  mat res;
  for (unsigned int i = 0; i < N; ++i) {
    for (unsigned int j = 0; j < N; ++j)
      res[i][j] = data[i] * t[j];
  }

  return res;
}

template <unsigned int N, typename T>
vec<N, T> mat<N, T>::operator * (const vec<N, T>& rhs) const {
  vec<N, T> res = vec<N, T>::zero();
  for (int i = 0; i < N; ++i)
    res[i] = data[i] * rhs;

  return res;
}

template <unsigned int N, typename T>
mat<N, T> mat<N, T>::operator / (double rhs) const {
  mat res;
  for (int i = 0; i < N; ++i)
    res[i] = data[i] / rhs;

  return res;
}

template <unsigned int N, typename T>
mat<N, T> mat<N, T>::identity() {
  mat res;
  for (int i = 0; i < N; ++i)
    res[i][i] = 1;

  return res;
}

template <unsigned int N, typename T>
mat<N, T> mat<N, T>::transpose() const {
  mat res;
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j)
      res[i][j] = data[j][i];
  }

  return res;
}

} // namespace la

template class la::mat<2, float>;
template class la::mat<3, float>;
template class la::mat<4, float>;
template class la::mat<2, double>;
template class la::mat<3, double>;
template class la::mat<4, double>;
template class la::mat<2, int>;
template class la::mat<3, int>;
template class la::mat<4, int>;
template class la::mat<2, unsigned int>;
template class la::mat<3, unsigned int>;
template class la::mat<4, unsigned int>;