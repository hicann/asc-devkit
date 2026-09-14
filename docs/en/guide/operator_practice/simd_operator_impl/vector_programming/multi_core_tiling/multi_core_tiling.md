# Multi-core Tiling<a name="ZH-CN_TOPIC_0000002532228159"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-26T14:04:25.361Z -->

The following figure shows the development process of an operator with Tiling implemented based on the Ascend C approach.

**Figure 1** Operator development process<a name="zh-cn_topic_0000002236197677_fig18330201774715"></a>  
![](../../../../figures/operator_development_process.png "Operator development process")

## Operator Analysis<a name="zh-cn_topic_0000002236197677_section179012044886"></a>

This sample applies to the scenario where input data is evenly split across cores and within each core. The Tiling strategy of this sample is as follows: the total data length TOTAL_LENGTH is 8 * 2048, the data is evenly distributed to 8 cores for execution, the data length BLOCK_LENGTH computed on each core is 2048, the single-core data is split into 16 blocks (the 16-block split is used only as a Tiling example and does not represent the optimal performance; it is for reference only), and the length TILE_LENGTH of each block is 128. The data splitting diagram is as follows:

**Figure 2** Data splitting diagram<a name="zh-cn_topic_0000002236197677_fig1986021174914"></a>  
![](../../../../figures/data_splitting_diagram.png "Data splitting diagram")

Based on the preceding analysis, the design specifications of the Ascend C Add operator are as follows:

-   Operator type (OpType): Add
-   Operator input and output:

    **Table 1** Input and output specifications of the Add operator

    <a name="table4934296305"></a>
    <table><thead align="left"><tr id="row59358913304"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p5503181819300"><a name="p5503181819300"></a><a name="p5503181819300"></a><strong id="b1850331853010"><a name="b1850331853010"></a><a name="b1850331853010"></a>name</strong></p>
    </th>
    <th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p1550381833017"><a name="p1550381833017"></a><a name="p1550381833017"></a><strong id="b7503171811309"><a name="b7503171811309"></a><a name="b7503171811309"></a>shape</strong></p>
    </th>
    <th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p1950391883014"><a name="p1950391883014"></a><a name="p1950391883014"></a><strong id="b2503111803020"><a name="b2503111803020"></a><a name="b2503111803020"></a>data type</strong></p>
    </th>
    <th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p14503218133015"><a name="p14503218133015"></a><a name="p14503218133015"></a><strong id="b8503141818301"><a name="b8503141818301"></a><a name="b8503141818301"></a>format</strong></p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row393589203016"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p1950331810308"><a name="p1950331810308"></a><a name="p1950331810308"></a>x (input)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p12328141215344"><a name="p12328141215344"></a><a name="p12328141215344"></a>(8, 2048)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p135031118173010"><a name="p135031118173010"></a><a name="p135031118173010"></a>half</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p19503131815305"><a name="p19503131815305"></a><a name="p19503131815305"></a>ND</p>
    </td>
    </tr>
    <tr id="row6935119173013"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p75031182305"><a name="p75031182305"></a><a name="p75031182305"></a>y (input)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p532871211344"><a name="p532871211344"></a><a name="p532871211344"></a>(8, 2048)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p45031818103018"><a name="p45031818103018"></a><a name="p45031818103018"></a>half</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p1503918103012"><a name="p1503918103012"></a><a name="p1503918103012"></a>ND</p>
    </td>
    </tr>
    <tr id="row59354943016"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p1450316186305"><a name="p1450316186305"></a><a name="p1450316186305"></a>z (output)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p632831214347"><a name="p632831214347"></a><a name="p632831214347"></a>(8, 2048)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p15503918173017"><a name="p15503918173017"></a><a name="p15503918173017"></a>half</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p1503101813012"><a name="p1503101813012"></a><a name="p1503101813012"></a>ND</p>
    </td>
    </tr>
    </tbody>
    </table>

-   Kernel function name: tiling\_strategy\_custom
-   Main APIs used:
    -   DataCopy: data movement API
    -   Add: basic vector arithmetic API
    -   EnQue, DeQue, and other APIs: Queue management APIs

-   Operator implementation file name: tiling\_strategy.asc

## Tiling Implementation<a name="zh-cn_topic_0000002236197677_section480741815522"></a>

