# PairReduceElem<a name="ZH-CN_TOPIC_0000002045329737"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:32:49.533Z pushedAt=2026-10-26T07:41:44.558Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reduction_compute/PairReduceElem_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_reduce_intf.h"`.

Adds each pair of adjacent values in the input **srcReg** and stores the resulting values in the low-order positions of **dstReg**.

![PairReduceElem diagram](../../../../figures/reg_pair_reduce_elem.png)

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <PairReduce type = PairReduce::SUM, typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void PairReduceElem(U& dstReg, U srcReg, MaskReg mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| type | Specific PairReduce type. Currently, only reduction sum is supported.<br>enum class PairReduce {<br>    SUM = 0,<br>}; |
| T | Data type of the target operand and the source operand. |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md), which selects the MERGING mode or the ZEROING mode.<br>&bull; ZEROING: Elements not filtered by the mask are set to zero in dst. Currently, only this mode is supported.<br>&bull; MERGING: Currently not supported. |
| U | RegTensor type of the target operand and the source operand. It is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Function parameter description

| Parameter | Description |
| --- | --- |
| dstReg | Target operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| mask | Valid indication of the operation on the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Types

Supported data types: **half**, **float**.

## Return Value

None

## Constraints<a name="section177921451558"></a>

- **mask** specifies whether the source operands participate in calculation. Elements that do not participate in calculation are treated as 0 during accumulation. For example:
  - If two consecutive elements a and b do not participate in calculation, the target operand result is 0.
  - If only element a of two consecutive elements a and b participates in calculation, the target operand result is a.
  - If only element b of two consecutive elements a and b participates in calculation, the target operand result is b.

- After summation, only the first half of the elements in **dst** are valid data, and the second half is filled with 0. Avoid data overwriting when moving data out to the Unified Buffer (UB).

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void PairReduceElemVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, 
 uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg calMask;
    AscendC::Reg::MaskReg carryMask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::H>();
    uint16_t dstOneRepeatSize = oneRepeatSize >> 1;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        calMask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::PairReduceElem<AscendC::Reg::PairReduce::SUM>(dstReg, srcReg, calMask);
        AscendC::Reg::StoreAlign(dstAddr + i * dstOneRepeatSize, dstReg, carryMask);
    }
}
```
