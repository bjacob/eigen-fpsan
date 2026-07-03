// Copyright (c) 2026 Advanced Micro Devices, Inc.
// SPDX-License-Identifier: MIT
//
// Exact solve demo: Semantics::Native changes under mathematically neutral
// scaling, while Semantics::Field proves the LU solves and the Householder QR
// family solves are each scale-invariant. Householder's root/sign choices remain
// separate algebraic paths.
//
#include "demo_utils.hpp"

#include <Eigen/Dense>

#include <iostream>

namespace {

template <class Scalar> Eigen::Matrix<Scalar, 4, 4> solve_matrix() {
  Eigen::Matrix<Scalar, 4, 4> a;
  a << Scalar(5.72f), Scalar(4.43f), Scalar(3.69f), Scalar(8.37f), Scalar(-5.85f), Scalar(5.13f),
      Scalar(2.25f), Scalar(7.49f), Scalar(2.28f), Scalar(-1.51f), Scalar(-7.77f), Scalar(-7.16f),
      Scalar(0.79f), Scalar(5.37f), Scalar(2.06f), Scalar(0.97f);
  return a;
}

template <class Scalar> Eigen::Matrix<Scalar, 4, 1> solve_rhs() {
  Eigen::Matrix<Scalar, 4, 1> b;
  b << Scalar(7.09f), Scalar(6.42f), Scalar(-8.93f), Scalar(-0.58f);
  return b;
}

template <fpsan::Semantics S> void print_results() {
  using Scalar = eigen_fpsan::demo::FpsanFloat<S>;
  using Matrix4 = Eigen::Matrix<Scalar, 4, 4>;
  using Vector4 = Eigen::Matrix<Scalar, 4, 1>;

  const Matrix4 a = solve_matrix<Scalar>();
  const Vector4 b = solve_rhs<Scalar>();
  const Scalar scale(1.3f);

  const Vector4 partial = a.partialPivLu().solve(b);
  const Vector4 partial_scaled = (scale * a).partialPivLu().solve(scale * b);
  const Vector4 full = a.fullPivLu().solve(b);
  const Vector4 full_scaled = (scale * a).fullPivLu().solve(scale * b);
  const Vector4 qr = a.householderQr().solve(b);
  const Vector4 qr_scaled = (scale * a).householderQr().solve(scale * b);
  const Vector4 col_piv_qr = a.colPivHouseholderQr().solve(b);
  const Vector4 col_piv_qr_scaled = (scale * a).colPivHouseholderQr().solve(scale * b);
  const Vector4 full_piv_qr = a.fullPivHouseholderQr().solve(b);
  const Vector4 full_piv_qr_scaled = (scale * a).fullPivHouseholderQr().solve(scale * b);

  eigen_fpsan::demo::print_section_header<S>("  These solves divide and pivot; Triton has limited "
                                             "division and order semantics, so rows split.");
  eigen_fpsan::demo::print_vector("PartialPivLU(A, b)", partial);
  eigen_fpsan::demo::print_vector("PartialPivLU(scale A, scale b)", partial_scaled);
  eigen_fpsan::demo::print_vector("FullPivLU(A, b)", full);
  eigen_fpsan::demo::print_vector("FullPivLU(scale A, scale b)", full_scaled);
  eigen_fpsan::demo::print_vector("HouseholderQR(A, b)", qr);
  eigen_fpsan::demo::print_vector("HouseholderQR(scale A, scale b)", qr_scaled);
  eigen_fpsan::demo::print_vector("ColPivHouseholderQR(A, b)", col_piv_qr);
  eigen_fpsan::demo::print_vector("ColPivHouseholderQR(scale A, scale b)", col_piv_qr_scaled);
  eigen_fpsan::demo::print_vector("FullPivHouseholderQR(A, b)", full_piv_qr);
  eigen_fpsan::demo::print_vector("FullPivHouseholderQR(scale A, scale b)", full_piv_qr_scaled);
}

} // namespace

int main() {
  print_results<fpsan::Semantics::Native>();
  print_results<fpsan::Semantics::Triton>();
  print_results<fpsan::Semantics::Field>();
  return 0;
}
