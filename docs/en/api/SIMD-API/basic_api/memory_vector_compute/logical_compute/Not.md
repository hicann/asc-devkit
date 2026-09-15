# Not<a name="ZH-CN_TOPIC_0000001379083606"></a>

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T12:35:41.699Z -->

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
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->
## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_unary_intf.h"`

Performs bitwise NOT on elements. The calculation formula is as follows:

$dst_i = \sim src_i$

## Prototype<a name="section620mcpsimp"></a>

- The first n data of the tensor are calculated consecutively

    ```cpp
    template <typename T>
    __aicore__ inline void Not(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count)
    ```

- The tensor is calculated by high-dimensional partitioning
    - Bitwise mask mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Not(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - Continuous mask mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Not(const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraint](../../../general_description_and_constraints.md). |
| src | Input | Source operand.<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [General Address Alignment Constraint](../../../general_description_and_constraints.md). |
| count | Input | Number of elements involved in the computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_computation.md). |
| mask[]/mask | Input | **mask** controls the elements that participate in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeat) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-dimensional Partitioning](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

<!-- npu="950" id10 -->
- For Ascend 950PR/Ascend 950DT, T supports the following data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, and uint64_t. The data types int8_t, uint8_t, int64_t, and uint64_t are supported only by the APIs for computing the first n data of a tensor.
<!-- end id10 -->
<!-- npu="A3" id11 -->
- For Atlas A3 training products/Atlas A3 inference products, T supports the following data types: int16_t and uint16_t.
<!-- end id11 -->
<!-- npu="910b" id12 -->
- For Atlas A2 training products/Atlas A2 inference products, T supports the following data types: int16_t and uint16_t.
<!-- end id12 -->
<!-- npu="310b" id13 -->
- For Atlas 200I/500 A2 inference products, T supports the following data types: int16_t and uint16_t.
<!-- end id13 -->
<!-- npu="310p" id14 -->
- For Atlas inference products AI Core, T supports the following data types: int16_t and uint16_t.
<!-- end id14 -->
<!-- npu="910" id15 -->
- For Atlas training products, T supports the following data types: int16_t and uint16_t.
<!-- end id15 -->
## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

<!-- npu="A3,910b" id18 -->
- For the following models, when the value of the count or repeatTime parameter is 0, no computation is performed, no write is made to the destination operand, and the API is treated as a NOP (no operation).
  <!-- npu="A3" id19 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id19 -->
  <!-- npu="910b" id20 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id20 -->
<!-- end id18 -->

## Example<a name="section642mcpsimp"></a>

- Tensor high-dimensional partitioning calculation example - continuous mask mode

    ```cpp
    uint64_t mask = 256 / sizeof(int16_t);
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Not(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- Tensor high-dimensional partitioning calculation example - bitwise mask mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::Not(dstLocal, srcLocal, mask, 4, { 1, 1, 8, 8 });
    ```

- Example of the API for calculating the first n data elements of a tensor

    ```cpp
    AscendC::Not(dstLocal, srcLocal, 512);
    ```

The result is as follows:

```bash
Input data (srcLocal): [9 -2 8 ... 9 0]
Output data (dstLocal): [-10 1 -9 ... -10 -1]
```
