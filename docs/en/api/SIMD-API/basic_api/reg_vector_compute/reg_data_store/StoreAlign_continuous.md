# Continuous Alignment Store (StoreAlign)<a name="ZH-CN_TOPIC_0000001929071462"></a>

<!-- md-trans-meta sourceCommit=af3f6adafe94d121d34283378bfbe7e348767ea3 translatedAt=2026-11-05T03:34:54.373Z pushedAt=2026-11-18T01:25:30.942Z nl=2 -->

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
- Kirin 9050 processor family products: Supported
<!-- end id8 -->
<!-- npu="kirin1" id9 -->
- KirinDev0001 processor family products: Supported
<!-- end id9 -->
<!-- npu="kirin2" id10 -->
- KirinDev0002 processor family products: Supported
<!-- end id10 -->
<!-- npu="kirin3" id11 -->
- KirinDev0003 processor family products: Supported
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reg_data_store/StoreAlign_continuous_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is: `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

StoreAlign transfers data continuously from [RegTensor](../register_data_types/RegTensor.md) to the Unified Buffer (UB), supporting both Single-Out Mode and Dual-Out Mode:

- Single-Out Mode: Transfers the VL amount of data in one RegTensor (VL) to the UB.
- Dual-Out Mode: Interleaves the data of two RegTensors (2*VL) into the UB, where elements at even and odd indices come from the two RegTensors respectively.

There are three types of APIs for continuous alignment transfer: the normal movement API, the PostUpdate extended movement API, and the API that uses the AddrReg register to store the offset.

The template parameter **regTrait** of RegTensor supports **RegTraitNumOne** and **RegTraitNumTwo**. The specific support is as follows:

| RegTensor Template Parameter regTrait | Normal Transfer API | PostUpdate Extended Transfer API | API Using AddrReg Register to Store Offset |
|-----|-----|-----|-----|
| RegTraitNumOne (Single-Out) | After completing one transfer, the UB address will not be updated automatically, and the address must be updated manually in each iteration. | &bull; POST_MODE_NORMAL mode: not supported.<br>&bull; POST_MODE_UPDATE mode: After completing one transfer, the UB address is updated automatically, and no manual address update is required in each iteration. | In each iteration, call CreateAddrReg to manually set the address offset before calling the transfer instruction. |
| RegTraitNumOne (Dual-Out) | After completing one transfer, the UB address will not be updated automatically, and the address must be updated manually in each iteration. | Not supported. | In each iteration, call CreateAddrReg to manually set the address offset before calling the transfer instruction. |
| RegTraitNumTwo (Single-Out) | After completing one transfer, the UB address will not be updated automatically, and the address must be updated manually in each iteration. | &bull; POST_MODE_NORMAL mode: not supported.<br>&bull; POST_MODE_UPDATE mode: After completing one transfer, the UB address is updated automatically, and no manual address update is required in each iteration. | Not supported. |
| RegTraitNumTwo (Dual-Out) | Not supported. | Not supported. | Not supported. |

During data transfer out, the data distribution mode can be configured through template parameters to implement functions such as compression and transferring only the first element. For details about the distribution modes, see [Table 3 StoreDist Parameter Description (Single-Out Mode)](#table-3-storedist-parameter-description-single-out-mode) and [Table 6 StoreDist Parameter Description (Dual-Out Mode)](#table-6-storedist-parameter-description-dual-out-mode). [Figure 1](#fig-storealign-dist) shows the DIST_NORM, DIST_FIRST_ELEMENT_B16, and DIST_PACK_B32 distribution modes.

**Figure 1**  Data transfer-out distribution mode diagram<a id="fig-storealign-dist"></a>

![Figure 1 Data transfer-out distribution mode](../../../../figures/reg_storealign_1.png)

For the calling methods of the three types of continuous alignment transfer-in/transfer-out APIs within an iteration, see [Key Features](#key-features).

## Prototype<a name="section620mcpsimp"></a>

- Single-Out Mode: Normal movement API

    ```cpp
    template <typename T = DefaultType, StoreDist dist = StoreDist::DIST_NORM, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg, MaskReg& mask)
    ```

- Single-Out Mode: **PostUpdate** extended movement API

    ```cpp
    template <typename T = DefaultType, PostLiteral postMode, StoreDist dist = StoreDist::DIST_NORM, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T*& dstAddr, U& srcReg, int32_t postUpdateStride, MaskReg& mask)
    ```

- Single-Out Mode: Uses **AddrReg** to store the offset and requires coordination with the **CreateAddrReg** API.

    ```cpp
    template <typename T = DefaultType, StoreDist dist = StoreDist::DIST_NORM, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg, AddrReg offset, MaskReg& mask)
    ```

- Dual-Out Mode: Normal movement API

    ```cpp
    template <typename T = DefaultType, StoreDist dist, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg0, U& srcReg1, MaskReg& mask)
    ```

- Dual-Out Mode: Uses **AddrReg** to store the offset and requires coordination with the **CreateAddrReg** API.   

    ```cpp
    template <typename T = DefaultType, StoreDist dist, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg0, U& srcReg1, AddrReg offset, MaskReg& mask)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template Parameter Description (Single-Out Mode)

