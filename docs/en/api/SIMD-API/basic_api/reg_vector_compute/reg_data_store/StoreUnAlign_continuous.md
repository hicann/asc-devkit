<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T22:34:16.610Z -->
# Continuous Non-aligned Store (StoreUnAlign)<a name="ZH-CN_TOPIC_0000001929071470"></a>


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

To improve the handling of irregular memory addresses, Reg vector computation supports accessing addresses that are not 32-byte aligned during data movement, reducing the performance overhead caused by non-aligned access. StoreUnAlign moves data continuously from [RegTensor](../register_data_types/RegTensor.md) to a non-aligned Unified Buffer (UB). It uses the non-aligned register UnalignRegForStore as a temporary buffer to hold data that crosses alignment boundaries, thereby achieving efficient continuous non-aligned data transfer.

The non-aligned move APIs include the PostUpdate extended move API, the API that uses the [AddrReg](../register_data_types/AddrReg.md) register to store the offset, and the API that uses the [AR register](../compare_and_select/Squeeze.md) to store the offset.

The **regTrait** template parameter of **RegTensor** supports **RegTraitNumOne** and **RegTraitNumTwo**. The specific support is as follows:

| RegTensor template parameter regTrait | PostUpdate extended move API | API using the AddrReg register to store the offset | API using the AR register to store the offset |
|-----|-----|-----|-----|
| RegTraitNumOne | &bull; POST_MODE_NORMAL mode: not supported.<br>&bull; POST_MODE_UPDATE mode: After completing one move, the UB address auto-updates, and no manual address update is required for each iteration. | In each iteration, call **CreateAddrReg** to manually set the address offset first, and then call the move instruction. | &bull; POST_MODE_NORMAL mode: After completing one move, the UB address does not auto-update, and the address must be updated manually in each iteration.<br>&bull; POST_MODE_UPDATE mode: After completing one move, the UB address auto-updates, and no manual address update is required for each iteration. |
| RegTraitNumTwo | &bull; POST_MODE_NORMAL mode: not supported.<br>&bull; POST_MODE_UPDATE mode: After completing one move, the UB address auto-updates, and no manual address update is required for each iteration. | Not supported | Not supported |

