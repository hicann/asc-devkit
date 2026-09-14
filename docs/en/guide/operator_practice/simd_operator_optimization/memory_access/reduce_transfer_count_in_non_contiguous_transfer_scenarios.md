# Reducing the Number of Transfers in Non-contiguous Transfer Scenarios<a name="ZH-CN_TOPIC_0000002529778475"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-26T14:18:45.613Z -->

[Priority] Medium

> [!NOTE] Note
> This performance optimization suggestion applies to the following product models:
> - Ascend 950PR/Ascend 950DT

In non-contiguous transfer scenarios, you can use the Loop mode of the DataCopyPad API and the multi-dimensional data movement API of DataCopy to reduce the number of transfers and optimize transfer performance.

## Reducing Non-contiguous Transfer Counts Using Loop Mode<a name="section191811840221"></a>

[Description] In addition to the Normal/Compact modes, the DataCopyPad API supports the Loop mode for moving two-dimensional data. Assume that we want to move eight 48-byte data blocks in the following manner:

![](../../../figures/data_movement_optimization.png)

[Negative Example] Calling the transfer API multiple times (using DataCopyPad as an example)

```
__aicore__ inline void CopyIn3(){
    AscendC::LocalTensor<T> xLocal = inQueueX.AllocTensor<T>();
    AscendC::Duplicate<T>(xLocal, 0, count);
    AscendC::DataCopyParams dataCopyParams;
    dataCopyParams.blockCount = 2;
    dataCopyParams.blockLen = 48;
    dataCopyParams.srcStride = 0;
    dataCopyParams.dstStride = 0;
    AscendC::DataCopyPadParams dataCopyPadParams;
    dataCopyPadParams.isPad = 0;
    dataCopyPadParams.leftPadding = 0;
    dataCopyPadParams.rightPadding = 0;
    dataCopyPadParams.paddingValue = 0;
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Compact>(xLocal, xGm, dataCopyParams, dataCopyPadParams);
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Compact>(xLocal[32], xGm[24], dataCopyParams, dataCopyPadParams);
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Compact>(xLocal[72], xGm[48], dataCopyParams, dataCopyPadParams);
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Compact>(xLocal[104], xGm[72], dataCopyParams, dataCopyPadParams);
    inQueueX.EnQue<T>(xLocal);
}
```

**Figure 1**  Moving data by calling the DataCopyPad API multiple times<a name="fig345313593314"></a>  
![](../../../figures/call_datacopypad_api_multiple_times_for_movement.png "Moving data by calling the DataCopyPad API multiple times")

[Positive Example] Moving data using Loop mode

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
    AscendC::DataCopyParams dataCopyParams;
    dataCopyParams.blockCount = 2;
    dataCopyParams.blockLen = 48;
    dataCopyParams.srcStride = 0;
    dataCopyParams.dstStride = 0;
    AscendC::DataCopyPadParams dataCopyPadParams;
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

**Figure 2**  Moving data using Loop mode<a name="fig26533539233"></a>  
![](../../../figures/use_loop_mode_for_movement.png "Moving data using Loop mode")

[Summary] When padding of different sizes needs to be inserted between data blocks, using Loop mode instead of multiple DataCopyPad calls reduces the number of transfer instructions and improves performance.

## Reducing Non-contiguous Transfers Using Multi-dimensional Data Movement<a name="section1229601461213"></a>

[Description] Assume that we want to move two 8B data blocks in the manner shown in the following figure:

**Figure 3** Data before and after transfer<a name="fig20466223158"></a>  
![](../../../figures/data_before_and_after_movement.png "Data before and after transfer")

[Negative Example] Use multiple DataCopyPad calls for the transfer

**Figure 4** Using multiple DataCopyPad calls for the transfer<a name="fig18188132522410"></a>  
![](../../../figures/call_datacopypad_api_multiple_times_for_movement.png "Using multiple DataCopyPad calls for the transfer")

```
__aicore__ inline void CopyIn5(){
    AscendC::LocalTensor<T> xLocal = inQueueX.AllocTensor<T>();
    AscendC::Duplicate<T>(xLocal, 0, count);
    AscendC::DataCopyParams dataCopyParams;
    dataCopyParams.blockCount = 1;
    dataCopyParams.blockLen = 8;
    dataCopyParams.srcStride = 0;
    dataCopyParams.dstStride = 0;
    AscendC::DataCopyPadParams dataCopyPadParams;
    dataCopyPadParams.isPad = 1;
    dataCopyPadParams.leftPadding = 5;
    dataCopyPadParams.rightPadding = 1;
    dataCopyPadParams.paddingValue = 0;
    // First transfer
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Normal>(xLocal, xGm, dataCopyParams, dataCopyPadParams);
    dataCopyPadParams.isPad = 1;
    dataCopyPadParams.leftPadding = 1;
    dataCopyPadParams.rightPadding = 5;
    dataCopyPadParams.paddingValue = 0;
    // Second transfer
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Normal>(xLocal[8], xGm[2], dataCopyParams, dataCopyPadParams);
    inQueueX.EnQue<T>(xLocal);
}
```

[Positive Example] Use multi-dimensional data movement

On Ascend 950PR/Ascend 950DT, the DataCopy API supports multi-dimensional data movement. For details, see [GMToUB Multi-dimensional Data Movement NDDMA (DataCopy)](../../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_GMToUB_NDDMA.md). The following uses 2D data movement as an example:

```
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

**Figure 5** Data before and after transfer<a name="fig284018179396"></a>  
![](../../../figures/data_before_and_after_movement_56.png "Data before and after transfer-56")

[Summary] Using multi-dimensional data movement can reduce the number of transfer instructions in some scenarios, thereby improving performance.
