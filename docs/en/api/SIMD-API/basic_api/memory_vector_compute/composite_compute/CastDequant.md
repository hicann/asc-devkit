# CastDequant<a name="ZH-CN_TOPIC_0000001788011400"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:09:21.673Z pushedAt=2026-10-28T03:14:39.457Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/composite_compute/CastDequant_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_vconv_intf.h"`.

Performs quantization and precision conversion on the input, and applies saturation processing to the output result. This API must be used together with the [SetDeqScale](../type_conversion_aux_config/SetDeqScale.md) API.

- The input data type is int16_t, with vector quantization mode disabled.

  Uses the scale, offset, and signMode parameters set by the `SetDeqScale` API to perform Per-Tensor quantization on the entire input tensor, and outputs the b8 data type. The calculation formula is as follows:

  $$
  dst_i = Cast((src_i * scale) + offset)
  $$

- The input data type is int16_t, with vector quantization mode enabled.

  Obtains 16 groups of quantization parameters from the `SetDeqScale` API, each group containing a scale, offset, and signMode, to perform Per-Block quantization on the input tensor. One repeat processes 8 DataBlocks, each DataBlock being 32B in size. Quantization and precision conversion are performed on the 16 input data elements in each DataBlock, with each element using an independent group of quantization parameters, and the output is the b8 data type. The calculation formula is as follows:

  $$
  dst_i = Cast((src_i * scale_j) + offset_j), j \in [0,15]
  $$

- Input type is int32_t, and the output data type is half.

  Uses the scale parameter set by the `SetDeqScale` API, with RINT precision rounding mode. The calculation formula is as follows:

  $$
  dst_i = Cast(src_i * scale)
  $$

## Prototype<a name="section620mcpsimp"></a>

- Tensor first n elements computation

  ```cpp
  template <typename T, typename U, bool isVecDeq = true, bool halfBlock = true>
  __aicore__ inline void CastDequant(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint32_t count)
  ```

