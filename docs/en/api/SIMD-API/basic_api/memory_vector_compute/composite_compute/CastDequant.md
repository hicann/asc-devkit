# CastDequant<a name="ZH-CN_TOPIC_0000001788011400"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-09-09T08:49:16.044Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id5 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id5 -->
<!-- npu="A3" id6 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id6 -->
<!-- npu="910b" id7 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id7 -->
<!-- npu="310b" id8 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id8 -->
<!-- npu="310p" id9 -->
- Atlas inference products AI Core: Supported
<!-- end id9 -->
<!-- npu="310p" id10 -->
- Atlas inference products Vector Core: Not supported
<!-- end id10 -->
<!-- npu="910" id11 -->
- Atlas training products: Not supported
<!-- end id11 -->


## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_vconv_intf.h"`.

Performs quantization and precision conversion on the input, and applies saturation processing to the overflow of the output result. This API must be used together with the [SetDeqScale](../type_conversion_aux_config/SetDeqScale.md) API.

- The input data type is int16_t, and the vector quantization mode is disabled.

  Uses the scale, offset, and signMode parameters set by the `SetDeqScale` API to perform Per-Tensor quantization on the entire input tensor, and outputs the b8 data type. The calculation formula is as follows:

  $$
  dst_i = Cast((src_i * scale) + offset)
  $$

- The input data type is int16_t, and the vector quantization mode is enabled.

  Sets 16 groups of quantization parameters from the `SetDeqScale` API, each group containing a scale, an offset, and a signMode, to perform Per-Block quantization on the input tensor. One repeat processes 8 DataBlocks, each of which is 32B in size. Quantization and precision conversion are performed on the 16 input data elements in each DataBlock, and each number is calculated using an independent group of quantization parameters. The output is the b8 data type. The calculation formula is as follows:

  $$
  dst_i = Cast((src_i * scale_j) + offset_j), j \in [0,15]
  $$

- The input type is int32_t, and the output is the half data type.

  Uses the scale parameter set by the `SetDeqScale` API and adopts the RINT precision rounding mode. The calculation formula is as follows:

  $$
  dst_i = Cast(src_i * scale)
  $$

## Prototype<a name="section620mcpsimp"></a>

- Computation of the first n data elements of the tensor

  ```cpp
  template <typename T, typename U, bool isVecDeq = true, bool halfBlock = true>
  __aicore__ inline void CastDequant(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint32_t count)
  ```

