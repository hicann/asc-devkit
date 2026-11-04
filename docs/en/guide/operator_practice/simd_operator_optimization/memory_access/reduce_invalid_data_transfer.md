# Reducing Invalid Data Transfer in Non-Aligned Scenarios<a name="ZH-CN_TOPIC_0000002497818522"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T12:40:47.425Z pushedAt=2026-11-03T07:05:13.332Z nl=2 -->

[Priority] Medium

> [!NOTE] Note
> This performance optimization suggestion applies to the following models:
><!-- npu="950" id1 -->
>- Ascend 950PR & 950DT products
><!-- end id1 -->

[Description] In non-aligned data transfer scenarios, the Ascend 950PR & 950DT products provide the DataCopyPad API at the basic API level. This API supports two transfer modes: Normal and Compact. When transferring multiple data blocks that are not 32B-aligned, using the Compact mode can reduce the amount of invalid data transferred and save bandwidth.

Assume that three data blocks need to be transferred, each with a size of 48B and a data type of float. Except for these three 48-byte data blocks, all other data is invalid.

[Negative example] Using the **DataCopyPad** API to transfer data in Normal mode

```
__aicore__ inline void CopyIn(){
    AscendC::LocalTensor<T> xLocal = inQueueX.AllocTensor<T>();
    AscendC::Duplicate<T>(xLocal, 0, count);
    AscendC::DataCopyParams dataCopyParams;
    dataCopyParams.blockCount = 3;
    dataCopyParams.blockLen = 48;
    dataCopyParams.srcStride = 0;
    dataCopyParams.dstStride = 0;
    AscendC::DataCopyPadParams dataCopyPadParams;
    dataCopyPadParams.isPad = 1;
    dataCopyPadParams.leftPadding = 0;
    dataCopyPadParams.rightPadding = 4;
    dataCopyPadParams.paddingValue = 0;
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Normal>(xLocal, xGm, dataCopyParams, dataCopyPadParams);
    inQueueX.EnQue<T>(xLocal);
}
```

After the transfer, the data in the Unified Buffer (UB) is as follows:

```
[1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0., 0., 0., 0., 
 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0., 0., 0., 0.,
 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0., 0., 0., 0.....]
```

**Figure 1**  Normal mode transfer<a name="fig1316440112511"></a>  
![](../../../figures/norm_copy.png "Normal mode transfer")

As shown in the figure, since each data block is 48B and not 32B-aligned, 16B of padding data must be inserted when transferring each data block to make the data 32B-aligned. As a result, 192B of data is transferred to the UB, including 48B of invalid data.

[Positive example] Using the Compact mode for optimization

```
__aicore__ inline void CopyIn(){
    AscendC::LocalTensor<T> xLocal = inQueueX.AllocTensor<T>();
    AscendC::Duplicate<T>(xLocal, 0, count);
    AscendC::DataCopyParams dataCopyParams;
    dataCopyParams.blockCount = 3;
    dataCopyParams.blockLen = 48;
    dataCopyParams.srcStride = 0;
    dataCopyParams.dstStride = 0;
    AscendC::DataCopyPadParams dataCopyPadParams;
    dataCopyPadParams.isPad = 1;
    dataCopyPadParams.leftPadding = 0;
    dataCopyPadParams.rightPadding = 4;
    dataCopyPadParams.paddingValue = 0;
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Compact>(xLocal, xGm, dataCopyParams, dataCopyPadParams);
    inQueueX.EnQue<T>(xLocal);
}
```

After the transfer, the data in the UB is as follows:

```
[1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.,
 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.,
 1., 1., 0., 0., 0., 0....]
```

**Figure 2**  Compact-mode transfer<a name="fig203951741467"></a>  
![](../../../figures/compact_copy.png "Compact mode transfer")

According to the diagram of the compact-mode transfer, 160B of data is transferred in total, including 16B of invalid data.

[Summary] By comparison, when transferring multiple data blocks that are not 32B-aligned, using the compact mode reduces the amount of invalid data transferred and saves bandwidth.
