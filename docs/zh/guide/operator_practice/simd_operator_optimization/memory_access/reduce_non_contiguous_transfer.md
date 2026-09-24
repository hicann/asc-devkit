# 非连续搬运场景减少搬运次数<a name="ZH-CN_TOPIC_0000002529778475"></a>

【优先级】中

>[!NOTE]说明 
>该性能优化建议适用于如下产品型号：
><!-- npu="950" id1 -->
>- Ascend 950PR&950DT系列产品
><!-- end id1 -->

在非连续搬运场景可以使用DataCopyPad接口的Loop模式和DataCopy的多维数据搬运接口来减少搬运次数，优化搬运性能。

## 使用Loop模式减少非连续搬运的次数<a name="section191811840221"></a>

【描述】DataCopyPad接口在Normal/Compact模式基础上，可以使用Loop模式搬运二维数据，假设我们希望以下图的方式搬运8个48B大小的数据块：

以下示例中，`DataCopyExtParams`的`blockLen`和`srcStride`、`LoopModeParams`的stride以及地址布局均以字节描述；`DataCopyExtParams`的`dstStride`以32B的dataBlock为单位，本例使用Compact模式时该参数无效并置为0；`DataCopyPadExtParams`的`leftPadding/rightPadding`和`LocalTensor`、`GlobalTensor`的`operator[]`偏移均以元素为单位。以下代码假定`T`为普通的按字节存储类型（如`float`、`half`或`int8_t`），并通过`sizeof(T)`将字节偏移转换为元素偏移；4bit打包类型需要单独处理。

![](../../../figures/copy_opt.png)

【反例】调用多次搬运接口进行搬运（以DataCopyPad为例）

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

**图1**  使用多次DataCopyPad接口进行搬运<a name="fig345313593314"></a>  
![](../../../figures/dcpad_api.png "使用多次DataCopyPad接口进行搬运")

【正例】使用Loop模式进行搬运

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

**图2**  使用Loop模式进行搬运<a name="fig26533539233"></a>  
![](../../../figures/loop_copy.png "使用Loop模式进行搬运")

【总结】当数据块之间需要插入不同大小Padding时，使用Loop模式搬运代替多次的DataCopyPad能够减少搬运指令的使用，提升性能。

## 使用多维数据搬运减少非连续搬运次数<a name="section1229601461213"></a>

【描述】假设我们希望以下图的方式搬运2个8B大小的数据块：

**图3**  搬运前后数据<a name="fig20466223158"></a>  
![](../../../figures/copy_data.png "搬运前后数据")

【反例】使用多次DataCopyPad进行搬运

**图4**  使用多次DataCopyPad进行搬运<a name="fig18188132522410"></a>  
![](../../../figures/dcpad_copy.png "使用多次DataCopyPad进行搬运")

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
    // 第一次搬运
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Normal>(xLocal, xGm, dataCopyParams, dataCopyPadParams);
    dataCopyPadParams.isPad = 1;
    dataCopyPadParams.leftPadding = rightPadding;
    dataCopyPadParams.rightPadding = leftPadding;
    dataCopyPadParams.paddingValue = 0;
    // 第二次搬运
    AscendC::DataCopyPad<T, AscendC::PaddingMode::Normal>(
        xLocal[dstOffset], xGm[srcOffset], dataCopyParams, dataCopyPadParams);
    inQueueX.EnQue<T>(xLocal);
}
```

【正例】使用多维数据搬运

DataCopy接口在Ascend 950PR&950DT系列产品上支持多维数据的搬运，具体可参考[DataCopy（GMToUB多维数据搬运NDDMA）](../../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_GMToUB_NDDMA.md)。以2D场景的搬运为例，代码如下：

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

**图5**  搬运前后数据<a name="fig284018179396"></a>  
![](../../../figures/copy_data_56.png "搬运前后数据-56")

【总结】使用多维数据搬运在部分场景下能够减少搬运指令的条数，从而提升性能。
