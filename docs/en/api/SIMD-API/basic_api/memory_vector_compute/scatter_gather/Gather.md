# Gather<a name="ZH-CN_TOPIC_0000002541491859"></a>

<!-- md-trans-meta sourceCommit=b51f731d97b8137e47d31515a918da577b1142d8 translatedAt=2026-11-05T01:51:31.474Z pushedAt=2026-11-18T01:25:30.745Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/scatter_gather/Gather_res.md#id1 -->

## Description<a name="section17600329101418"></a>

The header file path is `"basic_api/kernel_operator_vec_gather_intf.h"`.

The Gather API accepts an input tensor (src), an address offset tensor (srcOffset), and a base address (srcBaseOffset). It determines the index of the input tensor based on the base address and the address offset, and collects the corresponding elements from the input tensor into the result tensor (dst).

The API supports computation using the first n data elements and high-dimensional split computation. The principle of the first n data elements computation and the reference pseudocode are as follows:

```python
import numpy as np

def Gather(dst, src, srcOffset, srcBaseOffset, count):
    inputType = np.dtype("uint16")
    for i in range(count):
        dst[i] = src[(srcBaseOffset + srcOffset[i]) // inputType.itemsize]
```

For a complete example, see [Gather truth value computation](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather).

## Prototype<a name="section15660625202219"></a>

- Computes the first n elements of the tensor.

  ```cpp
  template <typename T> 
  __aicore__ inline void Gather(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& srcOffset, const uint32_t srcBaseOffset, const uint32_t count) 
  ```

- Tensor high-dimensional splitting computation.
  - **mask** bit-by-bit mode.

    ```cpp
    template <typename T> 
    __aicore__ inline void Gather(const LocalTensor<T>& dst, const LocalTensor<T>& src,const LocalTensor<uint32_t>& srcOffset, const uint32_t srcBaseOffset, const uint64_t mask[], const uint8_t repeatTime, const uint16_t dstRepStride) 
    ```

  - **mask** continuous mode.

    ```cpp
    template <typename T> 
    __aicore__ inline void Gather(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& srcOffset, const uint32_t srcBaseOffset, const uint64_t mask, const uint8_t repeatTime, const uint16_t dstRepStride) 
    ```

## Parameters<a name="section1619484392111"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Operand data type. |

**Table 2**  Parameters

