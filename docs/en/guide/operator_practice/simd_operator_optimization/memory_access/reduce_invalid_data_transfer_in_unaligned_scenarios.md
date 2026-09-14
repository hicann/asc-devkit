# Reducing Invalid Data Migration in Non-Aligned Scenarios<a name="ZH-CN_TOPIC_0000002497818522"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:18:18.187Z -->

[Priority] Medium

> [!NOTE] Note
> This performance optimization suggestion applies to the following models:
> - Ascend 950PR/Ascend 950DT

[Description] In non-aligned data migration scenarios, Ascend 950PR/Ascend 950DT provides the DataCopyPad API at the basic API layer. This API supports two migration modes: Normal and Compact. When migrating multiple non-32B-aligned data blocks, using the Compact mode can reduce the amount of invalid data migrated and save bandwidth.

Assume that three data blocks need to be migrated, each with a size of 48B and a data type of float. Except for these three 48-byte data blocks, all other data is invalid.

[Negative Example] Using the DataCopyPad API to migrate data in Normal mode

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

After migration, the data in the UB is as follows:

```
[1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0., 0., 0., 0., 
 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0., 0., 0., 0.,
 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0., 0., 0., 0.....]
```

**Figure 1** Normal mode migration<a name="fig1316440112511"></a>  
![](../../../figures/normal_mode_migration.png "Normal mode migration")

As shown in the figure, since each data block is 48B and not 32B-aligned, 16B of padding data must be inserted when migrating each data block to make the data 32B-aligned. Finally, 192B of data is migrated to the UB, including 48B of invalid data.

[Positive Example] Using the Compact mode for migration optimization

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

After the migration, the data in the UB is as follows:

```
[1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.,
 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.,
 1., 1., 0., 0., 0., 0....]
```

**Figure 2** Compact mode migration<a name="fig203951741467"></a>  
![](../../../figures/compact_mode_migration.png "Compact mode migration")

According to the diagram of Compact mode migration, a total of 160B of data is migrated, including 16B of invalid data.

[Summary] By comparison, when migrating multiple data blocks that are not 32B-aligned, using the Compact mode can reduce the amount of invalid data migrated and save bandwidth.

