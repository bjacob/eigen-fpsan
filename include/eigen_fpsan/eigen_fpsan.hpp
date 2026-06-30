// Copyright (c) 2026 Advanced Micro Devices, Inc.
// SPDX-License-Identifier: MIT
//
// eigen_fpsan/eigen_fpsan.hpp
//
// Minimal Eigen integration for fpsan::Value scalar types.
//
#ifndef EIGEN_FPSAN_EIGEN_FPSAN_HPP
#define EIGEN_FPSAN_EIGEN_FPSAN_HPP

#include <fpsan/fpsan.hpp>

#include <cmath>
#include <limits>
#include <type_traits>

namespace fpsan {

template <class FT, Semantics S, Conversions C> constexpr Value<FT, S, C> abs(Value<FT, S, C> x) {
  using V = Value<FT, S, C>;
  return x < V(FT(0)) ? -x : x;
}

template <class FT, Semantics S, Conversions C> constexpr Value<FT, S, C> real(Value<FT, S, C> x) {
  return x;
}

template <class FT, Semantics S, Conversions C> constexpr Value<FT, S, C> imag(Value<FT, S, C>) {
  return Value<FT, S, C>(FT(0));
}

template <class FT, Semantics S, Conversions C> constexpr Value<FT, S, C> conj(Value<FT, S, C> x) {
  return x;
}

template <class FT, Semantics S, Conversions C> bool isfinite(Value<FT, S, C> x) {
  using V = Value<FT, S, C>;
  if constexpr (V::is_algebraic) {
    return x.fpsan_payload() < V::alg_cfg().n;
  } else {
    using std::isfinite;
    return isfinite(x.to_float());
  }
}

template <class FT, Semantics S, Conversions C> bool isinf(Value<FT, S, C> x) {
  using V = Value<FT, S, C>;
  if constexpr (V::is_algebraic) {
    return x.fpsan_payload() == V::alg_cfg().inf_code;
  } else {
    using std::isinf;
    return isinf(x.to_float());
  }
}

template <class FT, Semantics S, Conversions C> bool isnan(Value<FT, S, C> x) {
  using V = Value<FT, S, C>;
  if constexpr (V::is_algebraic) {
    return x.fpsan_payload() == V::alg_cfg().nan_code;
  } else {
    using std::isnan;
    return isnan(x.to_float());
  }
}

} // namespace fpsan

#include <Eigen/Dense>

namespace Eigen {

template <class FT, fpsan::Semantics S, fpsan::Conversions C>
struct NumTraits<fpsan::Value<FT, S, C>> : GenericNumTraits<fpsan::Value<FT, S, C>> {
  using Scalar = fpsan::Value<FT, S, C>;
  using Real = Scalar;
  using NonInteger = Scalar;
  using Literal = Scalar;
  using Nested = Scalar;

  enum {
    IsComplex = 0,
    IsInteger = 0,
    IsSigned = 1,
    RequireInitialization = 1,
    ReadCost = 1,
    AddCost = 3,
    MulCost = 3
  };

  static constexpr Real epsilon() { return Real(FT(0)); }
  static constexpr Real dummy_precision() { return Real(FT(0)); }
  static constexpr Real highest() { return (std::numeric_limits<Scalar>::max)(); }
  static constexpr Real lowest() { return (std::numeric_limits<Scalar>::lowest)(); }
  static constexpr Real infinity() { return std::numeric_limits<Scalar>::infinity(); }
  static constexpr Real quiet_NaN() { return std::numeric_limits<Scalar>::quiet_NaN(); }
  static constexpr int digits() { return std::numeric_limits<FT>::digits; }
  static constexpr int digits10() { return std::numeric_limits<FT>::digits10; }
  static constexpr int max_digits10() { return std::numeric_limits<FT>::max_digits10; }
};

namespace internal {

template <class FT, fpsan::Semantics S, fpsan::Conversions C>
struct scalar_score_coeff_op<fpsan::Value<FT, S, C>> {
  using Scalar = fpsan::Value<FT, S, C>;
  using result_type = std::conditional_t<Scalar::is_fpsan, typename Scalar::bits_type, FT>;

  EIGEN_DEVICE_FUNC constexpr result_type operator()(const Scalar &x) const {
    if constexpr (Scalar::is_fpsan) {
      return x.fpsan_payload();
    } else {
      using std::abs;
      return abs(x.to_float());
    }
  }
};

template <class FT, fpsan::Semantics S, fpsan::Conversions C>
struct functor_traits<scalar_score_coeff_op<fpsan::Value<FT, S, C>>> {
  enum { Cost = 1, PacketAccess = false };
};

} // namespace internal
} // namespace Eigen

#endif // EIGEN_FPSAN_EIGEN_FPSAN_HPP
