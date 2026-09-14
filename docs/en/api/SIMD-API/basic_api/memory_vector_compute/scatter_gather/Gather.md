# Gather<a name="ZH-CN_TOPIC_0000002541491859"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T12:11:11.653Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id21 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id21 -->
<!-- npu="A3" id22 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id22 -->
<!-- npu="910b" id23 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id23 -->
<!-- npu="310b" id24 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id24 -->
<!-- npu="310p" id25 -->
- Atlas inference products AI Core: Supported
<!-- end id25 -->
<!-- npu="310p" id26 -->
- Atlas inference products Vector Core: Not supported
<!-- end id26 -->
<!-- npu="910" id27 -->
- Atlas training products: Not supported
<!-- end id27 -->


## Description<a name="section17600329101418"></a>

Header file path: `"basic_api/kernel_operator_vec_gather_intf.h"`.

The Gather API accepts an input tensor (src), an address offset tensor (srcOffset), and a base address (srcBaseAddr). It determines the index of the input tensor based on the base address and the address offset, and gathers the corresponding elements from the input tensor into the result tensor (dst).

The API supports computation using the first n data elements and computation using high-dimensional partitioning. The principle and reference pseudocode for computation using the first n data elements are as follows:

```python
import numpy as np

def Gather(src, dst, count, srcOffset, srcBaseAddr):
    inputType = np.dtype("uint16")
    for i in range(count):     
        dst[i] = src[(srcBaseAddr + srcOffset[i]) // inputType.itemsize]
```

For a complete example, see [Gather truth value computation](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather).

## Prototype<a name="section15660625202219"></a>

- Computes the first n data elements of the tensor.

  ```cpp
  template <typename T> 
  __aicore__ inline void Gather(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& srcOffset, const uint32_t srcBaseAddr, const uint32_t count) 
  ```

- Computes high-dimensional slicing of the tensor.
  - Bitwise mask mode.

    ```cpp
    template <typename T> 
    __aicore__ inline void Gather(const LocalTensor<T>& dst, const LocalTensor<T>& src,const LocalTensor<uint32_t>& srcOffset, const uint32_t srcBaseAddr, const uint64_t mask[], const uint8_t repeatTime, const uint16_t dstRepStride) 
    ```

  - Continuous mask mode.

    ```cpp
    template <typename T> 
    __aicore__ inline void Gather(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& srcOffset, const uint32_t srcBaseAddr, const uint64_t mask, const uint8_t repeatTime, const uint16_t dstRepStride) 
    ```

## Parameters<a name="section1619484392111"></a>

**Table 1** Template parameters

| Parameter | Description |
| :----- | :--- |
| T | Data type of the operand. |

**Table 2** Parameters

