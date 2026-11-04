# Mul<a name="ZH-CN_TOPIC_0000001378951492"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:30:54.640Z pushedAt=2026-10-28T03:14:39.413Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Mul_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_intf.h"`.

Mul is a binary vector computation API that is responsible for computing the element-wise product of the two input tensors. The computation formula is as follows:

$dst_i = src0_i \times src1_i$

## Prototype<a name="section620mcpsimp"></a>

- The entire tensor participates in computation

    ```cpp
    dst = src0 * src1;
    ```

- The first n data elements of the tensor are computed continuously

    ```cpp
    template <typename T>
    __aicore__ inline void Mul(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
    ```

- Tensor high-dimensional split computation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Mul(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Mul(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameters is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src0, src1 | Input | Source operands.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraints](../../../general_description_and_constraints.md). |
| count | Input | Number of elements that participate in computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_compute.md). |
| mask[]/mask | Input | mask controls the elements that participate in computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of contiguous data for computation each time. To complete the processing of the input data, multiple iterations (repeats) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Split](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameter that controls the address stride of the operands. Type: [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which contains parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Types

<!-- npu="950" id10 -->
- For Ascend 950PR&950DT products, the supported data types of T are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, and complex64. The data types complex32, int64_t, uint64_t, and complex64 support only the API for computing the first n data of a tensor and the operator overloads where the entire tensor participates in computation.
<!-- end id10 -->
<!-- npu="A3" id11 -->
- For Atlas A3 products, the supported data types of T are: int16_t, half, int32_t, and float.
<!-- end id11 -->
<!-- npu="910b" id12 -->
- For Atlas A2 products, the supported data types of T are: int16_t, half, int32_t, and float.
<!-- end id12 -->
<!-- npu="310b" id13 -->
- For Atlas 200I/500 A2 inference products, the supported data types of T are: int16_t, half, int32_t, and float.
<!-- end id13 -->
<!-- npu="310p" id14 -->
- For Atlas inference products AI Core, the supported data types of T are: int16_t, half, int32_t, and float.
<!-- end id14 -->
<!-- npu="910" id15 -->
- For Atlas training products, the supported data types of T are: half, int32_t, and float.
<!-- end id15 -->

## Return Value<a name="section135861771190"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).
- When the entire tensor participates in computation and the API symbol overload is used, the computation amount is the total length of the destination LocalTensor.

<!-- npu="A3,910b,950" id20 -->
- When the value of the count or repeatTime parameter is 0, the behavior of this API is as follows:
  <!-- npu="A3,910b" id23 -->
  - For the following models, when the value of the count or repeatTime parameter is 0, no computation is performed and no write is made to the destination operand. This API is treated as a NOP (no operation).
    <!-- npu="A3" id24 -->
    - Atlas A3 products
    <!-- end id24 -->
    <!-- npu="910b" id26 -->
    - Atlas A2 products
    <!-- end id26 -->
  <!-- end id23 -->
  <!-- npu="950" id27 -->
  - For Ascend 950PR&950DT products: This API is implemented for compatibility by calling the [Reg vector compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the value of the count or repeatTime parameter is 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id27 -->
<!-- end id20 -->
<!-- npu="950" id21 -->
- Notes on Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - Tensor high-dimensional splitting computation occupies 8 KB of UB.
  - Continuous computation of the first n tensor data does not involve the occupation of 8 KB of UB.
<!-- end id21 -->

## Example<a name="section642mcpsimp"></a>

- Tensor high-dimensional splitting computation example - mask continuous mode

    ```cpp
    uint64_t mask = 128;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats.
    AscendC::Mul(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
    ```

- Example of tensor high-dimensional splitting computation in mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats.
    AscendC::Mul(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
    ```

- Example of the API for computing the first n data elements of a tensor

    ```cpp
    AscendC::Mul(dstLocal, src0Local, src1Local, 512);
    ```

- Example of the entire tensor participating in computation

    ```cpp
    dstLocal = src0Local * src1Local;
    ```

The result is as follows:

```bash
Input data src0Local: [1 2 3 ... 512]
Input data src1Local: [2 2 2 ... 2]
Output data dstLocal: [2 4 6... 1024]
```
