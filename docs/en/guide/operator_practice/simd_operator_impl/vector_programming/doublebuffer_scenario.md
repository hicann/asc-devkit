# DoubleBuffer Scenarios<a name="ZH-CN_TOPIC_0000002532228161"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-26T14:03:24.514Z -->

Because operators often move data in and out multiple times, the [DoubleBuffer](../../../technical_appendix/concepts_and_terms/performance_optimization/DoubleBuffer.md) mechanism is introduced to make full use of hardware resources and implement multi-pipeline parallelism. [DoubleBuffer](../../../technical_appendix/concepts_and_terms/performance_optimization/DoubleBuffer.md) divides the input data into two blocks of equal size, making full use of the hardware resources of the AI Core to implement parallel execution of data moving in, computation, and data moving out. The following uses the "uneven split across cores and uneven split within a core" sample to describe how DoubleBuffer is implemented in an operator.

**Figure 1**  DoubleBuffer data tiling schematic diagram<a name="zh-cn_topic_0000002236197681_fig68713182104"></a>  
![](../../../figures/doublebuffer_data_tiling_diagram.png "DoubleBuffer data tiling schematic diagram")

## Tiling Implementation<a name="zh-cn_topic_0000002236197681_section1967484164119"></a>

After DoubleBuffer is enabled, each data block is divided into two blocks of equal size. Therefore, to enable DoubleBuffer, the total data length must be evenly divisible. To simplify processing, the available Unified Buffer space is divided into n dataBlocks at a granularity of 32 bytes. If n is not an even number, it is decremented by 1. In this way, a single set of code is compatible with both enabled and disabled DoubleBuffer. The corresponding steps are as follows:

1.  Check whether the total data length totalLength is 32-byte aligned. If not, calculate totalLengthAligned, which is totalLength rounded up to 32-byte alignment.

    ```
    constexpr uint32_t BLOCK_SIZE = 32;
    // For ease of calculation, define the variable alignNum as the alignment number based on the data type.
    uint32_t alignNum = BLOCK_SIZE / dataTypeSize;
    // totalLength is the total data length.
    uint32_t totalLengthAligned = (totalLength % alignNum == 0)?
            totalLength : ((totalLength + alignNum - 1) / alignNum) * alignNum;
    ```

2.  Based on totalLengthAligned, calculate the computation data length blockLength of each core. For the core splitting policy, see [Tail Core Tiling](multi_core_tiling/tail_core_tiling.md).
3.  Calculate the remaining Tiling parameters.

    Split the current available Unified Buffer space at a granularity of 32 bytes to calculate the number of data blocks UB\_BLOCK\_NUM. Based on whether DoubleBuffer is enabled, calculate the maximum number of currently available data blocks, denoted as MAX\_AVAILABLE\_UB\_BLOCK\_NUM. Finally, split blockLength at the granularity of MAX\_AVAILABLE\_UB\_BLOCK\_NUM. For ease of demonstration, the following code directly provides UB\_BLOCK\_NUM as the number of blocks (32 bytes) contained in the current available Unified Buffer space.

    ```
    constexpr uint32_t BUFFER_NUM = 2;
    constexpr uint32_t UB_BLOCK_NUM = 21;  // Maximum number of blocks that can be used by the UB.
    constexpr uint32_t MAX_AVAILABLE_UB_BLOCK_NUM = UB_BLOCK_NUM / BUFFER_NUM * BUFFER_NUM;
    
    tileNum = blockLength / (alignNum * MAX_AVAILABLE_UB_BLOCK_NUM);
    if (tileNum == 0) {
        // The length to be computed by a single core is smaller than the available UB space. Process it as containing only the tail block.
        tileLength = 0;
        lastTileLength = (blockLength + alignNum - 1) / alignNum * alignNum;
    } else if ((blockLength / alignNum) % MAX_AVAILABLE_UB_BLOCK_NUM == 0) {
        // The computation workload of a single core can be evenly divided by the currently available UB space, with only a main block and no tail block
        tileLength = MAX_AVAILABLE_UB_BLOCK_NUM * alignNum;
        lastTileLength = 0;
    } else {
        // Both a main block and a tail block exist
        tileLength = MAX_AVAILABLE_UB_BLOCK_NUM * alignNum;
        lastTileLength = blockLength - tileNum * tileLength;
    }
    ```

## Operator Class Implementation<a name="zh-cn_topic_0000002236197681_section09641704120"></a>

When DoubleBuffer is disabled, only the start address of the last block on each core needs to be processed. When DoubleBuffer is enabled, the length of the data block to be processed is halved, so the start addresses of the last two data blocks need to be processed.

To enable DoubleBuffer, refer to [InitBuffer API Prototype](../../../../api/SIMD-API/basic_api/resource-management/TPipe/InitBuffer.md) and set the num parameter to 2, that is, BUFFER\_NUM.

```
this->initBufferLength = AscendC::Std::max(this->tileLength, this->lastTileLength);
pipe.InitBuffer(inQueueX, BUFFER_NUM, this->initBufferLength * sizeof(dataType));
pipe.InitBuffer(inQueueY, BUFFER_NUM, this->initBufferLength * sizeof(dataType));
pipe.InitBuffer(outQueueZ, BUFFER_NUM, this->initBufferLength * sizeof(dataType));
```

Meanwhile, when calculating the length of each data block in a core, take the DoubleBuffer scenario into account and substitute the number of buffers, that is, BUFFER\_NUM=2, into the calculation.

```
this->tileLength = tiling.tileLength / BUFFER_NUM;
```

Because it cannot be guaranteed that the tail block meets the DoubleBuffer condition, the tail block is not tiled.

```
this->lastTileLength = tiling.lastTileLength;
```

The implementation code of the Init function is as follows:

```
__aicore__ inline void Init(GM_ADDR x, GM_ADDR y, GM_ADDR z, AddCustomTilingData tiling)
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

After DoubleBuffer is enabled, the number of main blocks after data tiling doubles. Therefore, in the Process function, BUFFER\_NUM must be substituted into the calculation of the loop count. The tail block is computed independently with DoubleBuffer disabled. The subsequent processing of the main block and tail block in the CopyIn, Compute, and CopyOut phases is the same as that in [Tail Block Tiling](multi_core_tiling/tail_block_tiling.md).

```
__aicore__ inline void Process()
{
    // The main block performs DoubleBuffer computation, so loopCount must be multiplied by 2.
    uint32_t loopCount = this->tileNum * BUFFER_NUM;
    for (uint32_t i = 0; i < loopCount; i++) {
        // CopyIn: DataCopy(xLocal, xGm[i * this->tileLength], this->tileLength);
        //         DataCopy(yLocal, yGm[i * this->tileLength], this->tileLength);
        // Compute: Add(zLocal, xLocal, yLocal, this->tileLength);;
        // CopyOut: DataCopy(zGm[i * this->tileLength], zLocal, this->tileLength);;
    }
    // The tail block is computed without DoubleBuffer.
    if (this->lastTileLength > 0U) {
        // CopyIn: DataCopy(xLocal, xGm[loopCount], this->lastTileLength);
        //         DataCopy(yLocal, yGm[loopCount], this->lastTileLength);
        // Compute: Add(zLocal, xLocal, yLocal, this->lastTileLength);;
        // CopyOut: DataCopy(zGm[loopCount], zLocal, this->lastTileLength);;
    }
}
```
