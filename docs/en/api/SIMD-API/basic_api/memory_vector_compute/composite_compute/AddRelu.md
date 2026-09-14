# AddRelu<a name="ZH-CN_TOPIC_0000001723846124"></a>

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T08:37:59.088Z -->

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

Header file path: `"basic_api/kernel_operator_vec_ternary_scalar_intf.h"`.

Performs element-wise addition, and then performs the ReLU computation (compares the result with 0 and takes the larger value). The computation formula is as follows:

$$
dst_i=Relu(src0_i+src1_i)
$$

## Prototype<a name="section620mcpsimp"></a>

- Computation on the first n data of the tensor

  ```cpp
  template <typename T>
  __aicore__ inline void AddRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
  ```

- High-dimensional splitting computation of the tensor
  - Bitwise mask mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void AddRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

  - Continuous mask mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void AddRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| :----- | :--- |
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operands.<br>The type is LocalTensor, and the supported TPosition is VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| count | Input | Number of elements that participate in the computation.<br><br>**Note: The value range of this parameter is related to the data type of the operand. Different data types support different maximum numbers of elements that can be processed, and the maximum amount of data to be processed cannot exceed the UB size limit.** |
| mask[]/mask | Input | **mask** controls the elements that participate in the computation in each iteration.<br>For details about the settings, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations.<br>The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeats) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Slicing API](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which includes parameters such as the address stride of the same datablock between adjacent iterations of the operands and the address stride of different datablocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

<!-- npu="950" id1 -->
Ascend 950PR/Ascend 950DT: The supported data types are int16_t, half, float, int64_t, and uint64_t.
<!-- end id1 -->

<!-- npu="A3" id2 -->
Atlas A3 training products/Atlas A3 inference products support the following data types: int16_t, half, and float.
<!-- end id2 -->

<!-- npu="910b" id3 -->
Atlas A2 training products/Atlas A2 inference products support the following data types: int16_t, half, and float.
<!-- end id3 -->

<!-- npu="310b" id4 -->
Atlas 200I/500 A2 inference products support the following data types: half and float.
<!-- end id4 -->

<!-- npu="310p" id5 -->
Atlas inference products AI Core support the following data types: int16_t, half, and float.
<!-- end id5 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).
<!-- npu="950" id6 -->
- For Ascend 950PR/Ascend 950DT, the int64\_t and uint64\_t data types support only the API for computing the first n data of a tensor.
<!-- end id6 -->

## Example<a name="section642mcpsimp"></a>

For a complete invocation example, see [Compound Computation Example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/element_wise_compound_compute).

- Tensor high-dimensional splitting computation example - continuous mask mode.

  ```cpp
  uint64_t mask = 128;
  // repeatTime = 4, compute 128 numbers per iteration, 512 numbers in total.
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration.
  // dstRepStride, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations.
  AscendC::AddRelu(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
  ```

- Tensor high-dimensional splitting computation example - bitwise mask mode.

  ```cpp
  uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
  // repeatTime = 4, one iteration computes 128 numbers, and 512 numbers are computed in total
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration
  // dstRepStride, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations
  AscendC::AddRelu(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
  ```

- Example of computing the first n data of a tensor.

  ```cpp
  AscendC::AddRelu(dstLocal, src0Local, src1Local, 512);
  ```

The result is as follows:

```plain
Input data src0Local: [1 -2 3 ... -6]
Input data src1Local: [1 3 -4 ... 5]
Output data dstLocal: [2 1 0 ... 0]
```
