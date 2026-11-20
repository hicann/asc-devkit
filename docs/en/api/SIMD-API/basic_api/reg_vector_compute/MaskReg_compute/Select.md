# Select<a name="ZH-CN_TOPIC_0000001985457929"></a>

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-05T03:13:07.770Z pushedAt=2026-11-18T01:25:30.908Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/MaskReg_compute/Select_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"`.

Given two source operands src0 and src1, selects elements based on the bit values of mask to obtain the destination operand dst. The selection rule is: when a mask bit is 1, the element at the corresponding position is selected from src0; when the bit is 0, the element is selected from src1.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__simd_callee__ inline void Select(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Description |
| --- | --- |
| dst | Destination operand, type is [MaskReg](../register_data_types/MaskReg.md). |
| src0 | Source operand, type is [MaskReg](../register_data_types/MaskReg.md). |
| src1 | Source operand, type is [MaskReg](../register_data_types/MaskReg.md). |
| mask | Indicates whether to select src0 or src1, of type [MaskReg](../register_data_types/MaskReg.md). |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Examples<a name="section932512912207"></a>

The following example processes 32 `float` data elements and combines two masks with different ranges through `Select`. The first 16 bits of `maskFirstHalf` are 1 and the last 16 bits are 0, so the generated `selMask` uses the corresponding bits of `maskFirstQuarter` for the first 16 bits and the corresponding bits of `maskFullLength` for the last 16 bits. Finally, `StoreAlign` moves out only the source data corresponding to the valid bits in `selMask`.

```cpp
__simd_vf__ inline void SelectVF(__ubuf__ float* dstAddr, __ubuf__ float* srcAddr)
{
    AscendC::Reg::RegTensor<float> srcReg;
    AscendC::Reg::MaskReg maskFirstQuarter =
        AscendC::Reg::CreateMask<float, AscendC::Reg::MaskPattern::VL8>();
    AscendC::Reg::MaskReg maskFirstHalf =
        AscendC::Reg::CreateMask<float, AscendC::Reg::MaskPattern::VL16>();
    AscendC::Reg::MaskReg maskFullLength =
        AscendC::Reg::CreateMask<float, AscendC::Reg::MaskPattern::VL32>();
    AscendC::Reg::MaskReg selMask;
    // Take the first 16 bits from maskFirstQuarter and the last 16 bits from maskFullLength.
    AscendC::Reg::Select(selMask, maskFirstQuarter, maskFullLength, maskFirstHalf);
    // Move in only the first 32 elements indicated by maskFullLength.
    AscendC::Reg::LoadAlign<float, AscendC::Reg::DataCopyMode::DATA_BLOCK_COPY>(
        srcReg, srcAddr, 1, maskFullLength);
    AscendC::Reg::StoreAlign(dstAddr, srcReg, selMask);
}
```

The input data is as follows: the first 16 elements of `srcAddr` are `1.0`, the last 16 elements are `2.0`, and all 32 elements of `dstAddr` are `0.0`:

```text
srcAddr = [1.0, 1.0, ..., 1.0, 2.0, 2.0, ..., 2.0]
dstAddr = [0.0, 0.0, ..., 0.0]
```

After `Select` is executed, the first 8 bits of `selMask` are 1, bits 9 through 16 are 0, the last 16 bits are 1, and all remaining bits are 0. `selMask` is of the `MaskReg` type, so its values represent mask bits:

```text
selMask = [1, 1, ..., 1, 0, 0, ..., 0, 1, 1, ..., 1]
```

After `StoreAlign` is executed, the first 8 elements of `dstAddr` are `1.0`, elements 9 through 16 retain their initial value `0.0`, and the last 16 elements are `2.0`:

```text
dstAddr = [1.0, 1.0, ..., 1.0, 0.0, 0.0, ..., 0.0, 2.0, 2.0, ..., 2.0]
```
