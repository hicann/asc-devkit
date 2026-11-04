# Pack<a name="ZH-CN_TOPIC_0000002010754854"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:11:05.186Z pushedAt=2026-10-26T07:41:44.508Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/data_compression/Pack_res.md#id1 -->


## Description<a name="section16390191715619"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_pack_intf.h"`.

Selects the low 8 bits (for the **b16** type), low 16 bits (for the **b32** type), or low 32 bits (for the **b64** type) of the elements in the source operand **srcReg** and writes them to the lower half or higher half of **dstReg**.

**Figure 1**  Pack diagram

![Pack](../../../../figures/reg_pack_1.png)

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U = DefaultType, HighLowPart part = HighLowPart::LOWEST, typename S, typename V>
__simd_callee__ inline void Pack(S& dstReg, V& srcReg)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ------ | ------ |
| T | Destination operand data type.<br> For the data type constraints between the source operand and the destination operand, see [Data type mapping between source operand and destination operand](#table3). |
| U | Source operand data type. |
| part | Enumeration type that controls whether to write to the lower half or the higher half of **dstReg**.<br>• **HighLowPart::LOWEST**: Low mode, writes to the lower half of **dstReg**.<br>• **HighLowPart::HIGHEST**: High mode, writes to the upper half of **dstReg**.<br> Note: **RegTraitNumTwo** supports only the LOWEST mode. |
| S | **RegTensor** type of the destination operand, automatically deduced by the compiler and does not need to be specified by the user. |
| V | **RegTensor** type of the source operand, automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Function parameter description

| Parameter | Description |
| ------ | ------ |
| dstReg | Destination operand.<br> The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Source operand.<br> The type is [RegTensor](../register_data_types/RegTensor.md). |

## Return Value<a name="section640mcpsimp"></a>

None

## Data Type

**Table 3** Data type mapping between the source operand and the destination operand<a id="table3"></a>

| T Data Type | U Data Type |
| ------ | ------ |
| uint8_t | int16_t |
| uint8_t | uint16_t |
| uint16_t | int32_t |
| uint16_t | uint32_t |
| uint32_t | int64_t |
| uint32_t | uint64_t |

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section932512912207"></a>

```cpp
template<typename T, typename U, int32_t mode = 0>
__simd_vf__ inline void PackVF(__ubuf__ T* dstAddr, __ubuf__ U* srcAddr, uint32_t oneDstRepSize, uint32_t oneSrcRepSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<U> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneSrcRepSize);
        if constexpr (mode == 0) {
            AscendC::Reg::Pack<T, U, AscendC::Reg::HighLowPart::LOWEST>(dstReg, srcReg);
        } else if constexpr (mode == 1) {
            AscendC::Reg::Pack<T, U, AscendC::Reg::HighLowPart::HIGHEST>(dstReg, srcReg);
        }
        AscendC::Reg::StoreAlign(dstAddr + i * oneDstRepSize, dstReg, mask);
    }
}
```
