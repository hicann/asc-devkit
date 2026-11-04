# Gather<a name="ZH-CN_TOPIC_0000002095379710"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:07:09.156Z pushedAt=2026-10-26T07:41:44.604Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/scatter_operations/Gather_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/reg_compute/kernel_reg_compute_gather_mask_intf.h"`.

This instruction collects elements from the **Source Operand** **srcReg** into the **Destination Operand** **dstReg** based on the **Index Value** **indexReg**. The collection process is shown in the following figure:

**Figure 1**  Gather Description  
![](../../../../figures/reg_gather_1.png "Gather Description")

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U = DefaultType, typename S, typename V>
__simd_callee__ inline void Gather(S& dstReg, S& srcReg, V& indexReg)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the source operand and destination operand. The two must be consistent. For supported data types, see [Data Type](#data-type). |
| U | Data type of the index value. For supported data types, see [Data Type](#data-type). |
| S | [RegTensor](../register_data_types/RegTensor.md) type of the destination operand, for example, `RegTensor<half>`. It is automatically deduced by the compiler and does not need to be manually specified by the user. |
| V | [RegTensor](../register_data_types/RegTensor.md) type of the index value, for example, `RegTensor<uint16_t>`. It is automatically deduced by the compiler and does not need to be manually specified by the user. |

**Table 2** Function parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Destination operand.<br>Type: [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Input | Source operand.<br>Type: [RegTensor](../register_data_types/RegTensor.md). |
| indexReg | Input | Index value, in elements.<br>Type: [RegTensor](../register_data_types/RegTensor.md).<br>The bit width of the data type must be consistent with that of the destination operand. Values in `indexReg` can be repeated.<br>If an index value in `indexReg` exceeds the maximum number of data elements that can be stored in the current RegTensor, it is processed as follows:<br>The maximum number of data elements that can be stored in a RegTensor is [VL](../register_data_types/RegTensor.md)/sizeof(T). If the index value in `indexReg` is i, the index value is updated to i % (VL/sizeof(T)).<br>Example: `srcReg` with elements of the `half` data type has 128 elements. When the index value in `indexReg` is 168, the index value is updated to 168 % 128 = 40. |

## Data Type

- The data types supported by the source operand and destination operand are **b8**, **b16**, and **b32**.
- The data types supported by the index value are **uint8_t**, **uint16_t**, and **uint32_t**.

## Return Value

None

## Constraints<a name="section177921451558"></a>

- The data types of the source operand and the destination operand must be consistent.
- The bit width of the data type of the index value and the destination operand must be consistent.

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
