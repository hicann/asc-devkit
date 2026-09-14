# Broadcast Scenarios<a name="ZH-CN_TOPIC_0000002500548092"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-26T14:02:54.607Z -->

In some scenarios, the two inputs may have different shapes. Since the [Add](../../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Add.md) API supports computation only on inputs with the same shape, you need to transform the input shapes before performing the Add computation. This section describes how Broadcast is handled in operator implementation for inputs that meet the Broadcast conditions. For other scenarios, refer to the ideas provided in this chapter.

>[!NOTE] Note
>The Broadcast mechanism expands the data of the smaller dimension so that inputs with different shapes can be computed, thereby avoiding explicit copy operations and improving computation efficiency. For data to be broadcast, the following conditions must be met: the two inputs have the same number of dimensions, and they differ in length in only one dimension, where one input has a length of 1 in that dimension. For example, two inputs with shapes \(32, 8\) and \(32, 1\) can be broadcast because they are both two-dimensional, the first dimension is equal in size, and in the unequal dimension the second input has a dimension of 1, which meets the conditions.

This section uses the [Broadcast](../../../../api/SIMD-API/advanced_api/tensor_transform/Broadcast.md) API, so the inputs must meet the constraints of this API. In addition, due to hardware limitations, the input addresses of this API must be 32-byte aligned. This section uses an example where the input has two dimensions and the second axis (axis = 1) needs to be broadcast. For the complete sample code, see [Add Operator Sample with Broadcast Input](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/08_transpose/add_broadcast).

## Tiling Implementation<a name="zh-cn_topic_0000002201157446_section598962019342"></a>

Compared with the scenario where the input shape is the same, add corresponding member variables to the Tiling structure to indicate whether the input needs to be broadcast, which dimension needs to be broadcast, and the multiple by which the broadcast axis needs to be expanded. Therefore, four new Tiling structure members are added:

-   xLen and yLen: indicate the data lengths of the two inputs.
-   axis: indicates which dimension of the input is broadcast.
-   coef: indicates the multiple by which the broadcast input needs to be expanded. For example, if the x shape is \(m, 1\) and the y shape is \(m, n\), then coef = n. As shown in the following figure, the parts with the same color in the figure are the data blocks for a single calculation.

**Figure 1**  Schematic diagram of coef when axis = 1<a name="zh-cn_topic_0000002201157446_fig202632320133"></a>  
![](../../../figures/axis_1_coef_diagram.png "Schematic diagram of coef when axis = 1")

The code for defining the Tiling structure is as follows:

```
struct AddCustomTilingData {
    uint32_t xLen;
    uint32_t yLen;
    uint32_t coef;
    uint32_t axis;
    ...
};
```

Let the length of the input that needs to be broadcast be shorterAxisLen, and the length of the input that does not need to be broadcast be totalLength.

```
constexpr uint32_t BLOCK_SIZE = 32;
...  // Read data
uint32_t totalLength = (xLen > yLen)? xLen : yLen;
uint32_t shorterAxisLen = (xLen < yLen)? xLen : yLen;
```

Use shorterAxisLen for core division calculation, and use the length after core division multiplied by coef as the core division length of totalLength.

```
constexpr uint32_t BLOCK_SIZE = 32;
uint32_t alignCoef = (tiling->axis == 0U) ? shorterAxisLen : totalLength / shorterAxisLen;
uint32_t divDimCoef = (tiling->axis == 0U) ? totalLength / shorterAxisLen : shorterAxisLen;
if (divDimCoef % blockDim == 0U) {
    uint32_t blockLength = divDimCoef / blockDim * alignCoef;    
    ...
} else {
    uint32_t formerNum = (divDimCoef / BUFFER_NUM) % blockDim;
    uint32_t tailNum = blockDim - formerNum;

    uint32_t formerLength = ((divDimCoef / BUFFER_NUM) / blockDim + 1U) * BUFFER_NUM * alignCoef;
    uint32_t tailLength = ((divDimCoef / BUFFER_NUM) / blockDim) * BUFFER_NUM * alignCoef;
    ....
}
```

