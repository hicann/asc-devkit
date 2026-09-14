# Continuous Non-aligned Scenario Optimization<a name="ZH-CN_TOPIC_0000002530088763"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T14:28:51.632Z -->

## Optimization Suggestions for Continuous Non-aligned Data Movement<a name="section14269121313385"></a>

-   Prioritize continuous aligned data movement to avoid extra overhead and improve overall performance.
-   Due to hardware support limitations, it is not recommended to use AddrReg to store offsets during continuous non-aligned data movement. Instead, use uint32\_t to store offsets and use the [post update](../../../../../api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/LoadUnAlign_continuous.md) mode for data movement. In this scenario, each API call automatically updates the address of the source operand on the UB. All subsequent optimization suggestions are based on this scenario.
-   During continuous non-aligned load-in, after each data movement from the UB to the RegTensor, the non-aligned register caches the subsequent non-aligned data. During the next data movement, the data in the non-aligned register is written to the RegTensor. Therefore, the initialization of continuous non-aligned load-in needs to be performed only once and should be moved outside the for loop.
-   During continuous non-aligned store-out, after each data movement from the RegTensor to the UB, the non-aligned register caches the subsequent non-aligned data. During the next data movement, the data cached in the non-aligned register is written to the UB. Therefore, the post-processing of continuous non-aligned store-out needs to be performed only once and should be moved outside the for loop.

[Positive Example]

```
// Initialize non-aligned load-in, use uint32_t to manage offsets, and move it outside the for loop.
AscendC::Reg::LoadUnAlignPre(ureg0, srcAddr);
for (uint16_t i = 0; i < repeatTimes; ++i) {
     AscendC::Reg::LoadUnAlign(srcReg, ureg0, srcAddr + i * postUpdateStride);
     AscendC::Reg::StoreUnAlign(dstAddr, srcReg, ureg1, postUpdateStride);
}
// Perform non-aligned store-out post-processing and move it outside the for loop.
AscendC::Reg::StoreUnAlignPost(dstAddr, ureg1, 0);
```

[Negative Example]

```
for (uint16_t i = 0; i < repeatTimes; ++i) {
     AscendC::Reg::LoadUnAlignPre(ureg0, srcAddr + i * postUpdateStride);
     AscendC::Reg::LoadUnAlign(srcReg, ureg0, srcAddr + i * postUpdateStride);
     AscendC::Reg::StoreUnAlign(dstAddr, srcReg, ureg1, postUpdateStride);
     AscendC::Reg::StoreUnAlignPost(dstAddr, ureg1, 0);
}
```

## Introduction to Continuous Non-aligned Data Movement APIs<a name="section242991114420"></a>

To improve the capability of handling irregular memory addresses, Reg vector computation supports accessing addresses that are not 32-byte aligned during data movement, which applies to scenarios of non-aligned data movement from UB to RegTensor or from RegTensor to UB. To reduce the performance overhead introduced by non-aligned access, RegBase introduces a non-aligned register caching mechanism. This mechanism uses the non-aligned registers UnalignRegForLoad and UnalignRegForStore as temporary buffers to cache data that crosses alignment boundaries, thereby implementing efficient continuous non-aligned data transfer.

Before reading a non-aligned address, initialize it with LoadUnAlignPre first, and then use LoadUnAlign. When writing a non-aligned address, use StoreUnAlign first, and then use StoreUnAlignPost for post-processing. The data movement-in and movement-out APIs that use uint32_t to manage the offset, and the maskReg movement-out API, are as follows:

-   Continuous non-aligned movement-in, using uint32_t to store the movement amount

    ```
    __simd_callee__ inline void LoadUnAlignPre(UnalignRegForLoad& ureg, __ubuf__ T* srcAddr);
    __simd_callee__ inline void LoadUnAlign(U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T*& srcAddr, uint32_t postUpdateStride);
    ```

-   Continuous non-aligned movement-out, using uint32_t to store the offset

    ```
    __simd_callee__ inline void StoreUnAlign(__ubuf__ T*& dstAddr, U& srcReg, UnalignRegForStore& ureg, uint32_t postUpdateStride);
    __simd_callee__ inline void StoreUnAlignPost(__ubuf__ T*& dstAddr, UnalignRegForStore& ureg, int32_t postUpdateStride);
    ```

