# eigen-fpsan

Tiny C++ demos combining Eigen custom scalar support with `hip-fpsan`'s
`fpsan::Value<float, ...>` wrapper.

The demos run the same Eigen code with `Semantics::Native`, `Semantics::Triton`,
and algebraic FPSan `Semantics::Field`, in that order. Native results are
printed as ordinary floats; Triton and Field results are printed as full FPSan
payloads. QR is limited to non-iterative Householder-based Eigen algorithms;
SVD, eigenvalues, and other iterative methods are intentionally left out.

## Build

```bash
git submodule update --init --recursive
cmake -S . -B ../eigen-fpsan-build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build ../eigen-fpsan-build
ctest --test-dir ../eigen-fpsan-build --output-on-failure
```

The top-level CMake file rejects in-source configuration.
