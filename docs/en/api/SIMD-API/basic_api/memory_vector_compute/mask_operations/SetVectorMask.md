# SetVectorMask<a name="ZH-CN_TOPIC_0000001839731333"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-09-09T14:46:05.581Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id2 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id2 -->
<!-- npu="A3" id3 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id3 -->
<!-- npu="910b" id4 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products AI Core: Supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products Vector Core: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Supported
<!-- end id8 -->


## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_common.h"`.

This API is used to set the mask during vector computation.

- When the template parameter isSetMask is false, it is recommended to use this API. Before using it, call [SetMaskCount](SetMaskCount.md)/[SetMaskNorm](SetMaskNorm.md) to manually set the mask mode, and then call this API to set the mask in Counter/Normal mode. The meaning of the mask varies with the mode:

    - In Normal mode, the mask parameter controls the number of elements involved in computation within a single iteration. In this mode, the following two sub-modes are available:
        - Continuous mode: indicates how many consecutive elements at the front are involved in computation. The value range depends on the data type of the operand. Different data types have different maximum numbers of elements that can be processed in each iteration. When the operand is 16-bit, mask∈\[0, 128\]; when the operand is 32-bit, mask∈\[0, 64\]; when the operand is 64-bit, mask∈\[0, 32\].

        - Bitwise mode: controls which elements are involved in computation on a bit-by-bit basis. A bit value of 1 indicates that the element is involved in computation, and 0 indicates that it is not. It is divided into maskHigh (high mask) and maskLow (low mask). The value range of the parameter depends on the data type of the operand. Different data types have different maximum numbers of elements that can be processed in each iteration. When the operand is 16-bit, maskLow, maskHigh∈\[0, 2<sup>64</sup>-1\], and they cannot be 0 at the same time; when the operand is 32-bit, maskHigh is 0, maskLow∈\[0, 2<sup>64</sup>-1\]; when the operand is 64-bit, maskHigh is 0, maskLow∈\[0, 2<sup>32</sup>-1\].

    - In Counter mode, the mask parameter indicates the number of elements of the entire vector involved in computation.

- When the template parameter isSetMask is set to true, you do not need to call this API. For details, see [Setting Mask Inside the API](../SIMD_compute/mask.md).

## Prototype<a name="section620mcpsimp"></a>

- Applies to the bitwise mode of the Normal mode and the Counter mode.

    ```cpp
    template <typename T, MaskMode mode = MaskMode::NORMAL>
    __aicore__ static inline void SetVectorMask(const uint64_t maskHigh, const uint64_t maskLow)
    ```

- Applies to the Mask continuous mode of the Normal mode and the Counter mode.

    ```cpp
    template <typename T, MaskMode mode = MaskMode::NORMAL>
    __aicore__ static inline void SetVectorMask(int32_t len)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the vector computation operands. |
| mode | Mask mode, of the MaskMode type, defined as follows:<br>enum class MaskMode : uint8_t {<br>    NORMAL = 0,  // Normal mode<br>    COUNTER      // Counter mode<br>};<br>|

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| maskHigh | Input | Normal mode: corresponds to the bitwise mode of the Normal mode, which controls the elements participating in the computation on a bit-by-bit basis. Passes the high-order mask value.<br>Counter mode: must be set to 0. This input parameter does not take effect. |
| maskLow | Input | Normal mode: corresponds to the bitwise mode of the Normal mode, which controls the elements participating in the computation on a bit-by-bit basis. Passes the low-order mask value.<br>Counter mode: number of elements participating in the computation during the entire vector computation. |
| len | Input | Normal mode: corresponds to the Mask continuous mode of the Normal mode, indicating the number of consecutive leading elements participating in the computation in a single iteration.<br>Counter mode: number of elements participating in the computation during the entire vector computation. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- This API takes effect only when the isSetMask template parameter of the vector computation API is set to false. After use, call [ResetMask](ResetMask.md) to restore the mask to its default value.

<!-- npu="910b,A3" id1 -->
- For Atlas A3 training series products/Atlas A3 inference series products and Atlas A2 training series products/Atlas A2 inference series products, mask = 0 indicates that the instruction does not perform the computation operation, and this API is treated as a NOP (no operation).
<!-- end id1 -->

## Calling Examples<a name="section837496171220"></a>

It can be used together with [SetMaskCount](SetMaskCount.md) and [SetMaskNorm](SetMaskNorm.md). Set the mask mode first and then set the mask:

- Normal mode calling example

    ```cpp
    AscendC::LocalTensor<half> dstLocal;
    AscendC::LocalTensor<half> src0Local;
    AscendC::LocalTensor<half> src1Local;
    
    // Normal mode
    AscendC::SetMaskNorm();
    AscendC::SetVectorMask<half, AscendC::MaskMode::NORMAL>(0xffffffffffffffff, 0xffffffffffffffff);  // Bitwise mode
    
    // SetVectorMask<half, MaskMode::NORMAL>(128);  // Continuous mode
    // When the vector computation API is called multiple times, the Normal mode can be set uniformly and the mask parameter can be set, eliminating the need to set them repeatedly inside the API. This avoids the repeated setting process and provides certain performance benefits.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration
    // dstRepStride, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations
    AscendC::Add<half, false>(dstLocal, src0Local, src1Local, AscendC::MASK_PLACEHOLDER, 1, { 2, 2, 2, 8, 8, 8 });
    AscendC::Sub<half, false>(src0Local, dstLocal, src1Local, AscendC::MASK_PLACEHOLDER, 1, { 2, 2, 2, 8, 8, 8 });
    AscendC::Mul<half, false>(src1Local, dstLocal, src0Local, AscendC::MASK_PLACEHOLDER, 1, { 2, 2, 2, 8, 8, 8 });
    AscendC::ResetMask();
    ```

- Counter mode calling example

    ```cpp
    // Used with the Counter mode and the tensor high-dimensional split computation API.
    AscendC::LocalTensor<half> dstLocal;
    AscendC::LocalTensor<half> src0Local;
    AscendC::LocalTensor<half> src1Local;
    int32_t len = 128;  // Number of elements involved in the computation.
    AscendC::SetMaskCount();
    AscendC::SetVectorMask<half, AscendC::MaskMode::COUNTER>(len);
    AscendC::Add<half, false>(dstLocal, src0Local, src1Local, AscendC::MASK_PLACEHOLDER, 1, { 1, 1, 1, 8, 8, 8 });
    AscendC::Sub<half, false>(src0Local, dstLocal, src1Local, AscendC::MASK_PLACEHOLDER, 1, { 1, 1, 1, 8, 8, 8 });
    AscendC::Mul<half, false>(src1Local, dstLocal, src0Local, AscendC::MASK_PLACEHOLDER, 1, { 1, 1, 1, 8, 8, 8 });
    AscendC::SetMaskNorm();
    AscendC::ResetMask();
    
    // Used with the Counter mode and the tensor first-n data computation API.
    AscendC::LocalTensor<half> dstLocal;
    AscendC::LocalTensor<half> src0Local;
    half num = 2; 
    AscendC::SetMaskCount();
    AscendC::SetVectorMask<half, AscendC::MaskMode::COUNTER>(128); // Number of elements involved in the computation is 128.
    AscendC::Adds<half, false>(dstLocal, src0Local, num, 1);
    AscendC::Muls<half, false>(dstLocal, src0Local, num, 1);
    AscendC::SetMaskNorm();
    AscendC::ResetMask();
    ```

For more examples, see [Setting Mask Outside the API](../SIMD_compute/mask.md).
