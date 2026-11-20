# Reducing Transfer Count in Non-Contiguous Transfer Scenarios<a name="ZH-CN_TOPIC_0000002529778475"></a>

<!-- md-trans-meta sourceCommit=49eda737d7d7268a9ddb02fcaba4732176bfc076 translatedAt=2026-11-09T10:36:29.817Z pushedAt=2026-11-19T06:32:19.777Z nl=2 -->

[Priority] Medium

>[!NOTE] Note 
>This performance optimization suggestion applies to the following product models:
><!-- npu="950" id1 -->
>- Ascend 950PR&950DT products
><!-- end id1 -->

In non-contiguous transfer scenarios, you can use the loop mode of the **DataCopyPad** API and the multi-dimensional data movement API of DataCopy to reduce the number of data movement operations and optimize data movement performance.

## Using the Loop Mode to Reduce the Number of Non-Contiguous Transfers<a name="section191811840221"></a>

[Description] In addition to the normal/compact modes, the **DataCopyPad** API supports loop mode for moving two-dimensional data. Assume that we want to move eight 48B data blocks in the manner shown in the following figure:

In the following examples, `blockLen` and `srcStride` of `DataCopyExtParams`, the stride of `LoopModeParams`, and the address layout are all described in bytes; `dstStride` of `DataCopyExtParams` is in units of 32B dataBlock, and this parameter is invalid and set to 0 when compact mode is used in this example; `leftPadding/rightPadding` of `DataCopyPadExtParams` and the `operator[]` offsets of `LocalTensor` and `GlobalTensor` are all in units of elements. The following code assumes that `T` is a common byte-stored type (such as `float`, `half`, or `int8_t`), and converts byte offsets to element offsets through `sizeof(T)`; 4-bit packed types need to be handled separately.

<img src="../../../figures/copy_opt.png" alt="" width="920px" height="130px">

[Negative example] Calling the transfer API multiple times (using **DataCopyPad** as an example)

```
__aicore__ inline void CopyIn3(){
    constexpr uint16_t blockCount = 2;
    constexpr uint32_t blockLenBytes = 48;
    constexpr uint32_t copyBytes = blockCount * blockLenBytes;
    constexpr uint32_t dstGap1Bytes = 32;
    constexpr uint32_t dstGap2Bytes = 64;
    constexpr uint32_t dstGap3Bytes = 32;
    constexpr uint32_t srcOffset1 = copyBytes / sizeof(T);
    constexpr uint32_t srcOffset2 = 2 * copyBytes / sizeof(T);
    constexpr uint32_t srcOffset3 = 3 * copyBytes / sizeof(T);
    constexpr uint32_t dstOffset1 = (copyBytes + dstGap1Bytes) / sizeof(T);
    constexpr uint32_t dstOffset2 = (2 * copyBytes + dstGap1Bytes + dstGap2Bytes) / sizeof(T);
    constexpr uint32_t dstOffset3 = (3 * copyBytes + dstGap1Bytes + dstGap2Bytes + dstGap3Bytes) / sizeof(T);
    AscendC::LocalTensor<T> xLocal = inQueueX.AllocTensor<T>();
    AscendC::Duplicate<T>(xLocal, 0, count);
    AscendC::DataCopyExtParams dataCopyParams;
    dataCopyParams.blockCount = blockCount;
    dataCopyParams.blockLen = blockLenBytes;
    dataCopyParams.srcStride = 0;
    dataCopyParams.dstStride = 0;
    dataCopyParams.rsv = 0;
    AscendC::DataCopyPadExtParams<T> dataCopyPadParams;
    dataCopyPadParams.isPad = 0;
    dataCopyPadParams.leftPadding = 0;
    dataCopyPadParams.rightPadding = 0;
    dataCopyPadParams.paddingValue = 0;
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Compact>(xLocal, xGm, dataCopyParams, dataCopyPadParams);
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Compact>(
        xLocal[dstOffset1], xGm[srcOffset1], dataCopyParams, dataCopyPadParams);
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Compact>(
        xLocal[dstOffset2], xGm[srcOffset2], dataCopyParams, dataCopyPadParams);
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Compact>(
        xLocal[dstOffset3], xGm[srcOffset3], dataCopyParams, dataCopyPadParams);
    inQueueX.EnQue<T>(xLocal);
}
```

**Figure 1**  Transfer using multiple DataCopyPad API calls<a name="fig345313593314"></a>  
<img src="../../../figures/dcpad_api.png" alt="Transfer using multiple DataCopyPad APIs" width="920px" height="220px">

[Positive example] Using the loop mode for transfer

