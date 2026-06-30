// Copyright (c) 2026 Advanced Micro Devices, Inc.
// SPDX-License-Identifier: MIT
//
// Shared printing helpers for the small Eigen + FPSan demo programs.
//
#ifndef EIGEN_FPSAN_DEMO_UTILS_HPP
#define EIGEN_FPSAN_DEMO_UTILS_HPP

#include "eigen_fpsan/eigen_fpsan.hpp"

#include <Eigen/Dense>

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>

namespace eigen_fpsan::demo {

template <fpsan::Semantics S>
using FpsanFloat = fpsan::Value<float, S, fpsan::Conversions::Explicit>;

template <fpsan::Semantics S> const char *section_name() {
  if constexpr (S == fpsan::Semantics::Native) {
    return "Semantics::Native values";
  } else {
    return "Semantics::Field payloads";
  }
}

template <class Scalar> void print_value(const Scalar &value) {
  if constexpr (Scalar::semantics == fpsan::Semantics::Native) {
    std::cout << std::defaultfloat << std::setprecision(std::numeric_limits<float>::max_digits10)
              << value.to_float();
  } else {
    std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0')
              << static_cast<std::uint64_t>(value.fpsan_payload()) << std::dec << std::setfill(' ');
  }
}

template <class Scalar> void print_scalar(const char *label, const Scalar &value) {
  std::cout << "  " << std::left << std::setw(42) << label << std::right;
  print_value(value);
  std::cout << "\n";
}

template <class Derived>
void print_vector(const char *label, const Eigen::MatrixBase<Derived> &value) {
  std::cout << "  " << std::left << std::setw(42) << label << std::right;
  for (Eigen::Index i = 0; i < value.size(); ++i) {
    std::cout << " ";
    print_value(value(i));
  }
  std::cout << "\n";
}

} // namespace eigen_fpsan::demo

#endif // EIGEN_FPSAN_DEMO_UTILS_HPP
