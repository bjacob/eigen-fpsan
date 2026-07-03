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
  } else if constexpr (S == fpsan::Semantics::Triton) {
    return "Semantics::Triton payloads";
  } else {
    return "Semantics::Field payloads";
  }
}

template <fpsan::Semantics S>
const char *section_comment(const char *triton_comment = nullptr,
                            const char *field_comment = nullptr) {
  if constexpr (S == fpsan::Semantics::Native) {
    return "  Rounding is path-dependent, so equivalent formulas can differ.";
  } else if constexpr (S == fpsan::Semantics::Triton) {
    return triton_comment != nullptr ? triton_comment
                                     : "  Triton preserves +, -, * payload algebra; division/root "
                                       "laws and order comparisons are conditional.";
  } else {
    return field_comment != nullptr ? field_comment
                                    : "  Dyadic +, -, *, and / are exact; order treats finite "
                                      "nonzero quadratic residues as positive.";
  }
}

template <fpsan::Semantics S>
void print_section_header(const char *triton_comment = nullptr,
                          const char *field_comment = nullptr) {
  std::cout << "\n"
            << section_name<S>() << "\n"
            << section_comment<S>(triton_comment, field_comment) << "\n";
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
  std::cout << "  " << std::left << std::setw(54) << label << std::right;
  print_value(value);
  std::cout << "\n";
}

template <class Derived>
void print_vector(const char *label, const Eigen::MatrixBase<Derived> &value) {
  std::cout << "  " << std::left << std::setw(54) << label << std::right;
  for (Eigen::Index i = 0; i < value.size(); ++i) {
    std::cout << " ";
    print_value(value(i));
  }
  std::cout << "\n";
}

} // namespace eigen_fpsan::demo

#endif // EIGEN_FPSAN_DEMO_UTILS_HPP
