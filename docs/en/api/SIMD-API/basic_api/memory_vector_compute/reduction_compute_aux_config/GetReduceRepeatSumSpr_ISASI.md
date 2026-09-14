# GetReduceRepeatSumSpr (ISASI)

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T10:01:45.911Z -->

## Applicable Products

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not supported
<!-- end id14 -->

## Description

Header file path: `"basic_api/kernel_operator_vec_reduce_intf.h"`.

Obtains the computation result of the [ReduceSum](../reduction_compute/ReduceSum.md) API. It is supported only by the API that continuously computes the first n data elements of a tensor. The computation result is stored in a global variable and can be obtained at any time.

## Prototype

```cpp
template <typename T>
__aicore__ inline T GetReduceRepeatSumSpr()
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. |

## Data Type

The supported data types of the operand are `half`/`float`.

## Return Value

Computation result of the `ReduceSum` API.

## Constraints

It is supported only when used together with the `ReduceSum` API that continuously computes the first n data elements of a tensor.

## Example

For details, see the [GetReduceRepeatSumSpr sample](https://gitcode.com/cann/asc-devkit/blob/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_computation).

```cpp
AscendC::LocalTensor<float> src;
AscendC::LocalTensor<float> work;
AscendC::LocalTensor<float> dst;
AscendC::ReduceSum(dst, src, work, 128);
float res = AscendC::GetReduceRepeatSumSpr<float>(); // Return the sum of the first 128 numbers.
```
