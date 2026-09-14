# Tail Block Tiling<a name="ZH-CN_TOPIC_0000002500548090"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:09:40.709Z -->

As shown in the following example, the input shape of the operator is (1, 2048), and the supported data type is half. The input data can be aligned to the size of one datablock (32 bytes). The input data is 2048 \* 2 / 32 = 128 datablocks, so it can be evenly distributed on each core (assuming 8 cores are used). Each core processes 256 numbers, that is, 16 datablocks. In this case, tail block processing is not required.

**Figure 1**  Shape alignment scenario<a name="zh-cn_topic_0000002201157442_fig55115212126"></a>  
![](../../../../figures/shape_alignment_scenario.png "shape alignment scenario")

For some shapes, for example, the input shape of the operator is (1, 1904), and the supported data type is half. The input data can be aligned to the size of one datablock (32 bytes) and evenly distributed on each core (assuming 8 cores are used). Each core processes 238 numbers, which cannot be evenly divided into datablocks. After 14 full datablocks are allocated, 14 numbers (28 bytes) remain. Therefore, tail block processing is required after multi-core splitting.

When data of inputs with different shapes is split, the tiled data may be evenly distributed across multiple cores, but the data within each core cannot be evenly divided. For this scenario, add the variable lastTileLength to the Tiling parameters to indicate the size of the last block, that is, the tail block. Therefore, the Tiling structure of the operator contains the following four members:

-   blockLength: length of the data computed on each core;
-   tileNum: number of main blocks split on each core;
-   tileLength: length of each main block on each core;
-   lastTileLength: length of the tail block on each core.

**Figure 2**  Multi-core Tiling tail block illustration<a name="zh-cn_topic_0000002201157442_fig48081401102"></a>  
![](../../../../figures/multi_core_tiling_tail_block_diagram.png "multi-core Tiling tail block illustration")

## Tiling Implementation<a name="zh-cn_topic_0000002201157442_section469034017306"></a>

The Tiling structure of the operator is defined as follows:

```
struct AddCustomTilingData {
    uint32_t blockLength;
    uint32_t tileNum;
    uint32_t tileLength;
    uint32_t lastTileLength;
    ...
};
```

The main task of Tiling implementation on the host side is to compute the four member variables above. The procedure is as follows:

1.  Determine whether the total data length `totalLength` is 32-byte aligned. If not, compute `totalLengthAligned`, which is `totalLength` rounded up to 32-byte alignment.

    ```
    constexpr uint32_t BLOCK_SIZE = 32;
    // For ease of calculation, define the variable `alignNum` as the alignment number based on the data type.
    uint32_t alignNum = BLOCK_SIZE / dataTypeSize;
    // `totalLength` is the total amount of data.
    totalLengthAligned = (totalLength % alignNum == 0U) ?
                          static_cast<uint32_t>(totalLength) :
                          ((static_cast<uint32_t>(totalLength) + alignNum - 1) / alignNum) * alignNum;
    ```

2.  Determine whether `totalLengthAligned` can be evenly divided by the number of cores used, `NumBlocks`. If so, compute the data length `blockLength` on each core.

    ```
    constexpr uint32_t NUM_BLOCKS = 8;
    constexpr uint32_t UB_BLOCK_NUM = 100;  // For ease of verification, `UB_BLOCK_NUM` is used as the number of blocks available in the Unified Buffer, so the available UB space size is `UB_BLOCK_NUM * BLOCK_SIZE`.
    uint32_t blockLength, tileNum;
    if ((totalLengthAligned / alignNum) % NUM_BLOCKS == 0U) {
        blockLength = totalLengthAligned / NUM_BLOCKS;
    }
    ```

3.  Compute `tileNum`. To reduce data transfer overhead, use the Unified Buffer space on the core as much as possible. Compute `tileNum` based on the workload on each core and the available Unified Buffer space size.

    ```
    tileNum = blockLength / (alignNum * UB_BLOCK_NUM);
    ```

4.  Based on the computed `tileNum`, compute `tileLength` and `lastTileLength`.

    If the computation workload on each core can be evenly divided by the currently available Unified Buffer space, the scenario is handled as one without a tail block.

    ```
    if (static_cast<uint32_t>(blockLength / alignNum) % UB_BLOCK_NUM == 0U) {
        // The computation workload on a single core can be evenly divided by the currently available UB space, so there is only a main block and no tail block.
        tileLength = UB_BLOCK_NUM * alignNum;
        lastTileLength = 0U;
    }
    ```

    Otherwise, the scenario is handled as one with a tail block, and the tail block length is the data length computed on a single core minus tileNum \* tileLength.

    ```
    if (tileNum == 0U) {
        // The length to be computed on a single core is smaller than the available UB space, so it is handled as a tail-block-only scenario.
        tileLength = 0U;
        lastTileLength = static_cast<uint32_t>(((blockLength + alignNum - 1) / alignNum) * alignNum);
    } else {
        // There are both a main block and a tail block.
        tileLength = UB_BLOCK_NUM * alignNum;
        lastTileLength = static_cast<uint32_t>(blockLength - tileNum * tileLength);
    }
    ```

The Host-side Tiling implementation code is as follows:

```
constexpr uint32_t BLOCK_SIZE = 32;
constexpr uint32_t NUM_BLOCKS = 8;
constexpr uint32_t UB_BLOCK_NUM = 100;  // For easy verification, UB_BLOCK_NUM is used here as the number of blocks available in the UB. Therefore, the available UB space size is UB_BLOCK_NUM * BLOCK_SIZE.
...

uint32_t alignNum = BLOCK_SIZE / dataTypeSize;  // For easy calculation, the variable alignNum is defined based on the data type as the alignment number, where dataTypeSize is the number of bytes corresponding to the data type of the operation data.
// totalLength is the total amount of data.
totalLengthAligned = (totalLength % alignNum == 0U) ?
                             static_cast<uint32_t>(totalLength) :
                             ((static_cast<uint32_t>(totalLength) + alignNum - 1) / alignNum) * alignNum;
uint32_t blockLength, tileNum;
if ((totalLengthAligned / alignNum) % NUM_BLOCKS == 0U) {
    blockLength = totalLengthAligned / NUM_BLOCKS;
    tileNum = blockLength / alignNum / UB_BLOCK_NUM;

    if (tileNum == 0) {
        // The length to be computed on a single core is smaller than the available UB space, so it is handled as a tail-block-only scenario.
        tileLength = 0;
        lastTileLength = ((blockLength + alignNum - 1) / alignNum) * alignNum;
    } else if ((blockLength / alignNum) % UB_BLOCK_NUM == 0) {
        // The computation workload of a single core can be evenly divided by the currently available UB space, so there is only a main block and no tail block.
        tileLength = UB_BLOCK_NUM * alignNum;
        lastTileLength = 0;
    } else {
        // There are both a main block and a tail block.
        tileLength = UB_BLOCK_NUM * alignNum;
        lastTileLength = blockLength - tileNum * tileLength;
    }
    ...
}
```

After the input data of shape \(1, 1904\) is computed, the values of the variables in the tiling structure are as follows:

```
struct AddCustomTilingData {
    uint32_t blockLength = 238;      // Each core computes 238 half values, and 8 cores compute 1904 half values in total.
    uint32_t tileNum = 0;            // The available UB space is sufficient, so this is a scenario with only a tail block.
    uint32_t tileLength = 0;         // There is no main block, so the main block length is 0.
    uint32_t lastTileLength = 240;   // 238 half values are not 32B-aligned, so they are aligned to 240 half values for transfer.
    ...
};
```

## Operator Class Implementation<a name="zh-cn_topic_0000002201157442_section2027413973119"></a>

Compared with [Multi-core Tiling](multi_core_tiling.md), when allocating memory for the input and output queues through the Pipe memory management object in the Init function, use the larger value between tileLength and lastTileLength as the length of the allocated memory. For example, when the length to be computed on a single core is smaller than the available UB space, it is processed as a tail-block-only scenario, where tileLength is 0 and lastTileLength is the datablock length. Therefore, the larger of the two values must be used to allocate memory.

```
uint32_t initBufferLength = AscendC::Std::max(this->tileLength, this->lastTileLength);
pipe->InitBuffer(inQueueX, 1, this->initBufferLength * sizeof(dataType));
```

Because the tail block length is lastTileLength, which differs from the main block datablock length, the datablock length parameter tileLength to be processed in the current loop, that is, the data length of the main block or tail block to be processed, is passed to the CopyIn, Compute, and CopyOut functions.

The Process function is implemented as follows:

```
__aicore__ inline void Process()
{
    // Compute the main block data, whose datablock length is tileLength
    for (uint32_t i = 0; i < this->tileNum; i++) {
        CopyIn(i, this->tileLength);
        Compute(i, this->tileLength);
        CopyOut(i, this->tileLength);
    }
    // Compute the tail block data, whose datablock length is lastTileLength
    if (this->lastTileLength > 0) {
        CopyIn(this->tileNum, this->lastTileLength);
        Compute(this->tileNum, this->lastTileLength);
        CopyOut(this->tileNum, this->lastTileLength);
    }
}
```

The CopyIn function is implemented as follows:

```
__aicore__ inline void CopyIn(int32_t progress, uint32_t tileLength)
{
    AscendC::LocalTensor<T> xLocal = inQueueX.AllocTensor<T>();
    AscendC::LocalTensor<T> yLocal = inQueueY.AllocTensor<T>();
    AscendC::DataCopy(xLocal, xGm[progress * this->tileLength], tileLength);
    AscendC::DataCopy(yLocal, yGm[progress * this->tileLength], tileLength);
    inQueueX.EnQue(xLocal);
    inQueueY.EnQue(yLocal);
}
```

The Compute function is implemented as follows:

```
__aicore__ inline void Compute(int32_t progress, uint32_t tileLength)
{
    AscendC::LocalTensor<T> xLocal = inQueueX.DeQue<T>();
    AscendC::LocalTensor<T> yLocal = inQueueY.DeQue<T>();
    AscendC::LocalTensor<T> zLocal = outQueueZ.AllocTensor<T>();
    AscendC::Add(zLocal, xLocal, yLocal, tileLength);
    outQueueZ.EnQue<T>(zLocal);
    inQueueX.FreeTensor(xLocal);
    inQueueY.FreeTensor(yLocal);
}
```

The CopyOut function is implemented as follows:

```
__aicore__ inline void CopyOut(int32_t progress, uint32_t tileLength)
{
    AscendC::LocalTensor<T> zLocal = outQueueZ.DeQue<T>();
    AscendC::DataCopy(zGm[progress * this->tileLength], zLocal, tileLength);
    outQueueZ.FreeTensor(zLocal);
}
```