In the preceding scenario, both the input and output of the operator have fixed shapes. However, in actual operator development scenarios, this information can change dynamically, making the scenarios more flexible and complex. In a dynamic shape scenario, the input shape is unknown. Some variables related to the input shape (such as the block size of each data transfer) need to be calculated through Tiling and then passed to the kernel side, where they are used for subsequent computation.

The specific implementation is as follows: analyze and design the Tiling parameters, define the Tiling structure, obtain the input and output shape information through the context on the host side, calculate the Tiling parameters based on the shape information, and set them into the corresponding Tiling structure. The Tiling information is then passed into the kernel function through the kernel function entry parameters. Inside the kernel function, the Tiling structure is parsed to obtain and use the relevant parameters to implement the internal logic of the kernel function. For details, see [Host-side Tiling Implementation](../../../../programming_guide/advanced_programming/aclnn_operator_development/host_tiling_implementation/basic_process.md). This section uses the splitting strategy in the preceding analysis as an example to describe how to implement Tiling.

Based on the splitting strategy in this section, Tiling needs to define the following parameters:

-   blockLength: length of the data computed by each core;
-   tileNum: number of data blocks to be computed by each core;
-   tileLength: length of each data block within each core.

Based on the determined Tiling parameters, the TilingData structure is defined using C++ syntax. The code is as follows.

```
struct AddCustomTilingData {
    uint32_t blockLength;
    uint32_t tileNum;
    uint32_t tileLength;
    ...
}
```

Next, the Tiling parameters are calculated. Since the data within each core is split into 16 blocks, the Tiling parameters are calculated based on the number of cores used and the number of splits within each core, and then written into the Tiling structure. The sample code is as follows:

```
constexpr int32_t NUM_BLOCKS = 8;                             // Number of cores used
constexpr int32_t TILE_NUM = 16;                             // Number of intra-core splits
void GenerateTilingData(uint8_t* tilingBuf, uint32_t numBlocks)
{
    uint32_t totalLength;
    // The method for obtaining the total data length TOTAL_LENGTH is omitted here and can be implemented as needed. This section only describes Tiling-related content.
    AddCustomTilingData* tiling = reinterpret_cast<AddCustomTilingData *>(tilingBuf);
    uint32_t blockLength = TOTAL_LENGTH / numBlocks;
    uint32_t tileNum = TILE_NUM;
    uint32_t tileLength = blockLength / tileNum;

    tiling->blockLength = blockLength;
    tiling->tileNum = tileNum;
    tiling->tileLength = tileLength;
}
```

Finally, in the host-side calling program, call the Tiling parameter calculation function above to compute the relevant parameters, and then pass them to the kernel-side kernel function.

```
    constexpr int32_t NUM_BLOCKS = 8;
    ...
    uint8_t *tiling = nullptr;
    size_t tilingSize = sizeof(AddCustomTilingData);
    GenerateTilingData(tiling, NUM_BLOCKS);  // Call the Tiling parameter calculation function
    ....
    
    tiling_strategy_custom<<<NUM_BLOCKS, 0, stream>>>(xDevice, yDevice, zDevice,
                                               *reinterpret_cast<AddCustomTilingData*>(tiling));
    ....
```

## Operator Class Implementation<a name="zh-cn_topic_0000002236197677_section849945172010"></a>

