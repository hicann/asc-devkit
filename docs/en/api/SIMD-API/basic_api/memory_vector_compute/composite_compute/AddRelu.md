# AddRelu<a name="ZH-CN_TOPIC_0000001723846124"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:05:07.429Z pushedAt=2026-10-28T03:14:39.452Z nl=2 -->

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
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/composite_compute/AddRelu_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_intf.h"`.

Performs element-wise summation, and then performs ReLU computation (compares the result with 0 and takes the larger value). The formula is as follows:

$$
dst_i=Relu(src0_i+src1_i)
$$

The formula of Relu is as follows:

$$
Relu(x) = max(0, x)
$$

## Prototype<a name="section620mcpsimp"></a>

- Tensor First n Elements Computation

  ```cpp
  template <typename T>
  __aicore__ inline void AddRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
  ```

- Tensor High-Dimensional Split Computation
  - **mask** bit-by-bit mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void AddRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

  - **mask** continuous mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void AddRelu(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operands.<br>The type is LocalTensor, with supported TPosition values of VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| count | Input | Number of elements involved in the computation.<br>**Note: The value range of this parameter depends on the data type of the operands. Different data types support different maximum numbers of elements, and the maximum amount of data to be processed cannot exceed the UB size limit.** |
| mask[]/mask | Input | mask controls the elements involved in the computation in each iteration.<br>For details, see [Mask Operations](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations.<br>The vector compute unit reads 256 bytes of consecutive data for computation each time. To process the input data, multiple iterations (repeat) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Splitting API](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameter that controls the address stride of the operands. The type is [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which contains parameters such as the address stride of the same datablock between adjacent iterations of the operands and the address stride of different datablocks within the same iteration of the operands.<br>For the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Types

<!-- npu="950" id8 -->
Ascend 950PR&950DT products support data types int16_t, half, float, int64_t, and uint64_t. Among them, the int64\_t and uint64\_t data types support only the API for computing the first n data of a tensor.
<!-- end id8 -->

<!-- npu="A3" id9 -->
Atlas A3 products support data types int16_t, half, and float.
<!-- end id9 -->

<!-- npu="910b" id10 -->
Atlas A2 products support data types int16_t, half, and float.
<!-- end id10 -->

<!-- npu="310b" id11 -->
Atlas 200I/500 A2 inference products support data types half and float.
<!-- end id11 -->

<!-- npu="310p" id12 -->
Atlas inference products AI Core supports data types int16_t, half, and float.
<!-- end id12 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).
<!-- npu="A3,910b,950" id13 -->
- When the count or repeatTime parameter is 0, the API behaves as follows:
  <!-- npu="A3,910b" id14 -->
  - For the following models, the API does not perform computation or write to the destination operand, and is treated as a NOP (no operation).
    <!-- npu="A3" id15 -->
    - Atlas A3 products
    <!-- end id15 -->
    <!-- npu="910b" id16 -->
    - Atlas A2 products
    <!-- end id16 -->
  <!-- end id14 -->
  <!-- npu="950" id17 -->
  - For Ascend 950PR&950DT products, the API achieves compatibility by calling the [Reg vector compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is 0, the API is not guaranteed to be treated as a NOP (no operation).
  <!-- end id17 -->
<!-- end id13 -->
<!-- npu="950" id18 -->
- Description of Unified Buffer (UB) space usage. For the Ascend 950PR&950DT products:
  - The tensor high-dimensional splitting compute API occupies 8 KB of UB temporary space.
  - The API for continuous computation of the first n tensor data does not involve the occupation of the 8 KB UB temporary space.
<!-- end id18 -->

## Example<a name="section642mcpsimp"></a>

For the complete calling example, see [Compound Compute Example](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/element_wise_compound_compute).

- Tensor high-dimensional splitting compute example - mask continuous mode.

  ```cpp
  uint64_t mask = 128;
  // repeatTime = 4, 128 numbers are computed per iteration, and 512 numbers are computed in total.
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration.
  // dstRepStride, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations.
  AscendC::AddRelu(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
  ```

- Tensor high-dimensional splitting compute example - mask bit-by-bit mode.

  ```cpp
  uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
  // repeatTime = 4, 128 numbers are computed per iteration, and 512 numbers are computed in total.
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration.
  // dstRepStride, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations.
  AscendC::AddRelu(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
  ```

- Example of computing the first n data elements of a tensor.

  ```cpp
  AscendC::AddRelu(dstLocal, src0Local, src1Local, 512);
  ```

The result is as follows:

```plain
Input data src0Local: [1 -2 3 ... -6]
Input data src1Local: [1 3 -4 ... 5]
Output data dstLocal: [2 1 0 ... 0]
```
