# Continuous Non-aligned Move In (LoadUnAlign)<a name="ZH-CN_TOPIC_0000001955990981"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T22:45:45.584Z -->

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

To improve the handling of irregular memory addresses, Reg vector computation supports accessing addresses that are not 32-byte aligned during data movement, reducing the performance overhead caused by non-aligned access. **LoadUnAlign** moves data continuously from a non-aligned Unified Buffer (UB) to [RegTensor](../register_data_types/RegTensor.md), using the non-aligned register [UnalignRegForLoad](../register_data_types/UnalignRegForLoad-UnalignRegForStore.md) as a temporary buffer to temporarily store data that crosses alignment boundaries, thereby achieving efficient continuous non-aligned data transfer.

There are three types of non-aligned move APIs: the normal move API, the PostUpdate extended move API, and the API that uses the [AddrReg](../register_data_types/AddrReg.md) register to store the offset.

The **regTrait** template parameter of **RegTensor** supports **RegTraitNumOne** and **RegTraitNumTwo**. The specific support is as follows:

| RegTensor template parameter regTrait | Normal move API | PostUpdate extended move API | API using the AddrReg register to store the offset |
|-----|-----|-----|-----|
| RegTraitNumOne | After completing one move, the UB address is not automatically updated, and the address must be updated manually in each iteration. | &bull; POST_MODE_NORMAL mode: Not supported.<br>&bull; POST_MODE_UPDATE mode: After completing one move, the UB address is automatically updated, and no manual address update is required in each iteration. | In each iteration, call CreateAddrReg to manually set the address offset before calling the move instruction. |
| RegTraitNumTwo | Not supported | &bull; POST_MODE_NORMAL mode: Not supported.<br>&bull; POST_MODE_UPDATE mode: After completing one move, the UB address is automatically updated, and no manual address update is required in each iteration. | Not supported |