The kernel-side operator implementation still follows the [vector operator kernel function implementation process](../basic_vector_operator.md#zh-cn_topic_0000002201157438_fig16061570280). The following focuses on the differences in the operator class implementation in this scenario.

-   Set the Global Memory addresses of the input and output Global Tensors.

    In this sample, data is distributed to multiple cores for processing, and each core processes different data. Therefore, the addresses of the data to be processed by different cores in Global Memory are different. In the initialization function Init, you need to obtain the offset addresses of the input and output data to be processed by a single core in Global Memory, and set these offset addresses to the GlobalTensor.

    Take obtaining the offset address of input x in Global Memory as an example. The total data length TOTAL\_LENGTH is 8 \* 2048, and the data is evenly distributed to 8 cores for running. The data length blockLength processed on each core is 2048. Call the [GetBlockIdx](../../../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) API to obtain the index of the current core. x + blockLength \* GetBlockIdx\(\) is the offset address of x in Global Memory in the single-core processing program. After obtaining the offset address, use the [SetGlobalBuffer](../../../../../api/SIMD-API/basic_api/data_structures/GlobalTensor/SetGlobalBuffer.md) API of the GlobalTensor class to set the start address and length of Global Memory on this core. For details, see [Figure 3](#zh-cn_topic_0000002236197677_fig398721711313). The code is as follows:

    ```
    xGm.SetGlobalBuffer((__gm__ half *)x + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
    ```

    **Figure 3**  Multi-core parallel processing diagram<a name="zh-cn_topic_0000002236197677_fig398721711313"></a>  
    ![](../../../../figures/multi_core_parallel_processing_diagram_38.png "multi-core parallel processing diagram-38")

-   Allocate memory for the input and output Queues through the Pipe memory management object.

    For the data processed on a single core, data tiling can be performed. In this example, for reference only, the data on a single core (2048 numbers) is split into 16 blocks (this does not mean that 16 blocks is the optimal performance), with tileLength (128) data in each block. The data splitting diagram is shown in [Figure 4](#zh-cn_topic_0000002236197677_fig1319211154719).

    **Figure 4**  Single-core data splitting diagram<a name="zh-cn_topic_0000002236197677_fig1319211154719"></a>  
    ![](../../../../figures/single_core_data_splitting_diagram_39.png "single-core data splitting diagram-39")

    Compared with the [basic vector operator](../basic_vector_operator.md), when allocating memory for the input and output Queues through the Pipe memory management object, use the length tileLength of each data block within a single core as the length of the allocated memory. For example, to allocate memory for the Queue of input x, the following code segment can be used. Pipe allocates a memory block of tileLength \* sizeof\(half\) bytes for inQueueX, and each memory block can hold tileLength (128) half-type data.

    ```
    pipe->InitBuffer(inQueueX, 1, this->tileLength * sizeof(half))
    ```

The specific initialization function code is as follows:

```
__aicore__ inline void Init(GM_ADDR x, GM_ADDR y, GM_ADDR z, AddCustomTilingData tiling, AscendC::TPipe* pipeIn)
{
    pipe = pipeIn;
    this->blockLength = tiling.blockLength;
    this->tileNum = tiling.tileNum;
    this->tileLength = tiling.tileLength;
    // Calculate the offset address on each core
    xGm.SetGlobalBuffer((__gm__ half *)x + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
    yGm.SetGlobalBuffer((__gm__ half *)y + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
    zGm.SetGlobalBuffer((__gm__ half *)z + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
    // pipe alloc memory to queue, the unit is Bytes
    pipe->InitBuffer(inQueueX, 1, this->tileLength * sizeof(half));
    pipe->InitBuffer(inQueueY, 1, this->tileLength * sizeof(half));
    pipe->InitBuffer(outQueueZ, 1, this->tileLength * sizeof(half));
}
```

Each core needs to perform copy-in, computation, and copy-out on tileNum data blocks respectively. Therefore, tileNum is used as the loop upper limit in the Process function.

```
__aicore__ inline void Process()
{
    int32_t loopCount = this->tileNum;
    // tiling strategy, pipeline parallel
    for (int32_t i = 0; i < loopCount; i++) {
        CopyIn(i, this->tileLength);
        Compute(i, this->tileLength);
        CopyOut(i, this->tileLength);
    }
}
```

Correspondingly, when copying in and copying out each data block within each core, the memory offset address of each data block in Global Memory must be located. Therefore, when the DataCopy API is used inside the CopyIn and CopyOut functions, the offset address of each data block must be added. The Compute function remains unchanged and is the same as that in [basic vector operator](../basic_vector_operator.md).

The CopyIn function implementation code is as follows:

```
__aicore__ inline void CopyIn(int32_t progress, uint32_t tileLength)
{
    ...
    // copy progress_th tile from global tensor to local tensor
    AscendC::DataCopy(xLocal, xGm[progress * this->tileLength], tileLength);
    AscendC::DataCopy(yLocal, yGm[progress * this->tileLength], tileLength);
    ...
}
```

The CopyOut function implementation code is as follows:

```
 __aicore__ inline void CopyOut(int32_t progress, uint32_t tileLength)
{
    ...
    // copy progress_th tile from local tensor to global tensor
    AscendC::DataCopy(zGm[progress * this->tileLength], zLocal, tileLength);
    ...
}
```
