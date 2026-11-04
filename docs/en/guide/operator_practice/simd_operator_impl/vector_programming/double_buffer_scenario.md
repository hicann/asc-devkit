# DoubleBuffer Scenarios<a name="ZH-CN_TOPIC_0000002532228161"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:22:30.430Z pushedAt=2026-11-03T07:05:13.313Z nl=2 -->

Because operators may move data in and out multiple times, the [DoubleBuffer](../../../technical_appendix/concepts_and_terms/performance_optimization/double_buffer.md) mechanism is introduced to make full use of hardware resources and implement multi-pipeline parallelism. [DoubleBuffer](../../../technical_appendix/concepts_and_terms/performance_optimization/double_buffer.md) divides the input data into two blocks of equal size and makes full use of the hardware resources of the AI Core to implement parallel execution of data copy-in, computation, and data copy-out. The following uses the "uneven split across cores, uneven split within a core" sample to describe how DoubleBuffer is implemented in an operator.

**Figure 1**  DoubleBuffer data splitting diagram<a name="zh-cn_topic_0000002236197681_fig68713182104"></a>  
![](../../../figures/dbuf_split.png "DoubleBuffer data splitting diagram")

## Tiling Implementation<a name="zh-cn_topic_0000002236197681_section1967484164119"></a>

After DoubleBuffer is enabled, each data block is split into two blocks of equal size. Therefore, to enable DoubleBuffer, the total data volume must be evenly divisible. To simplify processing, the available Unified Buffer (UB) space is divided into n dataBlocks at a granularity of 32 bytes. If n is not an even number, it is decremented by 1. In this way, a single set of code is compatible with both enabling and disabling DoubleBuffer. The corresponding steps are as follows:

1.  Check whether the total data length totalLength is 32-byte aligned. If not, calculate totalLengthAligned, which is totalLength rounded up to 32-byte alignment.

    ```
    constexpr uint32_t BLOCK_SIZE = 32;
    // For ease of calculation, define the variable alignNum as the alignment number based on the data type.
    uint32_t alignNum = BLOCK_SIZE / dataTypeSize;
    // totalLength is the total data volume.
    uint32_t totalLengthAligned = (totalLength % alignNum == 0)?
            totalLength : ((totalLength + alignNum - 1) / alignNum) * alignNum;
    ```

2.  Based on totalLengthAligned, calculate blockLength, the data length computed by each core. For the core splitting policy, see [Tail Core Splitting](multi_core_tiling/tail_core_split.md).
3.  Calculate the remaining Tiling parameters.

    Split the currently available UB space at a granularity of 32 bytes to calculate the number of data blocks UB\_BLOCK\_NUM. Based on whether DoubleBuffer is enabled, calculate the maximum number of data blocks currently available, denoted as MAX\_AVAILABLE\_UB\_BLOCK\_NUM. Finally, split blockLength at a granularity of MAX\_AVAILABLE\_UB\_BLOCK\_NUM. For ease of demonstration, the following code directly provides UB\_BLOCK\_NUM as the number of blocks (32 bytes) contained in the currently available UB space.

    ```
    constexpr uint32_t BUFFER_NUM = 2;
    constexpr uint32_t UB_BLOCK_NUM = 21;  // Maximum number of blocks that can be used by the UB
    constexpr uint32_t MAX_AVAILABLE_UB_BLOCK_NUM = UB_BLOCK_NUM / BUFFER_NUM * BUFFER_NUM;

    tileNum = blockLength / (alignNum * MAX_AVAILABLE_UB_BLOCK_NUM);
    if (tileNum == 0) {
        // The length to be computed by a single core is smaller than the available UB space. Process it as a tail block only.
        tileLength = 0;
        lastTileLength = (blockLength + alignNum - 1) / alignNum * alignNum;
    } else if ((blockLength / alignNum) % MAX_AVAILABLE_UB_BLOCK_NUM == 0) {
        // The computation workload of a single core can be evenly divided by the currently available UB space, so there is only a main block and no tail block.
        tileLength = MAX_AVAILABLE_UB_BLOCK_NUM * alignNum;
        lastTileLength = 0;
    } else {
        // There are both a main block and a tail block.
        tileLength = MAX_AVAILABLE_UB_BLOCK_NUM * alignNum;
        lastTileLength = blockLength - tileNum * tileLength;
    }
    ```

## Operator Class Implementation<a name="zh-cn_topic_0000002236197681_section09641704120"></a>

When DoubleBuffer is not enabled, you only need to process the start address of the last block on each core. When DoubleBuffer is enabled, the length of the data block to be processed becomes half of the original, so you need to process the start addresses of the last two data blocks.

To enable DoubleBuffer, refer to [InitBuffer API Function Prototype](../../../../api/SIMD-API/basic_api/resource_management/TPipe/InitBuffer.md) and set the num parameter to 2, that is, BUFFER\_NUM.

