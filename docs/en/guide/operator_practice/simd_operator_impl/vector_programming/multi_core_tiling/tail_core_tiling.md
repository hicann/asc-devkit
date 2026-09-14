# Tail Core Tiling<a name="ZH-CN_TOPIC_0000002532388125"></a>

<!-- md-trans-meta sourceCommit=198cbfdf29f72f78c8de735d88a5244dc258f2e7 translatedAt=2026-08-26T14:08:36.795Z -->

When data is split for inputs of different shapes, the data may not be evenly distributed across multiple cores. For example, when the input shape of an operator is \[1, 1999\], the number of cores is 8, and the data type is half, the total amount of data to be computed is 1 \* 1999 \* sizeof\(half\) = 3998 bytes. The 3998 bytes neither satisfy 32-byte alignment nor can be evenly divided among the 8 cores. Therefore, in this scenario, after the data is split across multiple cores, the amount of data computed by each core differs. In this case, the data should be distributed as evenly as possible. The amount of data computed on all cores falls into two cases: cores with a larger workload are called full cores, and cores with a smaller workload are called tail cores.

**Figure 1** Data alignment schematic diagram<a name="zh-cn_topic_0000002201317270_fig5261155043319"></a>  
![](../../../../figures/data_alignment__diagram.png "Data alignment schematic diagram")

## Tiling Implementation<a name="zh-cn_topic_0000002201317270_section209650864618"></a>

-   Because the AI processor requires that both the length of data to be moved and the start address of the Unified Buffer be 32-byte aligned when performing data movement and Vector computation, **the data to be processed must first be aligned upward to a 32-byte size**. For details about the subsequent movement and computation in this scenario, see [Unaligned Scenario](../non-aligned_scenario.md). The following code snippet shows an example of aligning data to the datablock size:

    ```
    constexpr uint32_t SIZE_OF_HALF = 2;
    constexpr uint32_t BLOCK_SIZE = 32;
    constexpr uint32_t NUM_BLOCKS = 8;
    constexpr uint32_t ALIGN_NUM = BLOCK_SIZE / SIZE_OF_HALF;
    // The 32 bytes to which the shape needs to be aligned. Assume that the original totalLength is 1999, which becomes 2000 after upward 32-byte alignment.
    uint32_t totalLengthAligned = (totalLength % ALIGN_NUM == 0U) ?
                          static_cast<uint32_t>(totalLength) :
                          ((static_cast<uint32_t>(totalLength) + ALIGN_NUM - 1) / ALIGN_NUM) * ALIGN_NUM;
    ```

-   After 32-byte alignment, the data should be distributed to each core as evenly as possible. If the data cannot be evenly divided, first evenly distribute the divisible part, and then distribute the remaining part to some cores, so that some cores compute one more datablock. To ensure that the split data still satisfies 32-byte alignment, distribute the data to all cores at the granularity of ALIGN\_NUM (ALIGN\_NUM data items equal 32 bytes). In this sample, the data type is half, and ALIGN\_NUM = BLOCK\_SIZE  /  sizeof\(half\) = 16. Divide the total amount of aligned data into x data blocks at the granularity of ALIGN\_NUM, where x = 2000 / 16 = 125.

    The number of AI processor cores NUM\_BLOCKS is 8, so 125 data blocks cannot be evenly divided among 8 cores. Distribute the data blocks to each core as evenly as possible by following these steps:

    1.  Calculate x / NUM\_BLOCKS = 15.
    2.  Calculate x % NUM\_BLOCKS = 5.

    Based on the preceding steps, if 15 data blocks are allocated to each core, 5 data blocks remain. Allocate these 5 remaining data blocks to 5 cores, resulting in 5 full cores that compute 16 data blocks and 3 tail cores that compute 15 data blocks. The following figure shows an example of multi-core splitting when the data cannot be evenly divided.

    **Figure 2**  Example of data that cannot be evenly divided among cores<a name="zh-cn_topic_0000002201317270_fig68506184496"></a>  
    ![](../../../../figures/example_of_unable_to_evenly_distribute_among_cores.png "Example of data that cannot be evenly divided among cores")

Based on the preceding description, the following operator tiling structure members are designed:

-   formerNum: number of cores assigned more data, that is, the number of full cores.
-   tailNum: number of cores assigned less data, that is, the number of tail cores.
-   formerLength: data length computed by a full core.
-   tailLength: data length computed by a tail core.

The code for calculating the tiling parameters is as follows:

