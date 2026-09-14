# AddReluCast<a name="ZH-CN_TOPIC_0000001834651001"></a>

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T08:41:23.422Z -->

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

Header file path: `"basic_api/kernel_operator_vec_vconv_intf.h"`

Sums the elements, compares the result with 0 and takes the larger value, and performs precision conversion based on the data types of the source operand and destination operand tensors. The computation formula is as follows:

$$
dst_i = Cast(Relu(src0_i + src1_i))
$$

## Prototype<a name="section620mcpsimp"></a>

- Computation of the first n data elements of the tensor

  ```cpp
  template <typename T, typename U>
  __aicore__ inline void AddReluCast(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const uint32_t count)
  ```

- High-dimensional splitting computation of the tensor
  - Bitwise mask mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void AddReluCast(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

  - Continuous mask mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void AddReluCast(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table**  Template parameters

| Parameter | Description |
| :----- | :--- |
| T | Data type of the destination operand. |
| U | Data type of the source operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table**  Parameters

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor must be 32-byte aligned. |
| src0, src1 | Input | Source operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor must be 32-byte aligned. |
| count | Input | Number of elements involved in the computation.<br>**Note: The value range of this parameter is related to the data type of the operands. Different data types support different maximum numbers of elements that can be processed, and the maximum amount of data to be processed cannot exceed the UB size limit.** |
| mask[]/mask | Input | mask is used to control the elements involved in the computation in each iteration.<br>Note: The mask for data type conversion filters elements based on the larger sizeof(dtype) between the input and output types.<br>For details about the settings, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations.<br>The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeat) are required to finish reading and computing all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Slicing](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which includes parameters such as the address stride of the same datablock between adjacent iterations of the operands and the address stride of different datablocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

<!-- npu="950" id1 -->
**Table**  Data type combination cases of Ascend 950PR/Ascend 950DT

| data types of src0 and src1 | dst data type |
| :----------------- | :---------- |
| half               | int8_t      |
| int16_t            | int8_t      |
| float              | half        |
| int64_t            | float       |
| int64_t            | int32_t     |
<!-- end id1 -->

<!-- npu="A3" id2 -->
**Table**  Data type combination cases of Atlas A3 training products/Atlas A3 inference products

| data types of src0 and src1 | dst data type |
| :----------------- | :---------- |
| half               | int8_t      |
| int16_t            | int8_t      |
| float              | half        |
<!-- end id2 -->

<!-- npu="910b" id3 -->
**Table**  Data type combination cases of Atlas A2 training products/Atlas A2 inference products

| data types of src0 and src1 | dst data type |
| :----------------- | :---------- |
| half               | int8_t      |
| int16_t            | int8_t      |
| float              | half        |
<!-- end id3 -->

<!-- npu="310b" id4 -->
**Table**  Data type combination cases of Atlas 200I/500 A2 inference products

| data types of src0 and src1 | dst data type |
| :----------------- | :---------- |
| half               | int8_t      |
| float              | half        |
<!-- end id4 -->

<!-- npu="310p" id5 -->
**Table**  Data type combination cases of Atlas inference products AI Core

| data types of src0 and src1 | data type of dst |
| :--------------------------- | :--------------- |
| half                         | int8_t           |
| int16_t                      | int8_t           |
| float                        | half             |
<!-- end id5 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For address alignment constraints, see [Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- For operand address overlap constraints, see [Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).
- This instruction involves precision conversion. For conversion rules, see [Precision Conversion Rules](../../data_structures/precision_conversion.md).
<!-- npu="950" id6 -->
- For Ascend 950PR/Ascend 950DT, the int64_t data type supports only the API for computing the first n data elements of a tensor.
<!-- end id6 -->

## Example<a name="section837496171220"></a>

In this example, srcLocal is of the half type, dstLocal is of the int8\_t type, and the mask is calculated based on half.

- Example of tensor high-dimensional splitting computation - continuous mask mode

  ```cpp
  uint64_t mask = 256 / sizeof(half); // 128
  // repeatTime = 4, 128 numbers are computed in one iteration, and 512 numbers are computed in total
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration
  // dstRepStride = 4, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations
  AscendC::AddReluCast(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 4, 8, 8 });
  ```

- Example of tensor high-dimensional splitting computation - bitwise mask mode

  ```cpp
  uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
  // repeatTime = 4, 128 numbers are computed in one iteration, and 512 numbers are computed in total
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration
  // dstRepStride = 4, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations
  AscendC::AddReluCast(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 4, 8, 8 });
  ```

- Computation example for the first n data of the tensor

  ```cpp
  AscendC::AddReluCast(dstLocal, src0Local, src1Local, 512);
  ```

The result is as follows:

```plain
Input data src0Local: [1 1 3 ... 512]
Input data src1Local: [0 0.5 -4 ... -513]
Output data dstLocal: [1 2 0 ... 0]
```