```
this->initBufferLength = AscendC::Std::max(this->tileLength, this->lastTileLength);
pipe.InitBuffer(inQueueX, BUFFER_NUM, this->initBufferLength * sizeof(dataType));
pipe.InitBuffer(inQueueY, BUFFER_NUM, this->initBufferLength * sizeof(dataType));
pipe.InitBuffer(outQueueZ, BUFFER_NUM, this->initBufferLength * sizeof(dataType));
```

At the same time, when calculating the length of each data block in the core, consider the DoubleBuffer scenario and bring the number of buffers, that is, BUFFER\_NUM=2, into the calculation.

```
this->tileLength = tiling.tileLength / BUFFER_NUM;
```

Since it cannot be guaranteed that the tail block meets the DoubleBuffer condition, the tail block is not split.

```
this->lastTileLength = tiling.lastTileLength;
```

The implementation code of the Init function is as follows:

```
__aicore__ inline void Init(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, AddCustomTilingData tiling)
{
    if (tiling.isEvenCore) {
        this->blockLength = tiling.blockLength;
        this->tileNum = tiling.tileNum;
        this->tileLength = tiling.tileLength / BUFFER_NUM;
        this->lastTileLength = tiling.lastTileLength;

        xGm.SetGlobalBuffer((__gm__ dataType *)x + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
        yGm.SetGlobalBuffer((__gm__ dataType *)y + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
        zGm.SetGlobalBuffer((__gm__ dataType *)z + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
    } else {
        if (AscendC::GetBlockIdx() < tiling.formerNum) {
            this->tileNum = tiling.formerTileNum;
            this->tileLength = tiling.formerTileLength / BUFFER_NUM;
            this->lastTileLength = tiling.formerLastTileLength;

            xGm.SetGlobalBuffer((__gm__ dataType *)x + tiling.formerLength * AscendC::GetBlockIdx(), tiling.formerLength);
            yGm.SetGlobalBuffer((__gm__ dataType *)y + tiling.formerLength * AscendC::GetBlockIdx(), tiling.formerLength);
            zGm.SetGlobalBuffer((__gm__ dataType *)z + tiling.formerLength * AscendC::GetBlockIdx(), tiling.formerLength);
        } else {
            this->tileNum = tiling.tailTileNum;
            this->tileLength = tiling.tailTileLength / BUFFER_NUM;
            this->lastTileLength = tiling.tailLastTileLength;

            xGm.SetGlobalBuffer((__gm__ dataType *)x + tiling.formerLength * tiling.formerNum +
                tiling.tailLength * (AscendC::GetBlockIdx() - tiling.formerNum), tiling.tailLength);
            yGm.SetGlobalBuffer((__gm__ dataType *)y + tiling.formerLength * tiling.formerNum +
                tiling.tailLength * (AscendC::GetBlockIdx() - tiling.formerNum), tiling.tailLength);
            zGm.SetGlobalBuffer((__gm__ dataType *)z + tiling.formerLength * tiling.formerNum +
                tiling.tailLength * (AscendC::GetBlockIdx() - tiling.formerNum), tiling.tailLength);
        }
    }

    uint32_t initBufferLength = AscendC::Std::max(this->tileLength, this->lastTileLength);
    pipe.InitBuffer(inQueueX, BUFFER_NUM, initBufferLength * sizeof(dataType));
    pipe.InitBuffer(inQueueY, BUFFER_NUM, initBufferLength * sizeof(dataType));
    pipe.InitBuffer(outQueueZ, BUFFER_NUM, initBufferLength * sizeof(dataType));
}
```

After DoubleBuffer is enabled, the number of main blocks after splitting doubles. In the Process function, BUFFER\_NUM needs to be brought into the calculation of the loop count. The tail block is calculated independently without enabling DoubleBuffer. The subsequent processing of the main block and tail block in the CopyIn, Compute, and CopyOut stages is the same as that in [Tail Block Even Split](multi_core_tiling/tail_block_even_split.md).

```
__aicore__ inline void Process()
{
    // The main block is calculated with DoubleBuffer, so loopCount must be multiplied by 2.
    uint32_t loopCount = this->tileNum * BUFFER_NUM;
    for (uint32_t i = 0; i < loopCount; i++) {
        // CopyIn: DataCopy(xLocal, xGm[i * this->tileLength], this->tileLength);
        //         DataCopy(yLocal, yGm[i * this->tileLength], this->tileLength);
        // Compute: Add(zLocal, xLocal, yLocal, this->tileLength);;
        // CopyOut: DataCopy(zGm[i * this->tileLength], zLocal, this->tileLength);;
    }
    // Compute the tail block without DoubleBuffer
    if (this->lastTileLength > 0U) {
        // CopyIn: DataCopy(xLocal, xGm[loopCount], this->lastTileLength);
        //         DataCopy(yLocal, yGm[loopCount], this->lastTileLength);
        // Compute: Add(zLocal, xLocal, yLocal, this->lastTileLength);;
        // CopyOut: DataCopy(zGm[loopCount], zLocal, this->lastTileLength);;
    }
}
```
