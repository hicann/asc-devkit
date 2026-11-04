# Continuous Unaligned Scenario Optimization<a name="ZH-CN_TOPIC_0000002530088763"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:56:25.542Z pushedAt=2026-11-03T07:05:13.355Z nl=2 -->

## Optimization Suggestions for Continuous Unaligned Movement<a name="section14269121313385"></a>

-   Prioritize continuous aligned move to avoid extra overhead and improve overall performance.
-   Due to hardware support limitations, it is not recommended to use AddrReg to store offsets during continuous unaligned move. It is recommended to use the uint32\_t type to store offsets and use the [post update](../../../../../api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/LoadUnAlign_continuous.md) mode for the move. In this scenario, each API call automatically updates the address of the source operand in the Unified Buffer (UB). The subsequent optimization suggestions are all based on this scenario.
-   During continuous unaligned move-in, after the UB data is moved to RegTensor each time, the unaligned register caches the subsequent unaligned data. During the next move, the data in the unaligned register is written to RegTensor. Therefore, the initialization of continuous unaligned move-in needs to be performed only once and should be moved outside the for loop.
-   During continuous unaligned move-out, after the RegTensor data is moved to UB each time, the unaligned register caches the subsequent unaligned data. During the next move, the data cached in the unaligned register is written to UB. Therefore, the post-processing of continuous unaligned move-out needs to be performed only once and should be moved outside the for loop.

[Positive example]

```
// Initialize unaligned move-in, use uint32_t to manage the offset, and move it outside the for loop.
AscendC::Reg::LoadUnAlignPre(ureg0, srcAddr);
for (uint16_t i = 0; i < repeatTimes; ++i) {
     AscendC::Reg::LoadUnAlign(srcReg, ureg0, srcAddr + i * postUpdateStride);
     AscendC::Reg::StoreUnAlign(dstAddr, srcReg, ureg1, postUpdateStride);
}
// Move the post-processing of unaligned move-out outside the for loop.
AscendC::Reg::StoreUnAlignPost(dstAddr, ureg1, 0);
```

[Negative example]

```
for (uint16_t i = 0; i < repeatTimes; ++i) {
     AscendC::Reg::LoadUnAlignPre(ureg0, srcAddr + i * postUpdateStride);
     AscendC::Reg::LoadUnAlign(srcReg, ureg0, srcAddr + i * postUpdateStride);
     AscendC::Reg::StoreUnAlign(dstAddr, srcReg, ureg1, postUpdateStride);
     AscendC::Reg::StoreUnAlignPost(dstAddr, ureg1, 0);
}
```

## Continuous Unaligned Movement APIs<a name="section242991114420"></a>

To improve the capability of handling irregular memory addresses, Reg vector computation supports accessing addresses that are not 32-byte aligned during data movement, which applies to scenarios of unaligned moves from UB to RegTensor or from RegTensor to UB. To reduce the performance overhead introduced by unaligned access, RegBase introduces an unaligned register caching mechanism. This mechanism uses the unaligned registers UnalignRegForLoad and UnalignRegForStore as temporary buffers to cache data that crosses alignment boundaries, thereby implementing efficient continuous unaligned data transfer.

Before reading an unaligned address, initialize it with LoadUnAlignPre first, and then use LoadUnAlign. When writing an unaligned address, use StoreUnAlign first, and then use StoreUnAlignPost for post-processing. The move-in and move-out APIs that use uint32\_t to manage offsets, and the maskReg move-out API, are as follows:

-   Continuous unaligned move-in, using uint32\_t to store the move amount

    ```
    __simd_callee__ inline void LoadUnAlignPre(UnalignRegForLoad& ureg, __ubuf__ T* srcAddr);
    __simd_callee__ inline void LoadUnAlign(U& dstReg, UnalignRegForLoad& ureg, __ubuf__ T*& srcAddr, uint32_t postUpdateStride);
    ```

-   Continuous unaligned move-out, using uint32\_t to store the offset

    ```
    __simd_callee__ inline void StoreUnAlign(__ubuf__ T*& dstAddr, U& srcReg, UnalignRegForStore& ureg, uint32_t postUpdateStride);
    __simd_callee__ inline void StoreUnAlignPost(__ubuf__ T*& dstAddr, UnalignRegForStore& ureg, int32_t postUpdateStride);
    ```

