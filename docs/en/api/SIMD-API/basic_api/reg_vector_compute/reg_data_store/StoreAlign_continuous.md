# Continuous Alignment Move-out (StoreAlign)<a name="ZH-CN_TOPIC_0000001929071462"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T22:32:03.884Z -->

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

**StoreAlign** moves data continuously from [RegTensor](../register_data_types/RegTensor.md) to the Unified Buffer (UB). It supports the single move-out mode and the dual move-out mode:

- Single move-out mode: Moves the VL amount of data in one RegTensor (VL) out to the UB.
- Dual move-out mode: Moves the data of two RegTensors (2*VL) out to the UB in an interleaved manner, where elements at even indices and odd indices come from the two RegTensors respectively.

Continuous alignment move provides three types of APIs: the normal move API, the PostUpdate extended move API, and the API that uses the AddrReg register to store the offset.

The template parameter **regTrait** of RegTensor supports **RegTraitNumOne** and **RegTraitNumTwo**. The specific support is as follows:

| RegTensor Template Parameter regTrait | Normal Move API | PostUpdate Extended Move API | API Using AddrReg Register to Store Offset |
|-----|-----|-----|-----|
| RegTraitNumOne (single move-out) | After completing one move, the UB address will not automatically update; each iteration requires manual address update. | &bull; POST_MODE_NORMAL mode: Not supported.<br>&bull; POST_MODE_UPDATE mode: After completing one move, the UB address will automatically update; each iteration does not require manual address update. | In each iteration, you must first call **CreateAddrReg** to manually set the address offset, and then call the move instruction. |
| RegTraitNumOne (dual move-out) | After completing one move, the UB address will not automatically update; each iteration requires manual address update. | Not supported | In each iteration, you must first call **CreateAddrReg** to manually set the address offset, and then call the move instruction. |
| RegTraitNumTwo (single move-out) | After completing one move, the UB address will not automatically update; each iteration requires manual address update. | &bull; POST_MODE_NORMAL mode: Not supported.<br>&bull; POST_MODE_UPDATE mode: After completing one move, the UB address will automatically update; each iteration does not require manual address update. | Not supported |
| RegTraitNumTwo (dual move-out) | Not supported | Not supported | Not supported |

