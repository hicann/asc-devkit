#!/usr/bin/env python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""Run actual LnCompute, SqrtImpl, Log2Impl and Log10Impl with scalar host instruction models.

The vln model flushes subnormal operands, exposing missing compensation at
exactly the largest subnormal. This is an algorithm regression, not NPU accuracy
certification. Pass --baseline to run the unmodified audit baseline body.
"""

import argparse
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
HEADER = "impl/basic_api/reg_compute/dav_{arch}/kernel_reg_compute_vec_unary_impl.h"
PRELUDE = r"""
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <type_traits>
using half = _Float16;
#define __simd_callee__
struct DefaultType {};
enum class MaskMergeMode { ZEROING };
enum class LogAlgo { INTRINSIC, PRECISION_1ULP_FTZ_TRUE, PRECISION_1ULP_FTZ_FALSE };
struct LogSpecificMode { MaskMergeMode mrgMode; LogAlgo algo; };
namespace Internal {
constexpr LogSpecificMode GetLogSpecificMode(const LogSpecificMode* p) { return *p; }
}
template<MaskMergeMode> constexpr int GetMaskMergeMode() { return 0; }
template<class A, class B> using IsSameType = std::is_same<A, B>;
template<class T> struct RegTensor { using ActualT = T; T value{}; };
struct MaskReg { bool value = true; };
union HalfUnion { uint16_t i; half f; };
union NotNumUnion { uint32_t i; float f; };
template<class T> void vcmps_le(MaskReg& d, RegTensor<T>& s, T t, MaskReg& m) {
 d.value = m.value && s.value <= t;
}
template<class T> void vcmps_lt(MaskReg& d, RegTensor<T>& s, T t, MaskReg& m) {
 d.value = m.value && s.value < t;
}
template<class T> void vmuls(RegTensor<T>& d, RegTensor<T>& s, T t, MaskReg& m, int) {
 d.value = m.value ? T(s.value * t) : T(0);
}
template<class T> void vadds(RegTensor<T>& d, RegTensor<T>& s, T t, MaskReg& m, int) {
 d.value = m.value ? T(s.value + t) : T(0);
}
template<class T> void vsel(RegTensor<T>& d, RegTensor<T>& a, RegTensor<T>& b, MaskReg& m) {
 d.value = m.value ? a.value : b.value;
}
template<class T> void vln(RegTensor<T>& d, RegTensor<T>& s, MaskReg& m, int) {
 double x = double(s.value);
 double minNormal = sizeof(T) == 2 ? std::ldexp(1., -14) : std::ldexp(1., -126);
 if (std::abs(x) < minNormal) x = std::copysign(0., x);
 d.value = m.value ? T(std::log(x)) : T(0);
}
"""
TEST = r"""
constexpr LogSpecificMode mode{MaskMergeMode::ZEROING, LogAlgo::PRECISION_1ULP_FTZ_FALSE};
template<class T, class Bits> int run(const Bits* cases, int count) {
 int failures = 0;
 for (int i = 0; i < count; ++i) {
  RegTensor<T> src, dst;
  std::memcpy(&src.value, &cases[i], sizeof(T));
  MaskReg mask;
  LnCompute<T, RegTensor<T>, &mode>(dst, src, mask);
  double expected = double(T(std::log(double(src.value))));
  double actual = double(dst.value);
  bool ok;
  if (std::isnan(expected)) ok = std::isnan(actual);
  else if (std::isinf(expected)) ok = expected == actual;
  else {
   double tolerance = sizeof(T) == 2 ? std::max(0.00001, std::abs(expected) / 1024) : 0.00002;
   ok = std::abs(actual - expected) <= tolerance;
  }
  if (!ok) { std::printf("FAIL dtype_bytes=%zu bits=%llx actual=%g expected=%g\n", sizeof(T),
    (unsigned long long)cases[i], actual, expected); ++failures; }
  mask.value = false;
  dst.value = T(7);
  LnCompute<T, RegTensor<T>, &mode>(dst, src, mask);
  if (double(dst.value) != 0.) ++failures;
 }
 return failures;
}
int main() {
 const uint16_t h[] = {0x0001,0x03fe,0x03ff,0x0400,0x0401,0,0x8000,0xbc00,0x7c00,0xfc00,0x7e00};
 const uint32_t f[] = {1,0x007ffffe,0x007fffff,0x00800000,0x00800001,0,0x80000000,
  0xbf800000,0x7f800000,0xff800000,0x7fc00000};
 int failures = run<half>(h,11) + run<float>(f,11);
 std::printf("Ln boundary/special/mask: 44 cases, failures=%d\n",failures);
 return failures ? 1 : 0;
}
"""


SQRT_PRELUDE = r"""
namespace Std = std;
enum class SqrtAlgo { INTRINSIC, PRECISION_1ULP_FTZ_TRUE, PRECISION_1ULP_FTZ_FALSE,
 PRECISION_0ULP_FTZ_FALSE, FAST_INVERSE };