- Tensor high-dimensional split computation
  - **mask** bit-by-bit mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true, bool isVecDeq = true, bool halfBlock = true>
    __aicore__ inline void CastDequant(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[], uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
    ```

  - **mask** continuous mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true, bool isVecDeq = true, bool halfBlock = true>
    __aicore__ inline void CastDequant(const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table**  Template parameter description

| Parameter | Description |
| :----- | :--- |
|T|Destination operand data type.<br>When the type is b8, it is used together with the signMode input parameter of the `SetDeqScale` API. When signMode=true, the output data type is int8_t; when signMode=false, the output data type is uint8_t.|
|U|Source operand data type.<br> |
|isSetMask|Whether to set the mask inside the API.<br>&bull; true: sets the mask inside the API.<br>&bull; false: sets the mask outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameters is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning.|
|isVecDeq|Controls whether to select the vector quantization mode.<br>Used together with the `SetDeqScale` API. When the `SetDeqScale` API receives a Tensor, isVecDeq must be true.|
|halfBlock|Valid when the output data type is b8. Indicates whether the output elements are stored in the upper or lower half of the DataBlock.<br>When halfBlock=true, the result is stored in the lower half of the DataBlock; when halfBlock=false, the result is stored in the upper half of the DataBlock.<br>As shown in the following figure, an input of one DataBlock size produces eight b8-type outputs. When halfBlock is false, these eight output data are stored in the upper half of the DataBlock.<br>![](../../../../figures/halfblock_description.png)<br>To obtain continuous destination operand data, you can divide the DataBlock into odd and even blocks and call the API twice to process the odd and even blocks separately. The output of odd blocks is stored in the lower half of the DataBlock, and the output of even blocks is stored in the upper half of the DataBlock.<br>![](../../../../figures/castdequant_continous.png) |

**Table**  Parameters

|Parameter|Input/Output|Description|
|---|---|---|
|dst|Output|Destination operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.|
|src|Input|Source operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.|
|count|Input|Number of elements involved in the computation.<br>The value range of this parameter depends on the operand data type. Different data types support different maximum numbers of elements that can be processed. The maximum amount of data processed cannot exceed the UB size limit.|
|mask/mask[]|Input|mask controls the elements involved in the computation in each iteration.<br>Note: The mask for data type conversion is filtered based on the larger sizeof(dtype) between the input and output types.<br>For details, see [Mask Operations](../mask_operations/mask_operations.md).|
|repeatTime|Input|Number of repeated iterations.<br>The vector computation unit reads 256 consecutive bytes of data for computation each time. To complete processing of the input data, multiple iterations (repeat) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Split](../SIMD_compute/high_dim_split.md).|
|repeatParams|Input|Parameters that control the operand address stride. Type: [UnaryRepeatParams](../../aux_data_structures//UnaryRepeatParams.md). Contains parameters such as the address stride of the same DataBlock between adjacent iterations of the operand, and the address stride of different DataBlocks within the same iteration of the operand.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md).|

## Data Types

<!-- npu="950" id8 -->

**Table**  Supported data type combinations for Ascend 950PR&950DT products

| src data type | dst data type |
| :---------- | :---------- |
| int32_t     | half        |
| int16_t     | int8_t      |
| int16_t     | uint8_t     |

<!-- end id8 -->

<!-- npu="A3" id9 -->

**Table**  Supported data type combinations for Atlas A3 products

| src data type | dst data type |
| :---------- | :---------- |
| int32_t     | half        |
| int16_t     | int8_t      |
| int16_t     | uint8_t     |

<!-- end id9 -->

<!-- npu="910b" id10 -->

**Table**  Supported data type combinations for Atlas A2 products

| src data type | dst data type |
| :---------- | :---------- |
| int32_t     | half        |
| int16_t     | int8_t      |
| int16_t     | uint8_t     |

<!-- end id10 -->

<!-- npu="310p" id11 -->

**Table**  Supported data type combinations for Atlas inference products AI Core

| src data type | dst data type |
| :---------- | :---------- |
| int16_t     | int8_t      |
| int16_t     | uint8_t     |

<!-- end id11 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).
- When the output type is b8, the destination operand data type must be used together with the signMode input parameter of the `SetDeqScale` API. When signMode is true, the output data type is int8_t; when signMode is false, the output data type is uint8_t.
- The halfBlock parameter is valid only when the output data is of the b8 type.
<!-- npu="A3,910b,950" id12 -->
- When the count or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id13 -->
  - For the following models, this API does not perform computation or write to the destination operand, and is treated as a NOP (no operation).
    <!-- npu="A3" id14 -->
    - Atlas A3 products
    <!-- end id14 -->
    <!-- npu="910b" id15 -->
    - Atlas A2 products
    <!-- end id15 -->
  <!-- end id13 -->
  <!-- npu="950" id16 -->
  - For Ascend 950PR&950DT products, this API is implemented for compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is set to 0, this API is not guaranteed to be treated as a NOP (no operation).
  <!-- end id16 -->
<!-- end id12 -->
<!-- npu="950" id17 -->
- Notes on Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - The tensor high-dimensional splitting calculation API occupies 8 KB of temporary UB space.
  - The API for continuous calculation of the first n tensor data does not involve the use of 8 KB of temporary UB space.
<!-- end id17 -->
- When the input type is int16_t and vector quantization mode is enabled, the `SetDeqScale` API requires a UB space of at least 128 B. If the space is less than 128 B, precision errors may occur.
- This API must be used together with `SetDeqScale`. The usage relationship mapping table is as follows:

  **Table**  Mapping between CastDequant and SetDeqScale usage

  |CastDequant usage scenario|Corresponding SetDeqScale function prototype|
  |---|---|
  |Input type is int16_t, with vector quantization mode disabled|<br>\_\_aicore\_\_ inline void SetDeqScale(float scale, int16_t offset, bool signMode)|
  |Input type is int16_t, with vector quantization mode enabled|<br>template &lt;typename T&gt;<br>\_\_aicore\_\_ inline void SetDeqScale(const LocalTensor&lt;T&gt;& vdeq, const VdeqInfo& vdeqInfo)|
  |Input type is int32_t, with output data type of half|<br>\_\_aicore\_\_ inline void SetDeqScale(half scale)|

## Key Features

### Overflow Protection

When performing dequantization from the int32_t to the half data type, this API applies overflow protection:

$$
dst_i = Cast(src_i / 2^{17} \times scale \times 2^{17}) = Cast(src_i \times scale)
$$
The intermediate downscaling step keeps the values within a safe range, effectively preventing floating-point overflow.

## Example<a name="section642mcpsimp"></a>

For a complete example, see [Compound Compute Example](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/element_wise_compound_compute).

- High-dimensional splitting calculation API example - mask contiguous mode.

  ```cpp
  int32_t mask = 256 / sizeof(int16_t);
  // repeatTime = 2, 128 elements one repeat, 256 elements total
  // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
  // dstRepStride, srcRepStride = 8, no gap between repeats
  AscendC::CastDequant<uint8_t, int16_t, true, true, true>(dstLocal, srcLocal, mask, 2, { 1, 1, 8, 8 });
  ```

- High-dimensional splitting calculation API example - mask bit-by-bit mode.

  ```cpp
  uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
  // repeatTime = 2, 128 elements one repeat, 256 elements total
  // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
  // dstRepStride, srcRepStride = 8, no gap between repeats
  AscendC::CastDequant<uint8_t, int16_t, true, true, true>(dstLocal, srcLocal, mask, 2, { 1, 1, 8, 8 });
  ```

- First n elements calculation API example.

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
