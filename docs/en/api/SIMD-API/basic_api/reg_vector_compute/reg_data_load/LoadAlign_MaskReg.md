# MaskReg Load (LoadAlign)<a name="ZH-CN_TOPIC_0000001958227253"></a>

<!-- md-trans-meta sourceCommit=af3f6adafe94d121d34283378bfbe7e348767ea3 translatedAt=2026-11-05T03:22:32.413Z pushedAt=2026-11-18T01:25:30.923Z nl=2 -->

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
- Kirin 9050 Processor products: Supported
<!-- end id8 -->
<!-- npu="kirin1" id9 -->
- KirinDev0001 Processor products: Supported
<!-- end id9 -->
<!-- npu="kirin2" id10 -->
- KirinDev0002 Processor products: Supported
<!-- end id10 -->
<!-- npu="kirin3" id11 -->
- KirinDev0003 Processor products: Supported
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/LoadAlign_MaskReg_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is: `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

**LoadAlign** loads data from the Unified Buffer (UB) to [MaskReg](../register_data_types/MaskReg.md). There are three types of load APIs:

- Normal movement API: After completing one transfer, the UB address will not be automatically updated, and each iteration requires manual address update.
- PostUpdate Extended Move API: Two modes are available. For details, see [Parameters](#parameters):
    - **POST_MODE_NORMAL** mode: not supported.
    - **POST_MODE_UPDATE** mode: After a transfer is completed, the UB address is automatically updated, so the address does not need to be updated manually in each iteration.

- Scenario where the [AddrReg](../register_data_types/AddrReg.md) register stores the offset: In each iteration, call [CreateAddrReg](../register_data_types/AddrReg.md) first to manually set the address offset, and then call the move instruction.

When loading data, you can configure the data distribution mode through template parameters to implement functions such as upsampling and downsampling. For details about the distribution modes, see [Table 3 MaskDist Parameter Description](#table-3-maskdist-parameter-description).

[Figure 1](#fig-loadalign-maskreg-norm), [Figure 2](#fig-loadalign-maskreg-us), and [Figure 3](#fig-loadalign-maskreg-ds) show the DIST_NORM, DIST_US, and DIST_DS distribution modes.

**Figure 1**  DIST_NORM Mode Load<a id="fig-loadalign-maskreg-norm"></a>  
![](../../../../figures/dist_norm_mode_load.png)

**Figure 2**  DIST_US Mode Load<a id="fig-loadalign-maskreg-us"></a>  
![](../../../../figures/dist_us_mode_load.png)

**Figure 3**  DIST_DS Mode Load<a id="fig-loadalign-maskreg-ds"></a>  
![](../../../../figures/dist_ds_mode_load.png)

## Prototype<a name="section620mcpsimp"></a>

- Normal Transfer API

    ```cpp
    template <typename T, MaskDist dist = MaskDist::DIST_NORM>
    __simd_callee__ inline void LoadAlign(MaskReg& mask, __ubuf__ T* srcAddr)
    ```

- PostUpdate Extended Transfer API

    ```cpp
    template <typename T, PostLiteral postMode, MaskDist dist = MaskDist::DIST_NORM>
    __simd_callee__ inline void LoadAlign(MaskReg& mask, __ubuf__ T* &srcAddr, int32_t offset)
    ```

- API that uses **AddrReg** to store the offset, which must be used together with the **CreateAddrReg** API.

    ```cpp
    template <typename T, MaskDist dist = MaskDist::DIST_NORM>
    __simd_callee__ inline void LoadAlign(MaskReg& mask, __ubuf__ T* srcAddr, AddrReg offset)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|-----|-----|
| T | Data type of the source operand. For supported data types, see [Data Type](#data-type). |
| postMode | Controls whether to enable the UB address automatic update function (post update). Type: **PostLiteral**. For specific parameter description, see [PostLiteral](../aux_data_types/PostLiteral.md). |
| dist | Transfer mode. Type: **MaskDist**. For details, see [Table 3 MaskDist parameter description](#table-3-maskdist-parameter-description). |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|-----|-----|-----|
| mask | Output | Destination operand, type is [MaskReg](../register_data_types/MaskReg.md). |
| srcAddr | Input/Output | Source operand, UB start address. |
| offset | Input | &bull; When the data type is **AddrReg**, it is an address offset register. Call **CreateAddrReg** to set the address offset before the move. The actual UB address for the move is **srcAddr** + the offset stored in the register. For the alignment constraint of the offset stored in the **AddrReg** register * sizeof(T), see [Table 3 MaskDist Parameter Description](#table-3-maskdist-parameter-description).<br>&bull; When the data type is **int32_t**, it is the address update step, in bytes. For the alignment constraint of **offset**, see [Table 3 MaskDist Parameter Description](#table-3-maskdist-parameter-description). Depending on the value of **postMode**, there are two cases:<br>&nbsp;&nbsp;&bull; **POST_MODE_NORMAL**: not supported.<br>&nbsp;&nbsp;&bull; **POST_MODE_UPDATE**: The actual UB start address for the move is **srcAddr**, and after the move, the address update **srcAddr += offset** is performed. |

<a id="table-3-maskdist-parameter-description"></a>
**Table 3** MaskDist parameter description

| MaskDist Value | Meaning | Transfer Alignment Constraint (Byte) |
|-----|-----|-----|
| DIST_NORM | Normal mode. Moves VL/8 of data. | VL/8 |
| DIST_US | Upsampling mode. Each bit of data is moved twice, expanding VL/16 data to VL/8 for loading. | VL/16 |
| DIST_DS | Downsampling mode. Every other bit is discarded, compressing VL/4 data to VL/8 for loading. | min(32, VL/4) |

## Data Type

The data types supported by the source operand are: b8, b16, b32, b64.

## Return Value<a name="section1575141714439"></a>

None

## Constraints<a name="section11585101304320"></a>

- The address constraint in UB, the alignment constraint of **offset**, and the alignment constraint of the offset stored in the **AddrReg** register multiplied by **sizeof(T)** are related to the distribution mode **MaskDist**. For specific address constraints, see [Table 3 MaskDist Parameter Description](#table-3-maskdist-parameter-description).
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

