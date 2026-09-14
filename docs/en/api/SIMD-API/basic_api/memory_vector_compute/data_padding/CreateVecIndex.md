# CreateVecIndex<a name="ZH-CN_TOPIC_0000002573904793"></a>

<!-- md-trans-meta sourceCommit=ab2c72ba23cbc7229f8b141e516d96812390984a translatedAt=2026-09-09T14:28:50.763Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id9 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id9 -->
<!-- npu="A3" id10 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id10 -->
<!-- npu="910b" id11 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id11 -->
<!-- npu="310b" id12 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products AI Core: Supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products Vector Core: Not supported
<!-- end id14 -->
<!-- npu="910" id15 -->
- Atlas training products: Not supported
<!-- end id15 -->


## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_createvecindex_intf.h"`

The CreateVecIndex API creates a vector index with a specified starting value.

The API supports computation using the first n data elements and high-dimensional splitting. The principle and reference pseudocode for computing the first n data elements are as follows:

```python
import numpy as np

def CreateVecIndex(dst, firstValue, count):
    dst = np.arange(firstValue, firstValue + count, dtype=np.float16)
```

## Prototype<a name="section620mcpsimp"></a>

- Computation of the first n data elements of a tensor

    ```cpp
    template <typename T> 
    __aicore__ inline void CreateVecIndex(LocalTensor<T> dst, const T &firstValue, uint32_t count) 
    ```

- High-dimensional splitting computation of a tensor
    - Bitwise mask mode

        ```cpp
        template <typename T> 
        __aicore__ inline void CreateVecIndex(LocalTensor<T> &dst, const T &firstValue, uint64_t mask[], uint8_t repeatTime, uint16_t dstBlkStride, uint8_t dstRepStride)  
        ```

    - Continuous mask mode

        ```cpp
        template <typename T> 
        __aicore__ inline void CreateVecIndex(LocalTensor<T> &dst, const T &firstValue, uint64_t mask, uint8_t repeatTime, uint16_t dstBlkStride, uint8_t dstRepStride) 
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| ------ | ------ |
| T | Data type of the operand. |

**Table 2** API parameters
| Parameters | Input/Output | Meaning |
| ------ | ------ | ------ |
| dst | Output | Destination operand, of type LocalTensor. The supported TPosition values are VECIN/VECCALC/VECOUT (stored in the Unified Buffer). The start address of the LocalTensor must be 32-byte aligned.|
| firstValue | Input | First value of the index. Its data type must be consistent with that of the elements in dst.|
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_computation.md).|
| mask/mask[] | Input | mask controls the elements involved in the computation in each iteration.<br>For details about the settings, see [Mask Operation](../mask_operations/mask_operation.md). |
| repeatTime | Input | Number of instruction iterations. Each iteration collects data from eight DataBlocks. Value range: repeatTime∈[0,255].|
| dstBlkStride | Input | Address stride between different DataBlocks of the vector destination operand within a single iteration, in DataBlocks. |
| dstRepStride | Input | Address stride of the same DataBlock of the vector destination operand between adjacent iterations, in DataBlocks. |

## Data Type

<!-- npu="950" id1 -->
Ascend 950PR/Ascend 950DT: supported data types are int8_t, int16_t, half, int32_t, float, and int64_t.
<!-- end id1 -->

<!-- npu="A3" id2 -->
Atlas A3 training products/Atlas A3 inference products support the following data types: int16_t, half, int32_t, float.
<!-- end id2 -->

<!-- npu="910b" id3 -->
Atlas A2 training products/Atlas A2 inference products support the following data types: int16_t, half, int32_t, float.
<!-- end id3 -->

<!-- npu="310b" id4 -->
Atlas 200I/500 A2 inference products support the following data types: int16_t, half, int32_t, float.
<!-- end id4 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- firstValue must not exceed the value range corresponding to the data type of the elements in dst.
<!-- npu="A3,910b" id8 -->
- repeatTime = 0 indicates that no computation is performed and no data is written to the destination operand. The API is treated as a NOP (no operation). This constraint applies to the following models.
  <!-- npu="A3" id5 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id5 -->
  <!-- npu="910b" id6 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id6 -->
<!-- end id8 -->

<!-- npu="950" id7 -->
- For Ascend 950PR/Ascend 950DT, the int8_t/int64_t data types are supported only by the API for computing the first n data of a tensor.
<!-- end id7 -->

## Example<a name="section642mcpsimp"></a>

This sample shows only some key code. To run the sample code, copy this code segment and replace the relevant code segment of the Compute function in the [CreateVecIndex sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/create_vec_index).

- Tensor high-dimensional splitting computation sample - mask continuous mode

    ```cpp
    // repeatTime = 1, mask = 128, 128 elements one repeat, 128 elements total
    // The data type of firstValue is int16_t, and the data type of dstLocal is int16_t.
    // dstBlkStride = 1, data is written continuously within a single iteration.
    // dstRepStride = 8, data is written continuously across adjacent iterations.
    AscendC::CreateVecIndex(dstLocal, (int16_t)0, mask, repeatTime, dstBlkStride, dstRepStride);
    ```

- Tensor high-dimensional splitting calculation example - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 1, 128 elements one repeat, 128 elements total
    // firstValue data type is int16_t, dstLocal data type is int16_t
    // dstBlkStride = 1, data is written continuously within a single iteration
    // dstRepStride = 8, data is written continuously across adjacent iterations
    AscendC::CreateVecIndex(dstLocal, (int16_t)0, mask, repeatTime, dstBlkStride, dstRepStride);
    ```

- Tensor first-n-data calculation example

    ```cpp
    uint32_t count = 128;    // Number of elements involved in the calculation
    AscendC::CreateVecIndex(dstLocal, (int16_t)0, count);
    ```

The result is as follows:

```plain
Input data (firstValue): 0
Output data (dstLocal): [0 1 2 ... 127]
```