| Parameter | Description |
|-----|-----|
| T | Data type of the destination operand. For supported data types, see [Data Type](#data-type).  |
| postMode | Controls whether to enable the UB address automatic update function (post update). Type: **PostLiteral**. For specific parameter description, see [PostLiteral](../aux_data_types/PostLiteral.md). |
| dist | Transfer mode. Type: **StoreDist**. For specific parameter description, see [Table 3 StoreDist Parameter Description (Single-Out Mode)](#table-3-storedist-parameter-description-single-out-mode). |
| U | **RegTensor** type of the source operand. For example RegTensor\<half>, which is automatically deduced by the compiler and does not need to be manually specified by the user. |

**Table 2**  Parameter Description (Single-Out Mode)

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstAddr | Input/Output | Destination operand, the start address of the UB. |
| srcReg | Input | Source operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| postUpdateStride | Input | Address update stride, of type **int32_t**, in elements. For the alignment constraint of postUpdateStride * sizeof(T), see [Table 3 StoreDist Parameter Description (Single-Out Mode)](#table-3-storedist-parameter-description-single-out-mode). Depending on the value of **postMode**, there are two cases:<br>&bull; **POST_MODE_NORMAL**: not supported.<br>&bull; **POST_MODE_UPDATE**: The actual transfer UB start address is **dstAddr**, and after the transfer, the address update dstAddr += postUpdateStride is performed. |
| offset | Input | Address offset register, of type **AddrReg**. Before the transfer, call **CreateAddrReg** to set the address offset. The actual transfer UB address is dstAddr + the offset stored in the register. For the alignment constraint of the offset stored in the **AddrReg** register * sizeof(T), see [Table 3 StoreDist Parameter Description (Single-Out Mode)](#table-3-storedist-parameter-description-single-out-mode).|
| mask | Input | Valid indicator for the operation on the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

<a id="table-3-storedist-parameter-description-single-out-mode"></a>
**Table 3**  StoreDist Parameter Description (Single-Out Mode)

| StoreDist | Meaning | Alignment Constraint (Byte) |
|-----|-----|-----|
| DIST_NORM_B8 | Normal mode. Transfers VL data, with data type **b8**. | 32 |
| DIST_NORM_B16 | Normal mode. Transfers VL data, with data type **b16**. | 32 |
| DIST_NORM_B32 | Normal mode. Transfers VL data, with data type **b32**. | 32 |
| DIST_FIRST_ELEMENT_B8 | Ignores **mask** and transfers the first element of **src** to **dst**, with data type **b8**. | 1 |
| DIST_FIRST_ELEMENT_B16 | Ignores **mask** and transfers the first element of **src** to **dst**, with data type **b16**. See [Figure 1 Data Transfer Out Distribution Mode](#fig-storealign-dist). | 2 |
| DIST_FIRST_ELEMENT_B32 | Ignores **mask** and transfers the first element of **src** to **dst**, with data type **b32**. | 4 |
| DIST_PACK_B16 | Compression mode, with data type **b16**. Based on **mask**, writes the lower half of the bit data of the valid elements in **src** to the corresponding compressed positions in **dst**.<br>Example: data type **uint16_t**:<br>src: [0x3210, 0x7654, 0xBA98, 0xFEDC, ..., 0xFEDC, 0xBA98, 0x7654, 0x3210].<br>dst: [0x5410, 0xDC98, ... 0x98DC, 0x1054]. | min(32, VL/2) |
| DIST_PACK_B32 | Compression mode, with data type **b32**. Based on **mask**, writes the lower half of the bit data of the valid elements in **src** to the corresponding compressed positions in **dst**. See [Figure 1 Data Transfer Out Distribution Mode](#fig-storealign-dist). | min(32, VL/2) |
| DIST_PACK_B64 | Compression mode, with data type **b64**. Based on **mask**, writes the lower half of the bit data of the valid elements in **src** to the corresponding compressed positions in **dst**. | min(32, VL/2) |
| DIST_PACK4_B32 | Compression mode, with data type **b32**. Based on **mask**, writes the lower 8 bits (one quarter) of the valid elements in **src** to the corresponding compressed positions in **dst**. | min(32, VL/4) |
| DIST_NORM | Normal mode. Transfers VL data, supporting data types **b8**/**b16**/**b32**. The system automatically selects **DIST_NORM_B8**, **DIST_NORM_B16**, or **DIST_NORM_B32** based on the template parameter **T**. See [Figure 1 Data Transfer Out Distribution Mode](#fig-storealign-dist). | 32 |

**Table 4**  Template Parameter Description (Dual-Out Mode)

| Parameter | Description |
|-----|-----|
| T | Data type of the destination operand. For supported data types, see [Data Type](#data-type). |
| dist | Transfer mode. Type: **StoreDist**. For specific parameters, see [Table 6 StoreDist Parameter Description (Dual-Out Mode)](#table-6-storedist-parameter-description-dual-out-mode). |
| U | **RegTensor** type of the source operand. For example RegTensor\<half>, which is automatically deduced by the compiler and does not need to be manually specified by the user. |

**Table 5**  Parameter Description (Dual-Out Mode)

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstAddr | Input/Output | Destination operand, the start address of the UB. |
| srcReg0 | Input | First source operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| srcReg1 | Input | Second source operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| offset | Input | Address offset, of type AddrReg. Before the transfer, call CreateAddrReg to set the address offset. The actual UB address for the transfer is dstAddr + the offset stored in offset, and the address is not updated after the transfer. The offset stored in the AddrReg register multiplied by sizeof(T) must be 32-byte aligned. |
| mask | Input | Valid indicator for the operation on the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

<a id="table-6-storedist-parameter-description-dual-out-mode"></a>
**Table 6**  StoreDist Parameter Description (Dual-Out Mode)

| StoreDist | Meaning | Alignment Constraint (Byte) |
|-----|-----|-----|
| DIST_INTLV_B8 | Dual-Out Mode, with data type b8, ignoring mask. The elements in src0 and src1 are interleaved and stored in dst, and the dst length must be 2*VL.<br>Example: data type uint8_t:<br>src0: [0, 2, 4, 6, ... 254, 0, 2, 4, ..., 252, 254].<br>src1: [1, 3, 5, 7, ..., 255, 1, 3, 5, ..., 253, 255].<br>dst: [0, 1, 2, 3, ..., 254, 255, 0, 1, 2, 3, ..., 253, 254, 255]. | 32 |
| DIST_INTLV_B16 | Dual-Out Mode, with data type b16, ignoring mask. The elements in src0 and src1 are interleaved and stored in dst, and the dst length must be 2*VL. | 32 |
| DIST_INTLV_B32 | Dual-Out Mode, with data type b32, ignoring mask. The elements in src0 and src1 are interleaved and stored in dst, and the dst length must be 2*VL. | 32 |

## Data Type

The data types of the destination operand and the source operand must be consistent.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumOne**, the supported data types are: b8, b16, b32, b64.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumTwo**, the supported data types are: complex32, b64.

## Return Value

None

## Constraints<a name="section587915597213"></a>

- The address constraint in UB, the alignment constraint of **postUpdateStride** * sizeof(T), and the alignment constraint of the offset stored in the **AddrReg** register \* sizeof(T) are related to the distribution mode **StoreDist**. For specific address constraints, see [Table 3 StoreDist Parameter Description (Single-Out Mode)](#table-3-storedist-parameter-description-single-out-mode) and [Table 6 StoreDist Parameter Description (Dual-Out Mode)](#table-6-storedist-parameter-description-dual-out-mode).
- When **RegTrait** is **RegTraitNumOne**, in single-out normal mode (**DIST_NORM**), if the remaining space of the destination UB is less than 256B, **mask** must be configured to ensure that the amount of valid element data transferred out does not exceed the remaining UB space, and the remaining UB space must not be less than 32B.
- Single-out mode: The b64 data type supports only **DIST_NORM** and **DIST_PACK_B64** in **StoreDist**.
- When the **RegTensor** template parameter **RegTrait** is **RegTraitNumOne** and **RegTraitNumTwo**, the support is as follows:

    | RegTensor Template Parameter RegTrait Value | Supported APIs | Supported Data Types |
    |-----|-----|-----|
    | RegTraitNumOne | All single-out APIs and dual-out APIs | b8/b16/b32/b64 |
    | RegTraitNumTwo | &bull; Single-out normal movement APIs<br>&bull; Single-out PostUpdate extended movement APIs | complex32/b64 |

## Key Features

**Calling Methods of the Three Transfer APIs for Continuous Alignment Load/Store**

Figure 2 shows the calling methods of the three movement modes for continuous alignment load/store within an iteration:

- Normal movement API: After completing one transfer, the UB address will not be automatically updated, and each iteration requires manual address update.
- PostUpdate extended movement API: After completing one transfer, the UB address will be automatically updated, and no manual address update is required in each iteration.
- API using AddrReg to store the offset: In each iteration, you must first call CreateAddrReg to manually set the address offset, and then call the transfer instruction.

**Figure 2** Calling methods of the three movement modes when regTrait is RegTraitNumOne<a id="fig-storealign-regtrait"></a>

![](../../../../figures/reg_loadstorealign.png)

## Example<a name="section5497339111410"></a>

- Single-In/Single-Out Mode: **Normal Transfer** API.

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

- Single-In/Single-Out Mode: **PostUpdate** Extended Transfer API.

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

- Single-In/Single-Out Mode: Use **AddrReg** to store the offset.

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

- Dual-In/Dual-Out Mode: **Normal Transfer** API.

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

- Dual-In/Dual-Out Mode: Use **AddrReg** to store the offset, which requires coordination with the **CreateAddrReg** API.

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

