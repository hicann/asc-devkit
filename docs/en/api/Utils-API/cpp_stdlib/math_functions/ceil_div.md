# ceil\_div

<!-- md-trans-meta sourceCommit=c891f0c5d5b2ac25c5683ff40810fa381d654ece translatedAt=2026-10-23T01:18:03.670Z pushedAt=2026-10-23T02:43:19.178Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/math_functions/ceil_div_res.md#id1 -->

## Description

The header file path is `"utils/std/cmath.h"`.

Computes the ceiling of the quotient of two integers a and b. This API can be used on both the Host and AI Core sides.

## Prototype

```cpp
template <typename T, typename U>
__host__ __aicore__ inline constexpr auto ceil_div(const T& a, const U& b);
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of parameter a, which must be an integer. |
| U | Data type of parameter b, which must be an integer. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| a | Input | Dividend. |
| b | Input | Divisor. |

## Return Value

Returns the ceiling of the quotient of two integers. In general, the return value type is deduced by `decltype(T{} / U{})`.

## Constraints

- T and U must be integers. Supported data types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t.
- When b is 0, the result is 0.
- For signed input, the caller must ensure that a >= 0 and b > 0.
<!-- npu="950" id8 -->
- For the Ascend 950PR&950DT products, when used on the vector side, the input parameter of this API supports only the `uint32_t` type. In the VF scenario, the return value must be converted to the `uint16_t` type using `static_cast`, and the caller must ensure that the ceiling result is within the representable range of `uint16_t`. This constraint originates from the VF Hardware Loop coding specifications. For more specifications, see [Hardware Loop Coding Specifications](../../../../guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_loop_optimization.md#section11326136133217).
<!-- end id8 -->

## Examples

In this example, ceil\_div is used to calculate the number of iterations repeatTime. By performing ceiling division on the data volume count and the data volume processed per iteration, it ensures that all data, including the tail block, is fully processed.

```cpp
template <typename T>
__simd_vf__ inline void AddCustom(__local_mem__ T *dst, __local_mem__ T *src0, __local_mem__ T *src1,
    uint32_t count)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(T);
    uint16_t repeatTime = static_cast<uint16_t>(AscendC::Std::ceil_div(count, oneRepeatSize));
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0 + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1 + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dst + i * oneRepeatSize, dstReg, mask);
    }
}
```
