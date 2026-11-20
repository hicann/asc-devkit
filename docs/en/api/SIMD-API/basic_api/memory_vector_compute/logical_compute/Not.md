# Not<a name="ZH-CN_TOPIC_0000001379083606"></a>

<!-- md-trans-meta sourceCommit=f266635ab179c912cc9641061c2d27246be9170c translatedAt=2026-11-02T08:36:42.890Z pushedAt=2026-11-18T01:25:07.146Z nl=2 -->

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
- Kirin X90 processor products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/logical_compute/Not_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_unary_intf.h"`.

Performs bitwise NOT on elements. The calculation formula is as follows:

$dst_i = \sim src_i$

## Prototype<a name="section620mcpsimp"></a>

- Continuous computation of the first n elements of the tensor

    ```cpp
    template <typename T>
    __aicore__ inline void Not(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
    ```

- Tensor high-dimensional slicing computation
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Not(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Not(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|---|---|
| T | Operand data type. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For address alignment constraints, see [Generic Address Alignment Constraints](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraint](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the calculation. For details about this parameter, see [Continuous Compute](../SIMD_compute/continuous_compute.md). |
| mask[]/mask | Input | mask controls the elements involved in the calculation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector compute unit reads 256 consecutive bytes of data for calculation each time. To process all input data, multiple iterations (repeat) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Split](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the address stride of the operand. Type: [UnaryRepeatParams](../../aux_data_structures//UnaryRepeatParams.md), which contains the address stride between the same DataBlock in adjacent iterations and the address stride between different DataBlocks in the same iteration.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride of DataBlock in the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Type

<!-- npu="950" id10 -->
- For Ascend 950PR&950DT products, T supports the following data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, and uint64_t. The data types int8_t, uint8_t, int64_t, and uint64_t are supported only by the API that computes the first n data elements of a tensor.
<!-- end id10 -->
<!-- npu="A3" id11 -->
- For Atlas A3 products, T supports the following data types: int16_t and uint16_t.
<!-- end id11 -->
<!-- npu="910b" id12 -->
- For Atlas A2 products, T supports the following data types: int16_t and uint16_t.
<!-- end id12 -->
<!-- npu="310b" id13 -->
- For Atlas 200I/500 A2 inference products, T supports the following data types: int16_t and uint16_t.
<!-- end id13 -->
<!-- npu="310p" id14 -->
- For Atlas inference products (AI Core), T supports the following data types: int16_t and uint16_t.
<!-- end id14 -->
<!-- npu="910" id15 -->
- For Atlas training products, T supports the following data types: int16_t and uint16_t.
<!-- end id15 -->
<!-- npu="x90" id16 -->
- For Kirin X90 processor products, T supports the following data types: int16_t and uint16_t.
<!-- end id16 -->
<!-- npu="9030" id17 -->
- For Kirin 9030 processor products, T supports the following data types: int16_t and uint16_t.
<!-- end id17 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/logical_compute/Not_res.md#id2 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).
<!-- npu="A3,910b,950" id18 -->
- When the count or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id19 -->
  - For the following models, the API does not perform any calculation and does not write to the destination operand. The API is treated as a NOP (no operation).
    <!-- npu="A3" id20 -->
    - Atlas A3 products
    <!-- end id20 -->
    <!-- npu="910b" id21 -->
    - Atlas A2 products
    <!-- end id21 -->
  <!-- end id19 -->
  <!-- npu="950" id22 -->
  - For Ascend 950PR&950DT products, the API implements compatibility by calling the [Reg vector compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is set to 0, it is not guaranteed that the API is treated as a NOP (no operation).
  <!-- end id22 -->
<!-- end id18 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/logical_compute/Not_res.md#id3 -->

## Example<a name="section642mcpsimp"></a>

- Tensor high-dimensional split calculation example - mask continuous mode

    ```cpp
    uint64_t mask = 256 / sizeof(int16_t);
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Not(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- Tensor high-dimensional split calculation example - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Not(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- example of the API for computing the first n data of a tensor

    ```cpp
    AscendC::Not(dstLocal, srcLocal, 512);
    ```

The result is as follows:

```plain
Input data (srcLocal): [9 -2 8 ... 9 0]
Output data (dstLocal): [-10 1 -9 ... -10 -1]
```
