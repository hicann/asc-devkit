# GetReduceRepeatMaxMinSpr(ISASI)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:34:51.363Z pushedAt=2026-10-28T03:14:39.565Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: not supported
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
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/reduction_compute_aux_config/GetReduceRepeatMaxMinSpr_ISASI_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_vec_reduce_intf.h"`.

This API obtains the maximum/minimum value and its index across all repeats when [ReduceRepeat<MAX/MIN>](../reduction_compute/ReduceRepeat.md) is called, or obtains the maximum/minimum value produced by [ReduceMax](../reduction_compute/ReduceMax.md)/[ReduceMin](../reduction_compute/ReduceMin.md). The computation result is stored as a global variable and can be obtained at any time.

## Function Prototype

<!-- npu="A3,910b" id8 -->

- Obtains the maximum/minimum value and its index across all repeats when [ReduceRepeat<MAX/MIN>](../reduction_compute/ReduceRepeat.md) is called, or obtains the maximum/minimum value produced by [ReduceMax](../reduction_compute/ReduceMax.md)/[ReduceMin](../reduction_compute/ReduceMin.md) (in this case, the obtained index is inaccurate). This function prototype is supported only on the following models:

    <!-- npu="A3" id9 -->
    - Atlas A3 products
    <!-- end id9 -->

    <!-- npu="910b" id10 -->
    - Atlas A2 products
    <!-- end id10 -->

    ```cpp
    template <typename T>
    __aicore__ inline void GetReduceRepeatMaxMinSpr(T &maxMinValue, T &maxMinIndex)
    ```
    
<!-- end id8 -->

<!-- npu="310p" id11 -->

- Obtains the maximum/minimum value across all repeats when [ReduceRepeat<MAX/MIN>](../reduction_compute/ReduceRepeat.md) is called, or obtains the maximum/minimum value produced by [ReduceMax](../reduction_compute/ReduceMax.md)/[ReduceMin](../reduction_compute/ReduceMin.md). This function prototype is supported only on the Atlas inference products AI Core.

    ```cpp
    template <typename T>
    __aicore__ inline void GetReduceRepeatMaxMinSpr(T &maxMinValue)
    ```

<!-- end id11 -->

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| maxMinValue | Output | Maximum/minimum value across all repeats when [ReduceRepeat<MAX/MIN>](../reduction_compute/ReduceRepeat.md) is called, or the maximum/minimum value produced by [ReduceMax](../reduction_compute/ReduceMax.md)/[ReduceMin](../reduction_compute/ReduceMin.md). |
| maxMinIndex | Output | Index of the maximum/minimum value across all repeats when [ReduceRepeat<MAX/MIN>](../reduction_compute/ReduceRepeat.md) is called. |

## Data Types

The supported operand data types are `half` and `float`.

## Return Value

None

## Constraints

- Only the continuous computation scenario of `ReduceRepeat<MAX/MIN>`, `ReduceMax`, and `ReduceMin` is supported. That is, when calling the preceding reduction APIs, the parameters must satisfy `srcRepStride=8` and `srcBlkStride=1`.
- Obtaining the result when the reduction API uses [Mask Counter mode](../SIMD_compute/mask.md#mask-mode) is not supported.
- When used in the [ReduceMax](../reduction_compute/ReduceMax.md)/[ReduceMin](../reduction_compute/ReduceMin.md) scenario, only the accurate maximum/minimum value can be obtained, and the corresponding accurate index cannot be obtained. If an accurate index is required, use the output of [ReduceMax](../reduction_compute/ReduceMax.md)/[ReduceMin](../reduction_compute/ReduceMin.md) itself.
- The index `maxMinIndex` is stored according to the operand data type. For example, when the operand uses the `half` type, `maxMinIndex` is stored as the `half` type. If it is read in the `half` format, the value of `maxMinIndex` is incorrect. Therefore, reading `maxMinIndex` requires using the `reinterpret_cast` method to convert it to an integer type. If the input data type is `half`, use `reinterpret_cast<uint16_t*>`. If the input is `float`, use `reinterpret_cast<uint32_t*>`.
- The operand data type T must be consistent with the destination operand data type of the reduction API used; otherwise, precision errors will occur.

## Example

For details, see [ReduceRepeat series reduction instruction examples](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_repeat).

1. After calling `ReduceRepeat<MAX>`, the maximum value and index within all repeats.

    <!-- npu="A3,910b" id12 -->

    - For the following models:
      <!-- npu="A3" id13 -->
      - Atlas A3 products
      <!-- end id13 -->
      <!-- npu="910b" id14 -->
      - Atlas A2 products
      <!-- end id14 -->

      ```cpp
      AscendC::LocalTensor<float> src;
      AscendC::LocalTensor<float> dst;
      int32_t mask = 64;
      float val = 0;   // Maximum value.
      float idx = 0;   // Maximum value index.
      AscendC::ReduceRepeat<AscendC::ReduceType::MAX>(dst, src, mask, 1, 1, 1, 8);
      AscendC::GetReduceRepeatMaxMinSpr<float>(val, idx); // Ensure the call order with ReduceRepeat and call them in pairs.
      ```

    <!-- end id12 -->

    <!-- npu="310p" id15 -->

    - For Atlas inference products AI Core:

        ```cpp
        AscendC::LocalTensor<float> src;
        AscendC::LocalTensor<float> dst;
        int32_t mask = 64;
        AscendC::ReduceRepeat<AscendC::ReduceType::MAX>(dst, src, mask, 1, 1, 1, 8);
        float val = 0;   // Maximum value.
        AscendC::GetReduceRepeatMaxMinSpr<float>(val); // Ensure the call order with ReduceRepeat and call them in pairs.
        ```

    <!-- end id15 -->

2. After calling `ReduceMax`, obtain the maximum value.

    <!-- npu="A3,910b" id16 -->

    - For the following models, `GetReduceRepeatMaxMinSpr` is used only to obtain the maximum value and cannot be used to obtain the accurate index corresponding to `ReduceMax`.
      <!-- npu="A3" id17 -->
      - Atlas A3 products
      <!-- end id17 -->
      <!-- npu="910b" id18 -->
      - Atlas A2 products
      <!-- end id18 -->

      ```cpp
      AscendC::LocalTensor<float> src;
      AscendC::LocalTensor<float> work;
      AscendC::LocalTensor<float> dst;
      int32_t mask = 64;
      AscendC::ReduceMax(dst, src, work, mask, 1, 8, true); // Continuous scenario, srcRepStride = 8, and calIndex = true.
      float val = 0;   // Maximum value.
      float idx = 0;   // Not used to obtain the accurate index corresponding to ReduceMax.
      AscendC::GetReduceRepeatMaxMinSpr<float>(val, idx); // Ensure the call order with ReduceMax and call them in pairs.
      ```

    <!-- end id16 -->

    <!-- npu="310p" id19 -->

    - For the AI Core version of Atlas inference products, you can call `GetReduceRepeatMaxMinSpr` directly after calling `ReduceMax` to obtain the maximum value.

        ```cpp
        AscendC::LocalTensor<float> src;
        AscendC::LocalTensor<float> work;
        AscendC::LocalTensor<float> dst;
        int32_t mask = 64;
        AscendC::ReduceMax(dst, src, work, mask, 1, 8, true);
        float val = 0;   // Maximum value.
        AscendC::GetReduceRepeatMaxMinSpr<float>(val); // Ensure the call order with ReduceMax and call them in pairs.
        ```

    <!-- end id19 -->