| Parameter | Input/Output | Meaning |
| :------- | :-------- | :--- |
| dst | Output | Destination operand, of type [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned. |
| src | Input | Source operand. Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned.<br><br>The data type must be the same as that of dst. |
| srcOffset | Input | Address offset of each element in src. Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned.<br><br>The offset is relative to the start base address srcBaseOffset of src, in bytes. For value requirements, see [Constraints](#constraints). |
| srcBaseOffset | Input | Specifies the start position of the source operand in the Gather operation, in bytes. The value must ensure bit-width alignment of the src element type; otherwise, unexpected behavior may occur. |
| count | Input | Number of elements involved in the computation.<br><br>The value range depends on the data type of the operand. Different data types support different maximum numbers of elements, and the maximum amount of data to be processed cannot exceed the Unified Buffer (UB) size limit. |
| mask/mask[] | Input | mask controls the elements involved in the computation in each iteration.<br><br>For details, see [mask operations](../mask_operations/mask_operations.md). |
| repeatTime | Input | Number of instruction iterations. The DataBlock processed in each iteration may differ depending on the model. For details, see [Constraints](#constraints).|
| dstRepStride | Input | Address stride between adjacent iterations, in DataBlock (32 bytes). |

## Data Type

<!-- npu="950" id8 -->
Ascend 950PR&950DT products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, and uint64_t. The data types int8\_t and uint8\_t support only the API for computing the first n data of a tensor.
<!-- end id8 -->

<!-- npu="A3" id9 -->
Atlas A3 products support the following data types: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.
<!-- end id9 -->

<!-- npu="910b" id10 -->
Atlas A2 products support the following data types: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.
<!-- end id10 -->

<!-- npu="310b" id11 -->
Atlas 200I/500 A2 inference products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float.
<!-- end id11 -->

<!-- npu="310p" id12 -->
For the Atlas inference products AI Core, the supported data types are: int16_t, uint16_t, half, int32_t, uint32_t, and float.
<!-- end id12 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/scatter_gather/Gather_res.md#id2 -->

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [Unified Buffer Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- The source operand and the destination operand cannot use the same memory address.
- The value requirements for srcOffset are as follows:
  - The value must ensure bit-width alignment of the src element type.
  - The address after the offset must not exceed the range of the UB-sized data.
  <!-- npu="950,310b" id13 -->
  - For the following models, the value range of the address offset is as follows: when the operand is 8-bit, the value range is [0, 2^16-1]; when the operand is 16-bit, the value range is [0, 2^17-1]; when the operand is 32-bit or 64-bit, it must not exceed the range of uint32_t.
    <!-- npu="950" id14 -->
    - Ascend 950PR&950DT products
    <!-- end id14 -->
    <!-- npu="310b" id15 -->
    - Atlas 200I/500 A2 inference products
    <!-- end id15 -->
  <!-- end id13 -->

  <!-- npu="A3,910b,310p" id16 -->
  - For the following models, the value range of the address offset cannot exceed the range of uint32_t.
    <!-- npu="A3" id17 -->
    - Atlas A3 products
    <!-- end id17 -->
    <!-- npu="910b" id18 -->
    - Atlas A2 products
    <!-- end id18 -->
    <!-- npu="310p" id19 -->
    - Atlas inference products AI Core
    <!-- end id19 -->
  <!-- end id16 -->
- The value of srcBaseOffset must ensure bit-width alignment of the src element type. Otherwise, unexpected behavior occurs: the program does not crash or report an error, but the data becomes misaligned, causing the result tensor to be inconsistent with expectations.
- Value range of repeatTime: repeatTime∈[0,255].

<!-- npu="950,A3,910b" id20 -->
- When the count or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id21 -->
  - For the following models, when the count or repeatTime parameter is set to 0, no computation is performed and no data is written to the destination operand. This API is treated as a NOP (no operation).
    <!-- npu="A3" id22 -->
    - Atlas A3 products
    <!-- end id22 -->
    <!-- npu="910b" id23 -->
    - Atlas A2 products
    <!-- end id23 -->
  <!-- end id21 -->

  <!-- npu="950" id24 -->
  - For Ascend 950PR&950DT products, this API is implemented for compatibility by calling the [Reg vector computation](../../reg_vector_compute/reg_vector_compute.md) API through VF. When the count or repeatTime parameter is set to 0, it is not guaranteed that this API is treated as a NOP (no operation).
  <!-- end id24 -->
<!-- end id20 -->

<!-- npu="950,310b" id25 -->
- For the following models, when the operand is **8-bit**, each iteration completes data collection of **4 DataBlocks**; in other cases, each iteration completes data collection of 8 DataBlocks.
  <!-- npu="950" id26 -->
  - Ascend 950PR&950DT products
  <!-- end id26 -->
  <!-- npu="310b" id27 -->
  - Atlas 200I/500 A2 inference products
  <!-- end id27 -->
<!-- end id25 -->

<!-- npu="A3,910b,310p" id28 -->
- For the following models, each iteration completes data collection of 8 DataBlocks.
  <!-- npu="A3" id29 -->
  - Atlas A3 products
  <!-- end id29 -->
  <!-- npu="910b" id30 -->
  - Atlas A2 products
  <!-- end id30 -->
  <!-- npu="310p" id31 -->
  - Atlas inference products AI Core
  <!-- end id31 -->
<!-- end id28 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/scatter_gather/Gather_res.md#id3 -->


## Examples<a name="section11276201527"></a>

This sample shows only key code. To run the sample code, copy this code segment and replace the corresponding part of the Compute function in scenario 3 of the [Gather class example](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather).

- Tensor high-dimensional splitting computation sample - mask continuous mode.

  ```cpp
  // repeatTime = 4, mask = 128, 128 elements one repeat, 512 elements total
  // The srcLocal data type is half, the srcOffsetLocal data type is uint32_t, and the dstLocal data type is half.
  // srcBaseOffset = 0, the starting base address of srcLocal is 0.
  // dstRepStride = 8, no gap between repeats 
  AscendC::Gather(dstLocal, srcLocal, srcOffsetLocal, (uint32_t)0, 128, 4, 8);
  ```

- Tensor high-dimensional splitting computation sample - mask bit-by-bit mode.

  ```cpp
  uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
  // repeatTime = 4, 128 elements one repeat, 512 elements total
  // The srcLocal data type is half, the srcOffsetLocal data type is uint32_t, and the dstLocal data type is half.
  // srcBaseOffset = 0, the starting base address of srcLocal is 0.
  // dstRepStride = 8, no gap between repeats 
  AscendC::Gather(dstLocal, srcLocal, srcOffsetLocal, (uint32_t)0, mask, 4, 8);
  ```

- Tensor first-n-data computation sample.

  ```cpp
  uint32_t count = 512;    // Number of elements involved in the computation.
  // The srcLocal data type is half, the srcOffsetLocal data type is uint32_t, and the dstLocal data type is half.
  // srcBaseOffset = 0, the starting base address of srcLocal is 0.
  AscendC::Gather(dstLocal, srcLocal, srcOffsetLocal, (uint32_t)0, count);
  ```

The result is as follows:

```plain
Input data srcOffsetLocal:
[254 252 250 ... 4 2 0]
Input data srcLocal (128 half-type data): 
[0 1 2 ... 125 126 127]
Initial value of the output data (dstLocal):
[0. 0. 0. 0. 0. 0. ... 0.]
After the Gather computation, the output data (dstLocal):
[127 126 125 ... 2 1 0]
```
