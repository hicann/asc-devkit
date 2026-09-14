# Tail Core & Tail Block<a name="ZH-CN_TOPIC_0000002500468234"></a>

<!-- md-trans-meta sourceCommit=51eb188d02e4be270d9dc56141153e3f75345dc0 translatedAt=2026-08-26T14:07:18.899Z -->

When splitting data for inputs of different shapes, the data may not be evenly distributed across multiple cores, and the data within each core may not be evenly split either. Refer to the handling methods mentioned in [Tail Block Handling in the Scenario of Even Distribution Across Cores](tail_block_tiling.md) and [Tail Core Handling in the Scenario of Uneven Distribution Across Cores](tail_core_tiling.md), and combine the two to consider the handling of the tail block of a full core and the tail block of a tail core.

## Tiling Implementation<a name="zh-cn_topic_0000002236277533_section132981926144714"></a>

Since the data cannot be evenly distributed across cores or within each core in this scenario, add two member variables to the Tiling structure defined in [Tail Core Processing in the Scenario of Uneven Distribution Across Cores](tail_core_tiling.md):

-   formerLastTileLength: size of the last tile of the core with more data, that is, the tail block size of the full core.

    During calculation, first split the core with more data according to the core splitting strategy mentioned in [Tail Core Tiling](tail_core_tiling.md).

    ```
    // datablock to which shape needs to be aligned
    uint32_t totalLengthAligned = (totalLength % alignNum == 0U) ?
                          static_cast<uint32_t>(totalLength) :
                          ((static_cast<uint32_t>(totalLength) + alignNum - 1) / alignNum) * alignNum;
    // Calculate the number of full cores
    uint32_t formerNum = (totalLengthAligned / alignNum) % numBlocks;
    // Calculate the data volume of full cores
    uint32_t formerLength = static_cast<uint32_t>(((totalLengthAligned + numBlocks - 1) / numBlocks + alignNum - 1) / alignNum) * alignNum;
    ```

    Then calculate the tail block length according to the splitting strategy in [Tail Block Tiling](tail_block_tiling.md).

    ```
    TilingParamsCalc(formerLength, alignNum, formerTileNum, formerTileLength, formerLastTileLength);
    
    void TilingParamsCalc(uint32_t length, uint32_t alignNum, uint32_t& tileNum, uint32_t& tileLength,
                          uint32_t& lastTileLength)
    {
        tileNum = length / (alignNum * UB_BLOCK_NUM);
        if (tileNum == 0U) {
            tileLength = 0U;
            lastTileLength = static_cast<uint32_t>(((length + alignNum - 1) / alignNum) * alignNum);
        } else if (static_cast<uint32_t>(length / alignNum) % UB_BLOCK_NUM == 0U) {
            tileLength = UB_BLOCK_NUM * alignNum;
            lastTileLength = 0U;
        } else {
            tileLength = UB_BLOCK_NUM * alignNum;
            lastTileLength = static_cast<uint32_t>(length - tileNum * tileLength);
        }
    }
    ```

-   tailLastTileLength: size of the last tile of the core with less data, that is, the tail block size of the tail core.

    During calculation, first split the core with less data according to the core splitting strategy mentioned in [Tail Core Tiling](tail_core_tiling.md).

    ```
    // Calculate the number of tail cores.
    uint32_t tailNum = numBlocks - formerNum;
    // Calculate the data volume of the tail core.
    uint32_t tailLength = (totalLengthAligned / numBlocks / alignNum) * alignNum;
    ```

    Then calculate the tail block length based on the splitting strategy in [tail block Tiling](tail_block_tiling.md).

    ```
    TilingParamsCalc(tailLength, alignNum, tailTileNum, tailTileLength, tailLastTileLength);
    
    void TilingParamsCalc(uint32_t length, uint32_t alignNum, uint32_t& tileNum, uint32_t& tileLength,
                          uint32_t& lastTileLength)
    {
        tileNum = length / (alignNum * UB_BLOCK_NUM);
        if (tileNum == 0U) {
            tileLength = 0U;
            lastTileLength = static_cast<uint32_t>(((length + alignNum - 1) / alignNum) * alignNum);
        } else if (static_cast<uint32_t>(length / alignNum) % UB_BLOCK_NUM == 0U) {
            tileLength = UB_BLOCK_NUM * alignNum;
            lastTileLength = 0U;
        } else {
            tileLength = UB_BLOCK_NUM * alignNum;
            lastTileLength = static_cast<uint32_t>(length - tileNum * tileLength);
        }
    }
    ```

## Operator Class Implementation<a name="zh-cn_topic_0000002236277533_section182075114713"></a>

The implementation of the Init and Process functions on the kernel side needs to combine the [tail block handling in the scenario of even distribution across cores](tail_block_tiling.md) with the [tail core handling in the scenario of uneven distribution across cores](tail_core_tiling.md).

In the Init function, since the tileLength and lastTileLength of the full core and the tail core are different, the full core and the tail core need to be handled separately as mentioned in [tail core handling in the scenario of uneven distribution across cores](tail_core_tiling.md). The subsequent handling of the CopyIn, Compute, and CopyOut functions for the main block and the tail block is the same as that in the [scenario of even distribution across cores](tail_block_tiling.md).

The implementation code of the Init function is as follows:

```
__aicore__ inline void Init(GM_ADDR x, GM_ADDR y, GM_ADDR z, AddCustomTilingData tiling, AscendC::TPipe* pipeIn)
{
    pipe = pipeIn;
    if (AscendC::GetBlockIdx() < tiling.formerNum) {
        this->tileNum = tiling.formerTileNum;
        this->tileLength = tiling.formerTileLength;
        this->lastTileLength = tiling.formerLastTileLength;
        uint64_t offset = tiling.formerLength * AscendC::GetBlockIdx();
        xGm.SetGlobalBuffer((__gm__ half *)x + offset, tiling.formerLength);
        yGm.SetGlobalBuffer((__gm__ half *)y + offset, tiling.formerLength);
        zGm.SetGlobalBuffer((__gm__ half *)z + offset, tiling.formerLength);
    } else {
        this->tileNum = tiling.tailTileNum;
        this->tileLength = tiling.tailTileLength;
        this->lastTileLength = tiling.tailLastTileLength;
        uint64_t offset = tiling.formerLength * tiling.formerNum
                          + tiling.tailLength * (AscendC::GetBlockIdx() - tiling.formerNum);
        xGm.SetGlobalBuffer((__gm__ half *)x + offset, tiling.tailLength);
        yGm.SetGlobalBuffer((__gm__ half *)y + offset, tiling.tailLength);
        zGm.SetGlobalBuffer((__gm__ half *)z + offset, tiling.tailLength);
    }
    
    // In the scenario with only a tail block, tileLength is 0. Therefore, the maximum of tileLength and lastTileLength is used for initialization.
    uint32_t initBufferLength = AscendC::Std::max(this->tileLength, this->lastTileLength);
    pipe->InitBuffer(inQueueX, 1, this->initBufferLength * sizeof(half));
    pipe->InitBuffer(inQueueY, 1, this->initBufferLength * sizeof(half));
    pipe->InitBuffer(outQueueZ, 1, this->initBufferLength * sizeof(half));
}
```

