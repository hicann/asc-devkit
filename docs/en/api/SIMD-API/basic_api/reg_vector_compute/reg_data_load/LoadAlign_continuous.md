# Continuous Alignment Load (LoadAlign)<a name="ZH-CN_TOPIC_0000001955990973"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T22:44:21.565Z -->

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

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

LoadAlign moves data continuously from the Unified Buffer (UB) to [RegTensor](../register_data_types/RegTensor.md). It supports the single-load mode and the dual-load mode:

- Single-load mode: Reads VL amount of data from the UB and loads it into one RegTensor (VL).
- Dual-load mode: Reads 2*VL amount of data from the UB, performs interleaved data movement, and loads the elements at even indices and odd indices into two RegTensors (2\*VL), respectively.

Continuous alignment data movement provides three types of APIs: the normal data movement API, the PostUpdate extended data movement API, and the API that uses the [AddrReg](../register_data_types/AddrReg.md) register to store the offset.

The template parameter regTrait of RegTensor supports RegTraitNumOne and RegTraitNumTwo. The specific support is as follows:

| RegTensor template parameter regTrait | Normal data movement API | PostUpdate extended data movement API | API using AddrReg register to store the offset |
|-----|-----|-----|-----|
| RegTraitNumOne (single-load/dual-load) | After one data movement is completed, the UB address is not automatically updated, and the address must be updated manually in each iteration. | &bull; POST_MODE_NORMAL mode: Not supported.<br>&bull; POST_MODE_UPDATE mode: After one data movement is completed, the UB address is automatically updated, and the address does not need to be updated manually in each iteration. | In each iteration, call CreateAddrReg to manually set the address offset first, and then call the data movement instruction. |
| RegTraitNumTwo (single-load) | After one data movement is completed, the UB address is not automatically updated, and the address must be updated manually in each iteration. | &bull; POST_MODE_NORMAL mode: Not supported.<br>&bull; POST_MODE_UPDATE mode: After one data movement is completed, the UB address is automatically updated, and the address does not need to be updated manually in each iteration. | Not supported |
| RegTraitNumTwo (dual-load) | Not supported | Not supported | Not supported |

