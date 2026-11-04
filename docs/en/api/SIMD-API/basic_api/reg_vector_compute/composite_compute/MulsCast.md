# MulsCast<a name="ZH-CN_TOPIC_0000002008487800"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:08:20.651Z pushedAt=2026-10-26T07:41:44.501Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

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
- Atlas 200I/500 A2 inference product: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/composite_compute/MulsCast_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_vec_fused_intf.h"`.

This API multiplies the source operand **src** by the scalar **scalar**, converts the result to the half type in CAST\_ROUND mode, and writes the computation result to the destination operand **dst** according to the mask. The calculation formula is as follows:

$$
dst_i = cast\_round\_to\_f16(src_i \times scalarValue)
$$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T0 = DefaultType, typename T1 = DefaultType, typename T2, RegLayout layout = RegLayout::ZERO, typename T3, typename T4>
__simd_callee__ inline void MulsCast(T3& dstReg, T4& srcReg, T2 scalarValue, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T0 | Data type of the destination operand. |
| T1 | Data type of the source operand. |
| T2 | Data type of the scalar source operand. |
| layout | [RegLayout](../aux_data_types/RegLayout.md) enumeration type.<br>Supports **RegLayout::ZERO** and **RegLayout::ONE**. |
| T3 | `RegTensor` type of the destination operand, for example, `RegTensor<float>`, which is automatically deduced by the compiler and does not need to be specified by the user. |
| T4 | `RegTensor` type of the source operand, for example, `RegTensor<float>`, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| scalarValue | Input | Source operand.<br>The type is scalar. |
| mask | Input | Validity indicator for element-wise operations on the source operand. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Return Value Description<a name="section640mcpsimp"></a>

None

## Data Types<a name="zh-cn_topic_0000002580343947_section1785515470716"></a>

**Table 3**  Data type combinations

| srcReg | scalarValue | dstReg |
| :----- | :---------- | :----- |
| float  | float       | half   |

## Constraints<a name="section633mcpsimp"></a>

- This API does not support overlap between the source operand register and the destination operand register, but supports overlap between source operand registers.
- The operands of this API are registers, so address alignment is not involved.
- This API does not modify the values of global registers.
- [Cast](../type_conversion/Cast.md) computation is rounded in the CAST\_ROUND mode.

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void MulsCastVF(__ubuf__ half* dstAddr, __ubuf__ float* srcAddr, float scalarValue, uint32_t count, uint32_t srcRepeatSize, uint32_t dstRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<float> srcReg0;
    AscendC::Reg::RegTensor<half> dstReg0;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<float>(count);
        AscendC::Reg::LoadAlign(srcReg0, srcAddr + i * srcRepeatSize);
        AscendC::Reg::MulsCast<half, float, float, AscendC::Reg::RegLayout::ZERO>(dstReg0, srcReg0, scalarValue, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * dstRepeatSize, dstReg0, mask);
    }
}
```
