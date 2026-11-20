# ExpSub<a name="ZH-CN_TOPIC_0000002008646072"></a>

<!-- md-trans-meta sourceCommit=af3f6adafe94d121d34283378bfbe7e348767ea3 translatedAt=2026-11-05T02:47:29.095Z pushedAt=2026-11-18T01:25:30.851Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/composite_compute/ExpSub_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_vec_fused_intf.h"`.

**srcReg1** is subtracted from **srcReg0**, and the difference is used as the exponent of e. The computation result is written to **dstReg** based on **mask**. The formula is as follows:

When the data type of **srcReg** is float:

$$
dstReg_i = e^{(srcReg0_i - srcReg1_i)}
$$

When the data type of **srcReg** is half:

$$
dstReg_i = e^{(cast\_f16\_to\_f32(srcReg0_i) - cast\_f16\_to\_f32(srcReg1_i))}
$$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U = DefaultType, RegLayout layout = RegLayout::ZERO, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S, typename V>
__simd_callee__ inline void ExpSub(S& dstReg, V& srcReg0, V& srcReg1, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Data type of the destination operand. |
| U | Data type of the source operand. |
| layout | [RegLayout](../aux_data_types/RegLayout.md) enumeration type.<br>• When the src type is **half**, **RegLayout::ZERO** and **RegLayout::ONE** are supported. For details, see [RegLayout](../aux_data_types/RegLayout.md).<br>• When the src type is **float**, the configuration does not take effect. |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration type. Selects the MERGING mode or the ZEROING mode.<br>• In ZEROING mode, elements not filtered by the mask are set to zero in dstReg.<br>• The MERGING mode is currently not supported. |
| S | `RegTensor` type of the destination operand, for example, `RegTensor<float>`, which is automatically deduced by the compiler and does not need to be specified by the user. |
| V | `RegTensor` type of the source operand, for example, `RegTensor<half>`, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg0 | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg1 | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| mask | Input | Valid indicator for the operation on the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Return Value<a name="section640mcpsimp"></a>

None

## Data Type

**Table 3**  Data type combinations

| srcReg0 | srcReg1 | dstReg |
| :------ | :------ | :----- |
| half    | half    | float  |
| float   | float   | float  |

## Constraints<a name="section633mcpsimp"></a>

- When the source operand data type is float, full register overlap is supported. When the source operand data type is half, only source operand register overlap is supported.
- The operands of this API are registers, so address alignment is not involved.
- This API does not modify the values of global registers.
- When the source operand type is half, the Vector computing unit processes at most 64 elements per computation. The valid mask depends on the input data type, and only even bits are valid, with a total of 128 valid bits. See the following figure:

  ![ExpSub half type processing diagram](../../../../figures/reg_expsub_half.png)

## Key Features

Precision improvement. When the source operand data type is half and the destination operand data type is float, the source operand is promoted to float before computation.

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T, typename U>
static __simd_vf__ inline void ExpSubVF(__ubuf__ T* dstAddr, __ubuf__ U* src0Addr, __ubuf__ U* src1Addr, uint32_t count, uint32_t srcRepeatSize, uint32_t dstRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<U> srcReg0;
    AscendC::Reg::RegTensor<U> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<U>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * srcRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * srcRepeatSize);
        AscendC::Reg::ExpSub<T, U, AscendC::Reg::RegLayout::ZERO, AscendC::Reg::MaskMergeMode::ZEROING>(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * dstRepeatSize, dstReg, mask);
    }
}
```
