# VectorPadding\(ISASI\)<a name="ZH-CN_TOPIC_0000001870220489"></a>

<!-- md-trans-meta sourceCommit=b51f731d97b8137e47d31515a918da577b1142d8 translatedAt=2026-11-02T08:29:10.964Z pushedAt=2026-11-18T01:25:07.133Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_padding/VectorPadding_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Performs a padding operation on the source operand by DataBlock according to padMode (pad mode) and padSide (pad side).

Assume that a DataBlock of the source operand has 16 numbers, DataBlock\[0:15\]=a\~p:

- padSide==false: Padding starts from the left side of the DataBlock, that is, the direction of the DataBlock start value \(a-\>p\).
- padSide==true: Padding starts from the right side of the DataBlock, that is, the direction of the DataBlock end value \(p-\>a\).
- padMode==0: Uses adjacent numbers as the padding value, for example: aaa|abc\(padSide=false\), nop|ppp\(padSide=true\).
- padMode==1: Uses adjacent DataBlock values for symmetric padding, for example: cba|abc\(padSide=false\), nop|pon\(padSide=true\).
- padMode==2: Uses adjacent DataBlock values for padding, offset by one number, performing symmetric padding. For example:
  - padSide=false: xcb|abc, xcb is padded. Padding process: a is discarded, symmetric padding is performed, and 0 is padded at x.
  - padSide=true: nop|onx, onx is padded. Padding process: p is discarded, symmetric padding is performed, and 0 is padded at x.

## Prototype<a name="section620mcpsimp"></a>

- Tensor first n elements computation

    ```cpp
    template <typename T>
    __aicore__ inline void VectorPadding(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint8_t padMode, const bool padSide, const uint32_t count)
    ```

- Tensor high-dimensional slicing computation
  - **mask** bit-by-bit mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void VectorPadding(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint8_t padMode, const bool padSide, const uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
    ```

  - **mask** continuous mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void VectorPadding(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint8_t padMode, const bool padSide, const uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ------ | ------ |
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** API parameter description

| Parameter | Input/Output | Meaning |
| ------ | ------ | ------ |
| dst | Output | Destination operand, of type LocalTensor, with supported TPosition values VECIN/VECCALC/VECOUT. The start address of the LocalTensor must be 32-byte aligned. |
| src | Input | Source operand, whose type is LocalTensor. The supported TPosition values are VECIN/VECCALC/VECOUT. The start address of the LocalTensor must be 32-byte aligned. The data type of the source operand must be the same as that of the destination operand. |
| padMode | Input | Padding mode, whose type is uint8_t and value range is [0,2].<br>&bull; 0: Use adjacent values as the padding values.<br>&bull; 1: Use adjacent DataBlock values for symmetric padding.<br>&bull; 2: Use adjacent DataBlock values, offset by one value, for symmetric padding. |
| padSide | Input | Padding direction, whose type is bool.<br>&bull; false: Left.<br>&bull; true: Right. |
| count | Input | Number of elements involved in the computation. |
| mask[]/mask | Input | mask controls the elements involved in the computation in each iteration. For details about the settings, see [Mask Operations](../mask_operations/mask_operations.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. |
| repeatParams | Input | Parameters that control the address stride of the operand, whose type is [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md). It includes parameters such as the address stride of the same DataBlock between adjacent iterations and the address stride of different DataBlocks within the same iteration. For details about the parameters, see [High-Dimensional Splitting API](../../../../../guide/programming_guide/library_api/basic_api/interface_classification/high_dim_split_api.md). |

<!-- npu="310p" id8 -->
## Data Types

- For the Atlas inference products AI Core, the supported data types for T are: int16_t, uint16_t, half, int32_t, uint32_t, and float.
<!-- end id8 -->

## Return Value<a name="section38228281712"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- mask controls only which elements in the destination operand are written. The read of the source operand is independent of mask.
- count indicates the total number of elements written to the destination operand. The read of the source operand is independent of count.
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_padding/VectorPadding_ISASI_res.md#id2 -->

## Example<a name="section642mcpsimp"></a>

The srcLocal and dstLocal in the sample are both of the half type.

For more samples, see [LINK](../basic_arithmetic/more_examples.md).

- Tensor high-dimensional split calculation example - mask continuous mode

    ```cpp
    uint64_t mask = 256 / sizeof(half);
    uint8_t padMode = 0;
    bool padSide = false;
    // repeatTime = 4, 128 elements one repeat, 512 elements total
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    AscendC::VectorPadding(dstLocal, srcLocal, padMode, padSide, mask, 4, { 1, 1, 8, 8 });
    ```

- Tensor high-dimensional split calculation example - mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    uint8_t padMode = 0;
    bool padSide = false;
    // repeatTime = 4, 128 elements one repeat, 512 elements total
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    AscendC::VectorPadding(dstLocal, srcLocal, padMode, padSide, mask, 4, { 1, 1, 8, 8 });
    ```

- Example of computing the first n tensor data

    ```cpp
    uint8_t padMode = 0;
    bool padSide = false;
    AscendC::VectorPadding(dstLocal, srcLocal, padMode, padSide, 512);
    ```

The result is as follows:

```text
// Take the value of one DataBlock in srcLocal as an example, which has 16 numbers.
// Input data (srcLocal): [6.938 -8.86 -0.2263 ... 1.971 1.778]
// Output data (dstLocal):
[6.938 6.938 6.938 ... 6.938 6.938]
```