| Parameter | Input/Output | Meaning |
| :------- | :-------- | :--- |
| dst | Output | Destination operand, of the type [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). The supported TPosition values are VECIN/VECCALC/VECOUT (storage location: Unified Buffer).<br><br>The start address of the LocalTensor must be 32-byte aligned. |
| src | Input | Source operand, of the type LocalTensor. The supported TPosition values are VECIN/VECCALC/VECOUT (storage location: Unified Buffer).<br><br>The start address of the LocalTensor must be 32-byte aligned.<br><br>The data type must be the same as that of dst. |
| srcOffset | Input | Address offset of each element in src, of the type LocalTensor. The supported TPosition values are VECIN/VECCALC/VECOUT (storage location: Unified Buffer).<br><br>The start address of the LocalTensor must be 32-byte aligned.<br><br>This offset is relative to the start base address of src. The unit is byte. For the value requirements, see [Constraints](#constraints). |
| srcBaseAddr | Input | Specifies the start position of the source operand in the Gather operation. The unit is byte. The value must ensure bit-width alignment of the src element type; otherwise, unexpected behavior may occur. |
| count | Input | Number of elements involved in the computation.<br><br>The value range of this parameter depends on the data type of the operand. Different data types support different maximum numbers of elements. The maximum amount of data to be processed cannot exceed the UB size limit.<br><br>**Note: count = 0 means that no computation is performed and no data is written to the destination operand. This API is treated as a NOP (no operation).** |
| mask/mask[] | Input | mask controls the elements involved in the computation within each iteration.<br><br>For details about the settings, see [Mask](../mask_operations/mask_operation.md). |
| repeatTime | Input | Number of instruction iterations. For different model numbers, the DataBlock processed in each iteration may differ. For details, see [Constraints](#constraints).<br><br>**Note: repeatTime = 0 means that no computation is performed and no data is written to the destination operand. This API is treated as a NOP (no operation).** |
| dstRepStride | Input | Address stride between adjacent iterations, in the unit of DataBlock (32 bytes). |

## Data Type

<!-- npu="950" id1 -->
Ascend 950PR/Ascend 950DT supports the following data types: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.
<!-- end id1 -->

<!-- npu="A3" id2 -->
Atlas A3 training products/Atlas A3 inference products support the following data types: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id2 -->

<!-- npu="910b" id3 -->
Atlas A2 training products/Atlas A2 inference products support the following data types: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id3 -->

<!-- npu="310b" id4 -->
Atlas 200I/500 A2 inference products support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float.
<!-- end id4 -->

<!-- npu="310p" id5 -->
Atlas inference products AI Core supports the following data types: int16_t, uint16_t, half, int32_t, uint32_t, float.
<!-- end id5 -->

## Constraints<a name="section633mcpsimp"></a>

- For the operand address alignment requirements, see [Unified Buffer address alignment constraints](../../../general_description_and_constraints.md#section796754519912).
- The source operand and destination operand cannot use the same memory address.
- The value requirements for srcOffset are as follows:
  - The value must ensure bit-width alignment of the src element type.
  - The address after the offset must not exceed the range of the UB data size.
<!-- npu="950,310b" id17 -->
- For the following model numbers, the value range of the address offset is as follows: when the operand is 8-bit, the value range is [0, 2^16-1]; when the operand is 16-bit, the value range is [0, 2^17-1]; when the operand is 32-bit or 64-bit, the value must not exceed the range of uint32_t.
    <!-- npu="950" id6 -->
    - Ascend 950PR/Ascend 950DT
    <!-- end id6 -->
    <!-- npu="310b" id7 -->
    - Atlas 200I/500 A2 inference products
    <!-- end id7 -->
<!-- end id17 -->
<!-- npu="A3,910b,310p" id18 -->
- For the following model numbers, the value range of the address offset must not exceed the range of uint32_t.
    <!-- npu="A3" id8 -->
    - Atlas A3 training products/Atlas A3 inference products
    <!-- end id8 -->
    <!-- npu="910b" id9 -->
    - Atlas A2 training products/Atlas A2 inference products
    <!-- end id9 -->
    <!-- npu="310p" id10 -->
    - Atlas inference products AI Core
    <!-- end id10 -->
<!-- end id18 -->
- Value range of repeatTime: repeatTime∈[0,255].
<!-- npu="950,310b" id19 -->
- For the following model numbers, when the operand is **8-bit**, each iteration completes data collection of **4 DataBlocks**; in other cases, each iteration completes data collection of 8 DataBlocks.
  <!-- npu="950" id11 -->
  - Ascend 950PR/Ascend 950DT
  <!-- end id11 -->
  <!-- npu="310b" id12 -->
  - Atlas 200I/500 A2 inference products
  <!-- end id12 -->
<!-- end id19 -->
<!-- npu="A3,910b,310p" id20 -->
- For the following model numbers, each iteration completes data collection of 8 DataBlocks.
  <!-- npu="A3" id13 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id13 -->
  <!-- npu="910b" id14 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id14 -->
  <!-- npu="310p" id15 -->
  - Atlas inference products AI Core
  <!-- end id15 -->
<!-- end id20 -->
<!-- npu="950" id16 -->
- For Ascend 950PR/Ascend 950DT, the int8\_t and uint8\_t data types support only the API for computing the first n data of a tensor.
<!-- end id16 -->

## Example<a name="section11276201527"></a>

This sample shows only some key code. To run the sample code, copy this code segment and replace the corresponding part of the Compute function in scenario 3 of the [Gather class sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather).

- Tensor high-dimensional slicing computation sample - continuous mask mode.

  ```cpp
  // repeatTime = 4, mask = 128, 128 elements one repeat, 512 elements total
  // The data type of srcLocal is half, the data type of srcOffsetLocal is uint32_t, and the data type of dstLocal is half
  // srcBaseAddr = 0, the start base address of srcLocal is 0
  // dstRepStride = 8, no gap between repeats 
  AscendC::Gather(dstLocal, srcLocal, srcOffsetLocal, (uint32_t)0, 128, 4, 8);
  ```

- Tensor high-dimensional slicing computation sample - bitwise mask mode.

  ```cpp
  uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
  // repeatTime = 4, 128 elements one repeat, 512 elements total
  // The data type of srcLocal is half, the data type of srcOffsetLocal is uint32_t, and the data type of dstLocal is half
  // srcBaseAddr = 0, the start base address of srcLocal is 0
  // dstRepStride = 8, no gap between repeats 
  AscendC::Gather(dstLocal, srcLocal, srcOffsetLocal, (uint32_t)0, mask, 4, 8);
  ```

- Sample of computing the first n data of a tensor.

  ```cpp
  uint32_t count = 512;    // Number of elements involved in the computation
  // The data type of srcLocal is half, that of srcOffsetLocal is uint32_t, and that of dstLocal is half
  // srcBaseAddr = 0, the start base address of srcLocal is 0
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
