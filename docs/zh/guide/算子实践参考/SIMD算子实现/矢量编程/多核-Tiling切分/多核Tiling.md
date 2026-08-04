# 多核Tiling<a name="ZH-CN_TOPIC_0000002532228159"></a>

基于Ascend C方式实现带有Tiling的算子的开发流程如下图所示。

**图1**  算子开发流程<a name="zh-cn_topic_0000002236197677_fig18330201774715"></a>  
![](../../../../figures/算子开发流程.png "算子开发流程")

## 算子分析<a name="zh-cn_topic_0000002236197677_section179012044886"></a>

本样例为输入数据在核间均分、核内均分场景。本样例的Tiling策略为：数据整体长度TOTAL\_LENGTH为8 \* 2048，数据平均分配到8个核上运行，每个核上计算的数据长度BLOCK\_LENGTH为2048，将单核上的数据切分成16块（此处切分成16块仅用来作为Tiling的样例，并不代表性能最佳，仅供参考），每块数据的长度TILE\_LENGTH为128。数据切分示意如下图所示：

**图2**  数据切分示意图<a name="zh-cn_topic_0000002236197677_fig1986021174914"></a>  
![](../../../../figures/数据切分示意图.png "数据切分示意图")

通过以上分析，得到Ascend C  Add算子的设计规格如下：

-   算子类型（OpType）：Add
-   算子输入输出：

    **表1**  Add算子输入输出规格

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
    <tbody><tr id="row393589203016"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p1950331810308"><a name="p1950331810308"></a><a name="p1950331810308"></a>x（输入）</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p12328141215344"><a name="p12328141215344"></a><a name="p12328141215344"></a>(8, 2048)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p135031118173010"><a name="p135031118173010"></a><a name="p135031118173010"></a>half</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p19503131815305"><a name="p19503131815305"></a><a name="p19503131815305"></a>ND</p>
    </td>
    </tr>
    <tr id="row6935119173013"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p75031182305"><a name="p75031182305"></a><a name="p75031182305"></a>y（输入）</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p532871211344"><a name="p532871211344"></a><a name="p532871211344"></a>(8, 2048)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p45031818103018"><a name="p45031818103018"></a><a name="p45031818103018"></a>half</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p1503918103012"><a name="p1503918103012"></a><a name="p1503918103012"></a>ND</p>
    </td>
    </tr>
    <tr id="row59354943016"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p1450316186305"><a name="p1450316186305"></a><a name="p1450316186305"></a>z（输出）</p>
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

-   核函数名称：tiling\_strategy\_custom
-   使用的主要接口：
    -   DataCopy：数据搬移接口
    -   Add：矢量基础算术接口
    -   EnQue、DeQue等接口：Queue队列管理接口

-   算子实现文件名称：tiling\_strategy.asc

## Tiling实现<a name="zh-cn_topic_0000002236197677_section480741815522"></a>

前述场景中算子的输入和输出均为固定shape，然而在实际的算子开发场景中，这些信息是支持动态变化的，场景会更加灵活和复杂。动态shape场景下，输入的shape是未知的。一些与输入shape相关的变量（比如每次搬运的块大小等），需要通过Tiling计算出来，然后传递到kernel侧，kernel侧使用该参数进行后续的计算。

具体实现方式为：分析设计Tiling参数、定义Tiling结构体，在Host侧通过上下文获取输入输出的shape信息，根据shape信息，计算Tiling参数并设置到对应的Tiling结构体中；通过核函数入口参数将Tiling信息传入核函数，在核函数内通过解析Tiling结构体，获取并使用相关参数来实现核函数内部逻辑，详细介绍请参考[Host侧tiling实现](../../../../编程指南/高级编程/Aclnn算子工程化开发/设计与实现/Host侧Tiling实现.md)。本节将以上述分析中的切分策略为例，说明如何实现Tiling。

基于本节的切分策略，Tiling需要定义如下参数：

-   blockLength：每个核的计算数据长度；
-   tileNum：每个核需要计算的数据块个数；
-   tileLength：每个核内每个数据块的长度。

根据确定的Tiling参数，使用C++语法定义TilingData结构体，代码如下。

```
struct AddCustomTilingData {
    uint32_t blockLength;
    uint32_t tileNum;
    uint32_t tileLength;
    ...
}
```

接下来完成Tiling参数的计算。由于每个核内数据被切分为16块，根据使用的核数和核内切分数，计算Tiling参数，并写入到Tiling结构体内。代码示例如下：

```
constexpr int32_t NUM_BLOCKS = 8;                             // 使用的核数
constexpr int32_t TILE_NUM = 16;                             // 核内切分数量
void GenerateTilingData(uint8_t* tilingBuf, uint32_t numBlocks)
{
    uint32_t totalLength;
    // 此处省略如何获取数据总长TOTAL_LENGTH，可以根据具体情况实现。本章节仅介绍Tiling相关内容。
    AddCustomTilingData* tiling = reinterpret_cast<AddCustomTilingData *>(tilingBuf);
    uint32_t blockLength = TOTAL_LENGTH / numBlocks;
    uint32_t tileNum = TILE_NUM;
    uint32_t tileLength = blockLength / tileNum;

    tiling->blockLength = blockLength;
    tiling->tileNum = tileNum;
    tiling->tileLength = tileLength;
}
```