```
constexpr uint32_t NUM_BLOCKS = 8;
constexpr uint32_t SIZE_OF_HALF = 2;
constexpr uint32_t BLOCK_SIZE = 32;
// Minimum unit to which shape needs to be aligned.
constexpr uint32_t ALIGN_NUM = BLOCK_SIZE / SIZE_OF_HALF;
...
void GenerateTilingData(uint8_t* tilingBuf, uint32_t numBlocks)
{
    // datablock to which shape needs to be aligned. Assume that the original totalLength is 1999, which becomes 2000 after being rounded up to 32-byte alignment.
    uint32_t totalLengthAligned = (totalLength % ALIGN_NUM == 0U) ?
                      static_cast<uint32_t>(totalLength) :
                      ((static_cast<uint32_t>(totalLength) + ALIGN_NUM - 1) / ALIGN_NUM) * ALIGN_NUM;
    // The number of cores is 8, and one datablock contains 16 numbers. Then the total number of datablocks is 2000 / 16 = 125.
    // Five cores are assigned 16 datablocks: 125 % 8 = 5. These cores are called full cores.
    // Three cores are assigned 15 datablocks: 8 - 5 = 3. These cores are called tail cores.
    uint32_t formerNum = (totalLengthAligned / ALIGN_NUM) % numBlocks; 
    uint32_t tailNum = numBlocks - formerNum;
    // Data length computed by the former core: totalLengthAligned / NUM_BLOCKS is the number of elements computed on each core, and formerLength is the result of rounding up the number of elements to 32-byte alignment
    uint32_t formerLength =
            static_cast<uint32_t>(((totalLengthAligned + numBlocks - 1) / numBlocks + ALIGN_NUM - 1) / ALIGN_NUM) * ALIGN_NUM;
    // Data length computed by the tail core: totalLengthAligned / NUM_BLOCKS is the number of elements computed on each core, and tailLength is the result of rounding down the number of elements to 32-byte alignment
    uint32_t tailLength = (totalLengthAligned / numBlocks / ALIGN_NUM) * ALIGN_NUM;
    ...
}
```

## Operator Class Implementation<a name="zh-cn_topic_0000002201317270_section11477133219466"></a>

In the Init function on the kernel side, when calculating the memory offset address of the input in Global Memory, you need to distinguish between full cores and tail cores.

On a full core, the code for calculating the memory offset address of the input is as follows:

```
xGm.SetGlobalBuffer((__gm__ T*)x + formerLength * AscendC::GetBlockIdx(), formerLength);
```

On a tail core, when calculating the memory offset address of the input, you need to add the offset of the tail core to the data length of all full cores. The code is as follows:

```
xGm.SetGlobalBuffer((__gm__ T*)x + formerLength * formerNum + tailLength * (AscendC::GetBlockIdx() - formerNum), tailLength);
```

The complete implementation of the Init function is as follows:

```
__aicore__ inline void Init(GM_ADDR x, GM_ADDR y, GM_ADDR z, AddCustomTilingData tiling, AscendC::TPipe* pipeIn)
{
    pipe = pipeIn;
    if (AscendC::GetBlockIdx() < tiling.formerNum) {
        this->tileLength = tiling.formerLength;
        uint64_t offset = tiling.formerLength * AscendC::GetBlockIdx();
        xGm.SetGlobalBuffer((__gm__ half *)x + offset, tiling.formerLength);
        yGm.SetGlobalBuffer((__gm__ half *)y + offset, tiling.formerLength);
        zGm.SetGlobalBuffer((__gm__ half *)z + offset, tiling.formerLength);
    } else {
        this->tileLength = tiling.tailLength;
        uint64_t offset = tiling.formerLength * tiling.formerNum
                          + tiling.tailLength * (AscendC::GetBlockIdx() - tiling.formerNum);
        xGm.SetGlobalBuffer((__gm__ half *)x +offset, tiling.tailLength);
        yGm.SetGlobalBuffer((__gm__ half *)y + offset, tiling.tailLength);
        zGm.SetGlobalBuffer((__gm__ half *)z + offset, tiling.tailLength);
    }
    pipe->InitBuffer(inQueueX, 1, this->tileLength * sizeof(half));
    pipe->InitBuffer(inQueueY, 1, this->tileLength * sizeof(half));
    pipe->InitBuffer(outQueueZ, 1, this->tileLength * sizeof(half));
}
```

The rest of the implementation is the same as that in [Multi-core Tiling](multi_core_tiling.md), and is not described again here.
