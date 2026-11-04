# Continuous Alignment Load (LoadAlign)<a name="ZH-CN_TOPIC_0000001955990973"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:40:52.498Z pushedAt=2026-10-26T07:41:44.572Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/LoadAlign_continuous_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

**LoadAlign** loads data continuously from Unified Buffer (UB) to [RegTensor](../register_data_types/RegTensor.md). It supports single-load mode and dual-load mode:

- Single-load mode: Reads VL amount of data from UB and loads it into one RegTensor (VL).
- Dual-load mode: Reads 2*VL amount of data from UB, performs interleaved load, and loads even-indexed and odd-indexed elements into two RegTensors (2\*VL) respectively.

Continuous alignment load provides three types of APIs: normal load API, **PostUpdate** extended load API, and API that uses [AddrReg](../register_data_types/AddrReg.md) registers to store offsets.

The template parameter **regTrait** of RegTensor supports **RegTraitNumOne** and **RegTraitNumTwo**. The specific support is as follows:

| RegTensor template parameter regTrait | Normal load API | PostUpdate extended load API | API using AddrReg registers to store offsets |
|-----|-----|-----|-----|
| **RegTraitNumOne** (single-load/dual-load) | After one load is completed, the UB address is not automatically updated, and the address must be updated manually in each iteration. | &bull; **POST_MODE_NORMAL** mode: not supported.<br>&bull; **POST_MODE_UPDATE** mode: After one load is completed, the UB address is automatically updated, and the address does not need to be updated manually in each iteration. | In each iteration, call **CreateAddrReg** to manually set the address offset first, and then call the load instruction. |
| **RegTraitNumTwo** (single-load) | After one load is completed, the UB address is not automatically updated, and the address must be updated manually in each iteration. | &bull; **POST_MODE_NORMAL** mode: not supported.<br>&bull; **POST_MODE_UPDATE** mode: After one load is completed, the UB address is automatically updated, and the address does not need to be updated manually in each iteration. | Not supported |
| **RegTraitNumTwo** (dual-load) | Not supported | Not supported | Not supported |