When performing intra-core data splitting, calculate the number of Unified Buffer data blocks, ubBlockAligned, which is the number aligned to coef and BUFFER\_NUM.

```
uint32_t ubBlockAligned =
        (MAX_AVAILABLE_UB_BLOCK_NUM * alignNum / (alignCoef * BUFFER_NUM) * (alignCoef * BUFFER_NUM) == 0U) ?
            MAX_AVAILABLE_UB_BLOCK_NUM :
            MAX_AVAILABLE_UB_BLOCK_NUM * alignNum / (alignCoef * BUFFER_NUM) * (alignCoef * BUFFER_NUM);
...
tileNum = length / ubBlockAligned;
if (length % ubBlockAligned == 0U || tileNum == 0U) {
    if (tileNum == 0U) {
        tileNum = 1U;
    }
    if (length < ubBlockAligned) {
        tileLength = length;
        lastTileLength = tileLength;
    } else {
        tileLength = ubBlockAligned;
        lastTileLength = tileLength;
    }
} else {
    tileNum++;
    tileLength = ubBlockNum;
    lastTileLength = (uint32_t)(length - (tileNum - 1) * tileLength);
}
```

## Operator Class Implementation<a name="zh-cn_topic_0000002201157446_section1017415713416"></a>

During the kernel function initialization phase, determine which input to broadcast based on the parameters passed in by the Tiling structure. Since the broadcast is performed on the second axis (axis = 1) of the input, it can be calculated that for the input requiring broadcast, the length of data moved into each core is blockLength / coef.

The initialization function code is as follows:

```
__aicore__ inline void Init(GM_ADDR x, GM_ADDR y, GM_ADDR z, AddCustomTilingData tiling, AscendC::TPipe* pipeIn)
{
    pipe = pipeIn;
    GM_ADDR longerInputPtr;
    GM_ADDR shorterInputPtr;
    if (tiling.xLen > tiling.yLen) {
        longerInputPtr = x;
        shorterInputPtr = y;
        this->shorterAxisLen = tiling.yLen;
    } else {
        longerInputPtr = y;
        shorterInputPtr = x;
        this->shorterAxisLen = tiling.xLen;
    }
    this->coef = tiling.coef;
    if (tiling.isEvenCore) {
        this->tileNum = tiling.tileNum;
        this->tileLength = tiling.tileLength / BUFFER_NUM;
        this->lastTileLength = tiling.lastTileLength;
        xGm.SetGlobalBuffer((__gm__ T*)longerInputPtr + tiling.blockLength * AscendC::GetBlockIdx(), tiling.blockLength);
        yGm.SetGlobalBuffer((__gm__ T*)shorterInputPtr, this->shorterAxisLen);
        zGm.SetGlobalBuffer((__gm__ T*)z + tiling.blockLength * AscendC::GetBlockIdx(), tiling.blockLength);
    } else {
        if (AscendC::GetBlockIdx() < tiling.formerNum) {
            this->tileNum = tiling.formerTileNum;
            this->tileLength = tiling.formerTileLength / BUFFER_NUM;
            this->lastTileLength = tiling.formerLastTileLength;
            xGm.SetGlobalBuffer((__gm__ T*)longerInputPtr + tiling.formerLength * AscendC::GetBlockIdx(), tiling.formerLength);
            yGm.SetGlobalBuffer((__gm__ T*)shorterInputPtr, this->shorterAxisLen);
            zGm.SetGlobalBuffer((__gm__ T*)z + tiling.formerLength * AscendC::GetBlockIdx(), tiling.formerLength);
        } else {
            this->tileNum = tiling.tailTileNum;
            this->tileLength = tiling.tailTileLength / BUFFER_NUM;
            this->lastTileLength = tiling.tailLastTileLength;
            xGm.SetGlobalBuffer((__gm__ T*)longerInputPtr + tiling.formerLength * tiling.formerNum +
                tiling.tailLength * (AscendC::GetBlockIdx() - tiling.formerNum), tiling.tailLength);
            yGm.SetGlobalBuffer((__gm__ T*)shorterInputPtr, this->shorterAxisLen);
            zGm.SetGlobalBuffer((__gm__ T*)z + tiling.formerLength * tiling.formerNum +
                tiling.tailLength * (AscendC::GetBlockIdx() - tiling.formerNum), tiling.tailLength);
        }
    }
    pipe->InitBuffer(inQueueX, BUFFER_NUM, this->tileLength * sizeof(T));
    pipe->InitBuffer(inQueueY, BUFFER_NUM, this->coef * sizeof(T));
    pipe->InitBuffer(outQueueZ, BUFFER_NUM, this->tileLength * sizeof(T));
    pipe->InitBuffer(tmpBuf0, this->tileLength * sizeof(dataType));
    pipe->InitBuffer(tmpBuf1, this->tileLength * sizeof(dataType));
}
```