When moving data out, you can configure the data distribution mode through the template parameter to implement functions such as compression and moving out only the first element. For details about the distribution modes, refer to [Table 3 StoreDist Parameter Description (Single Move-out Mode)](#table-3-storedist-parameter-description-single-move-out-mode) and [Table 6 StoreDist Parameter Description (Dual Move-out Mode)](#table6-storedist-parameter-description-dual-move-out-mode). [Figure 1](#fig-storealign-dist) shows the DIST_NORM, DIST_FIRST_ELEMENT_B16, and DIST_PACK_B32 distribution modes.

**Figure 1**  Data move-out distribution mode illustration<a id="fig-storealign-dist"></a>

![Figure 1 Data move-out distribution mode diagram](../../../../figures/reg_storealign_1.png)

For the in-iteration calling methods of the three types of move APIs for continuous alignment move-in/move-out, refer to [Key Features](#key-features).

## Prototype<a name="section620mcpsimp"></a>

- Single Move-out Mode: Normal Move API

    ```cpp
    template <typename T = DefaultType, StoreDist dist = StoreDist::DIST_NORM, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg, MaskReg& mask)
    ```

- Single Move-out Mode: PostUpdate Extended Move API

    ```cpp
    template <typename T = DefaultType, PostLiteral postMode, StoreDist dist = StoreDist::DIST_NORM, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T*& dstAddr, U& srcReg, int32_t postUpdateStride, MaskReg& mask)
    ```

- Single Move-out Mode: Uses **AddrReg** to store the offset, which requires coordination with the **CreateAddrReg** API

    ```cpp
    template <typename T = DefaultType, StoreDist dist = StoreDist::DIST_NORM, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg, AddrReg offset, MaskReg& mask)
    ```

- Dual Move-out Mode: Normal Move API

    ```cpp
    template <typename T = DefaultType, StoreDist dist, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg0, U& srcReg1, MaskReg& mask)
    ```

- Dual Move-out Mode: Uses **AddrReg** to store the offset, which requires coordination with the **CreateAddrReg** API

    ```cpp
    template <typename T = DefaultType, StoreDist dist, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg0, U& srcReg1, AddrReg offset, MaskReg& mask)
    ```

## Parameter Description<a name="section622mcpsimp"></a>

**Table 1** Template Parameter Description (Single Move-out Mode)

| Parameter | Description |
|-----|-----|
| T | Data type of the destination operand. For supported data types, see [Data Type](#data-type). |
| postMode | Controls whether to enable the automatic UB address update function (post update). PostLiteral type. For specific parameter description, see [PostLiteral](../aux_data_types/PostLiteral.md). |
| dist | Move mode, StoreDist type. For specific parameter description, see [Table 3 StoreDist Parameter Description (Single Move-out Mode)](#table-3-storedist-parameter-description-single-move-out-mode). |
| U | RegTensor type of the source operand. For example, RegTensor\<half>, which is automatically deduced by the compiler and does not need to be manually specified by the user. |

**Table 2** Parameter Description (Single Move-out Mode)

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstAddr | Input/Output | Destination operand, UB start address. |
| srcReg | Input | Source operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| postUpdateStride | Input | Address update stride, int32_t type, unit: number of elements. For the alignment constraint of postUpdateStride * sizeof(T), see [Table 3 StoreDist Parameter Description (Single Move-out Mode)](#table-3-storedist-parameter-description-single-move-out-mode). Depending on the value of postMode, there are two cases:<br>&bull; POST_MODE_NORMAL: Not supported.<br>&bull; POST_MODE_UPDATE: The actual UB start address for the move is dstAddr, and after the move, the address update dstAddr += postUpdateStride is performed. |
| offset | Input | Address offset register, AddrReg type. CreateAddrReg must be called before the move to set the address offset. The actual UB address for the move is dstAddr + the offset stored in the register. For the alignment constraint of the offset stored in the AddrReg register * sizeof(T), see [Table 3 StoreDist Parameter Description (Single Move-out Mode)](#table-3-storedist-parameter-description-single-move-out-mode).|
| mask | Input | Valid indication of the source operand element operation. For details, see [MaskReg](../register_data_types/MaskReg.md). |

<a id="table-3-storedist-parameter-description-single-move-out-mode"></a>
**Table 3** StoreDist Parameter Description (Single Move-out Mode)

| StoreDist | Meaning | Alignment Constraint (Byte) |
|-----|-----|-----|
| DIST_NORM_B8 | Normal mode, moves VL data, data type b8. | 32 |
| DIST_NORM_B16 | Normal mode, moves VL data, data type b16. | 32 |
| DIST_NORM_B32 | Normal mode, moves VL data, data type b32. | 32 |
| DIST_FIRST_ELEMENT_B8 | Ignores mask and moves the first element of src to dst, data type b8. | 1 |
| DIST_FIRST_ELEMENT_B16 | Ignores mask and moves the first element of src to dst, data type b16. See [Figure 1 Data Move-out Distribution Mode](#fig-storealign-dist). | 2 |
| DIST_FIRST_ELEMENT_B32 | Ignores mask and moves the first element of src to dst, data type b32. | 4 |
| DIST_PACK_B16 | Compression mode, data type b16. Based on mask, the lower half of the bit data of the valid elements in src is stored contiguously in dst.<br>Example: data type uint16_t:<br>src: [0x3210, 0x7654, 0xBA98, 0xFEDC, ..., 0xFEDC, 0xBA98, 0x7654, 0x3210].<br>dst: [0x5410, 0xDC98, ... 0x98DC, 0x1054]. | min(32, VL/2) |
| DIST_PACK_B32 | Compression mode, data type b32. Based on mask, the lower half of the bit data of the valid elements in src is stored contiguously in dst. See [Figure 1 Data Move-out Distribution Mode](#fig-storealign-dist). | min(32, VL/2) |
| DIST_PACK_B64 | Compression mode, data type b64. Based on mask, the lower half of the bit data of the valid elements in src is stored contiguously in dst. | min(32, VL/2) |
| DIST_PACK4_B32 | Compression mode, data type b32. Based on mask, the lower 8 bits (one quarter) of the valid elements in src are stored contiguously in dst. | min(32, VL/4) |
| DIST_NORM | Normal mode, moves VL data, supports data types b8/b16/b32. The system automatically selects DIST_NORM_B8, DIST_NORM_B16, or DIST_NORM_B32 based on the template parameter T. See [Figure 1 Data Move-out Distribution Mode](#fig-storealign-dist). | 32 |

**Table 4** Template Parameter Description (Dual Move-out Mode)

| Parameter | Description |
|-----|-----|
| T | Data type of the destination operand. For supported data types, see [Data Type](#data-type). |
| dist | Move mode, StoreDist type. For specific parameter description, see [Table 6 StoreDist Parameter Description (Dual Move-out Mode)](#table6-storedist-parameter-description-dual-move-out-mode). |
| U | RegTensor type of the source operand. For example, RegTensor\<half>, which is automatically deduced by the compiler and does not need to be manually specified by the user. |

**Table 5** Parameter Description (Dual Move-out Mode)

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstAddr | Input/Output | Destination operand, the start address of the UB. |
| srcReg0 | Input | First source operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| srcReg1 | Input | Second source operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| offset | Input | Address offset, of type AddrReg. Before the move, call CreateAddrReg to set the address offset. The actual UB address for the move is dstAddr + the offset stored in offset, and the address will not be updated after the move. The offset stored in the AddrReg register multiplied by sizeof(T) must be 32-byte aligned. |
| mask | Input | Valid indication for the operation on source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

<a id="table6-storedist-parameter-description-dual-move-out-mode"></a>
**Table 6**  StoreDist parameter description (dual move-out mode)

| StoreDist | Meaning | Alignment Constraint (Byte) |
|-----|-----|-----|
| DIST_INTLV_B8 | Dual move-out mode, with data type b8. The mask is ignored, and the elements in src0 and src1 are stored interleaved in dst. The dst length must be 2*VL.<br>Example: data type uint8_t:<br>src0: [0, 2, 4, 6, ... 254, 0, 2, 4, ..., 252, 254].<br>src1: [1, 3, 5, 7, ..., 255, 1, 3, 5, ..., 253, 255].<br>dst: [0, 1, 2, 3, ..., 254, 255, 0, 1, 2, 3, ..., 253, 254, 255]. | 32 |
| DIST_INTLV_B16 | Dual move-out mode, with data type b16. The mask is ignored, and the elements in src0 and src1 are stored interleaved in dst. The dst length must be 2*VL. | 32 |
| DIST_INTLV_B32 | Dual move-out mode, with data type b32. The mask is ignored, and the elements in src0 and src1 are stored interleaved in dst. The dst length must be 2*VL. | 32 |

## Data Type

The data types of the destination operand and the source operand must be consistent.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumOne**, the supported data types are: **b8**, **b16**, **b32**, **b64**.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumTwo**, the supported data types are: **complex32**, **b64**.

## Return Value

None

## Constraints<a name="section587915597213"></a>

- The address constraint in **Unified Buffer**, the alignment constraint of **postUpdateStride** * sizeof(T), and the alignment constraint of the offset stored in the **AddrReg** register * sizeof(T) are related to the distribution mode **StoreDist**. For specific address constraints, refer to [Table 3 StoreDist Parameter Description (Single Move-out Mode)](#table-3-storedist-parameter-description-single-move-out-mode) and [Table 6 StoreDist Parameter Description (Dual Move-out Mode)](#table6-storedist-parameter-description-dual-move-out-mode).

- In single move-out mode, the **b64** data type supports only the **DIST_NORM** and **DIST_PACK_B64** modes in **StoreDist**.
- When the **RegTensor** template parameter **RegTrait** is **RegTraitNumOne** or **RegTraitNumTwo**, the support is as follows:
    | **RegTensor** Template Parameter **RegTrait** Value | Supported APIs | Supported Data Types |
    |-----|-----|-----|
    | **RegTraitNumOne** | All single move-out and dual move-out APIs | **b8/b16/b32/b64** |
    | **RegTraitNumTwo** | &bull; Single move-out Normal Move API<br>&bull; Single move-out **PostUpdate** Extended Move API | **complex32/b64** |

## Key Features

**Calling Methods of the Three Types of Move APIs for Continuous Alignment Load/Store**

Figure 2 shows the calling methods of the three move modes for continuous alignment load/store within an iteration:

- Normal move API: After completing one move, the UB address will not automatically update, and each iteration requires manual address update.
- PostUpdate extended move API: After completing one move, the UB address will automatically update, and no manual address update is required in each iteration.
- API using AddrReg to store the offset: In each iteration, you must first call CreateAddrReg to manually set the address offset, and then call the move instruction.

**Figure 2** Calling methods of the three move modes when regTrait is RegTraitNumOne<a id="fig-storealign-regtrait"></a>

![](../../../../figures/reg_loadstorealign.png)

## Calling Example<a name="section5497339111410"></a>

- Single move-in/single move-out mode: normal move API

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

- Single move-in/single move-out mode: PostUpdate extended move API

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

- Single move-in/single move-out mode: use AddrReg to store the offset

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

- Dual move-in/dual move-out mode: normal move API

    ```cpp
    template <typename T>
    __simd_vf__ inline void ComputeMode04(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg0;
        AscendC::Reg::RegTensor<T> srcReg1;
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<uint8_t, AscendC::Reg::MaskPattern::ALL>();
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadAlign<T, AscendC::Reg::LoadDist::DIST_DINTLV_B8>(srcReg0, srcReg1, srcAddr + i * oneRepeatSize);
            AscendC::Reg::StoreAlign<T, AscendC::Reg::StoreDist::DIST_INTLV_B8>(dstAddr + i * oneRepeatSize, srcReg0, srcReg1, mask);
        }
    }
    ```

- Dual move-in/dual move-out mode: use AddrReg to store the offset, which requires coordination with the CreateAddrReg API

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

