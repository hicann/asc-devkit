# MulCast<a name="ZH-CN_TOPIC_0000001786581974"></a>

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T09:07:19.393Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id7 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id7 -->
<!-- npu="A3" id8 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id8 -->
<!-- npu="910b" id9 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id9 -->
<!-- npu="310b" id10 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id10 -->
<!-- npu="310p" id11 -->
- Atlas inference products AI Core: Supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products Vector Core: Not supported
<!-- end id12 -->
<!-- npu="910" id13 -->
- Atlas training products: Not supported
<!-- end id13 -->


## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_mulcast_intf.h"`

Computes the element-wise product and performs precision conversion based on the data types of the source operands and the destination operand Tensor. The calculation formula is as follows:

$$
dst_i = conv(src0_i * src1_i)
$$

## Prototype<a name="section620mcpsimp"></a>

- Computation on the first n data of the tensor

  ```cpp
  template <typename T, typename U>
  __aicore__ inline void MulCast(const LocalTensor<T> &dst, const LocalTensor<U> &src0, const LocalTensor<U> &src1, uint32_t count)
  ```

- Computation on the high-dimensional splitting of the tensor
  - Bitwise mask mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void MulCast(const LocalTensor<T> &dst, const LocalTensor<U> &src0, const LocalTensor<U> &src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams &repeatParams)
    ```

  - Continuous mask mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void MulCast(const LocalTensor<T> &dst, const LocalTensor<U> &src0, const LocalTensor<U> &src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams &repeatParams)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table** Template parameters

| Parameter | Description |
| :----- | :--- |
| T | Data type of the destination operand. |
| U | Data type of the source operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table** Parameters

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor must be 32-byte aligned. |
| src0, src1 | Input | Source operands.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor must be 32-byte aligned. |
| count | Input | Number of elements that participate in the computation.<br>**Note: The value range of this parameter is related to the data type of the operands. Different data types support different maximum numbers of elements to be processed, and the maximum amount of data to be processed cannot exceed the UB size limit.** |
| mask[]/mask | Input | **mask** controls the elements that participate in the computation in each iteration.<br>For details, see [Mask Operation](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations.<br>The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeats) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Split](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which includes parameters such as the address stride of the same datablock between adjacent iterations of the operands and the address stride of different datablocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

<!-- npu="950" id1 -->
**Table**  Data type combinations of Ascend 950PR/Ascend 950DT

| src0 and src1 | dst |
| :----------------- | :---------- |
| half               | int8_t      |
| half               | uint8_t     |
| int64_t            | float       |
| int64_t            | int32_t     |
<!-- end id1 -->

<!-- npu="A3" id2 -->
**Table**  Data type combinations of Atlas A3 training products/Atlas A3 inference products

| src0 and src1 | dst |
| :----------------- | :---------- |
| half               | int8_t      |
| half               | uint8_t     |
<!-- end id2 -->

<!-- npu="910b" id3 -->
**Table**  Data type combinations of Atlas A2 training products/Atlas A2 inference products 

| src0 and src1 | dst |
| :----------------- | :---------- |
| half               | int8_t      |
| half               | uint8_t     |
<!-- end id3 -->

<!-- npu="310b" id4 -->
**Table**  Data type combinations of Atlas 200I/500 A2 inference products

| src0 and src1 | dst |
| :----------------- | :---------- |
| half               | int8_t      |
| half               | uint8_t     |
<!-- end id4 -->

<!-- npu="310p" id5 -->
**Table**  Data type combinations of Atlas inference products AI Core

| src0 and src1 | dst |
| :--------------------------- | :------------ |
| half                         | int8_t        |
| half                         | uint8_t       |
<!-- end id5 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For address alignment constraints, see [Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- For operand address overlap constraints, see [Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).
- When using the tensor high-dimensional split computation API, the number of source operand elements processed in one iteration must be consistent with that of the destination operand. A single computation can input at most 128 half-type data elements, so the destination operand can output at most 128 b8-type data elements. Therefore, each iteration outputs to the first 4 datablocks, srcRepStride must be set to 8, and dstRepStride must be set to 4.
- This instruction involves precision conversion. For conversion rules, see [Precision Conversion Rules](../../data_structures/precision_conversion.md).
<!-- npu="950" id6 -->
- For Ascend 950PR/Ascend 950DT, int64\_t data type supports only the API for computing the first n data elements of a tensor.
<!-- end id6 -->

## Example<a name="section642mcpsimp"></a>

- Example of tensor high-dimensional slicing computation - continuous mask mode

  ```cpp
  uint64_t mask = 128;
  // repeatTime = 4, 128 data elements are computed in one iteration, and 512 data elements are computed in total
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration
  // dstRepStride = 4, data is written continuously between adjacent iterations
  // src0RepStride, src1RepStride = 8, data is read continuously between adjacent iterations.
  AscendC::MulCast(dstLocal, src0, src1Local, mask, repeatTime, repeatParams);
  ```

- Tensor high-dimensional slicing computation example - bitwise mask mode

  ```cpp
  uint64_t mask[2] = { UINT64_MAX, UINT64_MAX }; 
  // repeatTime = 4, 128 numbers are computed in one iteration, and 512 numbers are computed in total.
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration.
  // dstRepStride = 4, data is written continuously between adjacent iterations.
  // src0RepStride, src1RepStride = 8, data is read continuously between adjacent iterations.
  AscendC::MulCast(dstLocal, src0, src1Local, mask, repeatTime, repeatParams);
  ```

- Example of computing the first n data elements of a tensor

  ```cpp
  AscendC::MulCast(dstLocal, src0, src1Local, 512);
  ```

The result is as follows:

```plain
Input data src0: [1 -2 3 ... -6]
Input data src1Local: [1 3 -4 ... 5]
Output data dstLocal: [1 -6 -12 ... -30]
```
