// Copyright (c) 2026 Advanced Micro Devices, Inc.
// SPDX-License-Identifier: MIT
//
// Least-squares demo: Semantics::Native changes under global scaling, while
// Semantics::Field proves non-iterative least-squares solutions are
// scale-invariant when the control flow remains meaningful.
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
  const Vector3 col_piv_qr = a.colPivHouseholderQr().solve(b);
  const Vector3 col_piv_qr_scaled = (scale * a).colPivHouseholderQr().solve(scale * b);
  const Vector3 full_piv_qr = a.fullPivHouseholderQr().solve(b);
  const Vector3 full_piv_qr_scaled = (scale * a).fullPivHouseholderQr().solve(scale * b);
  const Vector3 cod = a.completeOrthogonalDecomposition().solve(b);
  const Vector3 cod_scaled = (scale * a).completeOrthogonalDecomposition().solve(scale * b);

  eigen_fpsan::demo::print_section_header<S>("  QR/COD use division/root steps and rank/pivot "
                                             "comparisons; Triton keeps those partly opaque.");
  eigen_fpsan::demo::print_vector("HouseholderQR(A, b)", qr);
  eigen_fpsan::demo::print_vector("HouseholderQR(scale A, scale b)", qr_scaled);
  eigen_fpsan::demo::print_vector("ColPivHouseholderQR(A, b)", col_piv_qr);
  eigen_fpsan::demo::print_vector("ColPivHouseholderQR(scale A, scale b)", col_piv_qr_scaled);
  eigen_fpsan::demo::print_vector("FullPivHouseholderQR(A, b)", full_piv_qr);
  eigen_fpsan::demo::print_vector("FullPivHouseholderQR(scale A, scale b)", full_piv_qr_scaled);
  eigen_fpsan::demo::print_vector("CompleteOrthogonalDecomposition(A, b)", cod);
  eigen_fpsan::demo::print_vector("CompleteOrthogonalDecomposition(scale A, scale b)", cod_scaled);
}

} // namespace

int main() {
  print_results<fpsan::Semantics::Native>();
  print_results<fpsan::Semantics::Triton>();
  print_results<fpsan::Semantics::Field>();
  return 0;
}