For the principle of non-aligned move-in, see [Key Features](#key-features).

## Prototype<a name="section620mcpsimp"></a>

The non-aligned register **UnalignRegForLoad** serves as a temporary buffer for staging data that crosses alignment boundaries, thereby enabling efficient continuous non-aligned data transfer. Before reading a non-aligned address, initialize it with **LoadUnAlignPre** to save the data that is not 32-byte aligned, and then call **LoadUnAlign** to move the data in.

- Normal move API:

    ```cpp
    template <typename T>
    __simd_callee__ inline void LoadUnAlignPre(UnalignRegForLoad& ureg, __ubuf__ T* srcAddr)

    template <typename T = DefaultType, typename U>
    __simd_callee__ inline void LoadUnAlign(U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T* srcAddr)
    ```

- PostUpdate extended move API:

    ```cpp
    template <typename T>
    __simd_callee__ inline void LoadUnAlignPre(UnalignRegForLoad& ureg, __ubuf__ T* srcAddr)

    template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
    __simd_callee__ inline void LoadUnAlign(U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T*& srcAddr, uint32_t postUpdateStride)
    ```

- API that uses [AddrReg](../register_data_types/AddrReg.md) to store the offset (used together with [CreateAddrReg](../register_data_types/AddrReg.md)):

    ```cpp
    template <typename T>
    __simd_callee__ inline void LoadUnAlignPre(UnalignRegForLoad& ureg, __ubuf__ T* srcAddr, AddrReg& areg)

    template <typename T = DefaultType, typename U>
    __simd_callee__ inline void LoadUnAlign(U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T*& srcAddr, AddrReg& areg, uint32_t inc)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

| Parameter | Description |
|-----|-----|
| **T** | Data type of the source operand. For supported data types, see [Data Type](#data-type). |
| **postMode** | Controls whether to enable the automatic UB address update feature (post update). **PostLiteral** type. For details, see [PostLiteral](../aux_data_types/PostLiteral.md). |
| **U** | **RegTensor** type of the destination operand. For example, **RegTensor**\<half>. Automatically deduced by the compiler; users do not need to specify it manually. |

**Table 2**  Parameter description

| Parameter | Input/Output | Description |
|-----|-----|-----|
| **dstReg** | Output | Destination operand, of type [RegTensor](../register_data_types/RegTensor.md). |
| **ureg** | Input/Output | Non-aligned register, of **UnalignRegForLoad** type, used to store data that is not 32-byte aligned. The register size is 32 bytes. |
| **srcAddr** | Input/Output | Source operand, the UB start address, which does not need to be 32-byte aligned. |
| **postUpdateStride** | Input | Address update stride, of **uint32_t** type, in units of number of elements. Depending on the value of **postMode**, there are two cases:<br>&bull; **POST_MODE_NORMAL**: not supported.<br>&bull; **POST_MODE_UPDATE**: the actual UB start address for the move is **srcAddr**, and after the move, the address update **srcAddr** += **postUpdateStride** is performed. |
| **areg** | Input | Address offset register, of **AddrReg** type. Call **CreateAddrReg** to set the address offset before the move. The actual UB address for the move is **srcAddr** + the offset stored in the register. |
| **inc** | Input | Unused. Set it to 0. |

## Data Type

The data types of the destination operand and the source operand must be consistent.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumOne**, the supported data types are: **b8**, **b16**, **b32**, and **b64**.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumTwo**, the supported data types are: **complex32** and **b64**.

## Return Value

None

## Constraints<a name="section1575141714439"></a>

- **LoadUnAlignPre** and **LoadUnAlign** must be used in combination.
- The **PostUpdate** extended move API supports only the **POST_MODE_UPDATE** mode.
- When the **RegTensor** template parameter **RegTrait** is **RegTraitNumOne** or **RegTraitNumTwo**, the support is as follows:

    | Value of the RegTensor template parameter RegTrait | Supported APIs | supported data types |
    |-----|-----|-----|
    | RegTraitNumOne | All APIs | b8, b16, b32, b64 |
    | RegTraitNumTwo | POST_MODE_UPDATE mode of the PostUpdate extended move API | complex32, b64 |

## Key Features

**1. Principle of Non-Aligned Data Move-In**

**Figure 1**  Non-aligned move-in example<a id="fig-loadunalign-1"></a>  
![](../../../../figures/reg_loadunalign.png)

As shown in [Figure 1 Non-aligned move-in example](#fig-loadunalign-1), data is read from UB addresses **srcAddr** to 304 and moved to the destination register **dstReg** (256B). The processing flow is as follows:

① Call **LoadUnAlignPre** to initialize the non-aligned move-in. The non-aligned register **ureg** caches the valid data at UB addresses 32 to 64, serving as the preceding data cache for subsequent non-aligned access.

② Call **LoadUnAlign**. The hardware instruction moves the aligned data at UB addresses 64 to 320 into the temporary register **tmpReg**, concatenates the data corresponding to **srcAddr** to 64 in **ureg** with the data corresponding to addresses 64 to 304 in **tmpReg**, and writes the result to **dstReg**. In addition, the data at UB addresses 288 to 320 is written to **ureg**.

**2. Continuous Non-Aligned Move-In/Out Example**

**Figure 2**  Continuous non-aligned move-in/out example (data type uint32_t)<a id="fig-loadunalign-2"></a>  
![](../../../../figures/reg_loadstoreunalign.png)

**During continuous non-aligned move-in, LoadUnAlign caches the subsequent non-aligned data in ureg, so the next move-in does not require calling LoadUnAlignPre again. LoadUnAlignPre only needs to be called once before the iteration starts, thereby optimizing the performance of non-aligned move-in.**

**During continuous non-aligned move-out, the StoreUnAlign of the next iteration writes the data cached in ureg by the StoreUnAlign of the current iteration to UB, so the current iteration does not need to call StoreUnAlignPost to write the ureg data to UB. StoreUnAlignPost only needs to be called once after the iteration ends, thereby optimizing the performance of non-aligned move-out.**

As shown in [Figure 2 Continuous non-aligned move-in/out example](#fig-loadunalign-2), moving the **uint32_t** data [1, 2, 3, ..., 128] at UB addresses 48 to 560 into **dstReg** and then back to UB requires two move-in/out operations, that is, the **for** loop executes twice, with initialization and post-processing moved outside the **for** loop. **postUpdateStride** = 256B / `sizeof(T)` (that is, the address offset is 256B each time), and **repeatTimes** = **dataSize** / 256B (that is, the number of iterations = total data size / VL).

The example snippet of the PostUpdate extended move API in **POST_MODE_UPDATE** mode is as follows:

```cpp
template <typename T>
__simd_vf__ inline void LoadUnAlignVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t postUpdateStride, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::UnalignRegForLoad ureg1;
    AscendC::Reg::UnalignRegForStore ureg2;
    // Initialize the non-aligned move-in. This only needs to be called once before the iteration starts.
    AscendC::Reg::LoadUnAlignPre(ureg1, srcAddr);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadUnAlign(dstReg, ureg1, srcAddr, postUpdateStride);
        AscendC::Reg::StoreUnAlign(dstAddr, dstReg, ureg2, postUpdateStride);
    }
    // Post-process the non-aligned move-out. This only needs to be called once after the iteration ends.
    AscendC::Reg::StoreUnAlignPost(dstAddr, ureg2, 0);
}
```

The specific move steps are as follows:

1.  Non-aligned move-in initialization: update **ureg1** = [1, 2, 3, 4].
2.  Non-aligned move-in: **tmpReg** = [5, 6, 7, ..., 68]. The partial data of **tmpReg** and the data of **ureg1** are written to **dstReg** = [1, 2, 3, ..., 64], and **ureg1** is updated to [61, 62, 63, ..., 68].
3.  Non-aligned move-out: the partial data [1, 2, 3, ..., 60] of **dstReg** is written to UB addresses 48 to 288, and **ureg2** is updated to [61, 62, 63, 64].
4.  Non-aligned move-in: **tmpReg** = [69, 70, 71, ..., 128]. The data of **tmpReg** and the partial data of **ureg1** are written to **dstReg** = [65, 66, 67, ..., 128].
5.  Non-aligned move-out: the data [61, 62, 63, 64] of **ureg2** and the partial data [65, 66, 67, ..., 124] of **dstReg** are written to UB addresses 288 to 544, and **ureg2** is updated to [125, 126, 127, 128].
6.  Post-processing after non-aligned move-out: write the cached data [125, 126, 127, 128] in ureg2 to UB addresses 544 to 560.

## Example<a name="section15860211204820"></a>

- Normal move API
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

- PostUpdate extended move API
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

- API using AddrReg register storage offset
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

