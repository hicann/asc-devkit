# GetReduceRepeatSumSpr(ISASI)

<!-- md-trans-meta sourceCommit=f266635ab179c912cc9641061c2d27246be9170c translatedAt=2026-11-05T01:46:40.017Z pushedAt=2026-11-18T01:25:30.740Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/reduction_compute_aux_config/GetReduceRepeatSumSpr_ISASI_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_vec_reduce_intf.h"`.

Obtains the computation result of the [ReduceSum](../reduction_compute/ReduceSum.md) API. This API is available only for the API that continuously computes the first n data elements of a tensor. The computation result is stored as a global variable and can be retrieved at any time.

## Function Prototype

```cpp
template <typename T>
__aicore__ inline T GetReduceRepeatSumSpr()
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. |

## Data Types

The supported operand data types are `half` and `float`.
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/reduction_compute_aux_config/GetReduceRepeatSumSpr_ISASI_res.md#id2 -->

## Return Value

The computation result of the `ReduceSum` API.

## Constraints

Only supported for use with the `ReduceSum` API that continuously computes the first n data of a tensor.
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/reduction_compute_aux_config/GetReduceRepeatSumSpr_ISASI_res.md#id3 -->

## Example

For details, see [GetReduceRepeatSumSpr example](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_computation).

```cpp
AscendC::LocalTensor<float> src;
AscendC::LocalTensor<float> work;
AscendC::LocalTensor<float> dst;
AscendC::ReduceSum(dst, src, work, 128);
float res = AscendC::GetReduceRepeatSumSpr<float>(); // Returns the sum of the first 128 numbers.
```
