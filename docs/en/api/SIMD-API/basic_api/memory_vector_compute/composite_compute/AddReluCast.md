# AddReluCast<a name="ZH-CN_TOPIC_0000001834651001"></a>

<!-- md-trans-meta sourceCommit=f266635ab179c912cc9641061c2d27246be9170c translatedAt=2026-11-02T07:28:53.928Z pushedAt=2026-11-18T01:25:07.072Z nl=2 -->

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

The formula for ReLU is as follows:

$$
Relu(x) = max(0, x)
$$

## Prototype<a name="section620mcpsimp"></a>

- Tensor first n elements computation

  ```cpp
  template <typename T, typename U>
  __aicore__ inline void AddReluCast(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const uint32_t count)
  ```

- Tensor high-dimensional slicing computation
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
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table**  Parameters

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dst | Output | Destination operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| src0, src1 | Input | Source operands.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| count | Input | Number of elements involved in the computation.<br>**Note: The value range of this parameter depends on the data type of the operand. Different data types support different maximum numbers of elements. The maximum amount of data to be processed cannot exceed the Unified Buffer (UB) size limit.** |
| mask[]/mask | Input | mask controls the elements involved in the computation in each iteration.<br>Note: For data type conversion, the mask filters elements based on the larger sizeof(dtype) between the input and output types.<br>For details, see [Mask Operations](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations.<br>The vector computation unit reads 256 consecutive bytes of data for computation each time. To process all input data, multiple iterations (repeats) are required. repeatTime specifies the number of iterations.<br>For details about this parameter, see [High-Dimensional Splitting](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the address step of the operands. Type: [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which contains parameters such as the address step of the same datablock between adjacent iterations of the operands and the address step of different datablocks within the same iteration of the operands.<br>For details about the address step between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address step of DataBlock within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Types

<!-- npu="950" id8 -->

**Table**  Data type combination scenarios for Ascend 950PR&950DT products

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

**Table**  Data type combination scenarios for Atlas A3 products

| data types of src0 and src1 | dst data type |
| :----------------- | :---------- |
| half               | int8_t      |
| int16_t            | int8_t      |
| float              | half        |

<!-- end id9 -->

<!-- npu="910b" id10 -->

**Table**  Data type combination scenarios for Atlas A2 products

| data types of src0 and src1 | dst data type |
| :----------------- | :---------- |
| half               | int8_t      |
| int16_t            | int8_t      |
| float              | half        |

<!-- end id10 -->

<!-- npu="310b" id11 -->

**Table**  Data type combination scenarios for Atlas 200I/500 A2 inference products

| data types of src0 and src1 | dst data type |
| :----------------- | :---------- |
| half               | int8_t      |
| float              | half        |

<!-- end id11 -->

<!-- npu="310p" id12 -->

**Table**  Data type combination scenarios for Atlas inference products AI Core

| data types of src0 and src1 | dst data type |
| :----------------- | :---------- |
| half               | int8_t      |
| int16_t            | int8_t      |
| float              | half        |

<!-- end id12 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/composite_compute/AddReluCast_res.md#id2 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).
- This instruction involves precision conversion. For conversion rules, see [Precision Conversion Rules](../../data_structures/precision_conversion.md).
<!-- npu="A3,910b,950" id13 -->
- When the count or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id14 -->
  - For the following models, the API does not perform any calculation and does not write to the destination operand. The API is treated as a NOP (no operation).
    <!-- npu="A3" id15 -->
    - Atlas A3 products
    <!-- end id15 -->
    <!-- npu="910b" id16 -->
    - Atlas A2 products
    <!-- end id16 -->
  <!-- end id14 -->
  <!-- npu="950" id17 -->
  - For Ascend 950PR&950DT products, the API implements compatibility by calling the [Reg vector compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is set to 0, it is not guaranteed that the API is treated as a NOP (no operation).
  <!-- end id17 -->
<!-- end id13 -->
<!-- npu="950" id18 -->
- UB space usage. For Ascend 950PR&950DT products:
  - The tensor high-dimensional split computation API occupies 8 KB of temporary UB space.
  - The API for continuous computation of the first n data of a tensor does not occupy the 8 KB temporary UB space.
<!-- end id18 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/composite_compute/AddReluCast_res.md#id3 -->

## Example<a name="section837496171220"></a>

In this example, srcLocal is of the half type, dstLocal is of the int8\_t type, and the mask is computed based on half.

- Tensor high-dimensional splitting computation sample - mask continuous mode.

  ```cpp
  uint64_t mask = 256 / sizeof(half); // 128
  // repeatTime = 4. Each iteration computes 128 numbers, totaling 512 numbers.
  // dstBlkStride, src0BlkStride, src1BlkStride = 1. Data is read and written continuously within a single iteration.
  // dstRepStride = 4, src0RepStride, src1RepStride = 8. Data is read and written continuously between adjacent iterations.
  AscendC::AddReluCast(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 4, 8, 8 });
  ```

- Tensor high-dimensional splitting computation sample - mask bit-by-bit mode.

  ```cpp
  uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
  // repeatTime = 4. Each iteration computes 128 numbers, totaling 512 numbers.
  // dstBlkStride, src0BlkStride, src1BlkStride = 1. Data is read and written continuously within a single iteration.
  // dstRepStride = 4, src0RepStride, src1RepStride = 8. Data is read and written continuously between adjacent iterations.
  AscendC::AddReluCast(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 4, 8, 8 });
  ```

- Tensor first-n-data computation sample.

  ```cpp
  AscendC::AddReluCast(dstLocal, src0Local, src1Local, 512);
  ```

The result is as follows:

```plain
Input data src0Local: [1 1 3 ... 512]
Input data src1Local: [0 0.5 -4 ... -513]
Output data dstLocal: [1 2 0 ... 0]
```