For the principle of non-aligned store, see [Key Features](#key-features).

## Prototype<a name="section620mcpsimp"></a>

The non-aligned register **UnalignRegForStore** serves as a temporary buffer for staging data that crosses alignment boundaries, thereby enabling efficient continuous non-aligned data transfer. Before writing to a non-aligned address, call **StoreUnAlign** first, and then call **StoreUnAlignPost** for post-processing to handle the tail data that is not 32-byte aligned.

- PostUpdate extended move API

    ```cpp
    template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
    __simd_callee__ inline void StoreUnAlign(__ubuf__ T*& dstAddr, U& srcReg, UnalignRegForStore& ureg, uint32_t postUpdateStride)

    template <typename T, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE>
    __simd_callee__ inline void StoreUnAlignPost(__ubuf__ T*& dstAddr, UnalignRegForStore& ureg, int32_t postUpdateStride)
    ```

- API that uses the AddrReg register to store the offset

    ```cpp
    template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
    __simd_callee__ inline void StoreUnAlign(__ubuf__ T*& dstAddr, U& srcReg, UnalignRegForStore& ureg, AddrReg& areg)

    template <typename T>
    __simd_callee__ inline void StoreUnAlignPost(__ubuf__ T*& dstAddr, UnalignRegForStore& ureg, AddrReg& areg)
    ```

- API that uses the AR register to store the offset: used together with [Squeeze](../Comparison and Selection/Squeeze.md). Squeeze stores the total number of bytes of valid elements into the AR special register, and this API uses the number of valid elements in the AR register as the storage offset.

    ```cpp
    template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
    __simd_callee__ inline void StoreUnAlign(__ubuf__ T* dstAddr, U& srcReg, UnalignRegForStore& ureg)

    template <typename T>
    __simd_callee__ inline void StoreUnAlignPost(__ubuf__ T* dstAddr, UnalignRegForStore& ureg)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|-----|-----|
| T | Data type of the destination operand. For supported data types, see [Data Type](#data-type). |
| postMode | Controls whether to enable the UB address auto-update feature (post update). It is of the **PostLiteral** type. For details about the parameters, see [PostLiteral](../aux_data_types/PostLiteral.md). |
| U | **RegTensor** type of the source operand, for example, **RegTensor\<half>**. It is automatically deduced by the compiler and does not need to be manually specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstAddr | Input/Output | Destination operand, the UB start address, which does not need 32-byte alignment. |
| srcReg | Input | Source operand, of the [RegTensor](../register_data_types/RegTensor.md) type. |
| ureg | Input/Output | Non-aligned register, of the **UnalignRegForStore** type, used to store data that is not 32-byte aligned. The register size is 32 bytes. |
| postUpdateStride | Input | Address update stride, of the **uint32_t** type, in elements. Depending on the value of **postMode**, there are two cases:<br>&bull; **POST_MODE_NORMAL**: not supported.<br>&bull; **POST_MODE_UPDATE**: the actual UB start address for the move is **dstAddr**, and after the move, the address update **dstAddr += postUpdateStride** is performed. |
| areg | Input | Address offset register, of the **AddrReg** type. Call **CreateAddrReg** to set the address offset before the move. The actual UB address for the move is **dstAddr** + the offset stored in the register. |

## Data Type

The data types of the destination operand and the source operand must be consistent.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumOne**, the supported data types are: **b8**, **b16**, **b32**, and **b64**.
- When the **RegTensor** template parameter **regTrait** is **RegTraitNumTwo**, the supported data types are: **complex32** and **b64**.

## Return Value

None

## Constraints<a name="section1575141714439"></a>

- **StoreUnAlign** and **StoreUnAlignPost** must be used in combination.
- For the API that uses the **AR register** to store the offset:
    - When the **store** template parameter of the **Squeeze** API is configured as **STORE_REG**, ensure that the **Squeeze** API and **StoreUnAlign** are used alternately.
    - Before invoking the computation, call the **ClearSpr** API to clear the **AR register** to zero.

- When the **RegTrait** template parameter of **RegTensor** is **RegTraitNumOne** and **RegTraitNumTwo**, the support is as follows:

    | RegTrait value of the RegTensor template parameter | Supported API | Supported data types |
    |-----|-----|-----|
    | RegTraitNumOne | All APIs | b8, b16, b32, b64 |
    | RegTraitNumTwo | PostUpdate extended move API | complex32, b64 |

## Key Features

**1. Principle of non-aligned data store**

The non-aligned data in the source register **srcReg** is written to the UB address **dstAddr**. Depending on the current state of **ureg**, there are two scenarios:

**Scenario 1: ureg is empty** (first iteration)

As shown in [Figure 1](#fig-storeunalign-1), data is read from the source register **srcReg** (256B) and moved to the target UB address (**dstAddr** ~ 304). The processing flow is as follows:

1) Call **StoreUnAlign**. At this point, **ureg** contains no valid data, indicating the starting state of continuous non-aligned store. The data corresponding to UB addresses 48 ~ 288 in **srcReg** is written to **dstAddr**. In addition, the data corresponding to UB addresses 288 ~ 304 in **srcReg** is written to **ureg**.

2) Call **StoreUnAlignPost** to perform post-processing of the non-aligned store. The data in cache in **ureg** is written to UB addresses 288 ~ 304.

**Figure 1** Non-aligned data store (ureg is empty)<a id="fig-storeunalign-1"></a>

![](../../../../figures/reg_storeunalign_1.png)

**Scenario 2: ureg is not empty** (except the first iteration)

As shown in [Figure 2](#fig-storeunalign-2), data is read from the source register **srcReg** (256B) and moved to the target UB address (**dstAddr** ~ 304). The processing flow is as follows:

① Call **StoreUnAlign**. At this point, **ureg** contains valid data. The system concatenates the data corresponding to UB addresses 32 ~ **dstAddr** in **ureg** with the data corresponding to UB addresses **dstAddr** ~ 288 in **srcReg**, and writes the result to UB address **dstAddr**. In addition, the data corresponding to UB addresses 288 ~ 304 in **srcReg** is written to **ureg**.

② Call **StoreUnAlignPost** to perform post-processing for the non-aligned store. The data cached in **ureg** is written to UB addresses 288 ~ 304.


**Figure 2** Non-aligned data store (**ureg** is not empty)<a id="fig-storeunalign-2"></a>

![](../../../../figures/reg_storeunalign_2.png)

**2. Continuous non-aligned move in/out example**

**Figure 3** Continuous non-aligned move in/out example (data type uint32_t)<a id="fig-storeunalign-3"></a>

![](../../../../figures/reg_loadstoreunalign.png)

**During continuous non-aligned load, LoadUnAlign caches the subsequent non-aligned data in ureg. Therefore, the next load does not need to call LoadUnAlignPre again; LoadUnAlignPre only needs to be called once before the iteration starts, thereby optimizing the performance of non-aligned load.**

**During continuous non-aligned store, the StoreUnAlign of the next iteration writes the data cached in ureg by the StoreUnAlign of the current iteration to UB. Therefore, the current iteration does not need to call StoreUnAlignPost to write the ureg data to UB; StoreUnAlignPost only needs to be called once after the iteration ends, thereby optimizing the performance of non-aligned store.**

As shown in [Figure 3 Continuous non-aligned move in/out example](#fig-storeunalign-3), moving the uint32_t data [1, 2, 3, ... , 128] at UB addresses 48 to 560 into dstReg and then storing it back to UB requires two move in/out operations, that is, the for loop executes twice, with initialization and post-processing moved outside the for loop.

postUpdateStride = 256B / sizeof(T), that is, the address offset is 256B each time, and repeatTimes = dataSize / 256B (that is, the number of iterations = total data size / VL).

An example snippet of the POST_MODE_UPDATE mode of the PostUpdate extended move API is as follows:

```cpp
template <typename T>
__simd_vf__ inline void LoadUnAlignVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t postUpdateStride, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::UnalignRegForLoad ureg1;
    AscendC::Reg::UnalignRegForStore ureg2;
    // Initialize the non-aligned load, which only needs to be called once before the iteration starts.
    AscendC::Reg::LoadUnAlignPre(ureg1, srcAddr);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadUnAlign(dstReg, ureg1, srcAddr, postUpdateStride);
        AscendC::Reg::StoreUnAlign(dstAddr, dstReg, ureg2, postUpdateStride);
    }
    // Post-process the non-aligned store, which only needs to be called once after the iteration ends.
    AscendC::Reg::StoreUnAlignPost(dstAddr, ureg2, 0);
}
```

The specific move steps are as follows:

1.  Non-aligned load initialization: update ureg1 = [1, 2, 3, 4].
2.  Non-aligned load: tmpReg = [5, 6, 7, ... , 68], the partial data of tmpReg and the data of ureg1 are written to dstReg = [1, 2, 3, ... , 64], and ureg1 is updated to [61, 62, 63, ... , 68].
3.  Non-aligned store: the partial data [1, 2, 3, ... , 60] of dstReg is written to UB addresses 48 to 288, and ureg2 is updated to [61, 62, 63, 64].
4.  Non-aligned load: tmpReg = [69, 70, 71, ... ,128], the data of tmpReg and the partial data of ureg1 are written to dstReg = [65, 66, 67, ... , 128].
5.  Non-aligned store: The **ureg2** data [61, 62, 63, 64] and part of the **dstReg** data [65, 66, 67, ... ,124] are written to UB addresses 288 to 544, and **ureg2** is updated to [125, 126, 127, 128].
6.  Non-aligned store post-processing: The data [125, 126, 127, 128] cached in **ureg2** is written to UB addresses 544 to 560.

## Example<a name="section642mcpsimp"></a>

-  PostUpdate extended move API
    ```cpp
    template <typename T>
    __simd_vf__ inline void StoreUnAlignVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t postUpdateStride, uint16_t repeatTimes)
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

- API using the AddrReg register to store the offset
    ```cpp
    template <typename T>
    __simd_vf__ inline void StoreUnAlignVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
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

- API using the AR register to store the offset
    ```cpp
    template <typename T>
    __aicore__ inline void SqueezeVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg0;
        AscendC::Reg::RegTensor<T> srcReg1;
        AscendC::Reg::UnalignRegForStore ureg;
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::H>();
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(srcReg0, srcAddr, oneRepeatSize);
            AscendC::Reg::Squeeze<T, AscendC::Reg::GatherMaskMode::STORE_REG>(srcReg1, srcReg0, mask);
            AscendC::Reg::StoreUnAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dstAddr, srcReg1, ureg);
        }
        AscendC::Reg::StoreUnAlignPost(dstAddr, ureg);
    }
    ```

