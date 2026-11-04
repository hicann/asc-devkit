# Add<a name="ZH-CN_TOPIC_0000001429830437"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:04:45.601Z pushedAt=2026-10-28T03:14:39.356Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Add_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_intf.h"`.

Add is a binary vector computation API that is responsible for computing the element-wise sum of the two input tensors. The formula is as follows:

$dst_i = src0_i + src1_i$

## Prototype<a name="section620mcpsimp"></a>

- The entire tensor participates in computation

    ```cpp
    dst = src0 + src1;
    ```

- The first n data elements of the tensor are computed continuously

    ```cpp
    template <typename T>
    __aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
    ```

- Tensor High-Dimensional Split Computation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use [SetVectorMask](../mask_operations/SetVectorMask.md) to set the mask value. In this mode, the mask value in the API input parameters is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [generic address alignment constraint](../../../general_description_and_constraints.md). |
| src0, src1 | Input | Source operands.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [generic address alignment constraint](../../../general_description_and_constraints.md). |
| count | Input | Number of elements that participate in computation. For details about this parameter, see [continuous computation](../SIMD_compute/continuous_compute.md). |
| mask[]/mask | Input | mask controls the elements that participate in computation in each iteration. For details, see [mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To process all input data, multiple iterations (repeats) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [high-dimensional splitting](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. Type: [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which contains the address stride of the same DataBlock between adjacent iterations of the operands, the address stride of different DataBlocks within the same iteration of the operands, and other parameters.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Types

<!-- npu="950" id10 -->
- For Ascend 950PR&950DT products, the supported data types for T are int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, and complex64. The data types int8_t, uint8_t, complex32, int64_t, uint64_t, and complex64 only support the API for computing the first n data of a tensor and the operator overloads where the entire tensor participates in computation.
<!-- end id10 -->
<!-- npu="A3" id11 -->
- For Atlas A3 products, the supported data types for T are int16_t, half, int32_t, and float.
<!-- end id11 -->
<!-- npu="910b" id12 -->
- For Atlas A2 products, the supported data types for T are int16_t, half, int32_t, and float.
<!-- end id12 -->
<!-- npu="310b" id13 -->
- For Atlas 200I/500 A2 inference products, the supported data types for T are int16_t, half, int32_t, and float.
<!-- end id13 -->
<!-- npu="310p" id14 -->
- For Atlas inference products AI Core, the supported data types for T are int16_t, half, int32_t, and float.
<!-- end id14 -->
<!-- npu="910" id15 -->
- For Atlas training products, the supported data types for T are half, int32_t, and float.
<!-- end id15 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).
- When using the operator overloads where the entire tensor participates in computation, the computation amount is the total length of the destination LocalTensor.

<!-- npu="A3,910b,950" id20 -->
- When the value of count or repeatTime is 0, the API behaves as follows:
  <!-- npu="A3,910b" id23 -->
  - For the following models, when the value of the count or repeatTime parameter is 0, no computation is performed and no write is made to the destination operand. The API is treated as a NOP (no operation).
    <!-- npu="A3" id24 -->
    - Atlas A3 products
    <!-- end id24 -->
    <!-- npu="910b" id26 -->
    - Atlas A2 products
    <!-- end id26 -->
  <!-- end id23 -->
  <!-- npu="950" id27 -->
  - For Ascend 950PR&950DT products: This API is implemented for compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the value of the count or repeatTime parameter is 0, the simulation behavior does not guarantee that the API is treated as a NOP (no operation).
  <!-- end id27 -->
<!-- end id20 -->
<!-- npu="950" id21 -->
- Notes on Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - tensor high-dimensional splitting computation occupies 8 KB of UB.
  - Continuous computation of the first n data elements of the tensor does not involve the 8 KB UB usage.
<!-- end id21 -->

## Example<a name="section642mcpsimp"></a>

- Example of tensor high-dimensional splitting computation - mask continuous mode

    ```cpp
    uint64_t mask = 128;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats.
    AscendC::Add(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
    ```

- Example of tensor high-dimensional splitting computation - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats.
    AscendC::Add(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
    ```

- Example of the API for computing the first n data elements of the tensor

    ```cpp
    AscendC::Add(dstLocal, src0Local, src1Local, 512);
    ```

- Example of the entire tensor participating in computation

    ```cpp
    dstLocal = src0Local + src1Local;
    ```

The result is as follows:

```bash
Input data src0Local: [1 2 3 ... 512]
Input data src1Local: [513 514 515 ... 1024]
Output data dstLocal: [514 516 518 ... 1536]
```
