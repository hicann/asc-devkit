# MaskReg Load (LoadAlign)<a name="ZH-CN_TOPIC_0000001958227253"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T22:46:30.672Z -->

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

**LoadAlign** moves data from the Unified Buffer (UB) to [MaskReg](../register_data_types/MaskReg.md). There are three types of move APIs:

- Normal move API: After a move is completed, the UB address is not automatically updated, and the address must be updated manually in each iteration.
- **PostUpdate** extended move API: Two modes are available. For details, see [Parameters](#parameters):
    - **POST_MODE_NORMAL** mode: Not supported.
    - **POST_MODE_UPDATE** mode: After a move is completed, the UB address is automatically updated, and the address does not need to be updated manually in each iteration.

- Scenario where the [AddrReg](../register_data_types/AddrReg.md) register stores the offset: In each iteration, call [CreateAddrReg](../register_data_types/AddrReg.md) to manually set the address offset before calling the move instruction.

When loading data, you can configure the data distribution mode of the move through template parameters to implement functions such as upsampling and downsampling. For details about the distribution modes, see [Table 3 MaskDist Parameter Description](#table-3-maskdist-parameter-description).

[Figure 1](#fig-loadalign-maskreg-norm), [Figure 2](#fig-loadalign-maskreg-us), and [Figure 3](#fig-loadalign-maskreg-ds) show the DIST_NORM, DIST_US, and DIST_DS distribution modes.

**Figure 1**  DIST_NORM mode load<a id="fig-loadalign-maskreg-norm"></a>  
![](../../../../figures/dist_norm_mode_load.png)

**Figure 2**  DIST_US mode load<a id="fig-loadalign-maskreg-us"></a>  
![](../../../../figures/dist_us_mode_load.png)

**Figure 3**  DIST_DS mode load<a id="fig-loadalign-maskreg-ds"></a>  
![](../../../../figures/dist_ds_mode_load.png)

## Prototype<a name="section620mcpsimp"></a>

- Normal move API

    ```cpp
    template <typename T, MaskDist dist = MaskDist::DIST_NORM>
    __simd_callee__ inline void LoadAlign(MaskReg& mask, __ubuf__ T* srcAddr)
    ```

- **PostUpdate** extended move API

    ```cpp
    template <typename T, PostLiteral postMode, MaskDist dist = MaskDist::DIST_NORM>
    __simd_callee__ inline void LoadAlign(MaskReg& mask, __ubuf__ T* &srcAddr, int32_t offset)
    ```

- API that uses **AddrReg** to store the offset, which must be used together with the **CreateAddrReg** API

    ```cpp
    template <typename T, MaskDist dist = MaskDist::DIST_NORM>
    __simd_callee__ inline void LoadAlign(MaskReg& mask, __ubuf__ T* srcAddr, AddrReg offset)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template Parameter Description

| Parameter | Description |
|-----|-----|
| T | Data type of the source operand. For supported data types, see [Data Type](#data-type). |
| postMode | Controls whether to enable the automatic UB address update (post update) feature. Type: **PostLiteral**. For details, see [PostLiteral](../aux_data_types/PostLiteral.md). |
| dist | Move mode. Type: **MaskDist**. For details, see [Table 3 MaskDist parameter description](#table-3-maskdist-parameter-description). |

**Table 2** Parameter Description

| Parameter | Input/Output | Description |
|-----|-----|-----|
| mask | Output | Destination operand. Type: [MaskReg](../register_data_types/MaskReg.md). |
| srcAddr | Input/Output | Source operand, the UB start address. |
| offset | Input | &bull; When the data type is **AddrReg**, it is an address offset register. Call **CreateAddrReg** to set the address offset before the move. The actual UB address to be moved is **srcAddr** + the offset stored in the register. For the alignment constraint of the offset stored in the **AddrReg** register multiplied by sizeof(T), see [Table 3 MaskDist parameter description](#table-3-maskdist-parameter-description).<br>&bull; When the data type is **int32_t**, it is the address update step, in bytes. For the alignment constraint of **offset**, see [Table 3 MaskDist parameter description](#table-3-maskdist-parameter-description). Depending on the value of **postMode**, there are two cases:<br>&nbsp;&nbsp;&bull; **POST_MODE_NORMAL**: Not supported.<br>&nbsp;&nbsp;&bull; **POST_MODE_UPDATE**: The actual UB start address to be moved is **srcAddr**. After the move, the address update **srcAddr += offset** is performed. |

<a id="table-3-maskdist-parameter-description"></a>
**Table 3** MaskDist Parameter Description

| MaskDist Value | Meaning | Move Alignment Constraint (Byte) |
|-----|-----|-----|
| DIST_NORM | Normal mode. The amount of data moved is VL/8. | VL/8 |
| DIST_US | Upsampling mode. Each bit of data is moved twice, expanding VL/16 data to VL/8 for loading. | VL/16 |
| DIST_DS | Downsampling mode. Data is discarded every other bit, compressing VL/4 data to VL/8 for loading. | min(32, VL/4) |

## Data Type

The data types supported by the source operand are **b8**, **b16**, **b32**, and **b64**.

## Return Value<a name="section1575141714439"></a>

None

## Constraints<a name="section11585101304320"></a>

- The address constraint in the **Unified Buffer**, the alignment constraint of the offset, and the alignment constraint of the offset stored in the **AddrReg** register multiplied by sizeof(T) are related to the distribution mode **MaskDist**. For details about the address constraints, see [Table 3 MaskDist Parameter Description](#table-3-maskdist-parameter-description).
- The **regTrait** template parameter of **MaskReg** supports only **RegTraitNumOne**.

## Example<a name="section642mcpsimp"></a>

- Normal Move API
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

- PostUpdate Extended Move API
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

- API for storing offsets using AddrReg
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

