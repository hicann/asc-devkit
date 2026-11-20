# Main Block Even Splitting

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-09T10:32:27.748Z pushedAt=2026-11-19T06:32:19.752Z nl=2 -->

Input data of length `totalLength` can be evenly split based on the number of cores involved in the computation and the main block length `mainTileLength`. After the main block even split, no residual data remains. Each core processes the same amount of data, with each core processing `tileNum` main blocks, and the core contains only main blocks.

**Figure 1** Data tiling schematic diagram

![Data tiling schematic diagram](../../../../figures/maintile_even.png "Data tiling schematic diagram")

For the complete code of the sample in this scenario, see [Scenario 0: Main Block Even Split](../../../../../../../examples/01_simd_cpp_api/02_features/02_tiling_selector/vector_tiling_strategy/README.md#scenario0-main-tile-only). The sample uses eight cores, with a main block length `mainTileLength` of 3200 `half` elements and a total data length of 256000 `half` elements. The data is evenly distributed across the eight cores for processing, with each core computing 32000 elements. Each core is split into 10 main blocks, with no tail block or tail core processing involved.

## Tiling Implementation

Based on the tiling strategy described in this section, tiling needs to define the following parameters:

- **blockLength**: length of data processed by each core, in elements.
- **tileNum**: number of main blocks that each core needs to compute.

Accordingly, the `MainTileOnlyTiling` structure is defined as follows:

```cpp
struct MainTileOnlyTiling {
    uint32_t blockLength; // number of elements processed by each core
    uint32_t tileNum;     // number of main blocks
};
```

Next, the tiling parameters are computed. The main variables involved include: the number of logical cores `numBlocks`, the number of bytes in one DataBlock `dataBlockSize`, the number of bytes in one `half` element `halfSize`, the number of `half` elements contained in one DataBlock `alignNum`, and the number of `half` elements contained in one main block `mainTileLength`. The steps are as follows:

1. Compute `alignNum` based on `dataBlockSize` and `halfSize`. In this sample, `alignNum` is 16. `totalLength` is the total number of input data elements. Align it upward by `alignNum` to obtain the aligned total element count `totalLengthAligned`.

    ```cpp
    constexpr uint32_t dataBlockSize = 32;
    constexpr uint32_t halfSize = 2;
    constexpr uint32_t alignNum = dataBlockSize / halfSize;

    uint32_t totalLengthAligned = AlignUp(totalLength, alignNum);
    ```

2. Divide `totalLengthAligned` by `numBlocks * mainTileLength` to obtain the number of main blocks evenly distributed to each core `mainTileNum`, and take the remainder to obtain the number of remaining elements after the main block even split `mainTileRemainder`. Then multiply `mainTileNum` by `mainTileLength` to obtain the number of elements already allocated to each core `baseLength`, and set `baseTileNum` directly to `mainTileNum`.

    ```cpp
    constexpr uint32_t numBlocks = 8;
    constexpr uint32_t mainTileLength = 3200;

    uint32_t mainTileNum = totalLengthAligned / (numBlocks * mainTileLength);
    uint32_t mainTileRemainder = totalLengthAligned % (numBlocks * mainTileLength);
    uint32_t baseLength = mainTileNum * mainTileLength;
    uint32_t baseTileNum = mainTileNum;
    ```

3. When `mainTileRemainder` is 0, it indicates that all data can be evenly distributed to each core by main blocks. In this case, the `MainTileOnlyTiling` structure is used to pass the tiling information.

    ```cpp
    if (mainTileRemainder == 0U) {
        MainTileOnlyTiling tiling{baseLength, baseTileNum};
        add_custom<MainTileOnlyTiling><<<numBlocks, 0, stream>>>(xDevice, yDevice, zDevice, tiling);
        return;
    }
    ```

## Operator Class Implementation

The kernel-side operator is implemented using [static tensor programming](../../../../programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md), where the Local Memory address is directly specified through the `LocalTensor` constructor. For memory management and synchronization management in static tensor programming, see the corresponding sections. This section focuses on the compute of the GM address for each core and the loop processing flow of the main block data.

- The kernel entry receives the tiling structure passed from the host side, and calls the `Init` and `Process` functions of the operator class in sequence to complete initialization and computation. In the main block even split scenario, the structure type passed in is `MainTileOnlyTiling`.

    ```cpp
    template <typename TilingData>
    __vector__ __global__ void add_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, TilingData tiling)
    {
        AscendC::InitSocState();
        KernelAdd op;
        op.Init(x, y, z, tiling);
        op.Process();
        AscendC::PipeBarrier<PIPE_ALL>();
    }
    ```

- Because the data is distributed across multiple cores for processing, the Global Memory address range accessed by each core differs. The `Init` function obtains the element count processed by each core through `tiling.blockLength`, and computes the starting `offset` of the data processed by the current core in Global Memory in combination with [GetBlockIdx](../../../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md). It then passes the offset and length to `InitGm` to complete the GM address setting. At the same time, it saves `tiling.tileNum` to a member variable for use by the `Process` function.

    ```cpp
    __aicore__ inline void Init(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z,
                                MainTileOnlyTiling tiling)
    {
        uint32_t offset = AscendC::GetBlockIdx() * tiling.blockLength;
        InitGm(x, y, z, offset, tiling.blockLength);
        this->tileNum = tiling.tileNum;
        this->lastTileLength = 0U;
    }
    ```

- The `InitGm` function sets the Global Memory addresses for the input `xGm`, `yGm`, and output `zGm` through [SetGlobalBuffer](../../../../../api/SIMD-API/basic_api/data_structures/GlobalTensor/SetGlobalBuffer.md). Taking the input x as an example, `(__gm__ half*)x + offset` is the starting address of the data processed by the current core in Global Memory, and `length` is the element count processed by this core.

    ```cpp
    __aicore__ inline void InitGm(
        __gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, uint32_t offset, uint32_t length)
    {
        xGm.SetGlobalBuffer((__gm__ half*)x + offset, length);
        yGm.SetGlobalBuffer((__gm__ half*)y + offset, length);
        zGm.SetGlobalBuffer((__gm__ half*)z + offset, length);
    }
    ```

- The `Process` function loops over the main block data based on the member variable `tileNum`. `xAddr`, `yAddr`, and `zAddr` are the Local Memory addresses pre-allocated for the static tensors. The number of loop iterations is computed using the general form `tileNum + (lastTileLength > 0U ? 1U : 0U)`. In the main block even split scenario, `lastTileLength` is 0, so the number of iterations equals `tileNum`. In each iteration, `mainTileLength` is used as the element count of the current main block, and the intra-core offset `startElement` is computed through `loopIdx * mainTileLength`, completing input loading, Add computation, and output storing in sequence.

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
            // ... synchronization management code omitted; see the complete sample code ...
            AscendC::DataCopy(xLocal, xGm[startElement], curLen);
            AscendC::DataCopy(yLocal, yGm[startElement], curLen);
            // ... synchronization management code omitted ...
            AscendC::Add(zLocal, xLocal, yLocal, curLen);
            // ... synchronization management code omitted ...
            AscendC::DataCopy(zGm[startElement], zLocal, curLen);
            // ... synchronization management code omitted ...
        }
    }
    ```