-   Continuous unaligned move-out, moving maskReg out to UB

    ```
    __simd_callee__ inline void StoreUnAlign(__ubuf__ T*& dstAddr, MaskReg& mask, UnalignRegForStore& ureg);
    __simd_callee__ inline void StoreUnAlignPost(__ubuf__ T*& dstAddr, UnalignRegForStore& ureg, int32_t postUpdateStride);
    ```

## Unaligned Register Principles<a name="section1949183624916"></a>

This section describes how the unaligned register works during unaligned moves and explains the internal principles in detail, helping developers understand why the performance optimization suggestions above can bring performance gains. It first introduces the implementation principles of continuous unaligned data move-in, continuous unaligned data move-out, and MaskReg continuous unaligned data move-out, and then connects these key processes through a specific move-in and move-out example.

-   **Unaligned data move-in**

    As shown in the following figure, data is read from UB address srcAddr \~ 304 and moved to the destination register dstReg (256B). The processing flow is as follows:

    -   Call **LoadUnAlignPre** to initialize the unaligned move-in. The unaligned register ureg caches the valid data at UB addresses 32 \~ 64 as the preceding data cache for subsequent unaligned access.
    -   Call **LoadUnAlign**. The hardware instruction moves the data at UB addresses 64 \~ 320 into the temporary register tmpReg, concatenates the data corresponding to srcAddr \~ 64 in ureg with the data corresponding to addresses 64 \~ 304 in tmpReg, and writes the result to dstReg. After this move, the data at UB addresses 288 \~ 320 is written to ureg. During continuous unaligned move-in, because LoadUnAlign caches the subsequent unaligned data to ureg, the next move-in does not need to call LoadUnAlignPre again. LoadUnAlignPre only needs to be called once before the first move-in, thereby achieving performance optimization for unaligned move-in.

        **Figure 1**  Unaligned data move-in<a name="fig638414472320"></a>  
        
        ![](../../../../figures/unaligned_in_77.png)

-   **Unaligned data move-out**

    Write the unaligned data in the source register srcReg to UB address dstAddr. Based on the current state of ureg, there are two scenarios:

    **Scenario 1: ureg is empty**

    -   Call **StoreUnAlign**. In this case, ureg contains no valid data, indicating the start state of a continuous unaligned move-out. The data corresponding to UB addresses 48 \~ 288 in srcReg is written to dstAddr, and the data corresponding to UB addresses 288 \~ 320 in srcReg is cached to ureg.
    -   Call **StoreUnAlignPost** to perform post-processing of the unaligned move-out. The valid data cached in ureg, corresponding to UB addresses 288 \~ 320, is written to UB.

    **Figure 2**  Unaligned data move-out (ureg is empty)<a name="fig34750199610"></a>  
    
    ![](../../../../figures/unaligned_out_0.png)

    **Scenario 2: ureg is not empty**

    -   Call **StoreUnAlign**. In this case, ureg contains valid data. The system concatenates the data corresponding to UB addresses 32 \~ dstAddr in ureg with the data corresponding to UB addresses dstAddr \~ 288 in srcReg, and writes the result to UB address dstAddr. At the same time, the data corresponding to UB addresses 288 \~ 320 in srcReg is cached to ureg. During a continuous unaligned move-out, when ureg is not empty, the next StoreUnAlign reads the data cached to ureg by the current StoreUnAlign. Therefore, StoreUnAlignPost does not need to be called for the current move-out; it only needs to be called once after the move-out is complete, thereby optimizing the performance of the unaligned move-out.
    -   Call **StoreUnAlignPost** to perform post-processing of the unaligned move-out. The valid data cached in ureg, corresponding to UB addresses 288 \~ 320, is written to UB.

    **Figure 3**  Unaligned data move-out (ureg is not empty)<a name="fig579119300430"></a>  
    
    ![](../../../../figures/unaligned_out_1.png)