Since the data is aligned to coef, the address may not satisfy the 32-byte alignment requirement during data copy. Therefore, [DataCopyPad (GM -> UB)](../../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopyPad_GMToUB.md) is used in the CopyIn function, and [DataCopyPad (UB -> GM)](../../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopyPad_UBToGM.md) is used in the CopyOut function for data copy.

The CopyIn function implementation code is as follows:

```
__aicore__ inline void CopyIn(int32_t progress)
{
    AscendC::LocalTensor<T> xLocal = inQueueX.AllocTensor<T>();
    AscendC::LocalTensor<T> yLocal = inQueueY.AllocTensor<T>();
    AscendC::DataCopyExtParams copyParams = {1, (uint32_t)(this->tileLength * sizeof(T)), 0, 0, 0};
    AscendC::DataCopyPadExtParams<T> padParams = {false, 0, 0, 0};
    AscendC::DataCopyPad<T>(xLocal, xGm[progress * this->tileLength], copyParams, padParams);
    AscendC::DataCopyPad<T>(yLocal, yGm[(progress % BUFFER_NUM) * this->tileLength], copyParams,
                                        padParams);
    inQueueX.EnQue(xLocal);
    inQueueY.EnQue(yLocal);
}
```

The CopyOut function implementation code is as follows:

```
__aicore__ inline void CopyOut(int32_t progress)
{
    AscendC::LocalTensor<T> zLocal = outQueueZ.DeQue<T>();
    AscendC::DataCopyExtParams copyParams = {1, (uint32_t)(this->tileLength * sizeof(T)), 0, 0, 0};
    AscendC::DataCopyPad<T>(zGm[progress * this->tileLength], zLocal, copyParams);
    outQueueZ.FreeTensor(zLocal);
}
```

In the Compute function, the input must be broadcast before calling the Add API. Here, the shapes before and after broadcast need to be calculated. Based on the data relationship mentioned earlier, the shapes before and after broadcast can be calculated as \{tileLength / broadcastCoef, 1\} and \{tileLength / broadcastCoef, broadcastCoef\}, respectively. On this basis, broadcast the input, store the calculation result in temporary space, and then perform the Add calculation. The implementation code example is as follows:

```
__aicore__ inline void Compute(int32_t progress)
{
    AscendC::LocalTensor<T> xLocal = inQueueX.DeQue<T>();
    AscendC::LocalTensor<T> yLocal = inQueueY.DeQue<T>();
    AscendC::LocalTensor<T> zLocal = outQueueZ.AllocTensor<T>();
    AscendC::LocalTensor<T> broadcastTmpTensor = tmpBuf2.Get<T>();
    uint32_t dstShape[] = {this->tileLength / this->coef, this->coef};
    uint32_t srcShape[] = {this->tileLength / this->coef, 1};
    AscendC::Broadcast<T, 2, 1>(broadcastTmpTensor, yLocal, dstShape, srcShape);
    ...
}
```
