# Duplicate<a name="ZH-CN_TOPIC_0000002543264872"></a>

<!-- md-trans-meta sourceCommit=ab2c72ba23cbc7229f8b141e516d96812390984a translatedAt=2026-09-09T14:32:29.105Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id14 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id14 -->
<!-- npu="A3" id15 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id15 -->
<!-- npu="910b" id16 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id16 -->
<!-- npu="310b" id17 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id17 -->
<!-- npu="310p" id18 -->
- Atlas inference products AI Core: Supported
<!-- end id18 -->
<!-- npu="310p" id19 -->
- Atlas inference products Vector Core: Not supported
<!-- end id19 -->
<!-- npu="910" id20 -->
- Atlas training products: Supported
<!-- end id20 -->


## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_duplicate_intf.h"`

The Duplicate API copies a variable or an immediate value multiple times and fills the vector with the copies.

The API supports computation using the first n data and computation using high-dimensional splitting. The principle of computation using the first n data and the reference pseudocode are as follows:

```python
import numpy as np

def Duplicate(scalarValue, dst, count):
    for i in range(count):     
        dst[i] = scalarValue
```

## Prototype<a name="section620mcpsimp"></a>

- Computation using the first n data of a tensor, with the source operand being a scalar

    ```cpp
    template <typename T>
    __aicore__ inline void Duplicate(const LocalTensor<T>& dst, const T& scalarValue, const int32_t& count)
    ```

<!-- npu="950" id1 -->
- Computation using the first n data of a tensor, with the source operand being a Tensor

    ```cpp
    // This function prototype is supported only on Ascend 950PR/Ascend 950DT.
    template <typename T>
    __aicore__ inline void Duplicate(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
    ```
<!-- end id1 -->

- High-dimensional splitting computation of a tensor, with mask in bit-by-bit mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Duplicate(const LocalTensor<T>& dst, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime, const uint16_t dstBlockStride, const uint8_t dstRepeatStride)
    ```

- Tensor high-dimensional splitting computation, mask continuous mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Duplicate(const LocalTensor<T>& dst, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const uint16_t dstBlockStride, const uint8_t dstRepeatStride)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameters | Description |
| ------ | ------ |
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** API parameters
| Parameters | Input/Output | Meaning |
| ------ | ------ | ------ |
| dst | Output | Destination operand, of type LocalTensor. The supported TPosition values are VECIN/VECCALC/VECOUT (the storage location is Unified Buffer). The start address of LocalTensor must be 32-byte aligned.|
| scalarValue | Input | Source operand to be copied. Its data type must be the same as that of the elements in dst.|
|src | Input| Source operand, of type LocalTensor. The supported TPosition values are VECIN/VECCALC/VECOUT.<br>Its data type must be the same as that of the elements in dst.<br>When this parameter is passed, src[0] is copied multiple times and filled into the vector.|
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_computation.md).|
| mask/mask[] | Input | mask is used to control the elements involved in the computation in each iteration.<br>For details about the settings, see [Mask Operation](../mask_operations/mask_operation.md). |
| repeatTime | Input | Number of instruction iterations. Each iteration completes data collection of eight DataBlocks. Value range: repeatTime∈[0,255].|
| dstBlockStride | Input | Address stride between different DataBlocks of the vector destination operand within a single iteration, in DataBlock. |
| dstRepeatStride | Input | Address stride of the same DataBlock of the vector destination operand between adjacent iterations, in DataBlock. |

## Data Type

<!-- npu="950" id2 -->
Ascend 950PR/Ascend 950DT supports the following data types: bool, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, complex64.
<!-- end id2 -->

<!-- npu="A3" id3 -->
Atlas A3 training products/Atlas A3 Inference products support the following data types: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id3 -->

<!-- npu="910b" id4 -->
Atlas A2 training products/Atlas A2 inference products support the following data types: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id4 -->

<!-- npu="310b" id5 -->
Atlas 200I/500 A2 Inference Product supports the following data types: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id5 -->

<!-- npu="310p" id6 -->
Atlas inference products AI Core supports the following data types: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id6 -->

<!-- npu="910" id7 -->
Atlas training products support the following data types: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id7 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
<!-- npu="A3,910b" id12 -->
- repeatTime = 0 indicates that no computation is performed, no data is written to the destination operand, and the API is treated as a NOP (no operation). This constraint applies to the following models.
  <!-- npu="A3" id8 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id8 -->
  <!-- npu="910b" id9 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id9 -->
<!-- end id12 -->

<!-- npu="950" id10 -->
- For Ascend 950PR/Ascend 950DT, the bool, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, complex32, int64_t, uint64_t, and complex64 data types support only the API for computing the first n data elements of a tensor.
<!-- end id10 -->

## Example<a name="section642mcpsimp"></a>

This example shows only part of the code in the Compute process. To run it, copy and paste the code segment to the corresponding location of the Compute function in the [Duplicate sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/duplicate).

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

- Sample for computing the first n data elements of a tensor, with the source operand being a scalar

    ```cpp
    half inputVal(18.0);
    int32_t srcDataSize = 256; // Number of elements involved in the computation
    AscendC::Duplicate<half>(dstLocal, inputVal, srcDataSize);
    ```

<!-- npu="950" id11 -->
- Sample for computing the first n data elements of a tensor, with the source operand being a Tensor
    ```cpp
    AscendC::Duplicate<half>(dstLocal, srcLocal, srcDataSize);	 
    ```
<!-- end id11 -->
The result is as follows:

```plain
scalar: 18.0
srcLocal: [18.0 1.0 2.0 ... 254.0 255.0]
dstLocal: [18.0 18.0 18.0 ... 18.0 18.0]
```
