#include "src/linalg/include/mat.hpp"

#include <stdexcept>
#include <cmath>

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
mat<N, T> mat<N, T>::operator + (const mat& rhs) const {
  mat res;
  for (unsigned int i = 0; i < N; ++i) {
    for (unsigned int j = 0; j < N; ++j)
      res[i][j] = data[i][j] + rhs[i][j];
  }

  return res;
}

template <unsigned int N, typename T>
mat<N, T> mat<N, T>::operator - (const mat& rhs) const {
  return *this + (-rhs);
}

template <unsigned int N, typename T>
mat<N, T> mat<N, T>::operator - () const {
  return -1 * *this;
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
mat<4, float> mat<N, T>::view(la::vec<3> eye, la::vec<3> target, la::vec<3> up) {
  la::vec<3> f = (target - eye).normalized();
  la::vec<3> r = f.cross(up).normalized();
  la::vec<3> u = r.cross(f);

  return mat<4, float>{
    {  r[0],  r[1],  r[2], static_cast<float>(-r * eye) },
    {  u[0],  u[1],  u[2], static_cast<float>(-u * eye) },
    { -f[0], -f[1], -f[2], static_cast<float>(f * eye)  },
    {   0  ,   0  ,   0  ,             1                }
  };
}

template <unsigned int N, typename T>
mat<4, float> mat<N, T>::projection(float fov, float aspectRatio, float near, float far) {
  float tanFOVinv = 1 / tan(fov / 2);
  float fmnInv = 1 / (far - near);

  return mat<4, float>{
    { tanFOVinv / aspectRatio, 0, 0, 0 },
    { 0, tanFOVinv, 0, 0 },
    { 0, 0, -(far + near) * fmnInv , -2 * far * near * fmnInv },
    { 0, 0, -1, 0 }
  };
}

template <unsigned int N, typename T>
mat<4, float> mat<N, T>::rotation(la::vec<3> rotator) {
  mat<4> rx = {
    { cos(rotator[0]), 0, sin(rotator[0]), 0 },
    { 0, 1, 0, 0 },
    { -sin(rotator[0]), 0, cos(rotator[0]), 0 },
    { 0, 0, 0, 1 }
  };

  mat<4> ry = {
    { 1, 0, 0, 0 },
    { 0, cos(rotator[1]), sin(rotator[1]), 0 },
    { 0, -sin(rotator[1]), cos(rotator[1]), 0 },
    { 0, 0, 0, 1 }
  };

  mat<4> rz = {
    { cos(rotator[2]), sin(rotator[2]), 0, 0 },
    { -sin(rotator[2]), cos(rotator[2]), 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 }
  };

  return rz * rx * ry;
}

template <unsigned int N, typename T>
mat<4, float> mat<N, T>::translation(la::vec<3> displacement) {
  return mat<4, float>{
    { 1, 0, 0, displacement[0] },
    { 0, 1, 0, displacement[1] },
    { 0, 0, 1, displacement[2] },
    { 0, 0, 0, 1 }
  };
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