最后，在Host侧调用程序中，调用上述Tiling参数计算函数，计算出相关参数，然后传递到Kernel侧核函数。

```
    constexpr int32_t NUM_BLOCKS = 8;
    ...
    uint8_t *tiling = nullptr;
    size_t tilingSize = sizeof(AddCustomTilingData);
    GenerateTilingData(tiling, NUM_BLOCKS);  // 调用tiling参数计算函数
    ....
    
    tiling_strategy_custom<<<NUM_BLOCKS, 0, stream>>>(xDevice, yDevice, zDevice,
                                               *reinterpret_cast<AddCustomTilingData*>(tiling));
    ....
```

## 算子类实现<a name="zh-cn_topic_0000002236197677_section849945172010"></a>

Kernel侧算子实现仍遵循[矢量算子核函数实现流程](../基础矢量算子.md#zh-cn_topic_0000002201157438_fig16061570280)，接下来重点介绍本场景中算子类实现的不同点。

-   设置输入输出Global Tensor的Global Memory内存地址。

    由于本样例中将数据分配到了多个核上进行处理，每个核处理不同的数据，因此不同核要处理的数据在Global Memory上的地址不同，在初始化函数Init中，需要获取单核所需处理的输入输出在Global Memory上的内存偏移地址，并将该偏移地址设置到GlobalTensor中。

    以获取输入x在Global Memory上的内存偏移地址为例，数据整体长度TOTAL\_LENGTH为8 \* 2048，平均分配到8个核上运行，每个核上处理的数据长度blockLength为2048，调用[GetBlockIdx](../../../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md)接口获取当前核的index，x + blockLength \* GetBlockIdx\(\)即为单核处理程序中x在Global Memory上的内存偏移地址，获取偏移地址后，使用GlobalTensor类的[SetGlobalBuffer](../../../../../api/SIMD-API/basic_api/data_structures/GlobalTensor/SetGlobalBuffer.md)接口设定该核上Global Memory的起始地址以及长度，具体示意图请参考[图3](#zh-cn_topic_0000002236197677_fig398721711313)。代码如下所示：

    ```
    xGm.SetGlobalBuffer((__gm__ half *)x + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
    ```

    **图3**  多核并行处理示意图<a name="zh-cn_topic_0000002236197677_fig398721711313"></a>  
    ![](../../../../figures/多核并行处理示意图-38.png "多核并行处理示意图-38")

-   通过Pipe内存管理对象为输入输出Queue分配内存。

    对于单核上的处理数据，可以进行数据切块（Tiling），在本示例中，仅作为参考，将单核上的数据（2048个数）切分成16块（并不意味着16块就是性能最优），每块tileLength（128）个数据。数据切分示意图如[图4](#zh-cn_topic_0000002236197677_fig1319211154719)所示。

    **图4**  单核数据切分示意图<a name="zh-cn_topic_0000002236197677_fig1319211154719"></a>  
    ![](../../../../figures/单核数据切分示意图-39.png "单核数据切分示意图-39")

    与[基础矢量算子](../基础矢量算子.md)相比，在通过Pipe内存管理对象为输入输出Queue分配内存时，需使用单核内每个数据块的长度tileLength作为分配内存的长度。比如，为输入x的Queue分配内存，可以通过如下代码段实现，Pipe为inQueueX分配了一块大小为tileLength \* sizeof\(half\)个字节的内存块，每个内存块能容纳tileLength（128）个half类型数据。

    ```
    pipe->InitBuffer(inQueueX, 1, this->tileLength * sizeof(half))
    ```

具体的初始化函数代码如下：

```
__aicore__ inline void Init(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, AddCustomTilingData tiling, AscendC::TPipe* pipeIn)
{
    pipe = pipeIn;
    this->blockLength = tiling.blockLength;
    this->tileNum = tiling.tileNum;
    this->tileLength = tiling.tileLength;
    // 计算每个核上的地址偏移
    xGm.SetGlobalBuffer((__gm__ half *)x + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
    yGm.SetGlobalBuffer((__gm__ half *)y + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
    zGm.SetGlobalBuffer((__gm__ half *)z + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
    // pipe alloc memory to queue, the unit is Bytes
    pipe->InitBuffer(inQueueX, 1, this->tileLength * sizeof(half));
    pipe->InitBuffer(inQueueY, 1, this->tileLength * sizeof(half));
    pipe->InitBuffer(outQueueZ, 1, this->tileLength * sizeof(half));
}
```

每个核需要对tileNum个数据块分别进行搬入、计算、搬出处理，因此Process函数内将tileNum作为循环上限。

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

对应的，每个核内搬入、搬出每个数据块时，需定位到每个数据块所在Global Memory上的内存偏移地址，因此在CopyIn和CopyOut函数内部使用DataCopy接口时，需增加每个数据块的地址偏移。Compute函数没有变化，与[基础矢量算子](../基础矢量算子.md)相同。

CopyIn函数实现代码如下：

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

CopyOut函数实现代码如下：

```
 __aicore__ inline void CopyOut(int32_t progress, uint32_t tileLength)
{
    ...
    // copy progress_th tile from local tensor to global tensor
    AscendC::DataCopy(zGm[progress * this->tileLength], zLocal, tileLength);
    ...
}
```
