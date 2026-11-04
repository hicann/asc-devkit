# SetVectorMask<a name="ZH-CN_TOPIC_0000001839731333"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:19:47.623Z pushedAt=2026-10-28T03:14:39.548Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/mask_operations/SetVectorMask_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_common.h"`.

This API is used to set the mask during vector computation.

- When the template parameter isSetMask = false, this API is recommended. Before using it, call [SetMaskCount](SetMaskCount.md)/[SetMaskNorm](SetMaskNorm.md) to manually set the Mask mode, and then call this API to set the mask in Counter/Normal mode. In different modes, the mask has different meanings:

    - In Normal mode, the mask parameter controls the number of elements that participate in calculation within a single iteration. In this mode, it can be further divided into the following two modes:
        - Continuous mode: indicates how many consecutive elements at the front participate in calculation. The value range is related to the data type of the operand. Different data types have different maximum numbers of elements that can be processed in each iteration. When the operand is 16-bit, mask∈\[0, 128\]; when the operand is 32-bit, mask∈\[0, 64\]; when the operand is 64-bit, mask∈\[0, 32\].

        - Bitwise mode: can be bitwise control which elements participate in calculation. A bit value of 1 indicates participation in calculation, and 0 indicates no participation. It is divided into maskHigh (high mask) and maskLow (low mask). The parameter value range is related to the data type of the operand. Different data types have different maximum numbers of elements that can be processed in each iteration. When the operand is 16-bit, maskLow, maskHigh∈\[0, 2<sup>64</sup>-1\], and they are not 0 at the same time; when the operand is 32-bit, maskHigh is 0, maskLow∈\[0, 2<sup>64</sup>-1\]; when the operand is 64-bit, maskHigh is 0, maskLow∈\[0, 2<sup>32</sup>-1\].

    - In Counter mode, the mask parameter indicates the number of elements that participate in calculation in the entire vector calculation.

- When the template parameter isSetMask = true, you do not need to call this API. See [Setting Mask in the API](../SIMD_compute/mask.md#setting-the-mask-inside-the-api).

## Prototype<a name="section620mcpsimp"></a>

- Applies to the bitwise mode of mask in Normal mode and Counter mode

    ```cpp
    template <typename T, MaskMode mode = MaskMode::NORMAL>
    __aicore__ static inline void SetVectorMask(const uint64_t maskHigh, const uint64_t maskLow)
    ```

- Applies to the continuous mode of Mask in Normal mode and Counter mode

    ```cpp
    template <typename T, MaskMode mode = MaskMode::NORMAL>
    __aicore__ static inline void SetVectorMask(int32_t len)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the vector calculation operand. |
| mode | Mask mode, of the MaskMode type, defined as follows:<br>enum class MaskMode : uint8_t {<br>    NORMAL = 0,  // Normal mode<br>    COUNTER      // Counter mode<br>};|

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| maskHigh | Input | Normal mode: Corresponds to the bitwise mode in Normal mode, which can be bitwise controlled to determine which elements participate in calculation. Pass the high-order mask value.<br>Counter mode: Set to 0. This input parameter does not take effect. |
| maskLow | Input | Normal mode: Corresponds to the bitwise mode in Normal mode, which can be bitwise controlled to determine which elements participate in calculation. Pass the low-order mask value.<br>Counter mode: Number of elements that participate in calculation during the entire vector calculation process. |
| len | Input | Normal mode: Corresponds to the Mask continuous mode in Normal mode, indicating the number of consecutive elements at the beginning that participate in calculation within a single iteration.<br>Counter mode: Number of elements that participate in calculation during the entire vector calculation process. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- This API takes effect only when the isSetMask template parameter of the vector calculation API is false. After use, call [ResetMask](ResetMask.md) to restore the mask to the default value.

<!-- npu="910b,A3" id10 -->
- For Atlas A3 products and Atlas A2 products, mask = 0 means that the instruction does not perform calculation, and this API is treated as a NOP (no operation).
<!-- end id10 -->

## Example<a name="section837496171220"></a>

It can be used together with [SetMaskCount](SetMaskCount.md) and [SetMaskNorm](SetMaskNorm.md). Set the Mask mode first and then set the mask:

- Normal mode call example

    ```cpp
    AscendC::LocalTensor<half> dstLocal;
    AscendC::LocalTensor<half> src0Local;
    AscendC::LocalTensor<half> src1Local;
    
    // Normal mode
    AscendC::SetMaskNorm();
    AscendC::SetVectorMask<half, AscendC::MaskMode::NORMAL>(0xffffffffffffffff, 0xffffffffffffffff);  // Bitwise mode
    
    // SetVectorMask<half, MaskMode::NORMAL>(128);  // Continuous mode
    // When calling the vector calculation API multiple times, you can uniformly set it to Normal mode and set the mask parameter, without repeatedly setting it inside the API. This saves the process of repeatedly setting it in the API and provides certain performance benefits.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration.
    // dstRepStride, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations.
    AscendC::Add<half, false>(dstLocal, src0Local, src1Local, AscendC::MASK_PLACEHOLDER, 1, { 1, 1, 1, 8, 8, 8 });
    AscendC::Sub<half, false>(src0Local, dstLocal, src1Local, AscendC::MASK_PLACEHOLDER, 1, { 1, 1, 1, 8, 8, 8 });
    AscendC::Mul<half, false>(src1Local, dstLocal, src0Local, AscendC::MASK_PLACEHOLDER, 1, { 1, 1, 1, 8, 8, 8 });
    AscendC::ResetMask();
    ```

- Counter mode examples

    ```cpp
    // Used together with the tensor high-dimensional split computation API in Counter mode.
    AscendC::LocalTensor<half> dstLocal;
    AscendC::LocalTensor<half> src0Local;
    AscendC::LocalTensor<half> src1Local;
    int32_t len = 128;  // Number of elements participating in the calculation.
    AscendC::SetMaskCount();
    AscendC::SetVectorMask<half, AscendC::MaskMode::COUNTER>(len);
    AscendC::Add<half, false>(dstLocal, src0Local, src1Local, AscendC::MASK_PLACEHOLDER, 1, { 1, 1, 1, 8, 8, 8 });
    AscendC::Sub<half, false>(src0Local, dstLocal, src1Local, AscendC::MASK_PLACEHOLDER, 1, { 1, 1, 1, 8, 8, 8 });
    AscendC::Mul<half, false>(src1Local, dstLocal, src0Local, AscendC::MASK_PLACEHOLDER, 1, { 1, 1, 1, 8, 8, 8 });
    AscendC::SetMaskNorm();
    AscendC::ResetMask();
    
    // Used together with the API for computing the first n data of a tensor in Counter mode.
    AscendC::LocalTensor<half> dstLocal;
    AscendC::LocalTensor<half> src0Local;
    half num = 2; 
    AscendC::SetMaskCount();
    AscendC::SetVectorMask<half, AscendC::MaskMode::COUNTER>(128); // Number of elements participating in the calculation is 128.
    AscendC::Adds<half, false>(dstLocal, src0Local, num, 1);
    AscendC::Muls<half, false>(dstLocal, src0Local, num, 1);
    AscendC::SetMaskNorm();
    AscendC::ResetMask();
    ```

For more examples, see [Setting Mask Outside the API](../SIMD_compute/mask.md#setting-the-mask-outside-the-api).
