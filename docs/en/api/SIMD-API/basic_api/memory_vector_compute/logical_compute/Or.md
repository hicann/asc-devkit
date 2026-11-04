# Or<a name="ZH-CN_TOPIC_0000001379430576"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:10:18.701Z pushedAt=2026-10-28T03:14:39.532Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/logical_compute/Or_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_binary_intf.h"`.

Performs a bitwise OR operation on each pair of elements. The formula is as follows:

$dst_i = src0_i | src1_i$

## Prototype<a name="section620mcpsimp"></a>

- The entire tensor participates in computation.

    ```cpp
    dst = src0 | src1;
    ```

- The first n elements of the tensor participate in computation.

    ```cpp
    template <typename T>
    __aicore__ inline void Or(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
    ```

- The tensor participates in computation through high-dimensional partitioning.
    - **mask** bit-by-bit mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Or(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

    - **mask** continuous mode

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void Or(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|---|---|
| T | Data type of the operand. |
| isSetMask | Whether to set the mask within the API.<br>&bull; true: The mask is set within the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameters is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
|---|---|---|
| dst | Output | Destination operand.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraint](../../../general_description_and_constraints.md). |
| src0, src1 | Input | Source operands.<br>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br>For the address alignment constraint, see [Generic Address Alignment Constraint](../../../general_description_and_constraints.md).<br>The data type of the source operands must be the same as that of the destination operand. |
| count | Input | Number of elements that participate in computation. For details about this parameter, see [Continuous Computation](../SIMD_compute/continuous_compute.md). |
| mask[]/mask | Input | mask controls the elements that participate in computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of contiguous data for computation each time. To process all input data, multiple iterations (repeats) are required to read and compute all data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Partitioning](../SIMD_compute/high_dim_split.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. Type: [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which contains the address stride of the same DataBlock between adjacent iterations of the operands, the address stride of different DataBlocks within the same iteration of the operands, and other parameters.<br>For details about the address stride between adjacent iterations, see [repeatStride](../SIMD_compute/high_dim_split.md). For details about the address stride of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dim_split.md). |

## Data Types

<!-- npu="950" id10 -->
- For Ascend 950PR&950DT products, T supports the following data types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, and uint64_t. The data types int8_t, uint8_t, int64_t, and uint64_t are supported only by the API that computes the first n elements of the tensor.
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

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).
- When the **entire tensor participates in computation** API symbol overload is used, the operation size is the total length of the destination LocalTensor.

<!-- npu="A3,910b,310b,310p,910" id18 -->
- In particular, for the Or operation on uint32_t/int32_t data, it can be implemented through ReinterpretCast: use ReinterpretCast of the Local Tensor to convert the data to uint16_t/int16_t and then call Or for computation. Directly passing uint32_t/int32_t data may behave differently across versions and models. For operator compatibility across versions, this direct-passing approach is not recommended:
    <!-- npu="A3,910b" id19 -->
    - The **entire tensor computation API** and the **first n data computation API** support the `uint32_t` and `int32_t` types. For the **first n data computation API**, `count` is actually calculated based on the b16 data width, regardless of the passed data type. To compute N b32 data elements, set `count` to 2N. The **tensor high-dimensional partitioning computation API** reports a compilation error directly, indicating that this data type is not supported. The preceding description applies to:
      <!-- npu="A3" id20 -->
      - Atlas A3 products.
      <!-- end id20 -->
      <!-- npu="910b" id21 -->
      - Atlas A2 products.
      <!-- end id21 -->
    <!-- end id19 -->
    <!-- npu="310b" id22 -->
    - A CPU alarm indicates that this data type is not supported. The preceding description applies to:
      - Atlas 200I/500 A2 inference products.
    <!-- end id22 -->
    <!-- npu="310p,910" id23 -->
    - A compilation error indicates that this data type is not supported. The preceding description applies to:
      <!-- npu="310p" id24 -->
      - Atlas inference products AI Core.
      <!-- end id24 -->
      <!-- npu="910" id25 -->
      - Atlas training products.
      <!-- end id25 -->
    <!-- end id23 -->
<!-- end id18 -->
<!-- npu="A3,910b,950" id26 -->
- When the count or repeatTime parameter is set to 0, this API behaves as follows:
  <!-- npu="A3,910b" id27 -->
  - For the following models, this API does not perform any computation or write to the destination operand, and is treated as a NOP (no operation).
    <!-- npu="A3" id28 -->
    - Atlas A3 products
    <!-- end id28 -->
    <!-- npu="910b" id29 -->
    - Atlas A2 products
    <!-- end id29 -->
  <!-- end id27 -->
  <!-- npu="950" id30 -->
  - For Ascend 950PR&950DT products, this API is implemented for compatibility by calling the [Reg vector computation API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the count or repeatTime parameter is set to 0, it is not guaranteed that this API is treated as a NOP (no operation).
  <!-- end id30 -->
<!-- end id26 -->

## Example<a name="section642mcpsimp"></a>

- Tensor high-dimensional partitioning computation example - mask continuous mode

    ```cpp
    uint64_t mask = 128;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats.
    AscendC::Or(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
    ```

- tensor high-dimensional partitioning computation example - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats.
    AscendC::Or(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 8, 8, 8 });
    ```

- tensor first n data computation example

    ```cpp
    AscendC::Or(dstLocal, src0Local, src1Local, 512);
    ```

The result is as follows:

```plain
Input data src0Local: [1 2 3 ... 512]
Input data src1Local: [513 512 511 ... 2]
Output data dstLocal: [513 514 511 ... 514]
```
