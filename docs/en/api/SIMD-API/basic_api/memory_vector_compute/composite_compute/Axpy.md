# Axpy<a name="ZH-CN_TOPIC_0000001424952592"></a>

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T08:44:55.032Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Supported
<!-- end id14 -->


## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_ternary_scalar_intf.h"`

Computes the product of a vector and a scalar, and adds the product to the output vector element by element. The computation formula is as follows:

$$
dst_j = scalar × src_i + dst_i
$$

## Prototype<a name="section620mcpsimp"></a>

- Computation of the first n data elements of the tensor

  ```cpp
  template <typename T, typename U>
  __aicore__ inline void Axpy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const U& scalarValue, const int32_t& count)
  ```

- Computation for high-dimensional splitting of the tensor
  - Bitwise mask mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void Axpy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const U& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
    ```

  - Continuous mask mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void Axpy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
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
| :----------- | :-------- | :--- |
| dst          | Output      | Destination operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| src          | Input      | Source operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| scalarValue  | Input      | Source operand, a scalar. The data type of scalarValue must be the same as that of src. |
| count        | Input      | Number of elements that participate in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_computation.md). |
| mask/mask[]  | Input      | mask controls the elements that participate in the computation in each iteration.<br>For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime   | Input      | Number of repeated iterations.<br>The vector computation unit reads 256 consecutive bytes of data for computation each time. To complete the processing of the input data, multiple iterations (repeats) are required to finish reading and computing all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Slicing API](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input      | Parameters that control the address stride of the operands. The type is [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md#address-stride-configuration). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md#address-stride-configuration). |

## Data Type<a name="zh-cn_topic_0000002547893099_section4219135304818"></a>

PAR indicates the number of elements that the vector computation unit can process in one iteration.

<!-- npu="950" id1 -->
**Table** Data type combinations supported by Ascend 950PR/Ascend 950DT

| src data type | scalarValue data type | dst data type | PAR |
| ----------- | -------------- | ----------- | --- |
| half        | half           | half        | 128 |
| float       | float          | float       | 64  |
| half        | half           | float       | 64  |
| int64_t     | int64_t        | int64_t     | 64  |
| uint64_t    | uint64_t       | uint64_t    | 64  |
| bfloat16_t  | bfloat16_t     | bfloat16_t  | 128 |
<!-- end id1 -->

<!-- npu="A3" id2 -->
**Table** Data type combinations supported by Atlas A3 training products/Atlas A3 inference products

| src data type | scalarValue data type | dst data type | PAR |
| ----------- | -------------- | ----------- | --- |
| half        | half           | half        | 128 |
| float       | float          | float       | 64  |
| half        | half           | float       | 64  |
<!-- end id2 -->

<!-- npu="910b" id3 -->
**Table** Data type combinations supported by Atlas A2 training products/Atlas A2 inference products

| src data type | scalarValue data type | dst data type | PAR |
| ----------- | -------------- | ----------- | --- |
| half        | half           | half        | 128 |
| float       | float          | float       | 64  |
| half        | half           | float       | 64  |
<!-- end id3 -->

<!-- npu="310b" id4 -->
**Table** Data type combinations supported by Atlas 200I/500 A2 inference products

| src data type | scalarValue data type | dst data type | PAR |
| ----------- | -------------- | ----------- | --- |
| half        | half           | half        | 128 |
| float       | float          | float       | 64  |
| half        | half           | float       | 64  |
<!-- end id4 -->

<!-- npu="310p" id5 -->
**Table** Data type combinations supported by Atlas inference products AI Core

| src data type | scalarValue data type | dst data type | PAR |
| ----------- | -------------- | ----------- | --- |
| half        | half           | half        | 128 |
| float       | float          | float       | 64  |
| half        | half           | float       | 64  |
<!-- end id5 -->

<!-- npu="910" id6 -->
**Table** Data type combinations supported by Atlas training products

| src data type | scalarValue data type | dst data type | PAR |
| ----------- | -------------- | ----------- | --- |
| half        | half           | half        | 128 |
| float       | float          | float       | 64  |
| half        | half           | float       | 64  |
<!-- end id6 -->

## Return Value<a name="section17124037164714"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For address alignment constraints, see [Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- For operand address overlap constraints, see [Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).
- When the source operand data type is half and the destination operand data type is float, address overlap is not supported.
- When using the tensor high-dimensional split computation API, if the data types of src and scalarValue are half and the data type of dst is float, a maximum of 64 input data elements can be processed in one iteration.
<!-- npu="950" id7 -->
- For Ascend 950PR/Ascend 950DT, the int64_t and uint64_t data types support only the tensor first-n-data computation API.
<!-- end id7 -->

## Example<a name="section642mcpsimp"></a>

This sample shows only some key code. To run the sample code, see the complete sample [Compound Computation Sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/element_wise_compound_compute).

- Tensor high-dimensional slicing computation sample - mask continuous mode

  ```cpp
  // repeatTime = 4, mask = 128, 128 elements one repeat, 512 elements total
  // The data type of srcLocal is half, the data type of scalarValue is half, and the data type of dstLocal is half
  // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
  // dstRepStride, srcRepStride = 8, no gap between repeats 
  AscendC::Axpy(dstLocal, srcLocal, (half)2.0, 128, 4,{ 1, 1, 8, 8 });
  
  // The data type of srcLocal is half, the data type of scalarValue is half, and the data type of dstLocal is float
  // repeatTime = 8, mask = 64, 64 elements one repeat, 512 elements total
  // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
  // dstRepStride = 8, srcRepStride = 4, no gap between repeats 
  AscendC::Axpy(dstLocal, srcLocal, (half)2.0, 64, 8,{ 1, 1, 8, 4 }); // Select the first 4 datablocks of the source operand for computation in each iteration
  ```

- tensor high-dimensional slicing computation example - mask bitwise mode

  ```cpp
  uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
  // repeatTime = 4, 128 elements one repeat, 512 elements total, half precision combination
  // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
  // dstRepStride, srcRepStride = 8, no gap between repeats
  AscendC::Axpy(dstLocal, srcLocal, (half)2.0, mask, 4,{ 1, 1, 8, 8 });
  ```

- Computation example for the first n data of the tensor

  ```cpp
  AscendC::Axpy(dstLocal, src0Local, (half)2.0, 512);// half precision combination
  ```

The result is as follows:

```plain
Input data (src0Local):
[1. 2. 3. 4. 5. 6. ... 512.]
Input data (scalarValue): 2.0
Initial value of output data (dstLocal):
[0. 0. 0. 0. 0. 0. ... 0.]
After the Axpy computation, output data (dstLocal):
[2. 4. 6. 8. 10. 12. ... 1024.]
```