-   **MaskReg continuous unaligned data move-out**

    Move the source register MaskReg (32B) out to UB.

    -   When the data type at the UB address is b16, the hardware instruction extracts the least significant bit (LSB) from each 2-bit data, packs the 32B data in MaskReg into 16B, and writes it to UB. After the move is complete, the UB address is updated by a 16B offset.
    -   When the data type at the UB address is b32, the hardware instruction extracts the least significant bit (LSB) from each 4-bit data, packs the 32B data in MaskReg into 8B, and writes it to UB. After the move is complete, the UB address is updated by an 8B offset.

    Taking the data type b16 at the UB address as an example, write the data of two MaskRegs to dstAddr \~ 72, and perform the unaligned move-out as follows:

    -   Cache the 16B compressed data of maskReg1 (UB address dstAddr \~ 56) to ureg.
    -   Concatenate the data in ureg with the partial data of the 16B compressed data of maskReg2 (UB address 56 \~ 64), and write the result to UB address dstAddr \~ 64.
    -   Cache the partial data of the 16B compressed data of maskReg2 (UB address 64 \~ 72) to ureg.
    -   Post-process the unaligned move-out. Write the data cached in ureg to UB address 64 \~ 72. The MaskReg continuous unaligned move-out is similar to the RegTensor continuous unaligned move-out. You only need to call StoreUnAlignPost once after the move-out to optimize the performance of the MaskReg continuous unaligned move-out.

    **Figure 4**  MaskReg continuous unaligned data move-out<a name="fig18974124104813"></a>  
    
    ![](../../../../figures/mask_reg_out.png)

-   **Continuous unaligned move-in and move-out example**

    As shown in the following figure, to move the uint32\_t data \[1, 2, 3, ... , 128\] from UB address 48 \~ 560 into dstReg and then move it back to UB, two move-in and move-out operations are required, that is, the for loop is executed twice, with initialization and post-processing moved outside the for loop.

    postUpdateStride = 256B / sizeof(T)  (that is, the address offset is 256B each time), repeatTimes = dataSize / 256B (that is, the number of iterations = total data size / VL).

    The following is an example snippet of the POST_MODE_UPDATE mode of the PostUpdate extended move API:

    ```cpp
    template <typename T> 
    __simd_vf__ inline void LoadUnAlignVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t postUpdateStride, uint16_t repeatTimes) 
    { 
    AscendC::Reg::RegTensor<T> srcReg; 
    AscendC::Reg::UnalignRegForLoad ureg0; 
    AscendC::Reg::UnalignRegForStore ureg1; 
    // Initialize the unaligned move-in. This needs to be called only once before the iteration starts. 
    AscendC::Reg::LoadUnAlignPre(ureg0, srcAddr); 
    for (uint16_t i = 0; i < repeatTimes; ++i) { 
        AscendC::Reg::LoadUnAlign(srcReg, ureg0, srcAddr, postUpdateStride); 
        AscendC::Reg::StoreUnAlign(dstAddr, srcReg, ureg1, postUpdateStride); 
    } 
    // Perform post-processing for the unaligned move-out. This needs to be called only once after the iteration ends. 
    AscendC::Reg::StoreUnAlignPost(dstAddr, ureg1, 0); 
    }
    ```

    1.  Unaligned move-in initialization: update ureg1 = \[1, 2, 3, 4\];
    2.  Unaligned move-in: tmpReg =\[5, 6, 7, ... , 68\], write the partial data of tmpReg and the data of ureg1 into dstReg = \[1, 2, 3, ... , 64\], update ureg1 = \[61, 62, 63, ... , 68\];
    3.  Unaligned move-out: write the partial data \[1, 2, 3, ... , 60\] of dstReg into UB address 48 \~ 288, update ureg2 = \[61, 62, 63, 64\];
    4.  Unaligned move-in: tmpReg = \[69, 70, 71, ... ,128\], the tmpReg data and partial data of ureg1 are written to dstReg = \[65, 66 67, ... , 128\];
    5.  Unaligned move-out: ureg2 data \[61, 62, 63, 64\] and partial data of dstReg \[65, 66, 67, ... ,124\] are written to UB address 288 \~ 544, and ureg2 is updated to \[125, 126, 127, 128\];
    6.  Unaligned move-out post-processing: the data cached in ureg2 \[125, 126, 127, 128\] is written to UB address 544 \~ 560.

    **Figure 5**  Continuous unaligned move-in and move-out<a name="fig98921837195710"></a>  
    
    ![](../../../../figures/unaligned_io.png)
