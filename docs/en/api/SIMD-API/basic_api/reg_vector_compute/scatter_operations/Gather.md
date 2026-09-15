# Gather<a name="ZH-CN_TOPIC_0000002095379710"></a>

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-27T22:05:49.505Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_gather_mask_intf.h"`.

This instruction collects the elements of the **Source Operand** **srcReg** into the **Destination Operand** **dstReg** based on the **Index Value** **indexReg**. The collection process is shown in the following figure:

**Figure 1** Gather description  
![](../../../../figures/reg_gather_1.png "Gather description")

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U = DefaultType, typename S, typename V>
__simd_callee__ inline void Gather(S& dstReg, S& srcReg, V& indexReg)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the source operand and destination operand, which must be consistent. For supported data types, see [Data Type](#data-type). |
| U | Data type of the index value. For supported data types, see [Data Type](#data-type). |
| S | [RegTensor](../register_data_types/RegTensor.md) type of the destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |
| V | [RegTensor](../register_data_types/RegTensor.md) type of the index value, for example, RegTensor&lt;uint16_t&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Destination operand.<br>Type: [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Input | Source operand.<br>Type: [RegTensor](../register_data_types/RegTensor.md). |
| indexReg | Input | Index value, in elements.<br>Type: [RegTensor](../register_data_types/RegTensor.md).<br>The bit width of the data type must be consistent with that of the destination operand. Values in indexReg can be repeated.<br>If an index value in indexReg exceeds the maximum number of data elements that can be stored in the current RegTensor, it is processed as follows:<br>The maximum number of data elements that can be stored in RegTensor is VL/sizeof(T). If the index value in indexReg is i, the index value is updated to i % (VL/sizeof(T)).<br>Example: srcReg with elements of the half data type has 128 elements. When the index value in indexReg is 168, the index value is updated to 168 % 128 = 40. |

## Data Type

- The data types supported by the Source Operand and Destination Operand are b8, b16, and b32.
- The data types supported by the Index Value are uint8_t, uint16_t, and uint32_t.

## Return Value

None

## Constraints<a name="section177921451558"></a>

- The data types of the source operand and the destination operand must be consistent.
- The bit widths of the index value and the destination operand data type must be consistent.

## Example<a name="section642mcpsimp"></a>

```cpp
// Collect elements from the source vector based on the index, dstReg[i] = srcReg[indexReg[i]].
template <typename T, typename U>
__simd_vf__ inline void GatherVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ U* indexAddr, uint32_t count, uint16_t oneRepeatSize)
{
    AscendC::Reg::RegTensor<T> srcReg, dstReg;
    AscendC::Reg::RegTensor<U> indexReg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
    uint16_t repeatTimes = AscendC::CeilDivision(count, oneRepeatSize);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(srcReg, srcAddr, oneRepeatSize);
        AscendC::Reg::LoadAlign<U, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(indexReg, indexAddr, oneRepeatSize);
        AscendC::Reg::Gather<T, U>(dstReg, srcReg, indexReg);
        AscendC::Reg::StoreAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dstAddr, dstReg, oneRepeatSize, mask);
    }
}
```
