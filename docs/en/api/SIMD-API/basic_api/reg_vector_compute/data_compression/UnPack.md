# UnPack<a name="ZH-CN_TOPIC_0000002047032737"></a>

<!-- md-trans-meta sourceCommit=af3f6adafe94d121d34283378bfbe7e348767ea3 translatedAt=2026-11-05T02:52:12.082Z pushedAt=2026-11-18T01:25:30.862Z nl=2 -->

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
- Kirin 9050 products: Supported
<!-- end id8 -->
<!-- npu="kirin1" id9 -->
- KirinDev0001 products: Supported
<!-- end id9 -->
<!-- npu="kirin2" id10 -->
- KirinDev0002 products: Supported
<!-- end id10 -->
<!-- npu="kirin3" id11 -->
- KirinDev0003 products: Supported
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/data_compression/UnPack_res.md#id1 -->


## Description<a name="section1922292299"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_pack_intf.h"`.

For unsigned integers, writes the elements in the lower half or upper half of the source operand **srcReg** into **dstReg** by extending the bit width with zero-padding of the high bits. For signed integers, writes the elements in the lower half or upper half of the source operand **srcReg** into **dstReg** by extending the bit width while preserving the sign bit.

**Figure 1** UnPack diagram

![UnPack](../../../../figures/reg_unpack_1.png)

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U = DefaultType, HighLowPart part = HighLowPart::LOWEST, typename S, typename V>
__simd_callee__ inline void UnPack(S& dstReg, V& srcReg)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ------ | ------ |
| T | Destination operand data type.<br> For the data type constraints of the source operand and destination operand, see [Table Data type mapping between source operand and destination operand](#table3).|
| U | Data type of the source operand. |
| part | Enumeration type that controls whether the lower half or upper half of srcReg is read.<br>• HighLowPart::LOWEST, low-bit mode, reads the lower half of srcReg.<br>• HighLowPart::HIGHEST, high-bit mode, reads the upper half of srcReg.<br>Note: RegTraitNumTwo supports only the LOWEST mode.|
| S | Destination operand RegTensor type. |
| V | Source operand RegTensor type. |

**Table 2** Function parameter description

| Parameter | Description |
| ------ | ------ |
| dstReg | Destination operand.<br> The type is [RegTensor](../register_data_types/RegTensor.md).|
| srcReg | Source operand.<br> The type is [RegTensor](../register_data_types/RegTensor.md).|

## Return Value<a name="section640mcpsimp"></a>

None

## Data Type

**Table 3**  Data type mapping between source operands and destination operands<a id="table3"></a> 

| T Data Type | U Data Type |
| ------ | ------ |
| int16_t | int8_t |
| uint16_t | uint8_t |
| int32_t | int16_t |
| uint32_t | uint16_t |
| int64_t | int32_t |
| uint64_t | uint32_t |

## Constraints<a name="section633mcpsimp"></a>

None

## Examples<a name="section932512912207"></a>

```cpp
template<typename T, typename U, int32_t mode>
__simd_vf__ inline void UnPackVF(__ubuf__ T* dstAddr, __ubuf__ U* srcAddr, uint32_t oneDstRepSize, uint16_t repeatTimes, uint32_t oneSrcRepSize)
{
    AscendC::Reg::RegTensor<U> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneSrcRepSize);
        if constexpr (mode == 0) {
            AscendC::Reg::UnPack<T, U, AscendC::Reg::HighLowPart::LOWEST>(dstReg, srcReg);
        } else if constexpr (mode == 1) {
            AscendC::Reg::UnPack<T, U, AscendC::Reg::HighLowPart::HIGHEST>(dstReg, srcReg);
        }
        AscendC::Reg::StoreAlign(dstAddr + i * oneDstRepSize, dstReg, mask);
    }
}
```
