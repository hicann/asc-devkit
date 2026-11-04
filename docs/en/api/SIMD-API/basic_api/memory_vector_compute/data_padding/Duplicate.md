# Duplicate<a name="ZH-CN_TOPIC_0000002543264872"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:59:23.838Z pushedAt=2026-10-28T03:14:39.519Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

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
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_padding/Duplicate_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_duplicate_intf.h"`.

The Duplicate API copies a variable or an immediate value multiple times and fills it into a vector.

The API supports computation using the first n data elements and high-dimensional splitting. The principle and reference pseudocode for computing the first n data elements are as follows:

```python
import numpy as np

def Duplicate(scalarValue, dst, count):
    for i in range(count):     
        dst[i] = scalarValue
```
<!-- npu="950" id26 -->
For Ascend 950PR&950DT products, to facilitate your use, the API for computing the first n data elements of a tensor also supports directly passing a LocalTensor. In this case, the first element of the LocalTensor is copied multiple times and filled into the vector.
<!-- end id26 -->

## Prototype<a name="section620mcpsimp"></a>

- Compute the first n data elements of a tensor, with the source operand being a scalar

    ```cpp
    template <typename T>
    __aicore__ inline void Duplicate(const LocalTensor<T>& dst, const T& scalarValue, const int32_t& count)
    ```

<!-- npu="950" id10 -->

- Compute the first n data elements of a tensor, with the source operand being a LocalTensor

    ```cpp
    // This function prototype is supported only on Ascend 950PR&950DT products.
    template <typename T>
    __aicore__ inline void Duplicate(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
    ```
<!-- end id10 -->

- High-dimensional splitting of a tensor, with mask in bit-by-bit mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Duplicate(const LocalTensor<T>& dst, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime, const uint16_t dstBlockStride, const uint8_t dstRepeatStride)
    ```

- High-dimensional splitting of a tensor, with mask in continuous mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Duplicate(const LocalTensor<T>& dst, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const uint16_t dstBlockStride, const uint8_t dstRepeatStride)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameters | Description |
| ------ | ------ |
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameters is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** API parameter description

| Parameters | Input/Output | Meaning |
| ------ | ------ | ------ |
| dst | Output | Destination operand, of type LocalTensor, with supported TPosition values VECIN/VECCALC/VECOUT. The start address of the LocalTensor must be 32-byte aligned.|
| scalarValue | Input | Source operand to be copied. Its data type must be the same as that of the elements in dst.|
|<!-- npu="950" id11 --> src | Input| Source operand, of type LocalTensor, with supported TPosition values VECIN/VECCALC/VECOUT.<br>Its data type must be the same as that of the elements in dst.<br>When this parameter is passed, src[0] is copied multiple times and filled into the vector.<!-- end id11 -->|
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Compute](../SIMD_compute/continuous_compute.md).|
| mask/mask[] | Input | mask controls the elements involved in the computation in each iteration.<br>For details about the settings, see [Mask Operations](../mask_operations/mask_operations.md). |
| repeatTime | Input | Number of instruction iterations. Each iteration completes data collection of eight DataBlocks. Value range: repeatTime∈[0,255].|
| dstBlockStride | Input | Address stride between different DataBlocks of the vector destination operand within a single iteration, in DataBlocks. |
| dstRepeatStride | Input | Address stride of the same DataBlock of the vector destination operand between adjacent iterations, in DataBlocks. |

## Data Type

<!-- npu="950" id12 -->
- For Ascend 950PR&950DT products, the data types supported by T are: bool, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, and complex64. Among them, the data types bool, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, fp8_e8m0_t, complex32, int64_t, uint64_t, and complex64 are supported only by the API for computing the first n data elements of a tensor.
<!-- end id12 -->
<!-- npu="A3" id13 -->
- For Atlas A3 products, the data types supported by T are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.
<!-- end id13 -->
<!-- npu="910b" id14 -->
- For Atlas A2 products, the data types supported by T are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.
<!-- end id14 -->
<!-- npu="310b" id15 -->
- For Atlas 200I/500 A2 inference products, the data types supported by T are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.
<!-- end id15 -->
<!-- npu="310p" id16 -->
- For Atlas inference products (AI Core), the data types supported by T are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.
<!-- end id16 -->
<!-- npu="910" id17 -->
- For Atlas training products, the data types supported by T are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.
<!-- end id17 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
<!-- npu="A3,910b,950" id20 -->
- When the count or repeatTime parameter is 0, this API behaves as follows:
  <!-- npu="A3,910b" id21 -->
  - For the following models, this API does not perform any computation and does not write to the destination operand. This API is treated as a NOP (no operation).
    <!-- npu="A3" id22 -->
    - Atlas A3 products
    <!-- end id22 -->
    <!-- npu="910b" id23 -->
    - Atlas A2 products
    <!-- end id23 -->
  <!-- end id21 -->
  <!-- npu="950" id24 -->
  - For Ascend 950PR&950DT products, this API is implemented for compatibility by calling the [Reg vector compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is 0, it is not guaranteed that this API is treated as a NOP (no operation).
  <!-- end id24 -->
<!-- end id20 -->

## Example<a name="section642mcpsimp"></a>

This example shows only the API-related code from the complete sample. To run the sample code, refer to the complete [Duplicate sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/duplicate) for compilation and execution.

- Tensor high-dimensional splitting computation sample, mask continuous mode

    ```cpp
    uint64_t mask = 128;
    half scalar = 18.0;
    // repeatTime = 2, 128 elements one repeat, 256 elements total
    // dstBlkStride = 1, no gap between blocks in one repeat
    // dstRepStride = 8, no gap between repeats
    AscendC::Duplicate(dstLocal, scalar, mask, 2, 1, 8 );
    ```

- Tensor high-dimensional splitting computation sample, mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    half scalar = 18.0;
    // repeatTime = 2, 128 elements one repeat, 256 elements total
    // dstBlkStride = 1, no gap between blocks in one repeat
    // dstRepStride = 8, no gap between repeats
    AscendC::Duplicate(dstLocal, scalar, mask, 2, 1, 8 );
    ```

- Sample for computing the first n data elements of a tensor, with a scalar source operand

    ```cpp
    half inputVal(18.0);
    int32_t srcDataSize = 256; // Number of elements involved in the computation.
    AscendC::Duplicate<half>(dstLocal, inputVal, srcDataSize);
    ```

<!-- npu="950" id25 -->

- Sample for computing the first n data elements of a tensor, with a Tensor source operand
    ```cpp
    AscendC::Duplicate<half>(dstLocal, srcLocal, srcDataSize);
    ```
<!-- end id25 -->
The result is as follows:

```plain
scalar: 18.0
srcLocal: [18.0 1.0 2.0 ... 254.0 255.0]
dstLocal: [18.0 18.0 18.0 ... 18.0 18.0]
```
