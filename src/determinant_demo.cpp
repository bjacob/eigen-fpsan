// Copyright (c) 2026 Advanced Micro Devices, Inc.
// SPDX-License-Identifier: MIT
//
// Determinant demo: Semantics::Native exposes order-sensitive rounding while
// algebraic FPSan semantics test determinant computations through several
// decompositions.
//
#include "demo_utils.hpp"

#include <Eigen/Dense>

#include <array>
#include <iostream>

namespace {

template <class Scalar> Eigen::Matrix<Scalar, 4, 4> determinant_matrix() {
  Eigen::Matrix<Scalar, 4, 4> a;
  a << Scalar(-4.79f), Scalar(7.23f), Scalar(-2.62f), Scalar(3.66f), Scalar(5.65f), Scalar(6.83f),
      Scalar(-4.73f), Scalar(1.13f), Scalar(-6.07f), Scalar(-2.59f), Scalar(0.86f), Scalar(6.46f),
      Scalar(-7.63f), Scalar(9.57f), Scalar(-8.31f), Scalar(0.49f);
  return a;
}

template <class Matrix> typename Matrix::Scalar leibniz_determinant(const Matrix &a) {
  static constexpr std::array<std::array<int, 4>, 24> permutations = {
      {{{0, 1, 2, 3}}, {{0, 1, 3, 2}}, {{0, 2, 1, 3}}, {{0, 2, 3, 1}}, {{0, 3, 1, 2}},
       {{0, 3, 2, 1}}, {{1, 0, 2, 3}}, {{1, 0, 3, 2}}, {{1, 2, 0, 3}}, {{1, 2, 3, 0}},
       {{1, 3, 0, 2}}, {{1, 3, 2, 0}}, {{2, 0, 1, 3}}, {{2, 0, 3, 1}}, {{2, 1, 0, 3}},
       {{2, 1, 3, 0}}, {{2, 3, 0, 1}}, {{2, 3, 1, 0}}, {{3, 0, 1, 2}}, {{3, 0, 2, 1}},
       {{3, 1, 0, 2}}, {{3, 1, 2, 0}}, {{3, 2, 0, 1}}, {{3, 2, 1, 0}}}};
  static constexpr std::array<int, 24> signs = {1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1,
                                                1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1};

  typename Matrix::Scalar result(0.0f);
  for (std::size_t p = 0; p < permutations.size(); ++p) {
    typename Matrix::Scalar term = a(0, permutations[p][0]) * a(1, permutations[p][1]);
    term *= a(2, permutations[p][2]);
    term *= a(3, permutations[p][3]);
    result += signs[p] > 0 ? term : -term;
  }
  return result;
}

template <fpsan::Semantics S> void print_results() {
  using Scalar = eigen_fpsan::demo::FpsanFloat<S>;

  const auto a = determinant_matrix<Scalar>();

  eigen_fpsan::demo::print_section_header<S>("  Triton preserves +, -, * in the direct rows; "
                                             "decompositions add /, roots, and comparisons.",
                                             "  QR determinant fingerprints can differ when a "
                                             "norm/root path sees a value that is not a "
                                             "quadratic residue in the finite field.");
  eigen_fpsan::demo::print_scalar("direct Leibniz expansion", leibniz_determinant(a));
  eigen_fpsan::demo::print_scalar("Eigen fixed-size determinant", a.determinant());
  eigen_fpsan::demo::print_scalar("PartialPivLU determinant", a.partialPivLu().determinant());
  eigen_fpsan::demo::print_scalar("FullPivLU determinant", a.fullPivLu().determinant());
  eigen_fpsan::demo::print_scalar("HouseholderQR determinant", a.householderQr().determinant());
  eigen_fpsan::demo::print_scalar("ColPivHouseholderQR determinant",
                                  a.colPivHouseholderQr().determinant());
  eigen_fpsan::demo::print_scalar("FullPivHouseholderQR determinant",
                                  a.fullPivHouseholderQr().determinant());
  eigen_fpsan::demo::print_scalar("CompleteOrthogonalDecomposition determinant",
                                  a.completeOrthogonalDecomposition().determinant());
}

} // namespace

int main() {
  print_results<fpsan::Semantics::Native>();
  print_results<fpsan::Semantics::Triton>();
  print_results<fpsan::Semantics::Field>();
  print_results<fpsan::Semantics::SophieGermainRing>();
  print_results<fpsan::Semantics::PythagoreanRing>();
  return 0;
}
