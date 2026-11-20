# Gather<a name="ZH-CN_TOPIC_0000002095379710"></a>

<!-- md-trans-meta sourceCommit=0f9a59471afc8678a19cd607420c13ec5e8e0290 translatedAt=2026-11-05T03:46:41.602Z pushedAt=2026-11-18T01:25:30.957Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/scatter_operations/Gather_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is: `"basic_api/reg_compute/kernel_reg_compute_gather_mask_intf.h"`.

This instruction gathers the source operand srcReg into the destination operand dstReg element by element according to the index value indexReg. The gathering process is shown in the following figure:

**Figure 1** Gather Function Description  
![](../../../../figures/reg_gather_1.png "Gather Function Description")

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
| S | The [RegTensor](../register_data_types/RegTensor.md) type of the destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |
| V | The [RegTensor](../register_data_types/RegTensor.md) type of the index value, for example, RegTensor&lt;uint16_t&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| indexReg | Input | Index value, unit: elements.<br>The type is [RegTensor](../register_data_types/RegTensor.md).<br>The bit width of the data type must be consistent with that of the destination operand. Values in indexReg can be repeated.<br>If an index value in indexReg exceeds the maximum number of data elements that can be stored in the current RegTensor, it is processed as follows:<br>The maximum number of data elements that a RegTensor can store is [VL](../register_data_types/RegTensor.md)/sizeof(T). If the index value in indexReg is i, the index value is updated to i % (VL/sizeof(T)).<br>Example: For srcReg with half data type elements and 128 elements, when the index value in indexReg is 168, the index value is updated to 168 % 128 = 40. |

## Data Type

<!-- npu="950" id8 -->
- Ascend 950PR&950DT products: the Source Operand and Destination Operand support the following data types: b8, b16, b32.
- Ascend 950PR&950DT products: the Index Value supports the following data types: uint8_t, uint16_t, uint32_t.
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/scatter_operations/Gather_res.md#id2 -->

## Return Value

None

## Constraints<a name="section177921451558"></a>

- The data types of the source operand and the destination operand must be consistent.
- The bit widths of the index value and the destination operand data type must be consistent.

## Example<a name="section642mcpsimp"></a>

```cpp
// Gather elements from the source vector by index, dstReg[i] = srcReg[indexReg[i]].
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
