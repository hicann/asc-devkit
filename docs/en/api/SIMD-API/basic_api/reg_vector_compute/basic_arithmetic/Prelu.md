# Prelu<a name="ZH-CN_TOPIC_0000002043043760"></a>

<!-- md-trans-meta sourceCommit=00f2fa768749ee1a0d8f4a453d77d0c75cf3b952 translatedAt=2026-11-05T02:36:47.263Z pushedAt=2026-11-18T01:25:30.829Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
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
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- npu="9050" id8 -->
- Kirin 9050 processor family products: Supported
<!-- end id8 -->
<!-- npu="kirin1" id9 -->
- KirinDev0001 processor family products: Supported
<!-- end id9 -->
<!-- npu="kirin2" id10 -->
- KirinDev0002 processor family products: Supported
<!-- end id10 -->
<!-- npu="kirin3" id11 -->
- KirinDev0003 processor family products: Supported
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/basic_arithmetic/Prelu_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"`.

This API implements the PReLU (Parametric ReLU) activation function: when the corresponding element in the source operand srcReg0 is greater than or equal to 0, the element is written directly to the destination operand dstReg; otherwise, the product of the corresponding elements in srcReg0 and srcReg1 is written to dstReg. Note: When the data in the source operand srcReg0 is positive zero (+0) of a floating-point number, it is treated as a positive number and output directly; when it is negative zero (-0) of a floating-point number, it is treated as a negative number and multiplied by srcReg1. The calculation formula is as follows:

$$
dstReg_i = \begin{cases} srcReg0_i & srcReg0_i >= 0 \\ srcReg0_i \times srcReg1_i & srcReg0_i < 0 \end{cases}
$$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Prelu(U &dstReg, U &srcReg0, U &srcReg1, MaskReg &mask)
```

## Parameters<a name="section389364115374"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [Data Type](#data-type). |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration type, which selects the MERGING mode or the ZEROING mode.<br>&bull; In ZEROING mode, the elements not selected by the mask are set to zero in **dstReg**.<br>&bull; The MERGING mode is not supported currently. |
| U | RegTensor type of the source operand and destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg0 | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md).<br>The data type of the source operand must be consistent with the destination operand. |
| srcReg1 | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md).<br>The data type of the source operand must be consistent with the destination operand. |
| mask | Input | Valid indicator for the elements in the source operand. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Type

The destination operand and source operand must have the same data type. Supported data types: half and float.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- When the data in the source operand `srcReg0` is positive zero (+0), it is treated as a positive number and output directly. When it is negative zero (-0), it is treated as a negative number and multiplied by `srcReg1`, with the multiplication following the IEEE 754 floating-point multiplication rules.

## Example<a name="section232816306478"></a>

```cpp
template<typename T>
__simd_vf__ inline void PreluVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Prelu(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
