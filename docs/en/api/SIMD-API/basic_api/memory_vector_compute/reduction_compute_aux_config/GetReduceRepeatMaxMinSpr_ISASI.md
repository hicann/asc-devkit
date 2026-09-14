# GetReduceRepeatMaxMinSpr (ISASI)

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T09:58:33.697Z -->

## Applicable Products

<!-- npu="950" id20 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id20 -->
<!-- npu="A3" id21 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id21 -->
<!-- npu="910b" id22 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id22 -->
<!-- npu="310b" id23 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id23 -->
<!-- npu="310p" id24 -->
- Atlas inference products AI Core: Supported
<!-- end id24 -->
<!-- npu="310p" id25 -->
- Atlas inference products Vector Core: Not supported
<!-- end id25 -->
<!-- npu="910" id26 -->
- Atlas training products: Not supported
<!-- end id26 -->

## Description

Header file path: `"basic_api/kernel_operator_vec_reduce_intf.h"`.

This API is used to obtain the maximum/minimum values and their indexes in all repeats when [ReduceRepeat<MAX/MIN>](../reduction_compute/ReduceRepeat.md) is called, or to obtain the maximum/minimum values obtained by calling [ReduceMax](../reduction_compute/ReduceMax.md)/[ReduceMin](../reduction_compute/ReduceMin.md). The computation results are stored as global variables and can be obtained at any time.

## Prototype

<!-- npu="A3,910b" id10 -->
- Obtains the maximum/minimum values and their indexes in all repeats when [ReduceRepeat<MAX/MIN>](../reduction_compute/ReduceRepeat.md) is called, or obtains the maximum/minimum values obtained by calling [ReduceMax](../reduction_compute/ReduceMax.md)/[ReduceMin](../reduction_compute/ReduceMin.md) (in this case, the obtained indexes are inaccurate). This function prototype supports only the following models:

    <!-- npu="A3" id1 -->
    - Atlas A3 training products/Atlas A3 inference products
    <!-- end id1 -->

    <!-- npu="910b" id2 -->
    - Atlas A2 training products/Atlas A2 inference products
    <!-- end id2 -->

    ```cpp
    template <typename T>
    __aicore__ inline void GetReduceRepeatMaxMinSpr(T &maxMinValue, T &maxMinIndex)
    ```
<!-- end id10 -->

<!-- npu="310p" id3 -->
- Obtains the maximum/minimum values in all repeats when [ReduceRepeat<MAX/MIN>](../reduction_compute/ReduceRepeat.md) is called, or obtains the maximum/minimum values obtained by calling [ReduceMax](../reduction_compute/ReduceMax.md)/[ReduceMin](../reduction_compute/ReduceMin.md). This function prototype supports only the Atlas inference product AI Core.

    ```cpp
    template <typename T>
    __aicore__ inline void GetReduceRepeatMaxMinSpr(T &maxMinValue)
    ```
<!-- end id3 -->

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| maxMinValue | Output | Maximum/minimum values in all repeats when [ReduceRepeat<MAX/MIN>](../reduction_compute/ReduceRepeat.md) is called, or the maximum/minimum values obtained by calling [ReduceMax](../reduction_compute/ReduceMax.md)/[ReduceMin](../reduction_compute/ReduceMin.md). |
| maxMinIndex | Output | Indexes of the maximum/minimum values in all repeats when [ReduceRepeat<MAX/MIN>](../reduction_compute/ReduceRepeat.md) is called. |

## Data Type

The supported data types of the operand are: `half`/`float`.

## Return Value

None

## Constraints

