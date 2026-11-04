# CreateVecIndex<a name="ZH-CN_TOPIC_0000002573904793"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:58:37.392Z pushedAt=2026-10-28T03:14:39.512Z nl=2 -->

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
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_padding/CreateVecIndex_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_createvecindex_intf.h"`.

The CreateVecIndex API creates a vector index with a specified start value.

The API supports computation using the first n elements and high-dimensional splitting. The principle and reference pseudocode of the first n elements computation are as follows:

```python
import numpy as np

def CreateVecIndex(dst, firstValue, count):
    dst = np.arange(firstValue, firstValue + count, dtype=np.float16)
```

## Prototype<a name="section620mcpsimp"></a>

- Tensor first n elements computation

    ```cpp
    template <typename T>
    __aicore__ inline void CreateVecIndex(LocalTensor<T> dst, const T &firstValue, uint32_t count)
    ```

- Tensor high-dimensional split computation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T>
        __aicore__ inline void CreateVecIndex(LocalTensor<T> &dst, const T &firstValue, uint64_t mask[], uint8_t repeatTime, uint16_t dstBlkStride, uint8_t dstRepStride)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T>
        __aicore__ inline void CreateVecIndex(LocalTensor<T> &dst, const T &firstValue, uint64_t mask, uint8_t repeatTime, uint16_t dstBlkStride, uint8_t dstRepStride)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ------ | ------ |
| T | Data type of the operand. |

**Table 2** API parameter description

| Parameters | Input/Output | Meaning |
| ------ | ------ | ------ |
| dst | Output | Destination operand of the LocalTensor type. Supported TPosition values are VECIN/VECCALC/VECOUT. The start address of the LocalTensor must be 32-byte aligned. |
| firstValue | Input | First value of the index. The data type must be consistent with that of the elements in dst. |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_compute.md). |
| mask/mask[] | Input | mask controls the elements involved in the computation in each iteration.<br>For details, see [Mask Operations](../mask_operations/mask_operations.md). |
| repeatTime | Input | Number of instruction iterations. Each iteration collects data from eight DataBlocks. Value range: repeatTime∈[0,255]. |
| dstBlkStride | Input | Address stride between different DataBlocks of the vector destination operand within a single iteration, in DataBlocks. |
| dstRepStride | Input | Address stride of the same DataBlock of the vector destination operand between adjacent iterations, in DataBlocks. |

## Data Type

<!-- npu="950" id8 -->
- For Ascend 950PR&950DT products, the data types supported by T are int8_t, int16_t, half, int32_t, float, and int64_t. Among them, the int8_t/int64_t data types are supported only by the API for computing the first n data elements of a tensor.
<!-- end id8 -->
<!-- npu="A3" id9 -->
- For Atlas A3 products, the data types supported by T are int16_t, half, int32_t, and float.
<!-- end id9 -->
<!-- npu="910b" id10 -->
- For Atlas A2 products, the data types supported by T are int16_t, half, int32_t, and float.
<!-- end id10 -->
<!-- npu="310b" id11 -->
- For Atlas 200I/500 A2 inference products, the data types supported by T are int16_t, half, int32_t, and float.
<!-- end id11 -->
<!-- npu="310p" id12 -->
- For Atlas inference products AI Core, the data types supported by T are int16_t, half, int32_t, and float.
<!-- end id12 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- firstValue must not exceed the value range of the data type of the elements in dst.
<!-- npu="A3,910b,950" id13 -->
- When the value of count or repeatTime is 0, the API behaves as follows:
  <!-- npu="A3,910b" id14 -->
  - For the following models, the API does not perform computation or write to the destination operand, and is treated as a NOP (no operation).
    <!-- npu="A3" id15 -->
    - Atlas A3 products
    <!-- end id15 -->
    <!-- npu="910b" id16 -->
    - Atlas A2 products
    <!-- end id16 -->
  <!-- end id14 -->
  <!-- npu="950" id17 -->
  - For Ascend 950PR&950DT products, the API achieves compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the value of count or repeatTime is 0, the API is not guaranteed to be treated as a NOP (no operation).
  <!-- end id17 -->
<!-- end id13 -->

## Example<a name="section642mcpsimp"></a>

This example shows only the API-related code from the complete sample. To run the sample code, compile and execute the complete sample [CreateVecIndex sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/create_vec_index).

- Tensor high-dimensional splitting sample - mask continuous mode

    ```cpp
    // repeatTime = 1, mask = 128, 128 elements one repeat, 128 elements total
    // The data type of firstValue is int16_t, and the data type of dstLocal is int16_t.
    // dstBlkStride = 1, data is written continuously within a single iteration.
    // dstRepStride = 8, data is written continuously across adjacent iterations.
    AscendC::CreateVecIndex(dstLocal, (int16_t)0, mask, repeatTime, dstBlkStride, dstRepStride);
    ```

- Tensor high-dimensional splitting sample - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 1, 128 elements one repeat, 128 elements total
    // The data type of firstValue is int16_t, and the data type of dstLocal is int16_t.
    // dstBlkStride = 1, data is written continuously within a single iteration.
    // dstRepStride = 8, data is written continuously across adjacent iterations.
    AscendC::CreateVecIndex(dstLocal, (int16_t)0, mask, repeatTime, dstBlkStride, dstRepStride);
    ```

- Tensor first n data computation sample

    ```cpp
    uint32_t count = 128;    // Number of elements involved in the computation.
    AscendC::CreateVecIndex(dstLocal, (int16_t)0, count);
    ```

The result is as follows:

```plain
Input data (firstValue): 0
Output data (dstLocal): [0 1 2 ... 127]
```
