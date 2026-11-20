# Continuous Non-aligned Load (LoadUnAlign)<a name="ZH-CN_TOPIC_0000001955990981"></a>

<!-- md-trans-meta sourceCommit=af3f6adafe94d121d34283378bfbe7e348767ea3 translatedAt=2026-11-05T03:24:27.930Z pushedAt=2026-11-18T01:25:30.931Z nl=2 -->

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
- Kirin 9050 processor products: Supported
<!-- end id8 -->
<!-- npu="kirin1" id9 -->
- KirinDev0001 processor products: Supported
<!-- end id9 -->
<!-- npu="kirin2" id10 -->
- KirinDev0002 processor products: Supported
<!-- end id10 -->
<!-- npu="kirin3" id11 -->
- KirinDev0003 processor products: Supported
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/LoadUnAlign_continuous_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is: `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

To improve the handling of irregular memory addresses, Reg vector computation supports accessing addresses that are not 32-byte aligned during data movement, reducing the performance overhead caused by non-aligned access. **LoadUnAlign** moves data continuously from a non-aligned Unified Buffer (UB) to [RegTensor](../register_data_types/RegTensor.md), using the non-aligned register [UnalignRegForLoad](../register_data_types/UnalignRegForLoad-UnalignRegForStore.md) as a temporary buffer to temporarily store data that crosses alignment boundaries, thereby achieving efficient continuous non-aligned data transfer.

There are three types of APIs for non-aligned move: the normal move API, the PostUpdate extended move API, and the API that uses [AddrReg](../register_data_types/AddrReg.md) registers to store offsets.

The template parameter **regTrait** of RegTensor supports **RegTraitNumOne** and **RegTraitNumTwo**. The specific support is as follows:

| RegTensor Template Parameter regTrait | Normal Transfer API | PostUpdate Extended Transfer API | API Using AddrReg Register to Store Offset |
|-----|-----|-----|-----|
| RegTraitNumOne | After completing one transfer, the UB address will not be updated automatically, and the address must be updated manually in each iteration. | &bull; POST_MODE_NORMAL mode: not supported.<br>&bull; POST_MODE_UPDATE mode: After completing one transfer, the UB address is updated automatically, and no manual address update is required in each iteration. | In each iteration, call CreateAddrReg to manually set the address offset before calling the transfer instruction. |
| RegTraitNumTwo | After completing one transfer, the UB address will not be updated automatically, and the address must be updated manually in each iteration. | &bull; POST_MODE_NORMAL mode: not supported.<br>&bull; POST_MODE_UPDATE mode: After completing one transfer, the UB address is updated automatically, and no manual address update is required in each iteration. | Not supported. |

For the principle of non-aligned load, see [Key Features](#key-features).

## Prototype<a name="section620mcpsimp"></a>

The non-aligned register **UnalignRegForLoad** serves as a temporary buffer for staging data that crosses alignment boundaries, thereby enabling efficient continuous non-aligned data transfer. Before reading a non-aligned address, initialize it with **LoadUnAlignPre** to save the data that is not 32-byte aligned, and then call **LoadUnAlign** to load the data.

- Normal movement API:

    ```cpp
    template <typename T>
    __simd_callee__ inline void LoadUnAlignPre(UnalignRegForLoad& ureg, __ubuf__ T* srcAddr)

    template <typename T = DefaultType, typename U>
    __simd_callee__ inline void LoadUnAlign(U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T* srcAddr)
    ```

- PostUpdate Extended Transfer API:

    ```cpp
    template <typename T>
    __simd_callee__ inline void LoadUnAlignPre(UnalignRegForLoad& ureg, __ubuf__ T* srcAddr)

    template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
    __simd_callee__ inline void LoadUnAlign(U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T*& srcAddr, uint32_t postUpdateStride)
    ```

- API that uses [AddrReg](../register_data_types/AddrReg.md) to store the offset (which must be used together with [CreateAddrReg](../register_data_types/AddrReg.md)):

    ```cpp
    template <typename T>
    __simd_callee__ inline void LoadUnAlignPre(UnalignRegForLoad& ureg, __ubuf__ T* srcAddr, AddrReg& areg)

    template <typename T = DefaultType, typename U>
    __simd_callee__ inline void LoadUnAlign(U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T*& srcAddr, AddrReg& areg, uint32_t inc)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|-----|-----|