```
__aicore__ inline void CopyIn3(){
    AscendC::LoopModeParams loopModeParams;
    loopModeParams.loop1Size = 2;
    loopModeParams.loop2Size = 2;
    loopModeParams.loop1SrcStride = 96;
    loopModeParams.loop1DstStride = 128;
    loopModeParams.loop2SrcStride = 192;
    loopModeParams.loop2DstStride = 288;
    AscendC::LocalTensor<T> xLocal = inQueueX.AllocTensor<T>();
    AscendC::Duplicate<T>(xLocal, 0, count);
    AscendC::DataCopyExtParams dataCopyParams;
    dataCopyParams.blockCount = 2;
    dataCopyParams.blockLen = 48;
    dataCopyParams.srcStride = 0;
    dataCopyParams.dstStride = 0;
    dataCopyParams.rsv = 0;
    AscendC::DataCopyPadExtParams<T> dataCopyPadParams;
    dataCopyPadParams.isPad = 0;
    dataCopyPadParams.leftPadding = 0;
    dataCopyPadParams.rightPadding = 0;
    dataCopyPadParams.paddingValue = 0;
    AscendC::SetLoopModePara(loopModeParams, AscendC::DataCopyMVType::OUT_TO_UB);
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Compact>(xLocal, xGm, dataCopyParams, dataCopyPadParams);
    AscendC::ResetLoopModePara(AscendC::DataCopyMVType::OUT_TO_UB);
    inQueueX.EnQue<T>(xLocal);
}
```

**Figure 2**  Transfer using loop mode<a name="fig26533539233"></a>  
<img src="../../../figures/loop_copy.png" alt="Transfer using Loop mode" width="920px" height="291px">

[Summary] When padding of different sizes needs to be inserted between data blocks, using loop mode instead of multiple **DataCopyPad** calls can reduce the number of transfer instructions and improve performance.

## Reducing Non-Contiguous Transfer Counts Using Multi-Dimensional Data Transfer<a name="section1229601461213"></a>

[Description] Assume that we want to move two 8-byte data blocks in the manner shown in the following figure:

**Figure 3**  Data before and after transfer<a name="fig20466223158"></a>  
<img src="../../../figures/copy_data.png" alt="Data before and after transfer" width="920px" height="175px">

[Negative example] Using multiple **DataCopyPad** calls for the transfer

**Figure 4**  Transfer using multiple DataCopyPad API calls<a name="fig18188132522410"></a>  
<img src="../../../figures/dcpad_copy.png" alt="Transfer using multiple DataCopyPad APIs" width="920px" height="260px">

```
__aicore__ inline void CopyIn5(){
    constexpr uint32_t blockLenBytes = 8;
    constexpr uint8_t leftPadding = 5;
    constexpr uint8_t rightPadding = 1;
    constexpr uint32_t srcOffset = blockLenBytes / sizeof(T);
    constexpr uint32_t dstOffset =
        ((blockLenBytes + (leftPadding + rightPadding) * sizeof(T) + 31) / 32 * 32) / sizeof(T);
    AscendC::LocalTensor<T> xLocal = inQueueX.AllocTensor<T>();
    AscendC::Duplicate<T>(xLocal, 0, count);
    AscendC::DataCopyExtParams dataCopyParams;
    dataCopyParams.blockCount = 1;
    dataCopyParams.blockLen = blockLenBytes;
    dataCopyParams.srcStride = 0;
    dataCopyParams.dstStride = 0;
    dataCopyParams.rsv = 0;
    AscendC::DataCopyPadExtParams<T> dataCopyPadParams;
    dataCopyPadParams.isPad = 1;
    dataCopyPadParams.leftPadding = leftPadding;
    dataCopyPadParams.rightPadding = rightPadding;
    dataCopyPadParams.paddingValue = 0;
    // First transfer
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Normal>(xLocal, xGm, dataCopyParams, dataCopyPadParams);
    dataCopyPadParams.isPad = 1;
    dataCopyPadParams.leftPadding = rightPadding;
    dataCopyPadParams.rightPadding = leftPadding;
    dataCopyPadParams.paddingValue = 0;
    // Second transfer
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Normal>(
        xLocal[dstOffset], xGm[srcOffset], dataCopyParams, dataCopyPadParams);
    inQueueX.EnQue<T>(xLocal);
}
```

[Positive example] Using multi-dimensional data movement

The **DataCopy** API supports multi-dimensional data movement on Ascend 950PR&950DT products. For details, see [DataCopy (GMToUB multi-dimensional data movement NDDMA)](../../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_GMToUB_NDDMA.md). Taking 2D data movement as an example, the code is as follows:

```cpp
__aicore__ inline void CopyIn6(){
    AscendC::LocalTensor<T> xLocal = inQueueX.AllocTensor<T>();
    AscendC::Duplicate<T>(xLocal, 0, count);
    AscendC::NdDmaLoopInfo<2> loopInfo{{1, 2}, {1, 4}, {2, 2}, {1, 1}, {1, 1}};
    AscendC::NdDmaParams<T, 2> params = {loopInfo, 0};
    AscendC::NdDmaDci();
    static constexpr AscendC::NdDmaConfig config = {false};
    AscendC::DataCopy<T, 2, config>(xLocal, xGm, params);
    inQueueX.EnQue<T>(xLocal);
}
```

**Figure 5**  Data before and after transfer<a name="fig284018179396"></a>  
<img src="../../../figures/copy_data_56.png" alt="Data before and after transfer - 56" width="600px" height="207px">

[Summary] Using multi-dimensional data movement can reduce the number of movement instructions in some scenarios, thereby improving performance.
