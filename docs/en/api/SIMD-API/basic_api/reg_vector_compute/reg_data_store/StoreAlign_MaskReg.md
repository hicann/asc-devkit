# MaskReg Store (StoreAlign)<a name="ZH-CN_TOPIC_0000001958347433"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:52:58.047Z pushedAt=2026-10-26T07:41:44.587Z nl=2 -->

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
- Atlas 200I/500 A2 Inference Product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reg_data_store/StoreAlign_MaskReg_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

**StoreAlign** transfers data from [MaskReg](../register_data_types/MaskReg.md) to the Unified Buffer (UB). There are three types of movement APIs:

- Normal Transfer API: After a transfer is completed, the UB address is not automatically updated, so the address must be updated manually in each iteration.
- PostUpdate Extended Transfer API:
    - **POST_MODE_NORMAL** mode: not supported.
    - **POST_MODE_UPDATE** mode: After a transfer is completed, the UB address is automatically updated, so the address does not need to be updated manually in each iteration.

- Scenario using the [AddrReg](../register_data_types/AddrReg.md) register to store the offset: In each iteration, call **CreateAddrReg** to manually set the address offset first, and then call the transfer instruction.

## Prototype<a name="section620mcpsimp"></a>

- Normal movement API

    ```cpp
    template <typename T, MaskDist dist = MaskDist::DIST_NORM>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, MaskReg& mask)
    ```

- PostUpdate extended movement API

    ```cpp
    template <typename T, PostLiteral postMode, MaskDist dist = MaskDist::DIST_NORM>
    __simd_callee__ inline void StoreAlign(__ubuf__ T*& dstAddr, MaskReg& mask, int32_t offset)
    ```

- API that uses [AddrReg](../register_data_types/AddrReg.md) to store the offset, which must be used together with **CreateAddrReg**

    ```cpp
    template <typename T, MaskDist dist = MaskDist::DIST_NORM>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, MaskReg& mask, AddrReg offset)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|-----|-----|
| T | Data type of the destination operand. For supported data types, see [Data Type](#data-type). |
| postMode | Controls whether to enable the UB address auto-update function (post update). Type: **PostLiteral**. For details, see [PostLiteral](../aux_data_types/PostLiteral.md). |
| dist | Transfer mode. Type: **MaskDist**. For details, see [Table 3 MaskDist parameter description](#table-3-maskdist-parameter-description). |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstAddr | Input/Output | Destination operand, UB start address, which must be 32-byte aligned. |
| mask | Input | Source operand. Type: [MaskReg](../register_data_types/MaskReg.md). |
| offset | Input | &bull; When the data type is **AddrReg**, it is the address offset register. Call **CreateAddrReg** to set the address offset before the transfer. The actual UB address for the transfer is dstAddr + the offset stored in the register. For the alignment constraint of the offset stored in the **AddrReg** register * sizeof(T), see [Table 3 MaskDist parameter description](#table-3-maskdist-parameter-description).<br>&bull; When the data type is **int32_t**, it is the address update step, in bytes. For the alignment constraint of offset, see [Table 3 MaskDist parameter description](#table-3-maskdist-parameter-description). Depending on the value of **postMode**, there are two cases:<br>&nbsp;&nbsp;&bull; **POST_MODE_NORMAL**: not supported.<br>&nbsp;&nbsp;&bull; **POST_MODE_UPDATE**: The actual transfer UB start address is dstAddr. After the transfer, the address is updated: dstAddr += offset. |

<a id="table-3-maskdist-parameter-description"></a>
**Table 3** MaskDist parameter description

| MaskDist Value | Meaning | Transfer Alignment Constraint (Byte) |
|-----|-----|-----|
| DIST_NORM | Normal mode, transfers VL/8 data. | VL/8 |
| DIST_PACK | Compressed mode, discards data at every other bit, compressing VL/8 data to VL/16 for storing. | VL/16 |

## Data Type

The data types supported by the destination operand are b8, b16, b32, and b64.

## Return Value<a name="section1575141714439"></a>

None

## Constraints<a name="section11585101304320"></a>

- The address constraint in the UB, the alignment constraint of the offset, and the alignment constraint of the offset stored in the AddrReg register \* sizeof(T) are related to MaskDist. For the specific address constraint, see [Table 3 MaskDist Parameter Description](#table-3-maskdist-parameter-description).
- The **regTrait** template parameter of **MaskReg** supports only **RegTraitNumOne**.

## Example<a name="section642mcpsimp"></a>

- Normal Transfer API
    ```cpp
    template <typename T>
    __simd_vf__ inline void ComputeMode01(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadAlign(mask, srcAddr + i * oneRepeatSize);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, mask);
        }
    }
    ```

- PostUpdate Extended Transfer API
    ```cpp
    template <typename T>
    __simd_vf__ inline void StoreAlignVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(mask, srcAddr, oneRepeatSize);
            AscendC::Reg::StoreAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dstAddr, mask, oneRepeatSize);
        }
    }
    ```

- API Using AddrReg to Store the Offset
    ```cpp
    template <typename T>
    __simd_vf__ inline void ComputeMode03(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::MaskReg mask;
        AscendC::Reg::AddrReg aReg;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            aReg = AscendC::Reg::CreateAddrReg<T>(i, oneRepeatSize);
            AscendC::Reg::LoadAlign(mask, srcAddr, aReg);
            AscendC::Reg::StoreAlign(dstAddr, mask, aReg);
        }
    }
    ```