-   Continuous non-aligned movement-out, moving maskReg out to UB

    ```
    __simd_callee__ inline void StoreUnAlign(__ubuf__ T*& dstAddr, MaskReg& mask, UnalignRegForStore& ureg);
    __simd_callee__ inline void StoreUnAlignPost(__ubuf__ T*& dstAddr, UnalignRegForStore& ureg, int32_t postUpdateStride);
    ```

## Non-aligned Register Principles<a name="section1949183624916"></a>

This section describes how the non-aligned register works during non-aligned data movement and explains the internal principles in detail, helping developers understand why the performance optimization suggestions above can bring performance gains. It first introduces the implementation principles of continuous non-aligned data load, continuous non-aligned data store, and MaskReg continuous non-aligned data store, and then connects these key processes through a specific load/store example.

-   **Non-aligned data load**

    As shown in the following figure, data is read from UB address srcAddr \~ 304 and moved to the destination register dstReg (256B). The processing flow is as follows:

    -   Call **LoadUnAlignPre** to initialize the non-aligned load. The non-aligned register ureg caches the valid data at UB addresses 32 \~ 64 as the preceding data cache for subsequent non-aligned access.
    -   Call **LoadUnAlign**. The hardware instruction moves the data at UB addresses 64 \~ 320 into the temporary register tmpReg, concatenates the data corresponding to srcAddr \~ 64 in ureg with the data corresponding to addresses 64 \~ 304 in tmpReg, and writes the result to dstReg. After this movement, the data at UB addresses 288 \~ 320 is written to ureg. During continuous non-aligned load, because LoadUnAlign caches the subsequent non-aligned data to ureg, the next load does not need to call LoadUnAlignPre again. LoadUnAlignPre only needs to be called once before the first load, thereby optimizing the performance of non-aligned load.

        **Figure 1**  Non-aligned data load<a name="fig638414472320"></a>  

        ![](../../../../figures/unaligned_move_in_77.png)

-   **Non-aligned data store**

    Write the non-aligned data in the source register srcReg to UB address dstAddr. Depending on the current state of ureg, there are two scenarios:

    **Scenario 1: ureg is empty**

    -   Call **StoreUnAlign**. In this case, ureg contains no valid data, indicating the start state of continuous non-aligned movement out. The data corresponding to UB addresses 48 to 288 in srcReg is written to dstAddr, and the data corresponding to UB addresses 288 to 320 in srcReg is cached to ureg.
    -   Call **StoreUnAlignPost** for post-processing of the non-aligned movement out. The valid data corresponding to UB addresses 288 to 320 cached in ureg is written to UB.

    **Figure 2**  Non-aligned data movement out (ureg is empty)<a name="fig34750199610"></a>  

    ![](../../../../figures/unaligned_move_out_with_ureg_empty.png)

    **Scenario 2: ureg is not empty**

    -   Call **StoreUnAlign**. In this case, ureg contains valid data. The system concatenates the data corresponding to UB addresses 32 to dstAddr in ureg with the data corresponding to UB addresses dstAddr to 288 in srcReg, and writes the result to UB address dstAddr. At the same time, the data corresponding to UB addresses 288 to 320 in srcReg is cached to ureg. During continuous non-aligned movement out, when ureg is not empty, the next StoreUnAlign reads the data cached to ureg by the current StoreUnAlign. Therefore, StoreUnAlignPost does not need to be called for the current movement out; it only needs to be called once after the movement out is complete, thereby optimizing the performance of non-aligned movement out.
    -   Call **StoreUnAlignPost** for post-processing of the non-aligned movement out. The valid data corresponding to UB addresses 288 to 320 cached in ureg is written to UB.

    **Figure 3**  Non-aligned data movement out (ureg is not empty)<a name="fig579119300430"></a>  

    ![](../../../../figures/unaligned_move_out_with_ureg_not_empty.png)

