# AddReluCast<a name="ZH-CN_TOPIC_0000001834651001"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:06:08.921Z pushedAt=2026-10-28T03:14:39.453Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/composite_compute/AddReluCast_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_vconv_intf.h"`.

Performs element-wise summation, compares the result with 0 and takes the larger value, and performs precision conversion based on the data types of the source and destination operand Tensors. The calculation formula is as follows:

$$
dst_i = Cast(Relu(src0_i + src1_i))
$$

The calculation formula of Relu is as follows:

$$
Relu(x) = max(0, x)
$$

## Prototype<a name="section620mcpsimp"></a>

- Tensor first n elements computation

  ```cpp
  template <typename T, typename U>
  __aicore__ inline void AddReluCast(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const uint32_t count)
  ```

- Tensor high-dimensional split computation
  - **mask** bit-by-bit mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void AddReluCast(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

  - **mask** continuous mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void AddReluCast(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table**  Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Data type of the destination operand. |
| U | Data type of the source operand. |
| isSetMask | Whether to set the mask within the API.<br>&bull; true: The mask is set within the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table**  Parameters

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dst | Output | Destination operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| src0, src1 | Input | Source operands.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| count | Input | Number of elements involved in the computation.<br>**Note: The value range of this parameter depends on the data type of the operand. Different data types support different maximum numbers of elements, and the maximum amount of data to be processed cannot exceed the UB size limit.** |
| mask[]/mask | Input | mask controls the elements involved in the computation in each iteration.<br>Note: The mask for data type conversion filters elements based on the larger sizeof(dtype) between the input and output types.<br>For details about the settings, see [Mask Operations](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations.<br>The vector computation unit reads 256 bytes of consecutive data for computation each time. To process the input data, multiple iterations (repeats) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Split](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the address stride of the operand. Type: [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), including the address stride of the same data block between adjacent iterations of the operand, the address stride of different data blocks within the same iteration of the operand, and other parameters.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride parameters of data blocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Types

<!-- npu="950" id8 -->

**Table**  Data type combination cases for Ascend 950PR&950DT products

| data types of src0 and src1 | dst data type |
| :----------------- | :---------- |
| half               | int8_t      |
| int16_t            | int8_t      |
| float              | half        |
| int64_t            | float       |
| int64_t            | int32_t     |

For Ascend 950PR&950DT products, the data type combination where the data types of src0 and src1 are int64_t and the data type of dst is int32_t is supported only by the API for computing the first n data of a tensor.

<!-- end id8 -->

<!-- npu="A3" id9 -->

**Table**  Data type combination cases for Atlas A3 products

| data types of src0 and src1 | dst data type |
| :----------------- | :---------- |
| half               | int8_t      |
| int16_t            | int8_t      |
| float              | half        |

<!-- end id9 -->

<!-- npu="910b" id10 -->

**Table**  Data type combination cases for Atlas A2 products

| data types of src0 and src1 | dst data type |
| :----------------- | :---------- |
| half               | int8_t      |
| int16_t            | int8_t      |
| float              | half        |

<!-- end id10 -->

<!-- npu="310b" id11 -->

**Table**  Data type combination cases for Atlas 200I/500 A2 inference products

| data types of src0 and src1 | dst data type |
| :----------------- | :---------- |
| half               | int8_t      |
| float              | half        |

<!-- end id11 -->

<!-- npu="310p" id12 -->

**Table**  Atlas inference products AI Core data type combination cases

| data types of src0 and src1 | dst data type |
| :----------------- | :---------- |
| half               | int8_t      |
| int16_t            | int8_t      |
| float              | half        |

<!-- end id12 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).
- This instruction involves precision conversion. For conversion rules, see [Precision Conversion Rules](../../data_structures/precision_conversion.md).
<!-- npu="A3,910b,950" id13 -->
- When the count or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id14 -->
  - For the following models, this API does not perform computation or write to the destination operand, and is treated as a NOP (no operation).
    <!-- npu="A3" id15 -->
    - Atlas A3 products
    <!-- end id15 -->
    <!-- npu="910b" id16 -->
    - Atlas A2 products
    <!-- end id16 -->
  <!-- end id14 -->
  <!-- npu="950" id17 -->
  - For Ascend 950PR&950DT products, this API is implemented for compatibility by calling the [Reg vector compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is set to 0, it is not guaranteed that this API is treated as a NOP (no operation).
  <!-- end id17 -->
<!-- end id13 -->
<!-- npu="950" id18 -->
- Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - The tensor high-dimensional split compute API occupies 8 KB of UB temporary space.
  - The tensor first-n-data continuous compute API does not involve the occupation of 8 KB of UB temporary space.
<!-- end id18 -->

## Example<a name="section837496171220"></a>

In this example, srcLocal is of the half type, dstLocal is of the int8\_t type, and the mask is calculated based on half.

- Example of tensor high-dimensional split computation - mask continuous mode.

  ```cpp
  uint64_t mask = 256 / sizeof(half); // 128
  // repeatTime = 4, 128 data elements are computed per iteration, and 512 data elements are computed in total.
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration.
  // dstRepStride = 4, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations.
  AscendC::AddReluCast(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 4, 8, 8 });
  ```

- Example of tensor high-dimensional split computation - mask bit-by-bit mode.

  ```cpp
  uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
  // repeatTime = 4, 128 data elements are computed per iteration, and 512 data elements are computed in total.
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration.
  // dstRepStride = 4, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations.
  AscendC::AddReluCast(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 4, 8, 8 });
  ```

- Example of computing the first n data elements of a tensor.

  ```cpp
  AscendC::AddReluCast(dstLocal, src0Local, src1Local, 512);
  ```

The result is as follows:

```plain
Input data src0Local: [1 1 3 ... 512]
Input data src1Local: [0 0.5 -4 ... -513]
Output data dstLocal: [1 2 0 ... 0]
```