During data loading, the data distribution mode of the data movement can be configured through template parameters to implement functions such as broadcast, upsampling, downsampling, and decompression. For details about the distribution modes, see [Table 3 LoadDist parameter description (single-load mode)](#table-3-loaddist-parameter-description-single-load-mode) and [Table 6 LoadDist parameter description (dual-load mode)](#table6-loaddist-parameter-description-dual-load-mode).

[Figure 1 Continuous alignment load distribution mode diagram](#fig-loadalign-dist) shows the DIST_NORM, DIST_BRC_B16, and DIST_UNPACK_B16 distribution modes.

**Figure 1**  Data load distribution mode diagram<a id="fig-loadalign-dist"></a>  

![](../../../../figures/reg_loadalign_1.png)

For the invocation of the three types of data movement APIs for continuous alignment load/store within an iteration, see [Key Features](#key-features).

## Prototype<a name="section620mcpsimp"></a>

- Single-load mode: normal data movement API

    ```cpp
    template <typename T = DefaultType, LoadDist dist = LoadDist::DIST_NORM, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T* srcAddr)
    ```

- Single-load mode: PostUpdate extended data movement API

    ```cpp
    template <typename T = DefaultType, PostLiteral postMode, LoadDist dist = LoadDist::DIST_NORM, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T*& srcAddr, int32_t postUpdateStride)
    ```

- Single-load mode: API that uses [AddrReg](../register_data_types/AddrReg.md) to store the offset, which must be used together with the [CreateAddrReg](../register_data_types/AddrReg.md) API.

    ```cpp
    template <typename T = DefaultType, LoadDist dist = LoadDist::DIST_NORM, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T* srcAddr, AddrReg offset)
    ```

- Dual-load mode: normal data movement API

    ```cpp
    template <typename T = DefaultType, LoadDist dist, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg0, U& dstReg1, __ubuf__ T* srcAddr)
    ```

- Dual-load mode: PostUpdate extended data movement API

    ```cpp
    template <typename T = DefaultType, PostLiteral postMode, LoadDist dist, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg0, U& dstReg1, __ubuf__ T*& srcAddr, int32_t postUpdateStride)
    ```

- Dual-load mode: API that uses [AddrReg](../register_data_types/AddrReg.md) to store the offset, which must be used together with the [CreateAddrReg](../register_data_types/AddrReg.md) API.

    ```cpp
    template <typename T = DefaultType, LoadDist dist, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg0, U& dstReg1, __ubuf__ T* srcAddr, AddrReg offset)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description (single-load mode)

| Parameter | Description |
|-----|-----|
| T | Data type of the source operand. For supported data types, see [data type](#data-type). |
| postMode | Controls whether to enable the automatic UB address update function (post update). Type: PostLiteral. For specific parameter description, see [PostLiteral](../aux_data_types/PostLiteral.md). |
| dist | Data distribution mode. Type: LoadDist. For specific parameter description, see [Table 3 LoadDist parameter description (single-load mode)](#table-3-loaddist-parameter-description-single-load-mode). |
| U | RegTensor type of the destination operand. For example, RegTensor\<half>. It is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description (single-load mode)

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstReg | Output | Destination operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| srcAddr | Input/Output | Source operand, the UB start address. |
| postUpdateStride | Input | Address update stride, of type int32_t, in elements. For the alignment constraint of postUpdateStride * sizeof(T), see [Table 3 LoadDist parameter description (single-load mode)](#table-3-loaddist-parameter-description-single-load-mode). Depending on the value of postMode, there are two cases: <br>&bull; **POST_MODE_NORMAL**: Not supported. <br>&bull; **POST_MODE_UPDATE**: The actual UB start address for data movement is srcAddr. After the data movement, the address is updated as srcAddr += postUpdateStride. |
| offset | Input | Address offset register, of type AddrReg. Before data movement, call CreateAddrReg to set the address offset. The actual UB address for data movement is srcAddr + the offset stored in the register. For the alignment constraint of the offset stored in the AddrReg register * sizeof(T), see [Table 3 LoadDist parameter description (single-load mode)](#table-3-loaddist-parameter-description-single-load-mode). |

<a id="table-3-loaddist-parameter-description-single-load-mode"></a>
**Table 3** LoadDist parameter description (single-load mode)

| LoadDist value | Meaning | alignment constraint for data movement (Byte) |
|-----|-----|-----|
| DIST_NORM | Normal mode, moves VL data. See [Figure 1 Continuous alignment load distribution mode diagram](#fig-loadalign-dist). | 32 |
| DIST_BRC_B8 | Moves one b8-type data element and broadcasts it to all element positions. | 1 |
| DIST_BRC_B16 | Moves one b16-type data element and broadcasts it to all element positions. See [Figure 1 Continuous alignment load distribution mode diagram](#fig-loadalign-dist). | 2 |
| DIST_BRC_B32 | Moves one b32-type data element and broadcasts it to all element positions. | 4 |
| DIST_US_B8 | 2x upsampling. The amount of loaded data is VL/2, each input element is repeated twice, and the data type is b8. | min(32, VL/2) |
| DIST_US_B16 | 2x upsampling. The amount of loaded data is VL/2, each input element is repeated twice, and the data type is b16. | min(32, VL/2) |
| DIST_DS_B8 | 2x downsampling. The amount of loaded data is 2*VL, every other data element is retained, and the data type is b8. | 32 |
| DIST_DS_B16 | 2x downsampling. The amount of loaded data is 2*VL, every other data element is retained, and the data type is b16. | 32 |
| DIST_UNPACK_B8 | Decompression mode. Loads VL/2 data as unsigned integer u8, and appends one zero-valued element after each element, that is, unpacks to VL. <br>For example, VL=256B: <br>src: [0x00, 0x01, 0x02, 0x03, ..., 0xFF, ...]. <br>dst: [0x00, 0x00, 0x01, 0x00, 0x02, 0x00, ..., 0x7F, 0x00]. | min(32, VL/2) |
| DIST_UNPACK_B16 | Decompression mode. Loads VL/2 data as unsigned integer u16, and appends one zero-valued element after each element, that is, unpacks to VL. See [Figure 1 Continuous alignment load distribution mode diagram](#fig-loadalign-dist). <br>For example, VL=256B: <br>src: [0x0000, 0x0001, 0x0002, 0x0003, ..., 0x007F, ...]. <br>dst: [0x0000, 0x0000, 0x0001, 0x0000, 0x0002, 0x0000, ..., 0x003F, 0x0000]. | min(32, VL/2) |
| DIST_BLK | Reads one DataBlock (32B) and broadcasts it to VL. | 32 |
| DIST_E2B_B16 | Loads (VL/DataBlock) B16 data elements and broadcasts each element (16 bits) to one DataBlock (32B). | VL/16 |
| DIST_E2B_B32 | Loads (VL/DataBlock) B32 data elements and broadcasts each element (32 bits) to one DataBlock (32B). | VL/8 |
| DIST_UNPACK_B32 | Decompression mode. Loads VL/2 data as unsigned integer u32, and appends one zero-valued element after each element, that is, unpacks to VL. <br>For example, VL=256B: <br>src: [0x00000000, 0x00000001, 0x00000002, ..., 0x000003F, ...]. <br>dst: [0x00000000, 0x00000000, 0x00000001, 0x00000000, ..., 0x0000001F, 0x00000000]. | min(32, VL/2) |
| DIST_UNPACK4_B8 | Decompression mode. Loads VL/4 data as unsigned integer u8 and unpacks to VL, appending three zero-valued elements after each element. <br>For example, VL=256B: <br>src: [0x00, 0x01, 0x02, 0x03, ..., 0xFF, ...]. <br>dst: [0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,..., 0x3F, 0x00, 0x00, 0x00]. | min(32, VL/4) |

**Table 4** Template parameter description (dual-load mode)

| Parameter | Description |
|-----|-----|
| T | Data type of the source operand. For supported data types, see [data type](#data-type). |
| postMode | Controls whether to enable the automatic UB address update function (post update). Type: PostLiteral. For specific parameter description, see [PostLiteral](../aux_data_types/PostLiteral.md). |
| dist | Data movement mode. Type: LoadDist. For specific parameter description, see [Table 6 LoadDist parameter description (dual-load mode)](#table6-loaddist-parameter-description-dual-load-mode). |
| U | RegTensor type of the destination operand. For example, RegTensor\<half>. It is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 5** Parameter description (dual-load mode)

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstReg0 | Output | First destination operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| dstReg1 | Output | Second destination operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| srcAddr | Input/Output | Source operand, the UB start address. |
| postUpdateStride | Input | Address update stride, of type int32_t, in units of element count. **postUpdateStride** * sizeof(T) must be 32-byte aligned.<br>Depending on the value of **postMode**, there are two cases:<br>&bull; **POST_MODE_NORMAL**: Not supported.<br>&bull; **POST_MODE_UPDATE**: The actual UB start address for data movement is **srcAddr**. After the data movement, the address update **srcAddr** += **postUpdateStride** is performed. |
| offset | Input | Address offset register, of type **AddrReg**. Call **CreateAddrReg** before the data movement to set the address offset. The actual UB address for data movement is **srcAddr** + the offset stored in the register. The offset stored in the **AddrReg** register * sizeof(T) must be 32-byte aligned. |

<a id="table6-loaddist-parameter-description-dual-load-mode"></a>
**Table 6**  LoadDist parameter description (dual-load mode)

| LoadDist value | Meaning | alignment constraint for data movement (Byte) |
|-----|-----|-----|
| DIST_DINTLV_B8 | Dual-load mode, element-based interleaved data movement. Reads 2*VL data from src, stores elements with even indices into dst0, and stores elements with odd indices into dst1. The data type is b8. | 32 |
| DIST_DINTLV_B16 | Dual-load mode, element-based interleaved data movement. Reads 2*VL data from src, stores elements with even indices into dst0, and stores elements with odd indices into dst1. The data type is b16. | 32 |
| DIST_DINTLV_B32 | Dual-load mode, element-based interleaved data movement. Reads 2*VL data from src, stores elements with even indices into dst0, and stores elements with odd indices into dst1. The data type is b32. | 32 |

## Data Type

The data types of the destination operand and the source operand must be consistent.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumOne**, the supported data types are **b8**, **b16**, **b32**, and **b64**.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumTwo**, the supported data types are **complex32** and **b64**.

## Return Value

None

## Constraints<a name="section11585101304320"></a>

- The address constraints in the **Unified Buffer**, the **postUpdateStride** * sizeof(T) alignment constraint, and the offset * sizeof(T) alignment constraint stored in the **AddrReg** register are related to the distribution mode **LoadDist**. For specific address constraints, see [Table 3 LoadDist parameter description (single-load mode)](#table-3-loaddist-parameter-description-single-load-mode) and [Table 6 LoadDist parameter description (dual-load mode)](#table6-loaddist-parameter-description-dual-load-mode).
- Single-load mode: The **b64** data type supports only the **DIST_NORM** mode in **LoadDist**.
- When the **RegTensor** template parameter **RegTrait** is **RegTraitNumOne** and **RegTraitNumTwo**, the support is as follows:

    | **RegTensor** Template Parameter **RegTrait** Value | Supported APIs | supported data types |
    |-----|-----|-----|
    | **RegTraitNumOne** | All single-load and dual-load APIs | **b8**/**b16**/**b32**/**b64** |
    | **RegTraitNumTwo** | &bull; Single-load normal data movement API<br>&bull; Single-load **PostUpdate** extended data movement API | **complex32**/**b64** |

## Key Features<a name="section18316112542110"></a>

**Calling methods of the three data movement APIs for continuous alignment load/store**

[Figure 2 Calling methods of the three data movement APIs when regTrait is RegTraitNumOne](#fig-loadalign-regtrait) shows how the three data movement modes for continuous alignment load/store are called within an iteration:

- Normal data movement API: After one data movement is completed, the UB address is not automatically updated, and the address must be manually updated in each iteration.
- PostUpdate extended data movement API: After one data movement is completed, the UB address is automatically updated, and no manual address update is required in each iteration.
- API that uses AddrReg to store the offset: In each iteration, call **CreateAddrReg** first to manually set the address offset, and then call the data movement instruction.

<a id="fig-loadalign-regtrait"></a>
**Figure 2**  Calling methods of the three data movement APIs when regTrait is RegTraitNumOne  
![](../../../../figures/reg_loadstorealign.png)

## Example<a name="section642mcpsimp"></a>

- Single-load/single-store mode: normal data movement API

    ```cpp
    template <typename T>
    __simd_vf__ inline void ComputeMode01(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dstSize, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            mask = AscendC::Reg::UpdateMask<T>(dstSize);
            AscendC::Reg::LoadAlign(dstReg, srcAddr + i * oneRepeatSize);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

- Single-load/single-store mode: **PostUpdate** extended data movement API

    ```cpp
    template <typename T>
    __simd_vf__ inline void ComputeMode02(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dstSize, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            mask = AscendC::Reg::UpdateMask<T>(dstSize);
            AscendC::Reg::LoadAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dstReg, srcAddr, oneRepeatSize);
            AscendC::Reg::StoreAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dstAddr, dstReg, oneRepeatSize, mask);
        }
    }
    ```

- Single-load/single-store mode: using **AddrReg** to store the offset

    ```cpp
    template <typename T>
    __simd_vf__ inline void ComputeMode03(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();
        AscendC::Reg::AddrReg aReg;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            aReg = AscendC::Reg::CreateAddrReg<T>(i, oneRepeatSize);
            AscendC::Reg::LoadAlign(dstReg, srcAddr, aReg);
            AscendC::Reg::StoreAlign(dstAddr, dstReg, aReg, mask);
        }
    }
    ```

- Dual-load/dual-store mode: normal data movement API

    ```cpp
    template <typename T>
    __simd_vf__ inline void ComputeMode04(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg0;
        AscendC::Reg::RegTensor<T> srcReg1;
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadAlign<T, AscendC::Reg::LoadDist::DIST_DINTLV_B8>(srcReg0, srcReg1, srcAddr + i * oneRepeatSize);
            AscendC::Reg::StoreAlign<T, AscendC::Reg::StoreDist::DIST_INTLV_B8>(dstAddr + i * oneRepeatSize, srcReg0, srcReg1, mask);
        }
    }
    ```

- Dual-load/dual-store mode: using **AddrReg** to store the offset, which must be used together with the **CreateAddrReg** API

    ```cpp
    template <typename T>
    __simd_vf__ inline void ComputeMode05(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg0;
        AscendC::Reg::RegTensor<T> srcReg1;
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();
        AscendC::Reg::AddrReg aReg;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            aReg = AscendC::Reg::CreateAddrReg<T>(i, oneRepeatSize);
            AscendC::Reg::LoadAlign<T, AscendC::Reg::LoadDist::DIST_DINTLV_B8>(srcReg0, srcReg1, srcAddr, aReg);
            AscendC::Reg::StoreAlign<T, AscendC::Reg::StoreDist::DIST_INTLV_B8>(dstAddr, srcReg0, srcReg1, aReg, mask);
        }
    }
    ```

