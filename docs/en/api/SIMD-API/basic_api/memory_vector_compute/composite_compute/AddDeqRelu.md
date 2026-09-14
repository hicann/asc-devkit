# AddDeqRelu<a name="ZH-CN_TOPIC_0000001815255356"></a>

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T08:34:31.323Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_ternary_scalar_intf.h"`.

Performs element-wise summation, then deq quantization on the result, and finally relu computation (compares the result with 0 and takes the larger value). The computation formula is as follows:

$$
dst_i = Relu(Deq(src0_i + src1_i))
$$

The computation formula of Deq is as follows, which uses the RINT rounding mode. The scale needs to be set through [SetDeqScale](../type_conversion_aux_config/SetDeqScale.md):

$$
Deq(x) = Cast(x \times scale)
$$

## Prototype<a name="section620mcpsimp"></a>

- Computation of the first n data elements of the tensor

  ```cpp
  __aicore__ inline void AddDeqRelu(const LocalTensor<half>& dst, const LocalTensor<int32_t>& src0, const LocalTensor<int32_t>& src1, const int32_t& count)
  ```

- High-dimensional splitting computation for the tensor
  - Bitwise mask mode

    ```cpp
    template <bool isSetMask = true>
    __aicore__ inline void AddDeqRelu(const LocalTensor<half>& dst, const LocalTensor<int32_t>& src0, const LocalTensor<int32_t>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

  - Continuous mask mode

    ```cpp
    template <bool isSetMask = true>
    __aicore__ inline void AddDeqRelu(const LocalTensor<half>& dst, const LocalTensor<int32_t>& src0, const LocalTensor<int32_t>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

When the operand uses the [TensorTrait](../../aux_data_structures/TensorTrait/TensorTrait.md) type, LocalTensor requires template parameters to be passed. The API that supports passing the operand data type as a template parameter is as follows:

- Computation of the first n data elements of the tensor

  ```cpp
  template <typename T, typename U>
  __aicore__ inline void AddDeqRelu(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const int32_t& count)
  ```

- High-dimensional splitting computation of tensor
  - Bitwise mask mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void AddDeqRelu(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

  - Continuous mask mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void AddDeqRelu(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| :----- | :--- |
| T | Data type of the destination operand. |
| U | Data type of the source operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operands.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| count | Input | Number of elements involved in the computation.<br>**Note: The value range of this parameter is related to the data type of the operands. Different data types support different maximum numbers of elements that can be processed, and the maximum amount of data to be processed cannot exceed the UB size limit.** |
| mask[]/mask | Input | mask controls the elements that participate in the computation in each iteration.<br>For details about the settings, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations.<br>The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeat) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Slicing](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which includes parameters such as the address stride of the same datablock between adjacent iterations of the operands and the address stride of different datablocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

**Table 3** Data type combinations

| src0, src1 data type | dst data type |
| :----------------- | :---------- |
| int32_t            | half        |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For address alignment constraints, see [Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- For operand address overlap constraints, see [Address Overlap Constraints](../../../general_description_and_constraints.md#section796754519912).
- This API must be used together with `SetDeqScale`. The relationship mapping table is as follows:

  | AddDeqRelu Usage Scenario | Corresponding `SetDeqScale` Function Prototype |
  | :------------------------ | :--------------------------------------------- |
  | Input type is int32_t     | `__aicore__ inline void SetDeqScale(half scale)` |

- This API involves the use of temporary space. The calculation formula is as follows: when the amount of data to be computed is less than or equal to 2K (2048), the temporary space used is linearly related to the amount of data to be computed; when the amount of data is greater than 2K, the temporary space size is fixed at 8KB:
  $$
  tmpSize =
  \begin{cases}
    count * sizeof(int32\_t), & \text{if } count \le 2K \\
    8K, & \text{if } count \gt 2K
  \end{cases}
  $$

## Example<a name="section837496171220"></a>

In this example, srcLocal is of the int32\_t type and dstLocal is of the half type. The int32\_t type is used as the basis when computing the mask.

- Example of high-dimensional splitting computation for a tensor - mask continuous mode.

  ```cpp
  uint64_t mask = 256 / sizeof(int32_t); // 64
  // repeatTime = 4, 64 numbers are computed in one iteration, and 256 numbers are computed in total
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration
  // dstRepStride = 4, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations
  half scale = 0.1;
  AscendC::SetDeqScale(scale);
  AscendC::AddDeqRelu(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 4, 8, 8 });
  ```

- Example of high-dimensional splitting computation for a tensor - mask bit-by-bit mode.

  ```cpp
  uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
  // repeatTime = 4, 64 numbers are computed in one iteration, and 256 numbers are computed in total
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration
  // dstRepStride = 4, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations
  half scale = 0.1;
  AscendC::SetDeqScale(scale);
  AscendC::AddDeqRelu(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 4, 8, 8 });
  ```

- Example of computing the first n data of a tensor.

  ```cpp
  half scale = 0.1;
  AscendC::SetDeqScale(scale);
  AscendC::AddDeqRelu(dstLocal, src0Local, src1Local, 512);
  ```

The result is as follows:

```plain
Input data src0Local: [70 36 43 54 28 49 27 82 95 ...]
Input data src1Local: [19 33 34 50 42  2 97 93 99 ...]
Output data dstLocal: [8.9 6.9 7.7 10.4 7.0 5.1 12.4 17.5 19.4 ...]
```