-   **MaskReg continuous non-aligned data movement out**

    Move the source register MaskReg (32B) out to the UB.

    -   When the data type at the UB address is b16, the hardware instruction extracts the least significant bit (LSB) from each 2-bit data, packs the 32B data in MaskReg into 16B, and writes it to the UB. After the data movement is complete, the UB address is updated by a 16B offset.
    -   When the data type at the UB address is b32, the hardware instruction extracts the least significant bit (LSB) from each 4-bit data, packs the 32B data in MaskReg into 8B, and writes it to the UB. After the data movement is complete, the UB address is updated by an 8B offset.

    Taking the data type b16 at the UB address as an example, to write two MaskReg data to dstAddr to 72, perform non-aligned movement out as follows:

    -   Cache the 16B data compressed from maskReg1 (UB addresses dstAddr to 56) to ureg.
    -   Concatenate the data in ureg with the partial data of the 16B data compressed from maskReg2 (UB addresses 56 to 64), and write the result to UB addresses dstAddr to 64.
    -   Cache the partial data of the 16B data compressed from maskReg2 (UB addresses 64 to 72) to ureg.
    -   Post-processing after non-aligned movement out. Write the data cached in ureg to UB addresses 64 to 72. MaskReg continuous non-aligned movement out is similar to RegTensor continuous non-aligned movement out. You only need to call StoreUnAlignPost once after the movement out to optimize the performance of MaskReg continuous non-aligned movement out.

    **Figure 4**  MaskReg continuous non-aligned data movement out<a name="fig18974124104813"></a>  

    ![](../../../../figures/maskreg_continuous_unaligned_move_out.png)

-   **Continuous non-aligned data movement in and out example**

    As shown in the following figure, to move the uint32_t data [1, 2, 3, ... , 128] at UB addresses 48 to 560 into dstReg and then move it back to UB, two data movement in and out operations are required. That is, the for loop is executed twice, and the initialization and post-processing are moved outside the for loop.

    postUpdateStride = 256B / sizeof(T) (that is, the address offset is 256B each time), and repeatTimes = dataSize / 256B (that is, the number of iterations = total data volume / VL).

    The following is an example snippet of the POST_MODE_UPDATE mode of the PostUpdate extended data movement API:

    ```cpp
    template <typename T> 
    __simd_vf__ inline void LoadUnAlignVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t postUpdateStride, uint16_t repeatTimes) 
    { 
    AscendC::Reg::RegTensor<T> srcReg; 
    AscendC::Reg::UnalignRegForLoad ureg0; 
    AscendC::Reg::UnalignRegForStore ureg1; 
    // Initialize the non-aligned data movement in. This only needs to be called once before the iteration starts.
    AscendC::Reg::LoadUnAlignPre(ureg0, srcAddr); 
    for (uint16_t i = 0; i < repeatTimes; ++i) { 
        AscendC::Reg::LoadUnAlign(srcReg, ureg0, srcAddr, postUpdateStride); 
        AscendC::Reg::StoreUnAlign(dstAddr, srcReg, ureg1, postUpdateStride); 
    } 
    // Post-process the non-aligned data movement out. This only needs to be called once after the iteration ends.
    AscendC::Reg::StoreUnAlignPost(dstAddr, ureg1, 0); 
    }
    ```

    1.  Non-aligned data movement in initialization: update ureg1 = [1, 2, 3, 4];
    2.  Non-aligned data movement in: tmpReg = [5, 6, 7, ... , 68], write the partial data of tmpReg and the data of ureg1 into dstReg = [1, 2, 3, ... , 64], and update ureg1 = [61, 62, 63, ... , 68];
    3.  Non-aligned data movement out: write the partial data [1, 2, 3, ... , 60] of dstReg to UB addresses 48 to 288, and update ureg2 = [61, 62, 63, 64];
    4.  Non-aligned load: tmpReg = \[69, 70, 71, ... ,128\], the tmpReg data and partial data of ureg1 are written to dstReg = \[65, 66 67, ... , 128\];
    5.  Non-aligned store: the ureg2 data \[61, 62, 63, 64\] and partial data of dstReg \[65, 66, 67, ... ,124\] are written to UB addresses 288 to 544, and ureg2 is updated to \[125, 126, 127, 128\];
    6.  Non-aligned store post-processing: write the data cached in ureg2 \[125, 126, 127, 128\] to UB addresses 544 to 560.

    **Figure 5** Continuous non-aligned load and store<a name="fig98921837195710"></a>  

    ![](../../../../figures/unaligned_move_in_move_out_example.png)
