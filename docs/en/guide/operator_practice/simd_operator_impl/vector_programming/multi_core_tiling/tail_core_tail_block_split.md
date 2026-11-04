# Tail Core Tail Block Split

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:31:11.258Z pushedAt=2026-11-03T07:05:13.321Z nl=2 -->

In this scenario, after the main blocks are evenly divided, there is remaining data `mainTileRemainder`. The remaining data can be evenly distributed to each core with DataBlock as the minimum unit, but a small number of DataBlocks still cannot be evenly divided (`remainingTailLength` is not 0) and can only be further allocated to some cores. The cores that obtain the extra DataBlock are whole cores, and the rest are tail cores. Both whole cores and tail cores contain tail blocks, and whole cores process more data. Since `tailBlockNumEachCore` is not 0, each core is allocated a basic tail block, so tail blocks also exist in tail cores. A whole core processes one more DataBlock than a tail core. After data tiling, the first `formerNum` cores are whole cores, each processing `formerLength` elements (including the main block and the longer tail block); the remaining cores are tail cores, each processing `tailLength` elements (including the main block and the shorter tail block).

For the complete code of this scenario, see [Scenario 3: Tail Block Tail Core Split](../../../../../../../examples/01_simd_cpp_api/02_features/02_tiling_selector/vector_tiling_strategy/README.md#scenario3-tail-block-tail-core). In the sample, 8 cores are used, the main block length `mainTileLength` is 3200 `half` elements, and the total data length is 258112 `half` elements. After the data is aligned by DataBlock, the first 4 cores each process 32272 elements, and the last 4 cores each process 32256 elements. A whole core contains 10 main blocks and a tail block of 272 elements, and a tail core contains 10 main blocks and a tail block of 256 elements. The data tiling is shown in the following figure.

**Figure 1** Data tiling schematic diagram

![Data tiling schematic diagram](../../../../figures/tailblock_tailcore.png "Data tiling schematic diagram")

In this scenario, the element counts, the numbers of main blocks, and the tail block lengths processed by whole cores and tail cores are all different, so the Tiling parameters of the two types of cores need to be recorded separately. Compared with [tail core splitting](tail_core_split.md), since tail blocks also exist in tail cores, the Tiling parameters add `tailLastTileLength` on top of `former*` and `tail*` to record the number of tail block elements of the tail core. The Tiling structure contains the following members:

- formerNum: number of cores allocated more data, that is, the number of whole cores.
- formerLength: element count processed by a whole core.
- formerTileNum: number of main blocks in a whole core.
- formerLastTileLength: element count of the tail block in a whole core.
- tailLength: element count computed by the tail core.
- tailTileNum: number of main blocks in the tail core.
- tailLastTileLength: element count of the tail block in the tail core.

## Tiling Implementation

The Tiling structure is defined as follows:

```cpp
struct TailBlockAndTailCoreTiling {
    uint32_t formerNum;            // Number of whole cores
    uint32_t formerLength;         // Element count processed by each whole core
    uint32_t formerTileNum;        // Number of main blocks in a whole core
    uint32_t formerLastTileLength; // Element count of the tail block in a whole core
    uint32_t tailLength;           // Element count processed by the tail core
    uint32_t tailTileNum;          // Number of main blocks in the tail core
    uint32_t tailLastTileLength;   // Element count of the tail block in the tail core
};
```

The host-side Tiling follows the DataBlock alignment, main block allocation, base tail block calculation, and whole core/tail core division in [tail core splitting](tail_core_split.md), yielding the number of whole cores `formerNum`, the element count processed by each whole core `formerLength`, the element count processed by each tail core `tailLength`, the element count of the base tail block of each core `baseLastTileLength`, the number of main blocks in a whole core `formerTileNum`, and the number of main blocks in the tail core `tailTileNum`. When both `remainingTailLength` and `tailBlockNumEachCore` are not 0, the tail core splitting branch does not hold, and the tail core tail block splitting scenario is entered. In this case, a whole core processes one more DataBlock than a tail core, and the whole core tail block length `formerLastTileLength` is the sum of the base tail block length `baseLastTileLength` and `alignNum` (the number of `half` elements contained in one DataBlock). If the sum equals `mainTileLength`, it is merged into one main block (`formerTileNum` is incremented by 1 and `formerLastTileLength` is set to 0). The tail core tail block length `tailLastTileLength` directly takes `baseLastTileLength`.

```cpp
uint32_t formerLastTileLength = baseLastTileLength + alignNum;
if (formerLastTileLength == mainTileLength) {
    formerTileNum += 1U;
    formerLastTileLength = 0U;
}
uint32_t tailLastTileLength = baseLastTileLength;
TailBlockAndTailCoreTiling tiling{
    formerNum,  formerLength, formerTileNum, formerLastTileLength,
    tailLength, tailTileNum,  tailLastTileLength};
add_custom<TailBlockAndTailCoreTiling><<<numBlocks, 0, stream>>>(xDevice, yDevice, zDevice, tiling);
```

After the input data with the shape `(1, 258112)` is calculated, the values of the variables in the Tiling structure are as follows:

```cpp
TailBlockAndTailCoreTiling tiling{
    4,     // formerNum: the first 4 cores are whole cores
    32272, // formerLength: a whole core calculates 32272 half
    10,    // formerTileNum: a whole core contains 10 main blocks
    272,   // formerLastTileLength: a whole core contains a tail block of 272 half
    32256, // tailLength: a tail core calculates 32256 half
    10,    // tailTileNum: a tail core contains 10 main blocks
    256    // tailLastTileLength: a tail core contains a tail block of 256 half
};
```

## Operator Class Implementation

The operator on the kernel function side is still implemented using [static tensor programming](../../../../programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md). Compared with [tail core splitting](tail_core_split.md), in this scenario both the whole core and the tail core contain a tail block. Therefore, the `Init` function needs to determine whether the current core is a whole core or a tail core based on `GetBlockIdx()`, and set the Global Memory offset, `tileNum`, and `lastTileLength` accordingly.

The whole core uses `formerLength`, `formerTileNum`, and `formerLastTileLength`, which are calculated as follows:

```cpp
uint32_t offset = tiling.formerLength * blockIdx;
InitGm(x, y, z, offset, tiling.formerLength);
```

The tail core uses `tailLength`, `tailTileNum`, and `tailLastTileLength`. The Global Memory offset of the tail core needs to be calculated after all whole core data, as shown in the following code:

```cpp
uint32_t offset = tiling.formerLength * tiling.formerNum + tiling.tailLength * (blockIdx - tiling.formerNum);
InitGm(x, y, z, offset, tiling.tailLength);
```

The complete implementation of the `Init` function is as follows:

```cpp
__aicore__ inline void Init(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z,
                            TailBlockAndTailCoreTiling tiling)
{
    uint32_t blockIdx = AscendC::GetBlockIdx();
    if (blockIdx < tiling.formerNum) {
        uint32_t offset = tiling.formerLength * blockIdx;
        InitGm(x, y, z, offset, tiling.formerLength);
        this->tileNum = tiling.formerTileNum;
        this->lastTileLength = tiling.formerLastTileLength;
    } else {
        uint32_t offset =
            tiling.formerLength * tiling.formerNum + tiling.tailLength * (blockIdx - tiling.formerNum);
        InitGm(x, y, z, offset, tiling.tailLength);
        this->tileNum = tiling.tailTileNum;
        this->lastTileLength = tiling.tailLastTileLength;
    }
}
```

After `Init` is completed, the `Process` function still loops based on `tileNum` and `lastTileLength`. Both the whole core and the tail core need to process the main block and the tail block. The loop logic is the same as the `Process` function in [tail block even split](tail_block_even_split.md), so it is not elaborated here.
