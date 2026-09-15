# MaskReg Store-Out (StoreAlign)<a name="ZH-CN_TOPIC_0000001958347433"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T22:32:22.029Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not Supported
<!-- end id7 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

**StoreAlign** transfers data from [MaskReg](../register_data_types/MaskReg.md) to the Unified Buffer (UB). There are three types of transfer APIs:

- Normal transfer API: After a transfer is completed, the UB address is not automatically updated, so the address must be updated manually in each iteration.
- PostUpdate extended transfer API:
    - POST_MODE_NORMAL mode: Not supported.
    - POST_MODE_UPDATE mode: After a transfer is completed, the UB address is automatically updated, so the address does not need to be updated manually in each iteration.

- Scenario using the [AddrReg](../register_data_types/AddrReg.md) register to store the offset: In each iteration, call **CreateAddrReg** to manually set the address offset first, and then call the transfer instruction.

## Prototype<a name="section620mcpsimp"></a>

- Normal Transfer API

    ```cpp
    template <typename T, MaskDist dist = MaskDist::DIST_NORM>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, MaskReg& mask)
    ```

- PostUpdate Extended Transfer API

    ```cpp
    template <typename T, PostLiteral postMode, MaskDist dist = MaskDist::DIST_NORM>
    __simd_callee__ inline void StoreAlign(__ubuf__ T*& dstAddr, MaskReg& mask, int32_t offset)
    ```

- API that uses [AddrReg](../register_data_types/AddrReg.md) to store the offset, which must be used together with the CreateAddrReg API

    ```cpp
    template <typename T, MaskDist dist = MaskDist::DIST_NORM>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, MaskReg& mask, AddrReg offset)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|-----|-----|
| **T** | Data type of the destination operand. For supported data types, see [Data Type](#data-type). |
| **postMode** | Controls whether to enable the automatic UB address update function (post update). Type: **PostLiteral**. For details, see [PostLiteral](../aux_data_types/PostLiteral.md). |
| **dist** | Transfer mode. Type: **MaskDist**. For details, see [Table 3 MaskDist parameter description](#table-3-maskdist-parameter-description). |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|-----|-----|-----|
| **dstAddr** | Input/Output | Destination operand, the UB start address, which must be 32-byte aligned. |
| **mask** | Input | Source operand. Type: [MaskReg](../register_data_types/MaskReg.md). |
| **offset** | Input | &bull; When the data type is **AddrReg**, it is an address offset register. Call **CreateAddrReg** to set the address offset before the transfer. The actual UB address for the transfer is dstAddr + the offset stored in the register. For the alignment constraint of the offset stored in the **AddrReg** register multiplied by sizeof(T), see [Table 3 MaskDist parameter description](#table-3-maskdist-parameter-description).<br>&bull; When the data type is **int32_t**, it is the address update step, in bytes. For the alignment constraint of offset, see [Table 3 MaskDist parameter description](#table-3-maskdist-parameter-description). Depending on the value of **postMode**, there are two cases:<br>&nbsp;&nbsp;&bull; **POST_MODE_NORMAL**: Not supported.<br>&nbsp;&nbsp;&bull; **POST_MODE_UPDATE**: The actual UB start address for the transfer is dstAddr, and after the transfer, the address update dstAddr += offset is performed. |

<a id="table-3-maskdist-parameter-description"></a>
**Table 3** MaskDist parameter description

| MaskDist Value | Meaning | Transfer Alignment Constraint (Byte) |
|-----|-----|-----|
| **DIST_NORM** | Normal mode, transfers VL/8 data. | VL/8 |
| **DIST_PACK** | Compression mode, discards data every other bit and compresses VL/8 data to VL/16 for transfer out. | VL/16 |

## Data Type

The data types supported by the destination operand are b8, b16, b32, and b64.

## Return Value<a name="section1575141714439"></a>

None

## Constraints<a name="section11585101304320"></a>

- The address constraint in the Unified Buffer, the alignment constraint of **offset**, and the alignment constraint of the offset stored in the **AddrReg** register multiplied by sizeof(T) are related to **MaskDist**. For details about the address constraints, see [Table 3 MaskDist Parameter Description](#table-3-maskdist-parameter-description).
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

- **PostUpdate** Extended Transfer API
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

- API for storing the offset using **AddrReg**
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

