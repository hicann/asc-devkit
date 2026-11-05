# Axpy<a name="ZH-CN_TOPIC_0000001424952592"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:08:03.712Z pushedAt=2026-10-28T03:14:39.455Z nl=2 -->

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
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/composite_compute/Axpy_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_ternary_scalar_intf.h"`.

Computes the product of a vector and a scalar, and adds the product to the output vector element by element. The formula is as follows:

$$
dst_j = scalar × src_i + dst_i
$$

## Prototype<a name="section620mcpsimp"></a>

- The first n data of the tensor are computed continuously.

  ```cpp
  template <typename T, typename U>
  __aicore__ inline void Axpy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const U& scalarValue, const int32_t& count)
  ```

- Tensor high-dimensional split computation.
  - **mask** bit-by-bit mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void Axpy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const U& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
    ```

  - **mask** continuous mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void Axpy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table**  Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Data type of the destination operand. |
| U | Data type of the source operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table**  Parameters

| Parameter | Input/Output | Description |
| :----------- | :-------- | :--- |
| dst          | Output      | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT. |
| src          | Input      | Source operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT. |
| scalarValue  | Input      | Source operand, a scalar. The data type of scalarValue must be consistent with that of src. |
| count        | Input      | Number of elements involved in the computation. For details about this parameter, see [continuous compute](../SIMD_compute/continuous_compute.md). |
| mask/mask[]  | Input      | mask controls the elements involved in the computation in each iteration.<br>For configuration details, see [mask operation](../SIMD_compute/mask.md). |
| repeatTime   | Input      | Number of repeated iterations.<br>The vector computation unit reads 256 consecutive bytes of data for computation each time. To process the input data completely, multiple iterations (repeat) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [high-dimensional split API](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input      | Parameter that controls the address stride of operands. Type: [UnaryRepeatParams](../../aux_data_structures//UnaryRepeatParams.md). It includes the address stride of the same DataBlock between adjacent iterations of an operand, the address stride of different DataBlocks within the same iteration of an operand, and other parameters.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md#address-stride-configuration). For details about the address stride of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md#address-stride-configuration). |

## Data Type<a name="zh-cn_topic_0000002547893099_section4219135304818"></a>

The PAR column indicates the number of elements that the vector computation unit can process in one iteration.

<!-- npu="950" id10 -->

**Table** Data type combinations supported by Ascend 950PR&950DT products

| src data type | scalarValue data type | dst data type | PAR |
| ----------- | -------------- | ----------- | --- |
| half        | half           | half        | 128 |
| float       | float          | float       | 64  |
| half        | half           | float       | 64  |
| int64_t     | int64_t        | int64_t     | 64  |
| uint64_t    | uint64_t       | uint64_t    | 64  |
| bfloat16_t  | bfloat16_t     | bfloat16_t  | 128 |

For Ascend 950PR&950DT products, the int64_t and uint64_t data types are supported only by the API for computing the first n data elements of a tensor.

<!-- end id10 -->

<!-- npu="A3" id11 -->

**Table** Data type combinations supported by Atlas A3 products

| src data type | scalarValue data type | dst data type | PAR |
| ----------- | -------------- | ----------- | --- |
| half        | half           | half        | 128 |
| float       | float          | float       | 64  |
| half        | half           | float       | 64  |

<!-- end id11 -->

<!-- npu="910b" id12 -->

**Table** Data type combinations supported by Atlas A2 products

| src data type | scalarValue data type | dst data type | PAR |
| ----------- | -------------- | ----------- | --- |
| half        | half           | half        | 128 |
| float       | float          | float       | 64  |
| half        | half           | float       | 64  |

<!-- end id12 -->

<!-- npu="310b" id13 -->

**Table** Data type combinations supported by Atlas 200I/500 A2 inference products

| src data type | scalarValue data type | dst data type | PAR |
| ----------- | -------------- | ----------- | --- |
| half        | half           | half        | 128 |
| float       | float          | float       | 64  |
| half        | half           | float       | 64  |

<!-- end id13 -->

<!-- npu="310p" id14 -->

**Table** Data type combinations supported by the Atlas inference products AI Core

| src data type | scalarValue data type | dst data type | PAR |
| ----------- | -------------- | ----------- | --- |
| half        | half           | half        | 128 |
| float       | float          | float       | 64  |
| half        | half           | float       | 64  |

<!-- end id14 -->

<!-- npu="910" id15 -->

**Table** Data type combinations supported by Atlas training products

| src data type | scalarValue data type | dst data type | PAR |
| ----------- | -------------- | ----------- | --- |
| half        | half           | half        | 128 |
| float       | float          | float       | 64  |
| half        | half           | float       | 64  |

<!-- end id15 -->

## Return Value<a name="section17124037164714"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).
- When the source operand data type is half and the destination operation data type is float, address overlap is not supported.
- When using the tensor high-dimensional split computation API, if the data types of src and scalarValue are half and the data type of dst is float, at most 64 input data elements are processed in one iteration.
<!-- npu="A3,910b,950" id18 -->
- When the count or repeatTime parameter is set to 0, the behavior of this API is as follows:
  <!-- npu="A3,910b" id19 -->
  - For the following models, this API does not perform computation and does not write to the destination operation. This API is treated as a NOP (no operation).
    <!-- npu="A3" id20 -->
    - Atlas A3 products
    <!-- end id20 -->
    <!-- npu="910b" id21 -->
    - Atlas A2 products
    <!-- end id21 -->
  <!-- end id19 -->
  <!-- npu="950" id22 -->
  - For Ascend 950PR&950DT products, this API is implemented for compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is set to 0, it is not guaranteed that this API is treated as a NOP (no operation).
  <!-- end id22 -->
<!-- end id18 -->
<!-- npu="950" id23 -->
- Notes on Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - The tensor high-dimensional split computation API occupies 8 KB of UB temporary space.
  - The API for continuous computation of the first n tensor data elements does not involve the occupation of 8 KB of UB temporary space.
<!-- end id23 -->

## Example<a name="section642mcpsimp"></a>

This sample shows only some key code. To run the sample code, see the complete sample [Compound Compute Sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/element_wise_compound_compute).

- Tensor high-dimensional split compute sample - mask continuous mode.

  ```cpp
  // repeatTime = 4, mask = 128, 128 elements one repeat, 512 elements total
  // srcLocal data type is half, scalarValue data type is half, dstLocal data type is half
  // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
  // dstRepStride, srcRepStride = 8, no gap between repeats 
  AscendC::Axpy(dstLocal, srcLocal, (half)2.0, 128, 4,{ 1, 1, 8, 8 });
  
  // srcLocal data type is half, scalarValue data type is half, dstLocal data type is float
  // repeatTime = 8, mask = 64, 64 elements one repeat, 512 elements total
  // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
  // dstRepStride = 8, srcRepStride = 4, no gap between repeats 
  AscendC::Axpy(dstLocal, srcLocal, (half)2.0, 64, 8,{ 1, 1, 8, 4 }); // Select the first 4 datablocks of the source operand for computation in each iteration
  ```

- Tensor high-dimensional split compute sample - mask bit-by-bit mode.

  ```cpp
  uint64_t mask[2] = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
  // repeatTime = 4, 128 elements one repeat, 512 elements total, half precision combination
  // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
  // dstRepStride, srcRepStride = 8, no gap between repeats
  AscendC::Axpy(dstLocal, srcLocal, (half)2.0, mask, 4,{ 1, 1, 8, 8 });
  ```

- Tensor first-n-data compute sample.

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
