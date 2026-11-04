# Tail Core Split

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:30:07.543Z pushedAt=2026-11-03T07:05:13.320Z nl=2 -->

In actual operator development, constrained by the total amount of input data and the tiling unit, not all scenarios can distribute computation completely evenly to every core. In this scenario, after the main block is evenly divided, there is remaining data `mainTileRemainder`, but the remaining data cannot be evenly distributed to every core with DataBlock as the minimum unit (`tailBlockNumEachCore` is 0 and `remainingTailLength` is not 0), so it can only be assigned to some cores. The cores that obtain an extra DataBlock are whole cores, and the rest are tail cores. In this case, the load is unbalanced across cores, so whole cores and tail cores must be processed separately. Since `tailBlockNumEachCore` is 0, no core is assigned a base tail block; therefore, a tail core contains only the main block, and a whole core processes one more DataBlock tail block than a tail core. After data tiling, the first `formerNum` cores are whole cores, each processing `formerLength` elements (including the main block and the tail block); the remaining cores are tail cores, each processing `tailLength` elements (including only the main block).

For the complete code of this scenario sample, see [Scenario 2: Tail Core Split](../../../../../../../examples/01_simd_cpp_api/02_features/02_tiling_selector/vector_tiling_strategy/README.md#scenario2-main-tile-tail-core). In the sample, 8 cores are fixed, the main block length `mainTileLength` is 3200 `half` elements, and the total data length is 256064 `half` elements. After the data is aligned by DataBlock, the first 4 cores each process 32016 elements, and the last 4 cores each process 32000 elements. A whole core contains 10 main blocks and a tail block of 16 elements, while a tail core contains only 10 main blocks. The data tiling is shown in the following figure.

**Figure 1** Data tiling schematic diagram

![Data tiling schematic diagram](../../../../figures/tailcore_tiling.png "Data tiling schematic diagram")

In this scenario, the number of elements, the number of main blocks, and the tail block situation processed by whole cores and tail cores differ, so a single set of `blockLength`, `tileNum`, and `lastTileLength` cannot describe all cores. Therefore, the Tiling parameters add `formerNum` to record the number of whole cores, and use `former*` and `tail*` parameters to describe and record the Tiling information related to whole cores and tail cores respectively. Since a tail core contains only the main block, there is no need to define `tailLastTileLength`. The Tiling structure contains the following members:

- formerNum: number of cores assigned more data, that is, the number of whole cores.
- formerLength: number of elements processed by a whole core.
- formerTileNum: number of main blocks in a whole core.
- formerLastTileLength: number of elements in the tail block of a whole core.
- tailLength: quantity of elements computed by the tail core.
- tailTileNum: quantity of main blocks in the tail core.

## Tiling Implementation

The Tiling structure is defined as follows:

```cpp
struct MainTileWithTailCoreTiling {
    uint32_t formerNum;            // Number of whole cores
    uint32_t formerLength;         // Number of elements processed by a whole core
    uint32_t formerTileNum;        // Number of main blocks in a whole core
    uint32_t formerLastTileLength; // Number of elements in the tail block of a whole core
    uint32_t tailLength;           // Number of elements processed by a tail core
    uint32_t tailTileNum;          // Number of main blocks in a tail core
};
```

On the host side, Tiling follows the DataBlock alignment, main block allocation, and base tail block calculation in [Tail Block Equal Division](tail_block_even_split.md), obtaining the following: the number of remaining elements `remainingTailLength` that still cannot be evenly distributed to each core after DataBlock equal division, the number of DataBlocks allocated to each core `tailBlockNumEachCore`, the number of elements in the base tail block of each core `baseLastTileLength`, the number of base elements already allocated to each core `baseLength`, and the corresponding number of main blocks `baseTileNum`. When `remainingTailLength` is not 0, the tail block equal division branch does not hold, and the following steps are executed:

1. The remaining data corresponding to `remainingTailLength` cannot be evenly distributed to each core and can only be allocated to the first `formerNum` whole cores. Divide `remainingTailLength` by `alignNum` to obtain the number of whole cores `formerNum`. A whole core processes one more DataBlock (containing `alignNum` `half` elements) than a tail core; therefore, `formerLength` is obtained by adding `alignNum` to `baseLength`, while `tailLength` directly takes `baseLength`.

    ```cpp
    if (remainingTailLength == 0U) {
        ...
        return;
    }

    uint32_t formerNum = remainingTailLength / alignNum;
    uint32_t formerLength = baseLength + alignNum;
    uint32_t tailLength = baseLength;
    ```

2. The whole cores and tail cores have the same number of main blocks, both equal to `baseTileNum`.

    ```cpp
    uint32_t formerTileNum = baseTileNum;
    uint32_t tailTileNum = baseTileNum;
    ```

3. Further determine whether `tailBlockNumEachCore` is 0. A value of 0 indicates that no core is assigned a base tail block (`baseLastTileLength` is 0), so the tail core contains only main blocks. The whole core processes one more DataBlock than the tail core, so the whole core tail block length `formerLastTileLength` is directly set to `alignNum`. In this case, the tail-core split scenario is entered, and the `MainTileWithTailCoreTiling` structure is used to pass the tiling information.

    ```cpp
    if (tailBlockNumEachCore == 0U) {
        uint32_t formerLastTileLength = alignNum;
        MainTileWithTailCoreTiling tiling{
            formerNum, formerLength, formerTileNum, formerLastTileLength, tailLength, tailTileNum};
        add_custom<MainTileWithTailCoreTiling><<<numBlocks, 0, stream>>>(xDevice, yDevice, zDevice, tiling);
    }
    ```

After calculation on the input data with the shape `(1, 256064)`, the values of the variables in the Tiling structure are as follows:

```cpp
MainTileWithTailCoreTiling tiling{
    4,     // formerNum: the first 4 cores are whole cores
    32016, // formerLength: the whole core computes 32016 half elements
    10,    // formerTileNum: the whole core contains 10 main blocks
    16,    // formerLastTileLength: the whole core contains a tail block of 16 half elements
    32000, // tailLength: the tail core computes 32000 half elements
    10     // tailTileNum: the tail core contains 10 main blocks
};
```

## Operator Class Implementation

The kernel function side operator is still implemented using [static tensor programming](../../../../programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md). Compared with [tail block equal division](tail_block_even_split.md), in this scenario the whole core and the tail core process data of different lengths. Therefore, the `Init` function needs to determine whether the current core is a whole core or a tail core based on `GetBlockIdx()`, and set the Global Memory offset and loop parameters separately. `blockIdx` indicates the index of the current logical core.

The whole core uses `formerLength`, `formerTileNum`, and `formerLastTileLength`, which are calculated as follows:

```cpp
uint32_t offset = tiling.formerLength * blockIdx;
InitGm(x, y, z, offset, tiling.formerLength);
```

The tail core uses `tailLength` and `tailTileNum`, and the tail core contains only main blocks, so `lastTileLength` is set to 0. The Global Memory offset of the tail core needs to be calculated after all whole core data, as shown in the following code:

```cpp
uint32_t offset = tiling.formerLength * tiling.formerNum + tiling.tailLength * (blockIdx - tiling.formerNum);
InitGm(x, y, z, offset, tiling.tailLength);
```

The complete implementation of the `Init` function is as follows:

```cpp
__aicore__ inline void Init(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z,
                            MainTileWithTailCoreTiling tiling)
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
        this->lastTileLength = 0U;
    }
}
```

After `Init` is complete, the `Process` function still performs loop processing based on `tileNum` and `lastTileLength`. The whole core needs to process main blocks and tail blocks, while the tail core processes only main blocks. The loop logic is the same as the `Process` function in [tail block even split](tail_block_even_split.md), so it is not elaborated here.
