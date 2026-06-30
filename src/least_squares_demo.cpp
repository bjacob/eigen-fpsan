// Copyright (c) 2026 Advanced Micro Devices, Inc.
// SPDX-License-Identifier: MIT
//
// Least-squares demo: Semantics::Native changes under global scaling, while
// Semantics::Field proves the HouseholderQR least-squares solution is
// scale-invariant.
//
#include "demo_utils.hpp"

#include <Eigen/Dense>

#include <iostream>

namespace {

template <class Scalar> Eigen::Matrix<Scalar, 4, 3> least_squares_matrix() {
  Eigen::Matrix<Scalar, 4, 3> a = Eigen::Matrix<Scalar, 4, 3>::Zero();
  a(0, 0) = Scalar(-6.72f);
  a(1, 1) = Scalar(2.63f);
  a(2, 2) = Scalar(-4.03f);
  return a;
}

template <class Scalar> Eigen::Matrix<Scalar, 4, 1> least_squares_rhs() {
  Eigen::Matrix<Scalar, 4, 1> b;
  b << Scalar(6.08f), Scalar(-7.85f), Scalar(-3.74f), Scalar(5.0f);
  return b;
}

template <fpsan::Semantics S> void print_results() {
  using Scalar = eigen_fpsan::demo::FpsanFloat<S>;
  using Matrix43 = Eigen::Matrix<Scalar, 4, 3>;
  using Vector3 = Eigen::Matrix<Scalar, 3, 1>;
  using Vector4 = Eigen::Matrix<Scalar, 4, 1>;

  const Matrix43 a = least_squares_matrix<Scalar>();
  const Vector4 b = least_squares_rhs<Scalar>();
  const Scalar scale(1.3f);

  const Vector3 qr = a.householderQr().solve(b);
  const Vector3 qr_scaled = (scale * a).householderQr().solve(scale * b);

  std::cout << "\n" << eigen_fpsan::demo::section_name<S>() << "\n";
  eigen_fpsan::demo::print_vector("HouseholderQR(A, b)", qr);
  eigen_fpsan::demo::print_vector("HouseholderQR(scale A, scale b)", qr_scaled);
}

} // namespace

int main() {
  print_results<fpsan::Semantics::Native>();
  print_results<fpsan::Semantics::Field>();
  return 0;
}