- High-dimensional slicing computation of the tensor
  - Bitwise mask mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true, bool isVecDeq = true, bool halfBlock = true>
    __aicore__ inline void CastDequant(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[], uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
    ```

  - Continuous mask mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true, bool isVecDeq = true, bool halfBlock = true>
    __aicore__ inline void CastDequant(const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table** Template parameters

| Parameter | Description |
| :----- | :--- |
|T|Data type of the destination operand.<br>When the type is b8, it is used together with the signMode input parameter of the `SetDeqScale` API. When signMode=true, the output data type is int8_t; when signMode=false, the output data type is uint8_t.|
|U|Data type of the source operand.<br> |
|isSetMask|Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning.|
|isVecDeq|Controls whether to select the vector quantization mode.<br>It is used together with the `SetDeqScale` API. When a Tensor is passed to the `SetDeqScale` API, isVecDeq must be true.|
|halfBlock|Valid when the output data type is b8. It indicates whether the output elements are stored in the upper or lower half of the DataBlock.<br>When halfBlock=true, the result is stored in the lower half of the DataBlock; when halfBlock=false, the result is stored in the upper half of the DataBlock.<br>As shown in the following figure, an input of one DataBlock size produces eight b8-type outputs. When halfBlock is false, these eight output data are stored in the upper half of the DataBlock.<br>![](../../../../figures/halfBlock_description.png)<br>To obtain continuous destination operand data, you can divide the DataBlock into odd and even blocks and call the API twice to process the odd and even blocks separately. The output of the odd blocks is stored in the lower half of the DataBlock, and the output of the even blocks is stored in the upper half of the DataBlock. |

**Table** Parameters

|Parameter|Input/Output|Description|
|---|---|---|
|dst|Output|Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor must be 32-byte aligned.|
|src|Input|Source operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor must be 32-byte aligned.|
|count|Input|Number of elements involved in the calculation.<br>The value range of this parameter is related to the data type of the operand. Different data types support different maximum numbers of elements to be processed, and the maximum amount of data to be processed cannot exceed the UB size limit.|
|mask/mask[]|Input|mask controls the elements involved in the calculation in each iteration.<br>Note: The mask for data type conversion filters elements based on the larger sizeof(dtype) between the input and output types.<br>For details about the settings, see [Mask](../mask_operations/mask_operation.md).|
|repeatTime|Input|Number of repeated iterations.<br>The vector calculation unit reads 256 consecutive bytes of data for calculation each time. To complete the processing of the input data, multiple iterations (repeat) are required to read and calculate all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Slicing](../SIMD_compute/high_dimension_slicing.md).|
|repeatParams|Input|Parameters that control the address stride of the operands. The type is [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md).|

## Data Type

<!-- npu="950" id1 -->
**Table** Supported data type combinations on Ascend 950PR/Ascend 950DT

| src data type | dst data type |
| :---------- | :---------- |
| int32_t     | half        |
| int16_t     | int8_t      |
| int16_t     | uint8_t     |
<!-- end id1 -->

<!-- npu="A3" id2 -->
**Table**  Data type combinations supported by Atlas A3 training products/Atlas A3 inference products

| src data type | dst data type |
| :---------- | :---------- |
| int32_t     | half        |
| int16_t     | int8_t      |
| int16_t     | uint8_t     |
<!-- end id2 -->

<!-- npu="910b" id3 -->
**Table**  Data type combinations supported by Atlas A2 training products/Atlas A2 inference products

| src data type | dst data type |
| :---------- | :---------- |
| int32_t     | half        |
| int16_t     | int8_t      |
| int16_t     | uint8_t     |
<!-- end id3 -->

<!-- npu="310b" id4 -->
**Table**  Data type combinations supported by Atlas inference products AI Core

| src data type | dst data type |
| :---------- | :---------- |
| int16_t     | int8_t      |
| int16_t     | uint8_t     |
<!-- end id4 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For address alignment constraints, see [Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- For operand address overlap constraints, see [Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).
- When the output type is b8, the destination operand data type must be used together with the signMode input parameter of the `SetDeqScale` API. When signMode is true, the output data type is int8_t; when signMode is false, the output data type is uint8_t.
- The halfBlock parameter is valid only when the output data is of the b8 type.
- When the input type is int16_t and the vector quantization mode is enabled, the `SetDeqScale` API requires a UB space of at least 128B. If the space is less than 128B, precision errors may occur.
- This API must be used together with `SetDeqScale`. The usage relationship mapping table is as follows:

  **Table**  CastDequant and SetDeqScale usage relationship mapping table
  |CastDequant usage scenario|Corresponding SetDeqScale function prototype|
  |---|---|
  |Input type is int16_t, vector quantization mode disabled |<br>\_\_aicore\_\_ inline void SetDeqScale(float scale, int16_t offset, bool signMode)<br>|
  |Input type is int16_t, vector quantization mode enabled|<br>template &lt;typename T&gt;<br>\_\_aicore\_\_ inline void SetDeqScale(const LocalTensor&lt;T&gt;& vdeq, const VdeqInfo& vdeqInfo)<br>|
  |Input type is int32_t, output data type is half |<br>\_\_aicore\_\_ inline void SetDeqScale(half scale)<br>|

## Example<a name="section642mcpsimp"></a>

For the complete call example, see [Compound Computation Example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/element_wise_compound_compute).

- High-dimensional slicing computation API example - continuous mask mode

  ```cpp
  int32_t mask = 256 / sizeof(int16_t);
  // repeatTime = 2, 128 elements one repeat, 256 elements total
  // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
  // dstRepStride, srcRepStride = 8, no gap between repeats
  AscendC::CastDequant<uint8_t, int16_t, true, true, true>(dstLocal, srcLocal, mask, 2, { 1, 1, 8, 8 });
  ```

- High-dimensional slicing computation API example - bitwise mask mode

  ```cpp
  uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
  // repeatTime = 2, 128 elements one repeat, 256 elements total
  // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
  // dstRepStride, srcRepStride = 8, no gap between repeats
  AscendC::CastDequant<uint8_t, int16_t, true, true, true>(dstLocal, srcLocal, mask, 2, { 1, 1, 8, 8 });
  ```

- Computation of the first n data elements

  ```cpp
  AscendC::CastDequant<uint8_t, int16_t, true, true>(dstLocal, srcLocal, 256);
  ```

The result is as follows:

```plain
Input data srcLocal: 
[20 53 26 12 36  6 20 93 66 30 56 99 59 92  7 37 22 47 98 10 85 29 14 46
 17 34 45 17 25 45 82 17 66 94 68 23 67  8 89  8 92  6 10 80 87 20  9 81
 70 62 11 58 38 83 32 14 38 47 41 63 94 26 96 89 88 35 86 55 60 82 15 65
 92 67 83 23 63 25 85 93 50 91 75 60 80 10 55 20 71 14 67 23 31 63  7 93
 69 45 61 23 43 86 11 81 81 36 76 58 53 25 23 51 59 78 82 10 39 40 24 50
 68 49 79 40  4 53 22 38 45 17 29 54  9 66 98 47 12 47 47 20 98  0 59 77
  1 21 39 70 66 20 68  8 77 77 54  0  3 33 37 37 48 60 83 88 27 70 31 49
 75 21 59  3 99 84 92 84 14 44 26 56 72 56 37 52 39 11  2 59 59 65 71 64
 10 65 62 48 42 79 69 69 27 99  8 38 36 77 34 34 60 50 52 50 41 31 95 68
 27 16 42 64 19 47  0 10 36 36 33 62 98 64 32 81 49 53 27 70 35  9 63  7
 10 89  3 39 94 23 89 16 23 60 71 42 46 58 65 90]
Output data dstLocal: 
[ 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 20 53 26 12 36  6 20 93
 66 30 56 99 59 92  7 37  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 22 47 98 10 85 29 14 46 17 34 45 17 25 45 82 17  0  0  0  0  0  0  0  0
   0  0  0  0  0  0  0  0 66 94 68 23 67  8 89  8 92  6 10 80 87 20  9 81
   0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 70 62 11 58 38 83 32 14
 38 47 41 63 94 26 96 89  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 88 35 86 55 60 82 15 65 92 67 83 23 63 25 85 93  0  0  0  0  0  0  0  0
   0  0  0  0  0  0  0  0 50 91 75 60 80 10 55 20 71 14 67 23 31 63  7 93
   0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 69 45 61 23 43 86 11 81
 81 36 76 58 53 25 23 51  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 59 78 82 10 39 40 24 50 68 49 79 40  4 53 22 38  0  0  0  0  0  0  0  0
   0  0  0  0  0  0  0  0 45 17 29 54  9 66 98 47 12 47 47 20 98  0 59 77
   0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1 21 39 70 66 20 68  8
 77 77 54  0  3 33 37 37  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 48 60 83 88 27 70 31 49 75 21 59  3 99 84 92 84  0  0  0  0  0  0  0  0
   0  0  0  0  0  0  0  0 14 44 26 56 72 56 37 52 39 11  2 59 59 65 71 64
   0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 10 65 62 48 42 79 69 69
 27 99  8 38 36 77 34 34  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 60 50 52 50 41 31 95 68 27 16 42 64 19 47  0 10  0  0  0  0  0  0  0  0
   0  0  0  0  0  0  0  0 36 36 33 62 98 64 32 81 49 53 27 70 35  9 63  7
   0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 10 89  3 39 94 23 89 16
 23 60 71 42 46 58 65 90]
```
