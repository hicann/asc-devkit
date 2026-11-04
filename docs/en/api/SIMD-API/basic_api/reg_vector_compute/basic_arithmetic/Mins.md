# Mins<a name="ZH-CN_TOPIC_0000001956986929"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:51:18.854Z pushedAt=2026-10-26T07:41:44.463Z nl=2 -->

## Product Support

<!-- npu="950" id1 -->
- Ascend 950PR & 950DT products: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/basic_arithmetic/Mins_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_vec_binary_scalar_intf.h"`.

Based on the mask, this API compares the source operand **srcReg** element-wise with a scalar. If the element is greater than the scalar, the scalar value is taken; if it is smaller than the scalar, the source operand value is taken. The result is written to the destination operand **dstReg**. The calculation formula is as follows:

$$
dstReg_i = \min(srcReg_i, scalarValue)
$$

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void Mins(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the vector destination operand and source operand. For supported data types, see [Data Type](#data-type). |
| U | Data type of the scalar source operand. For supported data types, see [Data Type](#data-type). |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration type, which selects the MERGING mode or the ZEROING mode.<br>&bull; In ZEROING mode, elements not selected by the mask are set to zero in dstReg.<br>&bull; The MERGING mode is not supported currently. |
| S | dstReg is of the RegTensor type, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| scalarValue | Input | Source operand.<br>The type is scalar. |
| mask | Input | Valid indicator for the operation on source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Type
The data types of the destination operand and the source operand must be consistent. The supported data types are **int8_t**, **uint8_t**, **int16_t**, **uint16_t**, **half**, **bfloat16_t**, **int32_t**, **uint32_t**, **float**, **int64_t**, and **uint64_t**.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

When the input **srcReg** is -0 and **scalarValue** is +0, the output **dstReg** is -0.

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void MinsVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, T scalarValue, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;    
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, src0Addr + i * oneRepeatSize);
        AscendC::Reg::Mins(dstReg, srcReg, scalarValue, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
