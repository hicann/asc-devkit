# AddDeqRelu<a name="ZH-CN_TOPIC_0000001815255356"></a>

<!-- md-trans-meta sourceCommit=f266635ab179c912cc9641061c2d27246be9170c translatedAt=2026-11-02T07:25:08.200Z pushedAt=2026-11-18T01:25:07.069Z nl=2 -->

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

The formula for ReLU is as follows:

$$
Relu(x) = max(0, x)
$$

## Prototype<a name="section620mcpsimp"></a>

- Tensor first n elements computation

  ```cpp
  __aicore__ inline void AddDeqRelu(const LocalTensor<half>& dst, const LocalTensor<int32_t>& src0, const LocalTensor<int32_t>& src1, const int32_t& count)
  ```

- Tensor high-dimensional slicing computation
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

- Tensor first n elements computation

  ```cpp
  template <typename T, typename U>
  __aicore__ inline void AddDeqRelu(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const int32_t& count)
  ```

- Tensor high-dimensional slicing computation
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
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT. |
| count | Input | Number of elements involved in the computation.<br>**Note: The value range of this parameter depends on the data type of the operand. Different data types support different maximum numbers of elements. The maximum amount of data to be processed cannot exceed the Unified Buffer (UB) size limit.** |
| mask[]/mask | Input | mask controls the elements involved in the computation in each iteration.<br>For details, see [Mask Operations](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations.<br>The vector computation unit reads 256 consecutive bytes of data for computation each time. To process all input data, multiple iterations (repeats) are required. repeatTime specifies the number of iterations.<br>For details about this parameter, see [High-Dimensional Splitting](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. Type: [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which contains parameters such as the address stride of the same datablock between adjacent iterations of the operands and the address stride of different datablocks within the same iteration of the operands.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride of DataBlock within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Type

**Table 3**  Data type combinations

| data types of src0 and src1 | dst data type |
| :----------------- | :---------- |
| int32_t            | half        |
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/composite_compute/AddDeqRelu_res.md#id2 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).
<!-- npu="A3,910b,950" id8 -->
- When the count or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id9 -->
  - For the following models, the API does not perform any calculation and does not write to the destination operand. The API is treated as a NOP (no operation).
    <!-- npu="A3" id10 -->
    - Atlas A3 products
    <!-- end id10 -->
    <!-- npu="910b" id11 -->
    - Atlas A2 products
    <!-- end id11 -->
  <!-- end id9 -->
  <!-- npu="950" id12 -->
  - For Ascend 950PR&950DT products, the API implements compatibility by calling the [Reg vector compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is set to 0, it is not guaranteed that the API is treated as a NOP (no operation).
  <!-- end id12 -->
<!-- end id8 -->
- This API must be used together with `SetDeqScale`. The mapping is as follows:

  | AddDeqRelu usage scenario | Corresponding `SetDeqScale` function prototype                  |
  | :----------------- | :----------------------------------------------- |
  | Input type is int32_t  | `__aicore__ inline void SetDeqScale(half scale)` |

- This API involves the use of temporary space. The calculation formula is as follows: when the amount of data to be computed is less than or equal to 2K (2048), the temporary space used is linearly related to the amount of data; when the amount of data is greater than 2K, the temporary space size is fixed at 8KB:
  $$
  tmpSize =
  \begin{cases}
    count * sizeof(int32\_t), & \text{if } count \le 2K \\
    8K, & \text{if } count \gt 2K
  \end{cases}
  $$
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/composite_compute/AddDeqRelu_res.md#id3 -->

## Key Features

### Overflow Protection

When performing dequantization from the int32_t to the half data type, this API applies overflow protection:

$
Deq(x) = Cast(x \times (1/2^{17}) \times scale \times 2^{17}) = Cast(x \times scale)
$
The intermediate downscaling step keeps the value within a safe range, effectively preventing floating-point overflow.

## Example<a name="section837496171220"></a>

In this example, srcLocal is of the int32\_t type and dstLocal is of the half type. The mask is computed based on int32\_t.

- Tensor high-dimensional splitting computation sample - mask continuous mode.

  ```cpp
  uint64_t mask = 256 / sizeof(int32_t); // 64
  // repeatTime = 4, 64 numbers are computed per iteration, and 256 numbers are computed in total.
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration.
  // dstRepStride = 4, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations.
  half scale = 0.1;
  AscendC::SetDeqScale(scale);
  AscendC::AddDeqRelu(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 4, 8, 8 });
  ```

- Tensor high-dimensional splitting computation sample - mask bit-by-bit mode.

  ```cpp
  uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
  // repeatTime = 4, 64 numbers are computed per iteration, and 256 numbers are computed in total.
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration.
  // dstRepStride = 4, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations.
  half scale = 0.1;
  AscendC::SetDeqScale(scale);
  AscendC::AddDeqRelu(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 4, 8, 8 });
  ```

- Tensor first-n-data computation sample.

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