- Only the continuous computation scenarios of `ReduceRepeat<MAX/MIN>`, `ReduceMax`, and `ReduceMin` are supported, that is, when calling the preceding reduction APIs, the parameters must satisfy `srcRepStride=8` and `srcBlkStride=1`.
- The result of the reduction API in [Mask Counter mode](../SIMD_compute/mask.md#mask-mode) cannot be obtained.
- When used in [ReduceMax](../reduction_compute/ReduceMax.md)/[ReduceMin](../reduction_compute/ReduceMin.md) scenarios, only the accurate maximum/minimum value can be obtained, and the corresponding accurate index cannot be obtained. If an accurate index is required, use the output of [ReduceMax](../reduction_compute/ReduceMax.md)/[ReduceMin](../reduction_compute/ReduceMin.md) itself as the reference.
- The index `maxMinIndex` is stored according to the operand data type. For example, when the operand uses the `half` type, `maxMinIndex` is stored in the `half` type. If it is read in the `half` format, the value of `maxMinIndex` is incorrect. Therefore, to read `maxMinIndex`, use the `reinterpret_cast` method to convert it to an integer type. If the input data type is `half`, use `reinterpret_cast<uint16_t*>`. If the input is `float`, use `reinterpret_cast<uint32_t*>`.
- The operand data type T must be consistent with the destination operand data type of the reduction API used. Otherwise, precision errors will occur.

## Example

For details, see [ReduceRepeat series reduction instruction examples](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_repeat).

1. After `ReduceRepeat<MAX>` is called, the maximum values and indices within all repeats are returned.

    <!-- npu="A3,910b" id11 -->
    - For the following models:
      <!-- npu="A3" id4 -->
      - Atlas A3 training products/Atlas A3 inference products
      <!-- end id4 -->
      <!-- npu="910b" id5 -->
      - Atlas A2 training products/Atlas A2 inference products
      <!-- end id5 -->

      ```cpp
      AscendC::LocalTensor<float> src;
      AscendC::LocalTensor<float> dst;
      int32_t mask = 64;
      float val = 0;   // Maximum value
      float idx = 0;   // Index of the maximum value
      AscendC::ReduceRepeat<AscendC::ReduceType::MAX>(dst, src, mask, 1, 1, 1, 8);
      AscendC::GetReduceRepeatMaxMinSpr<float>(val, idx); // Ensure the call order matches ReduceRepeat, and call them in pairs
      ```
    <!-- end id11 -->

    <!-- npu="310p" id6 -->
    - For Atlas inference products AI Core:

        ```cpp
        AscendC::LocalTensor<float> src;
        AscendC::LocalTensor<float> dst;
        int32_t mask = 64;
        AscendC::ReduceRepeat<AscendC::ReduceType::MAX>(dst, src, mask, 1, 1, 1, 8);
        float val = 0;   // Maximum value
        AscendC::GetReduceRepeatMaxMinSpr<float>(val); // Ensure the call order with ReduceRepeat, and call them in pairs
        ```
    <!-- end id6 -->

2. After calling `ReduceMax`, obtain the maximum value.

    <!-- npu="A3,910b" id12 -->
    - For the following models, `GetReduceRepeatMaxMinSpr` is used only to obtain the maximum value, and cannot be used to obtain the accurate index corresponding to `ReduceMax`.
      <!-- npu="A3" id7 -->
      - Atlas A3 training products/Atlas A3 inference products
      <!-- end id7 -->
      <!-- npu="910b" id8 -->
      - Atlas A2 training products/Atlas A2 inference products
      <!-- end id8 -->

      ```cpp
      AscendC::LocalTensor<float> src;
      AscendC::LocalTensor<float> work;
      AscendC::LocalTensor<float> dst;
      int32_t mask = 64;
      AscendC::ReduceMax(dst, src, work, mask, 1, 8, true); // Continuous scenario, srcRepStride = 8, and calIndex = true
      float val = 0;   // Maximum value
      float idx = 0;   // Not used to obtain the accurate index corresponding to ReduceMax
      AscendC::GetReduceRepeatMaxMinSpr<float>(val, idx); // Ensure the call order with ReduceMax, and call them in pairs
      ```
    <!-- end id12 -->

    <!-- npu="310p" id9 -->
    - For the Atlas inference products AI Core version, you can call the `GetReduceRepeatMaxMinSpr` instruction directly after calling `ReduceMax` to obtain the maximum value.

        ```cpp
        AscendC::LocalTensor<float> src;
        AscendC::LocalTensor<float> work;
        AscendC::LocalTensor<float> dst;
        int32_t mask = 64;
        AscendC::ReduceMax(dst, src, work, mask, 1, 8, true);
        float val = 0;   // maximum value
        AscendC::GetReduceRepeatMaxMinSpr<float>(val); // Ensure the call order with ReduceMax, and call them in pairs.
        ```
    <!-- end id9 -->
