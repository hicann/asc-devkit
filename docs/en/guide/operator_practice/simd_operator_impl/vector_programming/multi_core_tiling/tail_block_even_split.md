# Tail Block Even Split

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:29:39.941Z pushedAt=2026-11-03T07:05:13.319Z nl=2 -->

In this scenario, after the main block allocation is complete, there is still remaining data, but the remaining data cannot continue to be evenly distributed to each core by main block. To make balanced use of the multi-core computing capability, the remaining data `mainTileRemainder` after the main block even split is further evenly distributed to each core with DataBlock as the minimum unit. Each core is allocated `tailBlockNumEachCore` DataBlocks of data, and no remainder is left after the even split by DataBlock. As a result, each core processes the same amount of data, and in addition to the main block, each core also contains an equal-length tail block.

For the complete code of the sample in this scenario, see [Scenario 1: Tail Block Even Split](../../../../../../../examples/01_simd_cpp_api/02_features/02_tiling_selector/vector_tiling_strategy/README.md#scenario1-main-tile-tail-block). In the sample, 8 cores are used, the main block length `mainTileLength` is 3200 `half` elements, and the total data length is 260096 `half` elements. The data is evenly distributed to 8 cores for processing, with each core computing 32512 elements. Each core contains 10 main blocks and a tail block of 512 elements. The data tiling schematic diagram is shown in the following figure.

**Figure 1** Data tiling schematic diagram

![Data tiling schematic diagram](../../../../figures/tailblock_tiling.png "Data tiling schematic diagram")

Compared with the main block even split, in this scenario each core contains a tail block in addition to the main block. Therefore, in addition to `blockLength` and `tileNum`, the Tiling parameter adds `lastTileLength` to record the number of elements in the tail block.

## Tiling Implementation

The Tiling structure of the operator is defined as follows:

```cpp
struct MainTileWithTailBlockTiling {
    uint32_t blockLength;    // Number of elements processed by each core
    uint32_t tileNum;        // Number of main blocks
    uint32_t lastTileLength; // Number of elements in the tail block
};
```

The Host-side tiling follows the DataBlock alignment and main block allocation calculation in [main block even split](main_block_even_split.md), which yields the number of remaining elements after the main block even split `mainTileRemainder`, the number of base elements already allocated to each core `baseLength`, and the corresponding number of main blocks `baseTileNum`. When `mainTileRemainder` is not 0, the main block even split branch does not hold, and the following steps are performed:

1. Using DataBlock as the minimum unit, evenly distribute the remaining data `mainTileRemainder` after the main block even split to each core. A DataBlock contains `alignNum` `half` elements, so `alignNum` is used as the unit of even split in the code. Divide `mainTileRemainder` by `numBlocks * alignNum` to obtain the number of DataBlocks allocated to each core `tailBlockNumEachCore`, and take the remainder to obtain the number of remaining elements `remainingTailLength` that still cannot be evenly distributed to each core after the DataBlock-based even split. Then multiply `tailBlockNumEachCore` by `alignNum` to obtain the number of elements in the base tail block of each core `baseLastTileLength`, and add it to `baseLength` to obtain the number of base elements of each core.

    ```cpp
    uint32_t tailBlockNumEachCore = mainTileRemainder / (numBlocks * alignNum);
    uint32_t remainingTailLength = mainTileRemainder % (numBlocks * alignNum);
    uint32_t baseLastTileLength = tailBlockNumEachCore * alignNum;
    baseLength += baseLastTileLength;
    ```

2. Determine whether `remainingTailLength` is 0. A value of 0 indicates that `mainTileRemainder` can be evenly distributed to each core using DataBlock as the minimum unit. In this case, the tail block even split scenario is entered, and the `MainTileWithTailBlockTiling` structure is used to pass the tiling information.

    ```cpp
    if (remainingTailLength == 0U) {
        MainTileWithTailBlockTiling tiling{baseLength, baseTileNum, baseLastTileLength};
        add_custom<MainTileWithTailBlockTiling><<<numBlocks, 0, stream>>>(xDevice, yDevice, zDevice, tiling);
        ...
    }
    ```

After calculation for the input data with the shape `(1, 260096)`, the values of the variables in the Tiling structure are as follows:

```cpp
MainTileWithTailBlockTiling tiling{
    32512, // blockLength: each core computes 32512 half elements, and 8 cores compute 260096 half elements in total
    10,    // tileNum: each core contains 10 main blocks
    512    // lastTileLength: each core contains a tail block of 512 half elements
};
```

## Operator Class Implementation

The kernel function operator is still implemented using [static tensor programming](../../../../programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md). Compared with [main block even split](main_block_even_split.md), in this scenario the data length processed by each core remains the same, so the Global Memory offset is still calculated from `tiling.blockLength` and `GetBlockIdx()`. The difference is that the `Init` function needs to additionally save `tiling.lastTileLength` for the `Process` function to determine whether the last loop iteration processes the tail block.

```cpp
__aicore__ inline void Init(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z,
                            MainTileWithTailBlockTiling tiling)
{
    uint32_t offset = AscendC::GetBlockIdx() * tiling.blockLength;
    InitGm(x, y, z, offset, tiling.blockLength);
    this->tileNum = tiling.tileNum;
    this->lastTileLength = tiling.lastTileLength;
}
```

Because the tail block length `lastTileLength` differs from the main block length `mainTileLength`, the `Process` function needs to determine the data length to be processed in the current iteration based on the current loop position. `totalLoop` is the number of loop iterations that the current core needs to process: the first `tileNum` iterations process the main blocks, and the last iteration processes the tail block. `curLen` is the number of elements processed in the current iteration, and `startElement` is the element offset within the core for the current iteration.

```cpp
__aicore__ inline void Process()
{
    AscendC::LocalTensor<half> xLocal(AscendC::TPosition::VECCALC, xAddr, mainTileLength);
    AscendC::LocalTensor<half> yLocal(AscendC::TPosition::VECCALC, yAddr, mainTileLength);
    AscendC::LocalTensor<half> zLocal(AscendC::TPosition::VECCALC, zAddr, mainTileLength);

    uint32_t totalLoop = this->tileNum + (this->lastTileLength > 0U ? 1U : 0U);
    for (uint32_t loopIdx = 0; loopIdx < totalLoop; loopIdx++) {
        uint32_t curLen = (loopIdx < this->tileNum) ? mainTileLength : this->lastTileLength;
        uint32_t startElement = loopIdx * mainTileLength;
        // ... Synchronization management code omitted. For details, see the complete sample code ...
        AscendC::DataCopy(xLocal, xGm[startElement], curLen);
        AscendC::DataCopy(yLocal, yGm[startElement], curLen);
        // ... Synchronization management code omitted ...
        AscendC::Add(zLocal, xLocal, yLocal, curLen);
        // ... Synchronization management code omitted ...
        AscendC::DataCopy(zGm[startElement], zLocal, curLen);
        // ... Synchronization management code omitted ...
    }
}
```
