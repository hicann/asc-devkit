# AddDeqRelu<a name="ZH-CN_TOPIC_0000001815255356"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:04:26.012Z pushedAt=2026-10-28T03:14:39.450Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/composite_compute/AddDeqRelu_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_intf.h"`.

Sequentially performs element-wise summation, applies dequantization to the result, and then performs ReLU computation (compares the result with 0 and takes the larger value). The computation formula is as follows:

$$
dst_i = Relu(Deq(src0_i + src1_i))
$$

The Deq computation formula is as follows. It uses the RINT rounding mode, and scale must be set through [SetDeqScale](../type_conversion_aux_config/SetDeqScale.md):

$$
Deq(x) = Cast(x \times scale)
$$

The Relu computation formula is as follows:

$$
Relu(x) = max(0, x)
$$

## Prototype<a name="section620mcpsimp"></a>

- Tensor First n Elements Computation

  ```cpp
  __aicore__ inline void AddDeqRelu(const LocalTensor<half>& dst, const LocalTensor<int32_t>& src0, const LocalTensor<int32_t>& src1, const int32_t& count)
  ```

- tensor high-dimensional slicing computation
  - **mask** bit-by-bit mode

    ```cpp
    template <bool isSetMask = true>
    __aicore__ inline void AddDeqRelu(const LocalTensor<half>& dst, const LocalTensor<int32_t>& src0, const LocalTensor<int32_t>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

  - **mask** continuous mode

    ```cpp
    template <bool isSetMask = true>
    __aicore__ inline void AddDeqRelu(const LocalTensor<half>& dst, const LocalTensor<int32_t>& src0, const LocalTensor<int32_t>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

When the operand uses the [TensorTrait](../../aux_data_structures/TensorTrait/TensorTrait.md) type, LocalTensor requires template parameters. The APIs that support passing the operand data type as a template parameter are as follows:

- tensor first n elements computation

  ```cpp
  template <typename T, typename U>
  __aicore__ inline void AddDeqRelu(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const int32_t& count)
  ```

- tensor high-dimensional slicing computation
  - **mask** bit-by-bit mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void AddDeqRelu(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

  - **mask** continuous mode

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void AddDeqRelu(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Data type of the destination operand. |
| U | Data type of the source operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operands.<br>The type is LocalTensor, with supported TPosition values of VECIN/VECCALC/VECOUT. |
| count | Input | Number of elements involved in the computation.<br>**Note: The value range of this parameter depends on the data type of the operand. Different data types support different maximum numbers of elements, and the maximum amount of data to be processed cannot exceed the UB size limit.** |
| mask[]/mask | Input | mask controls the elements involved in the computation in each iteration.<br>For details, see [Mask Operations](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations.<br>The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeats) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Slicing](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameter that controls the address stride of the operand. The type is [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which contains parameters such as the address stride of the same datablock between adjacent iterations of the operand and the address stride of different datablocks within the same iteration of the operand.<br>For details about the address stride parameter between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride parameter of DataBlock within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Types

**Table 3**  Data type combinations

| src0 and src1 data type | dst data type |
| :---------------------- | :------------ |
| int32_t                 | half          |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).
<!-- npu="A3,910b,950" id8 -->
- When count or repeatTime is set to 0, the API behaves as follows:
  <!-- npu="A3,910b" id9 -->
  - For the following products, the API does not perform any computation or write to the destination operand, and is treated as a NOP (no operation).
    <!-- npu="A3" id10 -->
    - Atlas A3 products
    <!-- end id10 -->
    <!-- npu="910b" id11 -->
    - Atlas A2 products
    <!-- end id11 -->
  <!-- end id9 -->
  <!-- npu="950" id12 -->
  - For Ascend 950PR&950DT products, the API is implemented for compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_compute.md) through VF. When count or repeatTime is set to 0, it is not guaranteed that the API is treated as a NOP (no operation).
  <!-- end id12 -->
<!-- end id8 -->
- This API must be used together with `SetDeqScale`. The mapping is as follows:

  | AddDeqRelu usage scenario | Corresponding `SetDeqScale` function prototype |
  | :------------------------ | :--------------------------------------------- |
  | Input type is int32_t     | `__aicore__ inline void SetDeqScale(half scale)` |

- This API involves the use of temporary space. The calculation formula is as follows: when the amount of data to be computed is less than or equal to 2K (2048), the temporary space used is linearly related to the amount of data; when the amount of data is greater than 2K, the temporary space size is fixed at 8KB:
  $$
  tmpSize =
  \begin{cases}
    count * sizeof(int32\_t), & \text{if } count \le 2K \\
    8K, & \text{if } count \gt 2K
  \end{cases}
  $$

## Key Features

### Overflow Protection

When performing dequantization from the int32_t to the half data type, this API applies overflow protection:

$$
Deq(x) = Cast(x \times (1/2^{17}) \times scale \times 2^{17}) = Cast(x \times scale)
$$
The intermediate downscaling step keeps the value within a safe range, effectively preventing floating-point overflow.

## Example<a name="section837496171220"></a>

In this example, srcLocal is of the int32\_t type and dstLocal is of the half type. The mask is computed based on int32\_t.

- Example of high-dimensional slicing computation with mask in continuous mode.

  ```cpp
  uint64_t mask = 256 / sizeof(int32_t); // 64
  // repeatTime = 4. Each iteration computes 64 numbers, for a total of 256 numbers.
  // dstBlkStride, src0BlkStride, src1BlkStride = 1. Data is read and written continuously within a single iteration.
  // dstRepStride = 4, src0RepStride, src1RepStride = 8. Data is read and written continuously between adjacent iterations.
  half scale = 0.1;
  AscendC::SetDeqScale(scale);
  AscendC::AddDeqRelu(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 4, 8, 8 });
  ```

- Example of high-dimensional slicing computation with mask in bit-by-bit mode.

  ```cpp
  uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
  // repeatTime = 4. Each iteration computes 64 numbers, for a total of 256 numbers.
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration.
  // dstRepStride = 4, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations.
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
