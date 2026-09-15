# GatherB<a name="ZH-CN_TOPIC_0000001955999929"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T22:40:23.538Z -->

## Applicable Products<a name="section1550532418810"></a>

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

## Function Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

This instruction collects the source operand into the destination operand **dstReg** by **DataBlock** (32B) according to the index value **index**. The collection process is shown in Figure 1:

**Figure 1**  GatherB function description

![Figure 1 GatherB function description](../../../../figures/reg_gatherb.png)

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U, typename S>
__simd_callee__ inline void GatherB(U& dstReg, __ubuf__ T* baseAddr, S& index, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|-----|-----|
| T | Data type of the destination operand and source operand. For supported data types, see [Data Type](#data-type). |
| U | RegTensor type of the destination operand, for example, RegTensor\<half>. It is automatically deduced by the compiler and does not need to be manually specified by the user. |
| S | RegTensor type of the index value, for example, RegTensor\<uint32_t>. It is automatically deduced by the compiler and does not need to be manually specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstReg | Output | Destination operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| baseAddr | Input | Source operand, the base address in UB, which must be 32-byte aligned. |
| index | Input | Index value, the position of each DataBlock in dstReg relative to baseAddr in UB, in bytes. The type is [RegTensor](../register_data_types/RegTensor.md). The index value must be 32B aligned, that is, one index value corresponds to one DataBlock. Values in index can be repeated. For example:<br>baseAddr: [DataBlock0, DataBlock1, DataBlock2, DataBlock3, DataBlock4, DataBlock5, DataBlock6, DataBlock7, ... , DataBlock32, ...].<br>index: [0\*32, 1\*32, 2\*32, 3\*32, 4\*32, 5\*32, 6\*32, 32\*32]<br>dstReg: [DataBlock0, DataBlock1, DataBlock2, DataBlock3, DataBlock4, DataBlock5, DataBlock6, DataBlock32]. |
| mask | Input | Valid indication of the source operand element operation. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Type

The data types of the destination operand and the source operand must be consistent. The supported data types are **b8**, **b16**, **b32**, and **b64**.

The supported data type for the index value is **uint32_t**.

## Return Value

None

## Constraints<a name="section177921451558"></a>

- The address in **UB** must be 32-byte aligned.
- The **Source Operand** and **Destination Operand** must have the same data type.
- The **index** value must be 32-byte aligned, that is, one index value corresponds to one **DataBlock**.
- The data corresponding to the **index** value must be within the valid address range of **UB**.
- The **regTrait** template parameter of **RegTensor** supports only **RegTraitNumOne**.

## Example<a name="section642mcpsimp"></a>

```cpp
template <typename T>
__simd_vf__ inline void GatherBVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ uint32_t* indexAddr, uint32_t count, uint16_t oneRepeatSize)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::RegTensor<uint32_t> indexReg;
    AscendC::Reg::MaskReg mask;
    uint16_t repeatTimes = AscendC::CeilDivision(count, oneRepeatSize);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(indexReg, indexAddr + i * oneRepeatSize);
        AscendC::Reg::GatherB(dstReg, srcAddr, indexReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```