When loading data, the data distribution mode can be configured through template parameters to implement functions such as broadcast, upsampling, downsampling, and decompression. For details about distribution modes, see [Table 3 LoadDist parameter description (single-load mode)](#table-3-loaddist-parameter-description-single-load-mode) and [Table 6 LoadDist parameter description (dual-load mode)](#Table-6-loaddist-parameter-description-dual-load-mode).

[Figure 1 Continuous alignment load distribution mode diagram](#fig-loadalign-dist) shows the **DIST_NORM**, **DIST_BRC_B16**, and **DIST_UNPACK_B16** distribution modes.

**Figure 1**  Data load distribution mode diagram<a id="fig-loadalign-dist"></a>  

![](../../../../figures/reg_loadalign_1.png)

For how to call the three types of load/store APIs for continuous aligned movement within an iteration, see [key features](#key-features).

## Prototype<a name="section620mcpsimp"></a>

- Single-load mode: normal load API.

    ```cpp
    template <typename T = DefaultType, LoadDist dist = LoadDist::DIST_NORM, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T* srcAddr)
    ```

- Single-load mode: **PostUpdate** extended load API.

    ```cpp
    template <typename T = DefaultType, PostLiteral postMode, LoadDist dist = LoadDist::DIST_NORM, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T*& srcAddr, int32_t postUpdateStride)
    ```

- Single-load mode: API that uses [AddrReg](../register_data_types/AddrReg.md) to store the offset, which must be used together with the [CreateAddrReg](../register_data_types/AddrReg.md) API.

    ```cpp
    template <typename T = DefaultType, LoadDist dist = LoadDist::DIST_NORM, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T* srcAddr, AddrReg offset)
    ```

- Dual-load mode: normal load API.

    ```cpp
    template <typename T = DefaultType, LoadDist dist, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg0, U& dstReg1, __ubuf__ T* srcAddr)
    ```

- Dual-load mode: **PostUpdate** extended load API.

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
| T | Data type of the source operand. For supported data types, see [Data Types](#data-types). |
| postMode | Controls whether to enable the UB address auto-update function (post update). PostLiteral type. For details, see [PostLiteral](../aux_data_types/PostLiteral.md). |
| dist | Data distribution mode, LoadDist type. For details, see [Table 3 LoadDist parameter description (single-load mode)](#table-3-loaddist-parameter-description-single-load-mode). |
| U | RegTensor type of the destination operand. For example, RegTensor\<half>, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description (single-load mode)

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstReg | Output | Destination operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| srcAddr | Input/Output | Source operand, UB start address. |
| postUpdateStride | Input | Address update stride, int32_t type, unit: number of elements. For the alignment constraint on postUpdateStride * sizeof(T), see [Table 3 LoadDist parameter description (single-load mode)](#table-3-loaddist-parameter-description-single-load-mode). Depending on the value of postMode, there are two cases:<br>&bull; POST_MODE_NORMAL: not supported.<br>&bull; POST_MODE_UPDATE: The actual load UB start address is srcAddr, and after the load, the address update srcAddr += postUpdateStride is performed. |
| offset | Input | Address offset register, AddrReg type. Before the load, call CreateAddrReg to set the address offset. The actual load UB address is srcAddr + the offset stored in the register. For the alignment constraint on the offset stored in the AddrReg register multiplied by sizeof(T), see [Table 3 LoadDist parameter description (single-load mode)](#table-3-loaddist-parameter-description-single-load-mode). |

<a id="table-3-loaddist-parameter-description-single-load-mode"></a>
**Table 3** LoadDist parameter description (single-load mode)

| LoadDist value | Meaning | Alignment Constraint (Byte) |
|-----|-----|-----|
| DIST_NORM | Normal mode, loads VL data. See [Figure 1 Continuous alignment load distribution mode diagram](#fig-loadalign-dist). | 32 |
| DIST_BRC_B8 | Loads one piece of b8 data and broadcasts it to all element positions. | 1 |
| DIST_BRC_B16 | Loads one piece of b16 data and broadcasts it to all element positions. See [Figure 1 Continuous alignment load distribution mode diagram](#fig-loadalign-dist). | 2 |
| DIST_BRC_B32 | Loads one piece of b32 data and broadcasts it to all element positions. | 4 |
| DIST_US_B8 | 2x upsampling of data, loads VL/2 amount of data, each input element is repeated twice, data type is b8. | min(32, VL/2) |
| DIST_US_B16 | 2x upsampling of data, loads VL/2 amount of data, each input element is repeated twice, data type is b16. | min(32, VL/2) |
| DIST_DS_B8 | 2x downsampling of data, loads 2*VL amount of data, every other data element is retained, data type is b8. | 32 |
| DIST_DS_B16 | 2x downsampling of data, loads 2*VL amount of data, every other data element is retained, data type is b16. | 32 |
| DIST_UNPACK_B8 | Decompression mode, loads VL/2 data as unsigned integer u8, and appends one zero-valued element after each element, that is, unpacks to VL.<br>For example, VL=256B:<br>src: [0x00, 0x01, 0x02, 0x03, ..., 0xFF, ...].<br>dst: [0x00, 0x00, 0x01, 0x00, 0x02, 0x00, ..., 0x7F, 0x00]. | min(32, VL/2) |
| DIST_UNPACK_B16 | Decompression mode, loads VL/2 data as unsigned integer u16, and appends one zero-valued element after each element, that is, unpacks to VL. See [Figure 1 Continuous alignment load distribution mode diagram](#fig-loadalign-dist).<br>For example, VL=256B:<br>src: [0x0000, 0x0001, 0x0002, 0x0003, ..., 0x007F, ...].<br>dst: [0x0000, 0x0000, 0x0001, 0x0000, 0x0002, 0x0000, ..., 0x003F, 0x0000]. | min(32, VL/2) |
| DIST_BLK | Reads one DataBlock (32B) and broadcasts it to VL. | 32 |
| DIST_E2B_B16 | Loads (VL/DataBlock) pieces of B16 data and broadcasts each element (16 bits) to one DataBlock (32B). | VL/16 |
| DIST_E2B_B32 | Loads (VL/DataBlock) pieces of B32 data and broadcasts each element (32 bits) to one DataBlock (32B). | VL/8 |
| DIST_UNPACK_B32 | Decompression mode, loads VL/2 data as unsigned integer u32, and appends one zero-valued element after each element, that is, unpacks to VL.<br>For example, VL=256B:<br>src: [0x00000000, 0x00000001, 0x00000002, ..., 0x000003F, ...].<br>dst: [0x00000000, 0x00000000, 0x00000001, 0x00000000, ..., 0x0000001F, 0x00000000]. | min(32, VL/2) |
| DIST_UNPACK4_B8 | Decompression mode, loads VL/4 data as unsigned integer u8, unpacks to VL, and appends three zero-valued elements after each element.<br>For example, VL=256B:<br>src: [0x00, 0x01, 0x02, 0x03, ..., 0xFF, ...].<br>dst: [0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,..., 0x3F, 0x00, 0x00, 0x00]. | min(32, VL/4) |

**Table 4** Template parameter description (dual-load mode)

| Parameter | Description |
|-----|-----|
| T | Data type of the source operand. For supported data types, see [Data Types](#data-types). |
| postMode | Controls whether to enable the UB address auto-update function (post update). PostLiteral type. For details, see [PostLiteral](../aux_data_types/PostLiteral.md). |
| dist | Load mode, LoadDist type. For details, see [Table 6 LoadDist parameter description (dual-load mode)](#Table-6-loaddist-parameter-description-dual-load-mode). |
| U | RegTensor type of the destination operand. For example RegTensor\<half>, it is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 5** Parameter description (dual-load mode)

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstReg0 | Output | First destination operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| dstReg1 | Output | Second destination operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| srcAddr | Input/Output | Source operand, UB start address. |
| postUpdateStride | Input | Address update stride, of type int32_t, unit: number of elements. **postUpdateStride** * sizeof(T) must be 32-byte aligned.<br>Depending on the value of **postMode**, there are two cases:<br>&bull; **POST_MODE_NORMAL**: not supported.<br>&bull; **POST_MODE_UPDATE**: the actual load UB start address is **srcAddr**, and after the load, the address update **srcAddr** += **postUpdateStride** is performed. |
| offset | Input | Address offset register, of type **AddrReg**. Call **CreateAddrReg** before the load to set the address offset. The actual load UB address is **srcAddr** + the offset stored in the register. The offset stored in the **AddrReg** register * sizeof(T) must be 32-byte aligned. |

<a id="Table-6-loaddist-parameter-description-dual-load-mode"></a>
**Table 6**  LoadDist parameter description (dual-load mode)

| LoadDist value | Meaning | alignment constraint (Byte) |
|-----|-----|-----|
| **DIST_DINTLV_B8** | Dual-load mode, element-based interleaved load. Reads 2*VL data from **src**, stores elements with even indices into **dst0** and elements with odd indices into **dst1**. The data type is b8. | 32 |
| **DIST_DINTLV_B16** | Dual-load mode, element-based interleaved load. Reads 2*VL data from **src**, stores elements with even indices into **dst0** and elements with odd indices into **dst1**. The data type is b16. | 32 |
| **DIST_DINTLV_B32** | Dual-load mode, element-based interleaved load. Reads 2*VL data from **src**, stores elements with even indices into **dst0** and elements with odd indices into **dst1**. The data type is b32. | 32 |

## Data Types

The data types of the destination operand and the source operand must be consistent.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumOne**, the supported data types are: b8, b16, b32, b64.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumTwo**, the supported data types are: complex32, b64.

## Return Value

None

## Constraints<a name="section11585101304320"></a>

- The address constraint in UB, the postUpdateStride * sizeof(T) alignment constraint, and the alignment constraint of the offset stored in the AddrReg register * sizeof(T) are related to the LoadDist distribution mode. For specific address constraints, see [Table 3 LoadDist parameter description (single-load mode)](#table-3-loaddist-parameter-description-single-load-mode) and [Table 6 LoadDist parameter description (dual-load mode)](#Table-6-loaddist-parameter-description-dual-load-mode).
- When RegTrait is RegTraitNumOne, in the single-load normal mode (DIST_NORM), if the remaining space of the source UB is less than 256B, a UB out-of-bounds access occurs.
- Single-load mode: the b64 data type supports only the DIST_NORM mode in LoadDist.
- When the RegTensor template parameter RegTrait is RegTraitNumOne or RegTraitNumTwo, the support is as follows:

    | RegTensor template parameter RegTrait value | Supported APIs | Supported data types |
    |-----|-----|-----|
    | RegTraitNumOne | All single-load and dual-load APIs | b8/b16/b32/b64 |
    | RegTraitNumTwo | &bull; Single-load normal load API<br>&bull; Single-load PostUpdate extended load API | complex32/b64 |

## Key Features<a name="section18316112542110"></a>

**Calling Methods of the Three Types of Load APIs for Continuous Alignment Load/Store**

[Figure 2 Calling methods of the three types of load APIs when regTrait is RegTraitNumOne](#fig-loadalign-regtrait) shows how the three movement modes for continuous aligned movement are called within an iteration:

- Normal load API: After a load is completed, the UB address is not automatically updated, so the address must be updated manually in each iteration.
- PostUpdate extended load API: After a load is completed, the UB address is automatically updated, so no manual address update is required in each iteration.
- API using AddrReg to store the storage offset: In each iteration, call **CreateAddrReg** first to manually set the address offset, and then call the load instruction.

<a id="fig-loadalign-regtrait"></a>
**Figure 2**  Calling methods of the three types of load APIs when regTrait is RegTraitNumOne  
![](../../../../figures/reg_loadstorealign.png)

## Example<a name="section642mcpsimp"></a>

- Single-load/single-store mode: normal load API.

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

- Single-load/single-store mode: **PostUpdate** extended movement API.

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

- Single-load/single-store mode: use **AddrReg** to store the offset.

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

- Dual-load/dual-store mode: normal movement API.

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

- Dual-load/dual-store mode: use **AddrReg** to store the offset, which requires the **CreateAddrReg** API.

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