struct SqrtSpecificMode { MaskMergeMode mrgMode; bool precisionMode; SqrtAlgo algo; };
namespace Internal {
constexpr SqrtSpecificMode GetSqrtSpecificMode(const SqrtSpecificMode* p) { return *p; }
}
template<class T, class... Ts> constexpr bool SupportType() { return (std::is_same_v<T, Ts> || ...); }
template<auto, auto...> constexpr bool SupportEnum() { return true; }
template<class T, auto mode, class U> void SqrtFastInverseImpl(U&, U&, MaskReg&) {}
template<class T> void vsqrt(RegTensor<T>& d, RegTensor<T>& s, MaskReg& m, int) {
 double x = double(s.value);
 double minNormal = sizeof(T) == 2 ? std::ldexp(1., -14) : std::ldexp(1., -126);
 // Model hardware input flushing to expose missed software compensation.
 if (std::abs(x) < minNormal) x = std::copysign(0., x);
 d.value = m.value ? T(std::sqrt(x)) : T(0);
}
"""
SQRT_TEST = r"""
constexpr SqrtSpecificMode sqrtFalse{MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_1ULP_FTZ_FALSE};
constexpr SqrtSpecificMode sqrtTrue{MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_1ULP_FTZ_TRUE};
constexpr SqrtSpecificMode sqrtDefault{MaskMergeMode::ZEROING, false, SqrtAlgo::INTRINSIC};
template<class T, class Bits, const SqrtSpecificMode* mode> int runSqrt(const Bits* cases, int count) {
 int failures = 0;
 constexpr bool compensate = mode->algo == SqrtAlgo::PRECISION_1ULP_FTZ_FALSE ||
#ifndef __ASC_FTZ__
 mode->algo == SqrtAlgo::INTRINSIC;
#else
 false;
#endif
 for (int i = 0; i < count; ++i) {
  RegTensor<T> src, dst;
  std::memcpy(&src.value, &cases[i], sizeof(T));
  const double input = double(src.value);
  double x = input;
  if (!compensate && std::abs(x) < (sizeof(T) == 2 ? std::ldexp(1., -14) : std::ldexp(1., -126)))
   x = std::copysign(0., x);
  const double expected = double(T(std::sqrt(x)));
  for (bool alias : {false, true}) for (bool active : {true, false}) {
   src.value = T(input); dst.value = T(7);
   MaskReg mask{active};
   auto& output = alias ? src : dst;
   SqrtImpl<DefaultType, mode>(output, src, mask);
   const double actual = double(output.value);
   const double want = active ? expected : 0.;
   const double tolerance = std::abs(want) * (sizeof(T) == 2 ? 0.001 : 0.0000002);
   const bool ok = std::isnan(want) ? std::isnan(actual) :
    (std::isinf(want) ? actual == want : std::abs(actual - want) <= tolerance);
   if (active && want == 0. && std::signbit(actual) != std::signbit(want)) {
    ++failures;
   }
   if (!ok) {
    std::printf("Sqrt FAIL bytes=%zu bits=%llx algo=%d alias=%d mask=%d actual=%g expected=%g\n",
     sizeof(T), (unsigned long long)cases[i], int(mode->algo), alias, active, actual, want);
    ++failures;
   }
  }
 }
 return failures;
}
int main() {
 const uint16_t h[] = {1,0x03fe,0x03ff,0x0400,0x0401,0,0x8000,0xbc00,0x7c00,0xfc00,0x7e00};
 const uint32_t f[] = {1,0x007ffffe,0x007fffff,0x00800000,0x00800001,0,0x80000000,
  0xbf800000,0x7f800000,0xff800000,0x7fc00000};
 int failures = 0;
 failures += runSqrt<half,uint16_t,&sqrtFalse>(h,11) + runSqrt<float,uint32_t,&sqrtFalse>(f,11);
 failures += runSqrt<half,uint16_t,&sqrtTrue>(h,11) + runSqrt<float,uint32_t,&sqrtTrue>(f,11);
 failures += runSqrt<half,uint16_t,&sqrtDefault>(h,11) + runSqrt<float,uint32_t,&sqrtDefault>(f,11);
 std::printf("Sqrt boundary/special/mask/alias: 264 cases, failures=%d\n",failures);
 return failures ? 1 : 0;
}
"""


LOG_PRELUDE = r"""
using Log2Algo = LogAlgo;
using Log10Algo = LogAlgo;
using Log2SpecificMode = LogSpecificMode;
using Log10SpecificMode = LogSpecificMode;
namespace Internal {
constexpr LogSpecificMode GetLog2SpecificMode(const LogSpecificMode* p) { return *p; }
constexpr LogSpecificMode GetLog10SpecificMode(const LogSpecificMode* p) { return *p; }
}
// Scalar model of LogXImpl's half -> float -> logarithm -> half pipeline.
// Vector lane packing is outside this numerical boundary regression.
template<auto mode> void LogXImpl(RegTensor<half>& d, RegTensor<half>& s, MaskReg& m, float reciprocal) {
 RegTensor<float> widened{float(s.value)}, result;
 vln(result, widened, m, 0);
 vmuls(result, result, reciprocal, m, 0);
 d.value = half(result.value);
}
"""


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--baseline", action="store_true")
    parser.add_argument("--arch", choices=["3510", "9201"], default="9201")
    parser.add_argument("--cxx", default="c++", help="Host compiler supporting _Float16 (e.g. bisheng)")
    args = parser.parse_args()
    header = Path(HEADER.format(arch=args.arch))
    print(f"Unary host model dav_{args.arch}", flush=True)
    if args.baseline:
        source = subprocess.check_output(["git", "show", f"2488a6f1d:{header}"], cwd=ROOT, text=True)
    else:
        source = (ROOT / header).read_text()
    start = source.index("template <typename T = DefaultType, typename U, const LogSpecificMode* mode>")
    end = source.index("template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>", start)
    with tempfile.TemporaryDirectory(prefix="9201_sync_unary_") as tmp:
        cpp = Path(tmp) / "ln.cpp"
        binary = Path(tmp) / "ln"
        cpp.write_text(PRELUDE + source[start:end] + TEST)
        subprocess.run([args.cxx, "-std=c++17", "-O0", str(cpp), "-lm", "-o", str(binary)], check=True)
        result = subprocess.run([str(binary)]).returncode
        sqrt_start = source.rfind("template <", 0, source.index("__simd_callee__ inline void SqrtImpl("))
        sqrt_end = source.index(
            "template <typename T = DefaultType, typename U, const LogSpecificMode* mode>", sqrt_start
        )
        cpp.write_text(
            PRELUDE + "\n#include <initializer_list>\n" + SQRT_PRELUDE + source[sqrt_start:sqrt_end] + SQRT_TEST
        )
        for ftz in [False, True]:
            flags = ["-D__ASC_FTZ__=1"] if ftz else []
            subprocess.run([args.cxx, "-std=c++17", "-O0", *flags, str(cpp), "-lm", "-o", str(binary)], check=True)
            print(f"Sqrt host model __ASC_FTZ__={int(ftz)}", flush=True)
            result |= subprocess.run([str(binary)]).returncode
        for name, next_name, reference in [("Log2", "Log10", "log2"), ("Log10", "NegB64", "log10")]:
            log_start = source.rfind("template <", 0, source.index(f"__simd_callee__ inline void {name}Impl("))
            log_end = source.rfind("template <", 0, source.index(f"__simd_callee__ inline void {next_name}Impl("))
            for ftz, algo in [
                (False, "PRECISION_1ULP_FTZ_FALSE"),
                (True, "PRECISION_1ULP_FTZ_FALSE"),
                (False, "INTRINSIC"),
            ]:
                test = TEST.replace("LnCompute<T, RegTensor<T>, &mode>(", f"{name}Impl<T, &mode>(")
                test = test.replace("std::log(double(src.value))", f"std::{reference}(double(src.value))")
                test = test.replace("LogAlgo::PRECISION_1ULP_FTZ_FALSE", f"LogAlgo::{algo}")
                test = test.replace("Ln boundary/special/mask: 44", f"{name} half/float boundary/special/mask: 44")
                cpp.write_text(PRELUDE + SQRT_PRELUDE + LOG_PRELUDE + source[log_start:log_end] + test)
                flags = ["-D__ASC_FTZ__=1"] if ftz else []
                subprocess.run([args.cxx, "-std=c++17", "-O0", *flags, str(cpp), "-lm", "-o", str(binary)], check=True)
                print(f"{name} host model __ASC_FTZ__={int(ftz)} algo={algo}", flush=True)
                result |= subprocess.run([str(binary)]).returncode
        return result


if __name__ == "__main__":
    raise SystemExit(main())