| T | Data type of the source operand. For supported data types, see [Data Type](#data-type). |
| postMode | Controls whether to enable the UB address automatic update function (post update). Type: **PostLiteral**. For specific parameter description, see [PostLiteral](../aux_data_types/PostLiteral.md). |
| U | **RegTensor** type of the destination operand. For example RegTensor\<half>, it is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstReg | Output | Destination operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| ureg | Input/Output | Non-aligned register, of type **UnalignRegForLoad**, used to store non-32-byte-aligned data. The register size is 32 bytes. |
| srcAddr | Input/Output | Source operand, the UB start address, which does not need to be 32-byte aligned. |
| postUpdateStride | Input | Address update stride, of type **uint32_t**, in number of elements. Depending on the value of **postMode**, there are two cases:<br>&bull; **POST_MODE_NORMAL**: not supported.<br>&bull; **POST_MODE_UPDATE**: The actual UB start address to be moved is **srcAddr**. After the move, the address update **srcAddr += postUpdateStride** is performed. |
| areg | Input | Address offset register, of type **AddrReg**. Call **CreateAddrReg** to set the address offset before the move. The actual UB address to be moved is **srcAddr** + the offset stored in the register. |
| inc | Input | Unused. Set it to 0. |

## Data Type

The data types of the destination operand and the source operand must be consistent.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumOne**, the supported data types are: b8, b16, b32, b64.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumTwo**, the supported data types are: complex32, b64.

## Return Value

None

## Constraints<a name="section1575141714439"></a>

- **LoadUnAlignPre** and **LoadUnAlign** must be used in combination.
- The **PostUpdate** extended move API supports only the **POST_MODE_UPDATE** mode.
- When the RegTensor template parameter regTrait is RegTraitNumTwo, the srcAddr start position must have at least 2*VL (512B) of readable data.
- When the **RegTensor** template parameter **RegTrait** is **RegTraitNumOne** and **RegTraitNumTwo**, the support is as follows:

    | RegTensor Template Parameter RegTrait Value | Supported APIs | Supported Data Types |
    |-----|-----|-----|
    | RegTraitNumOne | All APIs | b8, b16, b32, b64 |
    | RegTraitNumTwo | &bull; Normal copy API<br>&bull; POST_MODE_UPDATE mode of the PostUpdate extended copy API | complex32, b64 |

## Key Features

**1. Principle of Non-aligned Data Load**

**Figure 1**  Non-aligned load example<a id="fig-loadunalign-1"></a>  
![](../../../../figures/reg_loadunalign.png)

The following uses the RegTraitNumOne scenario as an example. As shown in [Figure 1 Non-aligned load example](#fig-loadunalign-1), data is read from UB address srcAddr ~ 304 and moved to the destination register dstReg (256B). The processing flow is as follows:

① Call **LoadUnAlignPre** to initialize the non-aligned load. The non-aligned register ureg caches the valid data at UB addresses 32 ~ 64, serving as the preceding data cache for subsequent non-aligned access.

② Call **LoadUnAlign**. The hardware instruction moves the aligned data at UB addresses 64 ~ 320 into the temporary register tmpReg, concatenates the data corresponding to srcAddr ~ 64 in ureg with the data corresponding to addresses 64 ~ 304 in tmpReg, and writes the result to dstReg. In addition, the data at UB addresses 288 ~ 320 are written to ureg.

When the RegTensor template parameter regTrait is RegTraitNumTwo, each call to LoadUnAlign copies 2*VL (512B) of data.

**2. Continuous non-aligned copy-in/out example**

**Figure 2**  Continuous non-aligned load and store example (data type uint32_t)<a id="fig-loadunalign-2"></a>  
![](../../../../figures/reg_loadstoreunalign.png)

**During continuous non-aligned copy-in, LoadUnAlign caches the subsequent non-aligned data in ureg. Therefore, the next copy-in does not need to call LoadUnAlignPre again; LoadUnAlignPre only needs to be called once before the iteration starts, thereby optimizing the performance of non-aligned copy-in.**

**During continuous non-aligned copy-out, the StoreUnAlign of the next iteration writes the data cached in ureg by the StoreUnAlign of the current iteration to UB. Therefore, the current iteration does not need to call StoreUnAlignPost to write the ureg data to UB; StoreUnAlignPost only needs to be called once after the iteration ends, thereby optimizing the performance of non-aligned copy-out.**

As shown in [Figure 2 Continuous non-aligned load and store example](#fig-loadunalign-2), loading the uint32_t data [1, 2, 3, ..., 128] at UB addresses 48 to 560 into **dstReg** and then storing it back to UB requires two load and store operations, that is, the for loop executes twice, with initialization and post-processing moved outside the for loop. **postUpdateStride** = 256B / sizeof(T) (that is, the address is offset by 256B each time), and **repeatTimes** = dataSize / 256B (that is, the number of iterations = total data size / VL).

A sample snippet of the **POST_MODE_UPDATE** mode of the PostUpdate extended move API is as follows:

```cpp
template <typename T>
__simd_vf__ inline void LoadUnAlignVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t postUpdateStride, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::UnalignRegForLoad ureg1;
    AscendC::Reg::UnalignRegForStore ureg2;
    // Initialize the non-aligned load. Call this only once before the iteration starts.
    AscendC::Reg::LoadUnAlignPre(ureg1, srcAddr);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadUnAlign(dstReg, ureg1, srcAddr, postUpdateStride);
        AscendC::Reg::StoreUnAlign(dstAddr, dstReg, ureg2, postUpdateStride);
    }
    // Post-process the non-aligned store. Call this only once after the iteration ends.
    AscendC::Reg::StoreUnAlignPost(dstAddr, ureg2, 0);
}
```

The specific copy steps are as follows:

1.  Non-aligned copy-in initialization: update ureg1 = [1, 2, 3, 4].
2.  Non-aligned load: tmpReg = [5, 6, 7, ..., 68], write the partial data of tmpReg and the data of ureg1 into dstReg = [1, 2, 3, ..., 64], and update ureg1 = [61, 62, 63, ..., 68].
3.  Non-aligned store: write the partial data [1, 2, 3, ..., 60] of dstReg into UB addresses 48 to 288, and update ureg2 = [61, 62, 63, 64].
4.  Non-aligned load: tmpReg = [69, 70, 71, ...,128], the data of tmpReg and the partial data of ureg1 are written into dstReg = [65, 66, 67, ..., 128].
5.  Non-aligned store: write the ureg2 data [61, 62, 63, 64] and the partial data [65, 66, 67, ..., 124] of dstReg into UB addresses 288 to 544, and update ureg2 = [125, 126, 127, 128].
6.  Post-processing of non-aligned store out: Write the data in cache [125, 126, 127, 128] in ureg2 to UB addresses 544 to 560.

## Example<a name="section15860211204820"></a>

- Normal Transfer API

    ```cpp
    template <typename T>
    __simd_vf__ inline void LoadUnAlignVF1(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t postUpdateStride, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::UnalignRegForLoad ureg0;
        AscendC::Reg::UnalignRegForStore ureg1;
        AscendC::Reg::LoadUnAlignPre(ureg0, srcAddr);
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadUnAlign(srcReg, ureg0, srcAddr + i * postUpdateStride);
            AscendC::Reg::StoreUnAlign(dstAddr, srcReg, ureg1, postUpdateStride);
        }
        AscendC::Reg::StoreUnAlignPost(dstAddr, ureg1, 0);
    }
    ```

- PostUpdate Extended Transfer API

    ```cpp
    template <typename T>
    __simd_vf__ inline void LoadUnAlignVF2(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t postUpdateStride, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::UnalignRegForLoad ureg0;
        AscendC::Reg::UnalignRegForStore ureg1;
        AscendC::Reg::LoadUnAlignPre(ureg0, srcAddr);
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadUnAlign(srcReg, ureg0, srcAddr, postUpdateStride);
            AscendC::Reg::StoreUnAlign(dstAddr, srcReg, ureg1, postUpdateStride);
        }
        AscendC::Reg::StoreUnAlignPost(dstAddr, ureg1, 0);
    }
    ```

- API Using AddrReg Register to Store Offset

    ```cpp
    template <typename T>
    __simd_vf__ inline void LoadUnAlignVF3(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::UnalignRegForLoad ureg0;
        AscendC::Reg::UnalignRegForStore ureg1;
        AscendC::Reg::AddrReg aReg;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            aReg = AscendC::Reg::CreateAddrReg<T>(i, oneRepeatSize);
            AscendC::Reg::LoadUnAlignPre(ureg0, srcAddr, aReg);
            AscendC::Reg::LoadUnAlign(srcReg, ureg0, srcAddr, aReg, 0);
            AscendC::Reg::StoreUnAlign(dstAddr, srcReg, ureg1, aReg);
        }
        AscendC::Reg::StoreUnAlignPost(dstAddr, ureg1, aReg);
    }
    ```
