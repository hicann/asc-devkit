# Max<a name="ZH-CN_TOPIC_0000001379111028"></a>

<!-- md-trans-meta sourceCommit=f266635ab179c912cc9641061c2d27246be9170c translatedAt=2026-11-02T06:36:44.081Z pushedAt=2026-11-18T01:25:07.016Z nl=2 -->

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
<!-- npu="x90" id8 -->
- Kirin X90 processor family products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor family products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Max_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_intf.h"`.

Max is a binary vector computation API that is responsible for computing the element-wise maximum of the two input tensors. The formula is as follows:

$dst_i = max(src0_i, src1_i)$

## Prototype<a name="section620mcpsimp"></a>

- Continuous computation of the first n elements of the tensor

    ```cpp
    template <typename T>
    __aicore__ inline void Max(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
    ```

- Tensor high-dimensional slicing computation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Max(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Max(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Operand data type. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For address alignment constraints, see [Generic Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src0, src1 | Input | Source operands.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraint](../../../general_description_and_constraints.md).<br>The data types of the two source operands must be the same as that of the destination operand. |
| count | Input | Number of elements involved in the calculation. For details about this parameter, see [Continuous Compute](../SIMD_compute/continuous_compute.md). |
| mask[]/mask | Input | mask controls the elements involved in the calculation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector compute unit reads 256 consecutive bytes of data for calculation each time. To process all input data, multiple iterations (repeat) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Split](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. Type: [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which contains the address stride of the same DataBlock between adjacent iterations of the operands, the address stride of different DataBlocks within the same iteration of the operands, and other parameters.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Type

<!-- npu="950" id10 -->
- For Ascend 950PR&950DT products, the data types supported by T are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, and uint64_t. The data types int8_t, uint8_t, int64_t, and uint64_t are supported only by the API for computing the first n data elements of a tensor.
<!-- end id10 -->
<!-- npu="A3" id11 -->
- For Atlas A3 products, the data types supported by T are int16_t, half, int32_t, and float.
<!-- end id11 -->
<!-- npu="910b" id12 -->
- For Atlas A2 products, the data types supported by T are int16_t, half, int32_t, and float.
<!-- end id12 -->
<!-- npu="310b" id13 -->
- For Atlas 200I/500 A2 inference products, the data types supported by T are int16_t, half, int32_t, and float.
<!-- end id13 -->
<!-- npu="310p" id14 -->
- For Atlas inference products AI Core, the supported data types of T are int16_t, half, int32_t, and float.
<!-- end id14 -->
<!-- npu="910" id15 -->
- For Atlas training products, the supported data types of T are half, int32_t, and float.
<!-- end id15 -->
<!-- npu="x90" id16 -->
- For Kirin X90 processor family products, the data types supported by T are int16_t, half, int32_t, and float.
<!-- end id16 -->
<!-- npu="9030" id17 -->
- For Kirin 9030 processor family products, the data types supported by T are int16_t, half, int32_t, and float.
<!-- end id17 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Max_res.md#id2 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

<!-- npu="A3,910b,950" id20 -->
- When the count or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id23 -->
  - For the following models, when the count or repeatTime parameter is set to 0, no computation is performed and no data is written to the destination operand. This API is treated as a NOP (no operation).
    <!-- npu="A3" id24 -->
    - Atlas A3 products
    <!-- end id24 -->
    <!-- npu="910b" id26 -->
    - Atlas A2 products
    <!-- end id26 -->
  <!-- end id23 -->
  <!-- npu="950" id27 -->
  - For Ascend 950PR&950DT products: This API is implemented for compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is 0, the software simulation behavior does not guarantee that this API is treated as a NOP (no operation).
  <!-- end id27 -->
<!-- end id20 -->
<!-- npu="950" id21 -->
- Notes on Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - High-dimensional split computation of the tensor occupies 8 KB of UB.
  - Continuous computation of the first n data elements of the tensor does not involve the 8 KB UB usage.
<!-- end id21 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Max_res.md#id3 -->

## Example<a name="section642mcpsimp"></a>

- Tensor high-dimensional split calculation example - mask continuous mode

    ```cpp
    uint64_t mask = 128;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats.
    AscendC::Max(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
    ```

- Tensor high-dimensional split calculation example - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats.
    AscendC::Max(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
    ```

- example of the API for computing the first n data of a tensor

    ```cpp
    AscendC::Max(dstLocal, src0Local, src1Local, 512);
    ```

The result is as follows:

```bash
Input data src0Local: [1 2 3 ... 512]
Input data src1Local: [513 512 511 ... 2]
Output data dstLocal: [513 512 511 ... 512]
```
