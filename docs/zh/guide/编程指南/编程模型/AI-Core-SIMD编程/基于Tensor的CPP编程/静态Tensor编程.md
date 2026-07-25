# 静态Tensor编程<a name="ZH-CN_TOPIC_0000002463954885"></a>

在基于Pipe进行算子开发的方式中，由Pipe（TPipe类）统一管理Device端内存等资源，开发者无需感知内存管理、DoubleBuffer流水、同步等处理，只需要按照计算流编写算子即可，但由此也带来了一些运行时开销（如TPipe创建、InitBuffer等）。

基于以上原因，Ascend C提供了静态Tensor编程方式，相比基于Pipe的编程方式，这种方式避免了TPipe内存管理初始化过程（约数百纳秒），从而减少了运行时开销，更有助于开发者实现极致性能。通过直接构造指定地址和存储位置的LocalTensor，并将其传递给计算、搬运等API进行编程，提供了更高的灵活性。然而，这种编程方式也带来了更高的开发复杂性，需要开发者自行管理DoubleBuffer和同步流水，并且只能使用Ascend C的基础API，而非全部功能。

两种编程方式的对比如下：

![](../../../../figures/图1-开发Ascend-C算子的基本流程.png)

>[!NOTE]说明
>- 静态Tensor编程的使用约束和限制请参考[使用约束和限制](#section19853161834615)。
>- 本节涉及的完整样例请参考[静态Tensor编程样例](../../../../../../../examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance)。

## 编程范式<a name="section1486516584319"></a>

-   AI Core包括多种内存单元，比如用于矢量计算的Unified Buffer和用于矩阵计算的L1 Buffer、L0A Buffer、L0B Buffer、L0C Buffer等内存资源。开发者完全自主管理AI Core上的所有内存资源，创建Tensor分配地址时管理内存大小、内存复用关系并确保分配的地址有效性。
-   AI Core包括多种[指令流水类型](../../../../../api/SIMD-API/基础API/同步控制/核内同步/核内同步能力概述.md)，比如Vector/Cube/Scalar计算流水，MTE1、MTE2、MTE3搬运流水等，每条流水并行执行，它们之间的依赖关系通过[同步事件](../../../../../api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)来协调。开发者调用Ascend C提供的搬运或者计算类API编写算子并根据数据依赖关系插入对应的同步事件，以达成最优性能。

下图是一个典型矢量算子的示意图，开发者首先根据业务计算量进行数据分块处理，之后根据核内的数据依赖关系完成同步事件的插入：

![](../../../../figures/内存管理示意图-5.png)

## 内存管理<a name="section626154143513"></a>

静态Tensor编程方式下，开发者可以使用两种方式创建Tensor：

-   通过[LocalMemAllocator](../../../../../api/SIMD-API/基础API/资源管理/LocalMemAllocator/LocalMemAllocator.md)指定硬件位置进行Tensor分配。

    LocalMemAllocator是一种线性内存分配器，开发者可以调用Alloc方法进行内存分配，地址分配从0开始，根据调用次序依次向后进行线性分配，LocalMemAllocator只是一个简单的线性分配器，并不提供内存释放以及其它内存管理的能力。在不关注Bank冲突场景或者算子初始功能开发时，可以使用LocalMemAllocator简化算子编写，在后续性能优化时切换到使用LocalTensor进行地址分配的方式。

-   通过[LocalTensor构造函数](../../../../../api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor构造函数.md)创建Tensor，极致性能场景推荐使用此方式。

    开发者可以使用LocalTensor构造函数直接指定内存地址，实现内存的完全自主管理（本质上无需申请和释放内存）。使用时，需根据需求合理指定地址（不超过物理存储上限），并在保证功能正确的前提下进行内存复用。如果需要通过规避Bank冲突或者复用内存来获得极致性能时，推荐使用该方式。

```
    // 方式1：使用LocalMemAllocator进行内存分配
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<float> xLocalPing = ubAllocator.Alloc<float, TILE_LENGTH>();
    AscendC::LocalTensor<float> yLocalPing = ubAllocator.Alloc<float, TILE_LENGTH>();
    AscendC::LocalTensor<float> zLocalPing = ubAllocator.Alloc<float, TILE_LENGTH>();

    // 方式2：直接使用LocalTensor构造函数构造Tensor
    AscendC::LocalTensor<float> xLocalPing(AscendC::TPosition::VECCALC, xAddrPing, TILE_LENGTH);
    AscendC::LocalTensor<float> yLocalPing(AscendC::TPosition::VECCALC, yAddrPing, TILE_LENGTH);
    AscendC::LocalTensor<float> zLocalPing(AscendC::TPosition::VECCALC, zAddrPing, TILE_LENGTH);
```

## 同步管理<a name="section4338114663610"></a>

根据前文介绍的硬件架构，AI Core内部异步并行计算存在多条流水（包括矢量计算、矩阵计算、数据搬入、数据搬出等），多条流水之间存在数据依赖时，需要插入对应的同步事件。静态Tensor编程方式下，开发者使用[SetFlag/WaitFlag\(ISASI\)](../../../../../api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)和[PipeBarrier\(ISASI\)](../../../../../api/SIMD-API/基础API/同步控制/核内同步/PipeBarrier(ISASI).md)手动插入同步，事件的类型和事件ID由开发者自行管理，但需要注意事件ID不能使用6和7（可能与内部使用的事件ID出现冲突，进而出现未定义行为）。另外由于需要使用SetFlag/WaitFlag/PipeBarrier底层同步接口（属于ISASI硬件体系结构相关的接口），无法保证跨硬件版本兼容。

在同步依赖中，根据数据依赖和指令执行关系，存在两种依赖关系，即正向同步（循环内依赖）与反向同步（循环间依赖）：

-   正向同步

    在本次数据搬入和计算之间，插入MTE2\_V（矢量计算流水等待MTE2搬运流水）同步事件，确保数据搬入之后再进行计算；在本次数据计算和搬出之间，插入V\_MTE3（MTE3搬运流水等待矢量计算流水）同步事件，确保数据计算完成后再进行搬出。

-   反向同步

    在上一次的数据计算和本次数据搬入之间，插入V\_MTE2（MTE2搬运流水等待矢量计算流水）同步事件，确保上一次的数据计算完成后，本次的数据再进行搬入。防止本次的数据会覆盖掉上一次未计算完成的数据；在上一次的数据搬出和本次数据计算之间，插入MTE3\_V（矢量计算流水等待MTE3搬运流水）同步事件，确保上一次的数据搬出后，再进行本次数据的计算。防止本次的数据会覆盖掉上一次未搬出的数据。

上述的同步逻辑在使用Pipe编程框架时，框架会使用EnQue/DeQue/AllocTensor/FreeTensor进行封装。您可以通过[TPipe-TQue框架编程原理](../基于TPipe-TQue框架编程/TPipe-TQue框架编程原理.md)来了解应该如何在使用静态Tensor编程方式时手动进行同步控制。

```
    AscendC::LocalTensor<half> xLocal(AscendC::TPosition::VECCALC, xAddr, MAX_DATA_COPY_LEN);
    AscendC::LocalTensor<half> yLocal(AscendC::TPosition::VECCALC, yAddr, MAX_DATA_COPY_LEN);
    AscendC::LocalTensor<half> zLocal(AscendC::TPosition::VECCALC, zAddr, MAX_DATA_COPY_LEN);

    uint32_t totalBlocks = GetTotalBlocks();

    for (uint32_t loopIdx = 0; loopIdx < totalBlocks; loopIdx++) {
        uint32_t startElement = loopIdx * dataCopyLen;
        uint32_t remainElements = totalElementsPerCore - startElement;
        uint32_t curLen = remainElements > dataCopyLen ? dataCopyLen : remainElements;

        if (loopIdx != 0) {
            AscendC::WaitFlag<AscendC::HardEvent::V_MTE2>(EVENT_ID0);
        }

        AscendC::DataCopy(xLocal, xGm[startElement], curLen);
        AscendC::DataCopy(yLocal, yGm[startElement], curLen);

        AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

        if (loopIdx != 0) {
            AscendC::WaitFlag<AscendC::HardEvent::MTE3_V>(EVENT_ID0);
        }

        AscendC::Add(zLocal, xLocal, yLocal, curLen);

        if (loopIdx != (totalBlocks - 1)) {
            AscendC::SetFlag<AscendC::HardEvent::V_MTE2>(EVENT_ID0);
        }
        AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
        AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);

        AscendC::DataCopy(zGm[startElement], zLocal, curLen);

        if (loopIdx != (totalBlocks - 1)) {
            AscendC::SetFlag<AscendC::HardEvent::MTE3_V>(EVENT_ID0);
        }
    }
```

## 流水优化<a name="section121239188376"></a>

在基于TPipe的编程范式中，开发者只需要在InitBuffer时指定buffer数量为2，即可自动开启DoubleBuffer。但是静态Tensor编程方式下，开发者需要手动开启DoubleBuffer，具体示例如下，完整样例请参考[静态Tensor编程样例](../../../../../../../examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance)中的DoubleBuffer示例。

```
    // ping
    AscendC::LocalTensor<half> xPing(AscendC::TPosition::VECCALC, xAddrPing, MAX_DATA_COPY_LEN);
    AscendC::LocalTensor<half> yPing(AscendC::TPosition::VECCALC, yAddrPing, MAX_DATA_COPY_LEN);
    AscendC::LocalTensor<half> zPing(AscendC::TPosition::VECCALC, zAddrPing, MAX_DATA_COPY_LEN);
    // pong
    AscendC::LocalTensor<half> xPong(AscendC::TPosition::VECCALC, xAddrPong, MAX_DATA_COPY_LEN);
    AscendC::LocalTensor<half> yPong(AscendC::TPosition::VECCALC, yAddrPong, MAX_DATA_COPY_LEN);
    AscendC::LocalTensor<half> zPong(AscendC::TPosition::VECCALC, zAddrPong, MAX_DATA_COPY_LEN);

    uint32_t totalBlocks = GetTotalBlocks();

    // DoubleBuffer
    AscendC::SetFlag<AscendC::HardEvent::V_MTE2>(EVENT_ID0);
    AscendC::SetFlag<AscendC::HardEvent::V_MTE2>(EVENT_ID1);
    AscendC::SetFlag<AscendC::HardEvent::MTE3_V>(EVENT_ID0);
    AscendC::SetFlag<AscendC::HardEvent::MTE3_V>(EVENT_ID1);

    for (uint32_t loopIdx = 0; loopIdx < totalBlocks; loopIdx++) {
        uint32_t startElement = loopIdx * dataCopyLen;
        uint32_t remainElements = totalElementsPerCore - startElement;
        uint32_t curLen = remainElements > dataCopyLen ? dataCopyLen : remainElements;

        int32_t eventID = ((loopIdx & 1) == 0 ? EVENT_ID0 : EVENT_ID1);
        AscendC::LocalTensor<half>& xLocal = ((loopIdx & 1) == 0 ? xPing : xPong);
        AscendC::LocalTensor<half>& yLocal = ((loopIdx & 1) == 0 ? yPing : yPong);
        AscendC::LocalTensor<half>& zLocal = ((loopIdx & 1) == 0 ? zPing : zPong);

        AscendC::WaitFlag<AscendC::HardEvent::V_MTE2>(eventID);

        AscendC::DataCopy(xLocal, xGm[startElement], curLen);
        AscendC::DataCopy(yLocal, yGm[startElement], curLen);

        AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(eventID);
        AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(eventID);

        AscendC::WaitFlag<AscendC::HardEvent::MTE3_V>(eventID);

        AscendC::Add(zLocal, xLocal, yLocal, curLen);

        AscendC::SetFlag<AscendC::HardEvent::V_MTE2>(eventID);

        AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(eventID);
        AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(eventID);

        AscendC::DataCopy(zGm[startElement], zLocal, curLen);

        AscendC::SetFlag<AscendC::HardEvent::MTE3_V>(eventID);
    }

    AscendC::WaitFlag<AscendC::HardEvent::V_MTE2>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE2>(EVENT_ID1);
    AscendC::WaitFlag<AscendC::HardEvent::MTE3_V>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE3_V>(EVENT_ID1);
```

以下为不开启DoubleBuffer和开启DoubleBuffer的流水示意图。多数情况下，采用DoubleBuffer能有效提升Vector的时间利用率，缩减算子执行时间，详细内容可参考[DoubleBuffer](../../../../技术附录/概念原理和术语/性能优化技术原理/DoubleBuffer.md)。

![](../../../../figures/图1-开发Ascend-C算子的基本流程-6.png)

![](../../../../figures/图1-开发Ascend-C算子的基本流程-7.png)

## 使用约束和限制<a name="section19853161834615"></a>

静态Tensor编程方式需要遵循以下约束和限制：

-   开发者不能使用TPipe/TQue/TQueBind/TBufPool等框架接口，和上述框架接口混用可能会出现未定义行为。
-   只能使用部分API。具体支持的API列表见[支持的API范围](#section2633193623711)。因为不在列表范围内的API内部依赖TPipe分配事件ID，可能会和开发者定义的事件ID产生冲突。
-   对于支持的API，仍需关注其内部是否依赖Ascend C预留的UB空间（针对 [NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md#npu-arch)，Select等API内部使用了8KB的预留UB空间，用于存储中间数据；针对 [NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md#npu-arch)，Exp等API内部使用了2KB的预留UB空间，用于指令兼容或存储中间数据）。若开启[--cce-disable-asc-reserved-ubuf](../../../编译与运行/算子编译/AI-Core算子编译基本用法.md#常用的编译选项)编译选项，在对应产品版本下调用相关API会触发编译期报错。具体API范围见[使用预留UB空间的API列表](#section_reserved_ubuf_api)。
-   同步事件需要由开发者使用[SetFlag/WaitFlag\(ISASI\)](../../../../../api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)和[PipeBarrier\(ISASI\)](../../../../../api/SIMD-API/基础API/同步控制/核内同步/PipeBarrier(ISASI).md)手动插入，事件的类型和事件ID由开发者自行管理，但需要注意事件ID不能使用6和7（可能与内部使用的事件ID出现冲突，进而出现未定义行为）。
-   由于需要使用SetFlag/WaitFlag/PipeBarrier底层同步接口（属于ISASI硬件体系结构相关的接口），无法保证算子跨硬件版本兼容。
-   Kernel入口处需要开发者手动调用[InitSocState](../../../../../api/SIMD-API/基础API/工具接口/系统初始化/InitSocState.md)接口用来初始化全局状态寄存器。因为全局状态寄存器处于不确定状态，如果不调用该接口，可能导致算子执行过程中出现未定义行为。在TPipe框架编程中，初始化过程由TPipe完成，无需开发者关注。
-   Kernel结束前需要开发者手动调用[PipeBarrier<PIPE_ALL>()](../../../../../api/SIMD-API/基础API/同步控制/核内同步/PipeBarrier(ISASI).md)。

## 支持的API范围<a name="section2633193623711"></a>

<!-- npu="310p" id1 -->
**表1**  针对Atlas 推理系列产品AI Core，支持的API范围

<a name="table56285428438"></a>
<table><thead align="left"><tr id="row46697425436"><th class="cellrowborder" valign="top" width="30.28%" id="mcps1.2.3.1.1"><p id="p866911429430"><a name="p866911429430"></a><a name="p866911429430"></a>接口分类</p>
</th>
<th class="cellrowborder" valign="top" width="69.72%" id="mcps1.2.3.1.2"><p id="p186691442174319"><a name="p186691442174319"></a><a name="p186691442174319"></a>接口名称</p>
</th>
</tr>
</thead>
<tbody><tr id="row146691342124318"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1669154234318"><a name="p1669154234318"></a><a name="p1669154234318"></a>基础API &gt; 标量计算</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p0669164213434"><a name="p0669164213434"></a><a name="p0669164213434"></a>GetBitCount、CountLeadingZero、Cast、CountBitsCntSameAsSignBit、GetSFFValue</p>
</td>
</tr>
<tr id="row1966914211437"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p7669174224314"><a name="p7669174224314"></a><a name="p7669174224314"></a>基础API &gt; 矢量计算 &gt; 基础算术</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p3669184224317"><a name="p3669184224317"></a><a name="p3669184224317"></a>Exp、Ln、Abs、Reciprocal、Sqrt、Rsqrt、Relu、VectorPadding、Add、Sub、Mul、Div、Max、Min、BilinearInterpolation、Adds、Muls、Maxs、Mins、LeakyRelu</p>
</td>
</tr>
<tr id="row666974224312"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p2066924274314"><a name="p2066924274314"></a><a name="p2066924274314"></a>基础API &gt; 矢量计算 &gt; 逻辑计算</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1166964217431"><a name="p1166964217431"></a><a name="p1166964217431"></a>Not、And、Or</p>
</td>
</tr>
<tr id="row46691542124317"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p766994224314"><a name="p766994224314"></a><a name="p766994224314"></a>基础API &gt; 矢量计算 &gt; 复合计算</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1559917570124"><a name="p1559917570124"></a><a name="p1559917570124"></a>Axpy、CastDequant、AddRelu、AddReluCast、AddDeqRelu、SubRelu、SubReluCast、MulAddDst、MulCast、FusedMulAdd、MulAddRelu</p>
</td>
</tr>
<tr id="row176691942154313"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p126691442184311"><a name="p126691442184311"></a><a name="p126691442184311"></a>基础API &gt; 矢量计算 &gt; 比较与选择</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1670442104314"><a name="p1670442104314"></a><a name="p1670442104314"></a>Compare、Compare（结果存入寄存器）、Compares、GetCmpMask、SetCmpMask、</p>
<p id="p367013429431"><a name="p367013429431"></a><a name="p367013429431"></a>Select、GatherMask</p>
</td>
</tr>
<tr id="row36706425430"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p8670124211438"><a name="p8670124211438"></a><a name="p8670124211438"></a>基础API &gt; 矢量计算 &gt; 类型转换</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p166709429433"><a name="p166709429433"></a><a name="p166709429433"></a>Cast</p>
</td>
</tr>
<tr id="row267074213433"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1167094274316"><a name="p1167094274316"></a><a name="p1167094274316"></a>基础API &gt; 矢量计算 &gt; 归约计算</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p156703427434"><a name="p156703427434"></a><a name="p156703427434"></a>ReduceRepeat、ReduceDataBlock、ReducePairElem、GetReduceRepeatMaxMinSpr</p>
</td>
</tr>
<tr id="row067015420439"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p667094294319"><a name="p667094294319"></a><a name="p667094294319"></a>基础API &gt; 矢量计算 &gt; 数据转换</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p166701427437"><a name="p166701427437"></a><a name="p166701427437"></a>Transpose、TransDataTo5HD</p>
</td>
</tr>
<tr id="row66709428433"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p7670842124315"><a name="p7670842124315"></a><a name="p7670842124315"></a>基础API &gt; 矢量计算 &gt; 数据填充</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1867064211432"><a name="p1867064211432"></a><a name="p1867064211432"></a>Duplicate</p>
</td>
</tr>
<tr id="row11670174211438"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p767044244315"><a name="p767044244315"></a><a name="p767044244315"></a>基础API &gt; 矢量计算 &gt; 排序组合</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p196701142204317"><a name="p196701142204317"></a><a name="p196701142204317"></a>ProposalConcat、ProposalExtract、RpSort16、MrgSort4、GetMrgSortResult</p>
</td>
</tr>
<tr id="row86708421431"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1367054274312"><a name="p1367054274312"></a><a name="p1367054274312"></a>基础API &gt; 矢量计算 &gt; 离散与聚合</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p6670442114317"><a name="p6670442114317"></a><a name="p6670442114317"></a>Gather、Scatter</p>
</td>
</tr>
<tr id="row467024220436"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p206701242204318"><a name="p206701242204318"></a><a name="p206701242204318"></a>基础API &gt; 矢量计算 &gt; 掩码操作</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1670134217439"><a name="p1670134217439"></a><a name="p1670134217439"></a>SetMaskCount、SetMaskNorm、SetVectorMask、ResetMask</p>
</td>
</tr>
<tr id="row11670142144320"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1670174218434"><a name="p1670174218434"></a><a name="p1670174218434"></a>基础API &gt; 矢量计算 &gt; 量化设置</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p14670144210437"><a name="p14670144210437"></a><a name="p14670144210437"></a>SetDeqScale</p>
</td>
</tr>
<tr id="row8670194214432"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p20670174219436"><a name="p20670174219436"></a><a name="p20670174219436"></a>基础API &gt; 数据搬运 &gt; DataCopy</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1567014216432"><a name="p1567014216432"></a><a name="p1567014216432"></a>基础数据搬运</p>
</td>
</tr>
<tr id="row166701142174317"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p126701142174316"><a name="p126701142174316"></a><a name="p126701142174316"></a>基础API &gt; 同步控制 &gt; 核内同步</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p167024224318"><a name="p167024224318"></a><a name="p167024224318"></a>SetFlag/WaitFlag、PipeBarrier</p>
</td>
</tr>
<tr id="row166719424430"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1067184244310"><a name="p1067184244310"></a><a name="p1067184244310"></a>基础API &gt; 缓存控制</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p20671042134320"><a name="p20671042134320"></a><a name="p20671042134320"></a>DataCachePreload、DataCacheCleanAndInvalid、ICachePreLoad</p>
</td>
</tr>
<tr id="row12671124274312"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1867119427433"><a name="p1867119427433"></a><a name="p1867119427433"></a>基础API &gt; 系统变量访问</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p06713423434"><a name="p06713423434"></a><a name="p06713423434"></a>GetBlockNum、GetBlockIdx、GetDataBlockSizeInBytes、GetArchVersion、GetTaskRatio、InitSocState、GetProgramCounter、CheckLocalMemoryIA</p>
</td>
</tr>
<tr id="row96714427439"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p106711742154319"><a name="p106711742154319"></a><a name="p106711742154319"></a>基础API &gt; 原子操作</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p17671042164319"><a name="p17671042164319"></a><a name="p17671042164319"></a>SetAtomicAdd、DisableDmaAtomic</p>
</td>
</tr>
<tr id="row13671194254311"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p6671184224317"><a name="p6671184224317"></a><a name="p6671184224317"></a>基础API &gt; 矩阵计算</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p147142311613"><a name="p147142311613"></a><a name="p147142311613"></a>Fill、LoadData、SetAippFunctions、LoadImageToLocal、LoadUnzipIndex、LoadDataUnzip、SetLoadDataBoundary、SetLoadDataPaddingValue、Mmad</p>
</td>
</tr>
</tbody>
</table>
<!-- end id1 -->

<!-- npu="910b" id2 -->
**表2**  针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的API范围

<a name="table1842818536431"></a>
<table><thead align="left"><tr id="row104742531436"><th class="cellrowborder" valign="top" width="29.9%" id="mcps1.2.4.1.1"><p id="p10474205364319"><a name="p10474205364319"></a><a name="p10474205364319"></a>接口分类</p>
</th>
<th class="cellrowborder" valign="top" width="44.25%" id="mcps1.2.4.1.2"><p id="p1047418533430"><a name="p1047418533430"></a><a name="p1047418533430"></a>接口名称</p>
</th>
<th class="cellrowborder" valign="top" width="25.85%" id="mcps1.2.4.1.3"><p id="p4474125374319"><a name="p4474125374319"></a><a name="p4474125374319"></a>备注</p>
</th>
</tr>
</thead>
<tbody><tr id="row747410531434"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p12474153154311"><a name="p12474153154311"></a><a name="p12474153154311"></a>基础API &gt; 标量计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p5821115016275"><a name="p5821115016275"></a><a name="p5821115016275"></a>GetBitCount、CountLeadingZero、CountBitsCntSameAsSignBit、GetSFFValue、Cast（float转half、int32_t）、Cast（float转bfloat16_t）、Cast（多类型转float）</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p10474175318436"><a name="p10474175318436"></a><a name="p10474175318436"></a>-</p>
</td>
</tr>
<tr id="row64759532431"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p747511535436"><a name="p747511535436"></a><a name="p747511535436"></a>基础API &gt; 矢量计算 &gt; 基础算术</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p10475753174311"><a name="p10475753174311"></a><a name="p10475753174311"></a>Exp、Ln、Abs、Reciprocal、Sqrt、Rsqrt、Relu、Add、Sub、Mul、Div、Max、Min、BilinearInterpolation、Adds、Muls、Maxs、Mins、LeakyRelu</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p114751853194317"><a name="p114751853194317"></a><a name="p114751853194317"></a>-</p>
</td>
</tr>
<tr id="row1647525319432"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p447513537436"><a name="p447513537436"></a><a name="p447513537436"></a>基础API &gt; 矢量计算 &gt; 逻辑计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p747525319434"><a name="p747525319434"></a><a name="p747525319434"></a>Not、And、Or、ShiftLeft、ShiftRight</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p84751853144316"><a name="p84751853144316"></a><a name="p84751853144316"></a>-</p>
</td>
</tr>
<tr id="row1247575318435"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p164751053124316"><a name="p164751053124316"></a><a name="p164751053124316"></a>基础API &gt; 矢量计算 &gt; 复合计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p1169817233416"><a name="p1169817233416"></a><a name="p1169817233416"></a>Axpy、CastDequant、AddRelu、AddReluCast、SubRelu、SubReluCast、MulAddDst、MulCast、FusedMulAdd、MulAddRelu</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p5475153164312"><a name="p5475153164312"></a><a name="p5475153164312"></a>-</p>
</td>
</tr>
<tr id="row13475185394314"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p20475105310432"><a name="p20475105310432"></a><a name="p20475105310432"></a>基础API &gt; 矢量计算 &gt; 比较与选择</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p16475553114318"><a name="p16475553114318"></a><a name="p16475553114318"></a>Compare、Compare（结果存入寄存器）、Compares、GetCmpMask、SetCmpMask、GatherMask</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p6475653204318"><a name="p6475653204318"></a><a name="p6475653204318"></a>-</p>
</td>
</tr>
<tr id="row18475195364317"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p4475853204310"><a name="p4475853204310"></a><a name="p4475853204310"></a>基础API &gt; 矢量计算 &gt; 类型转换</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p18475253154318"><a name="p18475253154318"></a><a name="p18475253154318"></a>Cast</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p847515384318"><a name="p847515384318"></a><a name="p847515384318"></a>-</p>
</td>
</tr>
<tr id="row1047545324316"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p14475165364311"><a name="p14475165364311"></a><a name="p14475165364311"></a>基础API &gt; 矢量计算 &gt; 归约计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p847595312435"><a name="p847595312435"></a><a name="p847595312435"></a>ReduceRepeat、ReduceDataBlock、ReducePairElem、GetReduceRepeatSumSpr、GetReduceRepeatMaxMinSpr</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p18475115318430"><a name="p18475115318430"></a><a name="p18475115318430"></a>-</p>
</td>
</tr>
<tr id="row947565318439"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p54759533439"><a name="p54759533439"></a><a name="p54759533439"></a>基础API &gt; 矢量计算 &gt; 数据转换</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p3475553184310"><a name="p3475553184310"></a><a name="p3475553184310"></a>Transpose、TransDataTo5HD</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p147451518192713"><a name="p147451518192713"></a><a name="p147451518192713"></a>TransDataTo5HD （不支持dst与src类型为uint64_t的LocalTensor的场景）</p>
<p id="p2047518539434"><a name="p2047518539434"></a><a name="p2047518539434"></a></p>
</td>
</tr>
<tr id="row847545310433"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p1347555344319"><a name="p1347555344319"></a><a name="p1347555344319"></a>基础API &gt; 矢量计算 &gt; 数据填充</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p2475135318433"><a name="p2475135318433"></a><a name="p2475135318433"></a>Duplicate、Brcb</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1647535364315"><a name="p1647535364315"></a><a name="p1647535364315"></a>-</p>
</td>
</tr>
<tr id="row7475175384319"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p1147575304315"><a name="p1147575304315"></a><a name="p1147575304315"></a>基础API &gt; 矢量计算 &gt; 排序组合</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p747595317433"><a name="p747595317433"></a><a name="p747595317433"></a>Sort32、MrgSort、GetMrgSortResult</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1547545312433"><a name="p1547545312433"></a><a name="p1547545312433"></a>-</p>
</td>
</tr>
<tr id="row104754537434"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p11475105315431"><a name="p11475105315431"></a><a name="p11475105315431"></a>基础API &gt; 矢量计算 &gt; 离散与聚合</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p1147612530434"><a name="p1147612530434"></a><a name="p1147612530434"></a>Gather、Gatherb</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p347635384311"><a name="p347635384311"></a><a name="p347635384311"></a>-</p>
</td>
</tr>
<tr id="row9476353114310"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p1547675319439"><a name="p1547675319439"></a><a name="p1547675319439"></a>基础API &gt; 矢量计算 &gt; 掩码操作</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p18476053164310"><a name="p18476053164310"></a><a name="p18476053164310"></a>SetMaskCount、SetMaskNorm、SetVectorMask、ResetMask</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1047610535435"><a name="p1047610535435"></a><a name="p1047610535435"></a>-</p>
</td>
</tr>
<tr id="row1147614531435"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p4476185324312"><a name="p4476185324312"></a><a name="p4476185324312"></a>基础API &gt; 矢量计算 &gt; 量化设置</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p247625310439"><a name="p247625310439"></a><a name="p247625310439"></a>SetDeqScale</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p19476185312436"><a name="p19476185312436"></a><a name="p19476185312436"></a>-</p>
</td>
</tr>
<tr id="row447613539435"><td class="cellrowborder" rowspan="4" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p1947685374310"><a name="p1947685374310"></a><a name="p1947685374310"></a>基础API &gt; 数据搬运 &gt; DataCopy</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p34761253144318"><a name="p34761253144318"></a><a name="p34761253144318"></a>基础数据搬运</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p19476205324319"><a name="p19476205324319"></a><a name="p19476205324319"></a>不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row154764536438"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p16476353104318"><a name="p16476353104318"></a><a name="p16476353104318"></a>增强数据搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p9476165310436"><a name="p9476165310436"></a><a name="p9476165310436"></a>不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row2476185317437"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p4476253144320"><a name="p4476253144320"></a><a name="p4476253144320"></a>切片数据搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1447635324319"><a name="p1447635324319"></a><a name="p1447635324319"></a>-</p>
</td>
</tr>
<tr id="row1847610536433"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p11975173673218"><a name="p11975173673218"></a><a name="p11975173673218"></a>随路转换ND2NZ搬运</p>
<p id="p5975136163216"><a name="p5975136163216"></a><a name="p5975136163216"></a>随路转换NZ2ND搬运</p>
<p id="p1397583616325"><a name="p1397583616325"></a><a name="p1397583616325"></a>随路量化激活搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1047612538437"><a name="p1047612538437"></a><a name="p1047612538437"></a>随路转换ND2NZ搬运不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row747635344315"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p547665354313"><a name="p547665354313"></a><a name="p547665354313"></a>基础API &gt; 数据搬运</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p4476135364311"><a name="p4476135364311"></a><a name="p4476135364311"></a>Copy、DataCopyPad、SetPadValue</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p8476165374314"><a name="p8476165374314"></a><a name="p8476165374314"></a>DataCopyPad不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row114761053204315"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p2047615394311"><a name="p2047615394311"></a><a name="p2047615394311"></a>基础API &gt; 同步控制 &gt; 核内同步</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p14476853154310"><a name="p14476853154310"></a><a name="p14476853154310"></a>SetFlag/WaitFlag、PipeBarrier、DataSyncBarrier</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p947605334315"><a name="p947605334315"></a><a name="p947605334315"></a>-</p>
</td>
</tr>
<tr id="row1947615537431"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p04761853204310"><a name="p04761853204310"></a><a name="p04761853204310"></a>基础API &gt; 同步控制 &gt; 核间同步</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p10476115315430"><a name="p10476115315430"></a><a name="p10476115315430"></a>CrossCoreSetFlag、CrossCoreWaitFlag</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p6476155320439"><a name="p6476155320439"></a><a name="p6476155320439"></a>-</p>
</td>
</tr>
<tr id="row1547655354318"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p54761353104315"><a name="p54761353104315"></a><a name="p54761353104315"></a>基础API &gt; 缓存控制</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p1476195316434"><a name="p1476195316434"></a><a name="p1476195316434"></a>DataCachePreload、DataCacheCleanAndInvalid、ICachePreLoad、GetICachePreloadStatus</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p104766539434"><a name="p104766539434"></a><a name="p104766539434"></a>-</p>
</td>
</tr>
<tr id="row847685384313"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p194761534433"><a name="p194761534433"></a><a name="p194761534433"></a>基础API &gt; 系统变量访问</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p647655312439"><a name="p647655312439"></a><a name="p647655312439"></a>GetBlockNum、GetBlockIdx、GetDataBlockSizeInBytes、GetArchVersion、GetTaskRatio、InitSocState、GetProgramCounter、GetSubBlockNum、GetSubBlockIdx、GetSystemCycle、</p>
<p id="p716419515422"><a name="p716419515422"></a><a name="p716419515422"></a>CheckLocalMemoryIA</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p2047615324310"><a name="p2047615324310"></a><a name="p2047615324310"></a>-</p>
</td>
</tr>
<tr id="row174769538434"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p14765531438"><a name="p14765531438"></a><a name="p14765531438"></a>基础API &gt; 原子操作</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p847615314436"><a name="p847615314436"></a><a name="p847615314436"></a>SetAtomicAdd、SetAtomicType、DisableDmaAtomic、SetAtomicMax、SetAtomicMin、SetStoreAtomicConfig、GetStoreAtomicConfig</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p10476153144316"><a name="p10476153144316"></a><a name="p10476153144316"></a>-</p>
</td>
</tr>
<tr id="row7476053114311"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p747710531433"><a name="p747710531433"></a><a name="p747710531433"></a>基础API &gt; 矩阵计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p847713536432"><a name="p847713536432"></a><a name="p847713536432"></a>Mmad、MmadWithSparse、SetHF32Mode、SetHF32TransMode、SetMMRowMajor、SetMMColumnMajor、SetFixPipeConfig、SetFixpipeNz2ndFlag、SetFixpipePreQuantFlag、Fill、LoadData、LoadDataWithTranspose、SetAippFunctions、LoadImageToLocal、LoadDataWithSparse、SetFmatrix、SetLoadDataBoundary、SetLoadDataRepeat、SetLoadDataPaddingValue、Fixpipe</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p154771853144319"><a name="p154771853144319"></a><a name="p154771853144319"></a>-</p>
</td>
</tr>
<tr id="row4477125311436"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p1747745334311"><a name="p1747745334311"></a><a name="p1747745334311"></a>Utils API &gt; C++标准库 &gt; 算法</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p4477125313438"><a name="p4477125313438"></a><a name="p4477125313438"></a>max、min、index_sequence</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p547711539430"><a name="p547711539430"></a><a name="p547711539430"></a>-</p>
</td>
</tr>
<tr id="row647718531435"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p9477145317433"><a name="p9477145317433"></a><a name="p9477145317433"></a>Utils API &gt; C++标准库 &gt; 容器函数</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p12477105344313"><a name="p12477105344313"></a><a name="p12477105344313"></a>tuple、get、make_tuple</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p144771253194313"><a name="p144771253194313"></a><a name="p144771253194313"></a>-</p>
</td>
</tr>
<tr id="row7477145344310"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p114771553194319"><a name="p114771553194319"></a><a name="p114771553194319"></a>Utils API &gt; C++标准库 &gt; 类型特性</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p9477135312433"><a name="p9477135312433"></a><a name="p9477135312433"></a>is_convertible、is_base_of、is_same、enable_if、conditional</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p047765314438"><a name="p047765314438"></a><a name="p047765314438"></a>-</p>
</td>
</tr>
</tbody>
</table>
<!-- end id2 -->

<!-- npu="A3" id3 -->
**表3**  针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的API范围

<a name="table167051848128"></a>
<table><thead align="left"><tr id="row07054481922"><th class="cellrowborder" valign="top" width="29.630000000000003%" id="mcps1.2.4.1.1"><p id="p135155581014"><a name="p135155581014"></a><a name="p135155581014"></a>接口分类</p>
</th>
<th class="cellrowborder" valign="top" width="44.519999999999996%" id="mcps1.2.4.1.2"><p id="p12351955181013"><a name="p12351955181013"></a><a name="p12351955181013"></a>接口名称</p>
</th>
<th class="cellrowborder" valign="top" width="25.85%" id="mcps1.2.4.1.3"><p id="p635165571012"><a name="p635165571012"></a><a name="p635165571012"></a>备注</p>
</th>
</tr>
</thead>
<tbody><tr id="row1870594819210"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1035755151018"><a name="p1035755151018"></a><a name="p1035755151018"></a>基础API &gt; 标量计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p193515557101"><a name="p193515557101"></a><a name="p193515557101"></a>GetBitCount、CountLeadingZero、CountBitsCntSameAsSignBit、GetSFFValue、Cast（float转half、int32_t）、Cast（float转bfloat16_t）、Cast（多类型转float）</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p2351455171020"><a name="p2351455171020"></a><a name="p2351455171020"></a>-</p>
</td>
</tr>
<tr id="row1470617481025"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p18361755191019"><a name="p18361755191019"></a><a name="p18361755191019"></a>基础API &gt; 矢量计算 &gt; 基础算术</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p173685515109"><a name="p173685515109"></a><a name="p173685515109"></a>Exp、Ln、Abs、Reciprocal、Sqrt、Rsqrt、Relu、Add、Sub、Mul、Div、Max、Min、BilinearInterpolation、Adds、Muls、Maxs、Mins、LeakyRelu</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p93614551107"><a name="p93614551107"></a><a name="p93614551107"></a>-</p>
</td>
</tr>
<tr id="row18706148928"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p12361655191017"><a name="p12361655191017"></a><a name="p12361655191017"></a>基础API &gt; 矢量计算 &gt; 逻辑计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p10362055181013"><a name="p10362055181013"></a><a name="p10362055181013"></a>Not、And、Or、ShiftLeft、ShiftRight</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p23615581013"><a name="p23615581013"></a><a name="p23615581013"></a>-</p>
</td>
</tr>
<tr id="row137061481120"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p53695511016"><a name="p53695511016"></a><a name="p53695511016"></a>基础API &gt; 矢量计算 &gt; 复合计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p6361255131014"><a name="p6361255131014"></a><a name="p6361255131014"></a>Axpy、CastDequant、AddRelu、AddReluCast、SubRelu、SubReluCast、MulAddDst、MulCast、FusedMulAdd、MulAddRelu</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p153610555105"><a name="p153610555105"></a><a name="p153610555105"></a>-</p>
</td>
</tr>
<tr id="row070674812215"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p153615521020"><a name="p153615521020"></a><a name="p153615521020"></a>基础API &gt; 矢量计算 &gt; 比较与选择</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p236125551015"><a name="p236125551015"></a><a name="p236125551015"></a>Compare、Compare（结果存入寄存器）、Compares、GetCmpMask、SetCmpMask、GatherMask</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p936555181012"><a name="p936555181012"></a><a name="p936555181012"></a>-</p>
</td>
</tr>
<tr id="row137068483216"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1436955161018"><a name="p1436955161018"></a><a name="p1436955161018"></a>基础API &gt; 矢量计算 &gt; 类型转换</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1536115511017"><a name="p1536115511017"></a><a name="p1536115511017"></a>Cast</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p5365550108"><a name="p5365550108"></a><a name="p5365550108"></a>-</p>
</td>
</tr>
<tr id="row1570618486213"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p33635571010"><a name="p33635571010"></a><a name="p33635571010"></a>基础API &gt; 矢量计算 &gt; 归约计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p103612558107"><a name="p103612558107"></a><a name="p103612558107"></a>ReduceRepeat、ReduceDataBlock、ReducePairElem、GetReduceRepeatSumSpr、GetReduceRepeatMaxMinSpr</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1736195513103"><a name="p1736195513103"></a><a name="p1736195513103"></a>-</p>
</td>
</tr>
<tr id="row13706104817211"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1736135519109"><a name="p1736135519109"></a><a name="p1736135519109"></a>基础API &gt; 矢量计算 &gt; 数据转换</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p9369554103"><a name="p9369554103"></a><a name="p9369554103"></a>Transpose、TransDataTo5HD</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p143612554106"><a name="p143612554106"></a><a name="p143612554106"></a>TransDataTo5HD （不支持dst与src类型为uint64_t的LocalTensor的场景）</p>
</td>
</tr>
<tr id="row670711481724"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p12368557106"><a name="p12368557106"></a><a name="p12368557106"></a>基础API &gt; 矢量计算 &gt; 数据填充</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p136185551010"><a name="p136185551010"></a><a name="p136185551010"></a>Duplicate、Brcb</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p4372055191014"><a name="p4372055191014"></a><a name="p4372055191014"></a>-</p>
</td>
</tr>
<tr id="row137071648520"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p13379558109"><a name="p13379558109"></a><a name="p13379558109"></a>基础API &gt; 矢量计算 &gt; 排序组合</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p133716557102"><a name="p133716557102"></a><a name="p133716557102"></a>Sort32、MrgSort、GetMrgSortResult</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p93717559104"><a name="p93717559104"></a><a name="p93717559104"></a>-</p>
</td>
</tr>
<tr id="row14707648226"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1537155514103"><a name="p1537155514103"></a><a name="p1537155514103"></a>基础API &gt; 矢量计算 &gt; 离散与聚合</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p103765551014"><a name="p103765551014"></a><a name="p103765551014"></a>Gather、Gatherb</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p113725561014"><a name="p113725561014"></a><a name="p113725561014"></a>-</p>
</td>
</tr>
<tr id="row157073482219"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p13795571019"><a name="p13795571019"></a><a name="p13795571019"></a>基础API &gt; 矢量计算 &gt; 掩码操作</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p53785515103"><a name="p53785515103"></a><a name="p53785515103"></a>SetMaskCount、SetMaskNorm、SetVectorMask、ResetMask</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1537105541013"><a name="p1537105541013"></a><a name="p1537105541013"></a>-</p>
</td>
</tr>
<tr id="row1170719484217"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p737125571012"><a name="p737125571012"></a><a name="p737125571012"></a>基础API &gt; 矢量计算 &gt; 量化设置</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p3371755121016"><a name="p3371755121016"></a><a name="p3371755121016"></a>SetDeqScale</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p73765516101"><a name="p73765516101"></a><a name="p73765516101"></a>-</p>
</td>
</tr>
<tr id="row157073481627"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p123775516109"><a name="p123775516109"></a><a name="p123775516109"></a>基础API &gt; 数据搬运 &gt; DataCopy</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p183725521010"><a name="p183725521010"></a><a name="p183725521010"></a>基础数据搬运</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p337175521010"><a name="p337175521010"></a><a name="p337175521010"></a>不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row7708104819220"><td class="cellrowborder" rowspan="4" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p5381155151015"><a name="p5381155151015"></a><a name="p5381155151015"></a>基础API &gt; 数据搬运</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p137125510109"><a name="p137125510109"></a><a name="p137125510109"></a>增强数据搬运</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p19371551101"><a name="p19371551101"></a><a name="p19371551101"></a>不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row7708948725"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p637175591011"><a name="p637175591011"></a><a name="p637175591011"></a>切片数据搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1637355101013"><a name="p1637355101013"></a><a name="p1637355101013"></a>-</p>
</td>
</tr>
<tr id="row57081487219"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p437185512102"><a name="p437185512102"></a><a name="p437185512102"></a>随路转换ND2NZ搬运</p>
<p id="p9371855171017"><a name="p9371855171017"></a><a name="p9371855171017"></a>随路转换NZ2ND搬运</p>
<p id="p13381755131011"><a name="p13381755131011"></a><a name="p13381755131011"></a>随路量化激活搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p938195512100"><a name="p938195512100"></a><a name="p938195512100"></a>随路转换ND2NZ搬运不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row370817486213"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p103895521010"><a name="p103895521010"></a><a name="p103895521010"></a>Copy、DataCopyPad、SetPadValue</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1338185515107"><a name="p1338185515107"></a><a name="p1338185515107"></a>DataCopyPad不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row177081648422"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p113885518108"><a name="p113885518108"></a><a name="p113885518108"></a>基础API &gt; 同步控制 &gt; 核内同步</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1538855191014"><a name="p1538855191014"></a><a name="p1538855191014"></a>SetFlag/WaitFlag、PipeBarrier、DataSyncBarrier</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p838255191017"><a name="p838255191017"></a><a name="p838255191017"></a>-</p>
</td>
</tr>
<tr id="row37087481225"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p143815519101"><a name="p143815519101"></a><a name="p143815519101"></a>基础API &gt; 同步控制 &gt; 核间同步</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p53885510109"><a name="p53885510109"></a><a name="p53885510109"></a>CrossCoreSetFlag、CrossCoreWaitFlag</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p83825511014"><a name="p83825511014"></a><a name="p83825511014"></a>-</p>
</td>
</tr>
<tr id="row1070894819215"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p12388559109"><a name="p12388559109"></a><a name="p12388559109"></a>基础API &gt; 缓存控制</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p103865521014"><a name="p103865521014"></a><a name="p103865521014"></a>DataCachePreload、DataCacheCleanAndInvalid、ICachePreLoad、GetICachePreloadStatus</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p10383551101"><a name="p10383551101"></a><a name="p10383551101"></a>-</p>
</td>
</tr>
<tr id="row87081348428"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p338155141011"><a name="p338155141011"></a><a name="p338155141011"></a>基础API &gt; 系统变量访问</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p938955181019"><a name="p938955181019"></a><a name="p938955181019"></a>GetBlockNum、GetBlockIdx、GetDataBlockSizeInBytes、GetArchVersion、GetTaskRatio、InitSocState、GetProgramCounter、GetSubBlockNum、GetSubBlockIdx、GetSystemCycle、</p>
<p id="p63865551011"><a name="p63865551011"></a><a name="p63865551011"></a>CheckLocalMemoryIA</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p143815557104"><a name="p143815557104"></a><a name="p143815557104"></a>-</p>
</td>
</tr>
<tr id="row1170934815213"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p938105531011"><a name="p938105531011"></a><a name="p938105531011"></a>基础API &gt; 原子操作</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1738145591014"><a name="p1738145591014"></a><a name="p1738145591014"></a>SetAtomicAdd、SetAtomicType、DisableDmaAtomic、SetAtomicMax、SetAtomicMin、SetStoreAtomicConfig、GetStoreAtomicConfig</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p5388559106"><a name="p5388559106"></a><a name="p5388559106"></a>-</p>
</td>
</tr>
<tr id="row67096482218"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p93875511014"><a name="p93875511014"></a><a name="p93875511014"></a>基础API &gt; 矩阵计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p53919552100"><a name="p53919552100"></a><a name="p53919552100"></a>Mmad、MmadWithSparse、SetHF32Mode、SetHF32TransMode、SetMMRowMajor、SetMMColumnMajor、SetFixPipeConfig、SetFixpipeNz2ndFlag、SetFixpipePreQuantFlag、Fill、LoadData、LoadDataWithTranspose、SetAippFunctions、LoadImageToLocal、LoadDataWithSparse、SetFmatrix、SetLoadDataBoundary、SetLoadDataRepeat、SetLoadDataPaddingValue、Fixpipe</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p153985521019"><a name="p153985521019"></a><a name="p153985521019"></a>-</p>
</td>
</tr>
<tr id="row9709184811214"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p9391855101018"><a name="p9391855101018"></a><a name="p9391855101018"></a>Utils API &gt; C++标准库 &gt; 算法</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p163955551011"><a name="p163955551011"></a><a name="p163955551011"></a>max、min、index_sequence</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p53965581013"><a name="p53965581013"></a><a name="p53965581013"></a>-</p>
</td>
</tr>
<tr id="row167094482220"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p14394556105"><a name="p14394556105"></a><a name="p14394556105"></a>Utils API &gt; C++标准库 &gt; 容器函数</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p113912552108"><a name="p113912552108"></a><a name="p113912552108"></a>tuple、get、make_tuple</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p13917555103"><a name="p13917555103"></a><a name="p13917555103"></a>-</p>
</td>
</tr>
<tr id="row97091948726"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p139855171017"><a name="p139855171017"></a><a name="p139855171017"></a>Utils API &gt; C++标准库 &gt; 类型特性</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p739155111015"><a name="p739155111015"></a><a name="p739155111015"></a>is_convertible、is_base_of、is_same、enable_if、conditional</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p73935581010"><a name="p73935581010"></a><a name="p73935581010"></a>-</p>
</td>
</tr>
<tr id="row137091485215"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p914812282549"><a name="p914812282549"></a><a name="p914812282549"></a>高阶API &gt; C++标准库 &gt; 类型特性</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1414882810547"><a name="p1414882810547"></a><a name="p1414882810547"></a>is_convertible、is_base_of、is_same、enable_if、conditional</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1514822875412"><a name="p1514822875412"></a><a name="p1514822875412"></a>-</p>
</td>
</tr>
<tr id="row070934810215"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1642119533450"><a name="p1642119533450"></a><a name="p1642119533450"></a>高阶API &gt; 模板库函数 &gt; type_traits</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p042105320459"><a name="p042105320459"></a><a name="p042105320459"></a>is_convertible、is_base_of、is_same、enable_if、conditional</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p5421553134513"><a name="p5421553134513"></a><a name="p5421553134513"></a>-</p>
</td>
</tr>
</tbody>
</table>
<!-- end id3 -->

<!-- npu="950" id4 -->
**表4**  针对Ascend 950PR/Ascend 950DT，支持的API范围

<a name="table1798673512413"></a>
<table><thead align="left"><tr id="row109864353413"><th class="cellrowborder" valign="top" width="29.630000000000003%" id="mcps1.2.4.1.1"><p id="p1098613524117"><a name="p1098613524117"></a><a name="p1098613524117"></a>接口分类</p>
</th>
<th class="cellrowborder" valign="top" width="44.519999999999996%" id="mcps1.2.4.1.2"><p id="p29871535174117"><a name="p29871535174117"></a><a name="p29871535174117"></a>接口名称</p>
</th>
<th class="cellrowborder" valign="top" width="25.85%" id="mcps1.2.4.1.3"><p id="p9987835154115"><a name="p9987835154115"></a><a name="p9987835154115"></a>备注</p>
</th>
</tr>
</thead>
<tbody><tr id="row1298763513418"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p15987173524114"><a name="p15987173524114"></a><a name="p15987173524114"></a>基础API &gt; 标量计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p12167195363614"><a name="p12167195363614"></a><a name="p12167195363614"></a>GetBitCount、CountLeadingZero、CountBitsCntSameAsSignBit、GetSFFValue、Cast（float转half、int32_t）、Cast（float转bfloat16_t）、Cast（多类型转float）、Nop、GetUintDivMagicAndShift、WriteGmByPassDCache、ReadGmByPassDCache</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p298711352411"><a name="p298711352411"></a><a name="p298711352411"></a>-</p>
</td>
</tr>
<tr id="row1098743584116"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p69871035114113"><a name="p69871035114113"></a><a name="p69871035114113"></a>基础API &gt; 矢量计算 &gt; 基础算术</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1068419974813"><a name="p1068419974813"></a><a name="p1068419974813"></a>Exp、Ln、Abs、Reciprocal、Sqrt、Rsqrt、Relu、Neg、Add、Sub、Mul、Div、Max、Min、BilinearInterpolation、Prelu、Mull、Adds、Adds（灵活标量位置）、Muls、Muls（灵活标量位置）、Maxs、Maxs（灵活标量位置）、Mins、Mins（灵活标量位置）、Subs、Divs、LeakyRelu</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p798711356416"><a name="p798711356416"></a><a name="p798711356416"></a>-</p>
</td>
</tr>
<tr id="row45561731104215"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p35561731164210"><a name="p35561731164210"></a><a name="p35561731164210"></a>基础API &gt; 矢量计算 &gt; 逻辑计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p18556103154212"><a name="p18556103154212"></a><a name="p18556103154212"></a>Not、And、Or、ShiftLeft（左移位数为Tensor）、ShiftRight（右移位数为Tensor）、Ands、Ors、ShiftLeft、ShiftRight</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p35567315428"><a name="p35567315428"></a><a name="p35567315428"></a>-</p>
</td>
</tr>
<tr id="row1798773512412"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p189871935184120"><a name="p189871935184120"></a><a name="p189871935184120"></a>基础API &gt; 矢量计算 &gt; 复合计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p10987133511418"><a name="p10987133511418"></a><a name="p10987133511418"></a>Axpy、CastDequant、AddRelu、AddReluCast、AddDeqRelu、SubRelu、SubReluCast、MulAddDst、MulCast、FusedMulAdd、MulAddRelu、AbsSub、FusedExpSub、MulsCast</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p179879356419"><a name="p179879356419"></a><a name="p179879356419"></a>-</p>
</td>
</tr>
<tr id="row5600192234115"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p7600112274111"><a name="p7600112274111"></a><a name="p7600112274111"></a>基础API &gt; 矢量计算 &gt; 比较与选择</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p16681911154710"><a name="p16681911154710"></a><a name="p16681911154710"></a>Compare、Compares、Compares（灵活标量位置）、GetCmpMask、SetCmpMask、GatherMask、Select、Select（灵活标量位置）</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p46001422164119"><a name="p46001422164119"></a><a name="p46001422164119"></a>Select不支持：tensor高位切分计算->Select模式0和模式2->不传入mask参数接口中的模式0。</p>
</td>
</tr>
<tr id="row998718356411"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p5987335194120"><a name="p5987335194120"></a><a name="p5987335194120"></a>基础API &gt; 矢量计算 &gt; 类型转换</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p198717354419"><a name="p198717354419"></a><a name="p198717354419"></a>Cast、Truncate</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p798719352419"><a name="p798719352419"></a><a name="p798719352419"></a>-</p>
</td>
</tr>
<tr id="row7987193534118"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p898712358414"><a name="p898712358414"></a><a name="p898712358414"></a>基础API &gt; 矢量计算 &gt; 归约计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p675915224145"><a name="p675915224145"></a><a name="p675915224145"></a>ReduceMax、ReduceMin、ReduceRepeat、ReduceDataBlock、ReducePairElem</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1398713564110"><a name="p1398713564110"></a><a name="p1398713564110"></a>-</p>
</td>
</tr>
<tr id="row19871835174112"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p298716354416"><a name="p298716354416"></a><a name="p298716354416"></a>基础API &gt; 矢量计算 &gt; 数据转换</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p79872035154118"><a name="p79872035154118"></a><a name="p79872035154118"></a>Transpose</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p149871335154112"><a name="p149871335154112"></a><a name="p149871335154112"></a>-</p>
</td>
</tr>
<tr id="row162305694316"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p123135644320"><a name="p123135644320"></a><a name="p123135644320"></a>基础API &gt; 矢量计算 &gt; 数据填充</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p48541044144819"><a name="p48541044144819"></a><a name="p48541044144819"></a>Duplicate、Brcb、CreateVecIndex</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p102335634317"><a name="p102335634317"></a><a name="p102335634317"></a>-</p>
</td>
</tr>
<tr id="row7987113516412"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p139871235164110"><a name="p139871235164110"></a><a name="p139871235164110"></a>基础API &gt; 矢量计算 &gt; 排序组合</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p8325152773515"><a name="p8325152773515"></a><a name="p8325152773515"></a>Sort32、MrgSort、GetMrgSortResult</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p159871035134111"><a name="p159871035134111"></a><a name="p159871035134111"></a>-</p>
</td>
</tr>
<tr id="row9987535154118"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p14987163518416"><a name="p14987163518416"></a><a name="p14987163518416"></a>基础API &gt; 矢量计算 &gt; 离散与聚合</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1987163510416"><a name="p1987163510416"></a><a name="p1987163510416"></a>Gather、Gatherb、Scatter</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p169872035144115"><a name="p169872035144115"></a><a name="p169872035144115"></a>-</p>
</td>
</tr>
<tr id="row098783516417"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p139874354419"><a name="p139874354419"></a><a name="p139874354419"></a>基础API &gt; 矢量计算 &gt; 掩码操作</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p99871335184114"><a name="p99871335184114"></a><a name="p99871335184114"></a>SetMaskCount、SetMaskNorm、SetVectorMask、ResetMask</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p18988435204114"><a name="p18988435204114"></a><a name="p18988435204114"></a>-</p>
</td>
</tr>
<tr id="row1198714156411"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1698718951871"><a name="p1698718951871"></a><a name="p1698718951871"></a>基础API &gt; 矢量计算 &gt; 量化设置</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1752718121879"><a name="p1752718121879"></a><a name="p1752718121879"></a>SetDeqScale</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1687112812182"><a name="p1687112812182"></a><a name="p1687112812182"></a>-</p>
</td>
</tr>
<tr id="row098783516418"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p139874354418"><a name="p139874354418"></a><a name="p139874354418"></a>基础API &gt; 矢量计算 &gt; 数据重排</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p99871335184115"><a name="p99871335184115"></a><a name="p99871335184115"></a>Interleave、DeInterleave</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p18988435204115"><a name="p18988435204115"></a><a name="p18988435204115"></a>-</p>
</td>
</tr>
<tr id="row1598863534117"><td class="cellrowborder" rowspan="6" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1998811353419"><a name="p1998811353419"></a><a name="p1998811353419"></a>基础API &gt; 数据搬运 &gt; DataCopy</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p398811358414"><a name="p398811358414"></a><a name="p398811358414"></a>基础数据搬运</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p11988335184112"><a name="p11988335184112"></a><a name="p11988335184112"></a>不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row19881535134113"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1995517203185"><a name="p1995517203185"></a><a name="p1995517203185"></a>增强数据搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p172481414205"><a name="p172481414205"></a><a name="p172481414205"></a>不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row831243535112"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1095612051813"><a name="p1095612051813"></a><a name="p1095612051813"></a>切片数据搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p11956142012183"><a name="p11956142012183"></a><a name="p11956142012183"></a>-</p>
</td>
</tr>
<tr id="row49881535124115"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p179561920111815"><a name="p179561920111815"></a><a name="p179561920111815"></a>随路转换ND2NZ搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p595642041818"><a name="p595642041818"></a><a name="p595642041818"></a>不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row76231552182019"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p11956102019184"><a name="p11956102019184"></a><a name="p11956102019184"></a>随路转换NZ2ND搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1062311525208"><a name="p1062311525208"></a><a name="p1062311525208"></a>-</p>
</td>
</tr>
<tr id="row022720172113"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p7956192001812"><a name="p7956192001812"></a><a name="p7956192001812"></a>随路量化激活搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p13228105216"><a name="p13228105216"></a><a name="p13228105216"></a>-</p>
</td>
</tr>
<tr id="row16988143510417"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p192041749112615"><a name="p192041749112615"></a><a name="p192041749112615"></a>基础API &gt; 数据搬运</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p16956102014187"><a name="p16956102014187"></a><a name="p16956102014187"></a>Copy、DataCopyPad、SetPadValue</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p2702434251"><a name="p2702434251"></a><a name="p2702434251"></a>DataCopyPad不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row6778713162710"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p20882139162713"><a name="p20882139162713"></a><a name="p20882139162713"></a>基础API &gt; 同步控制 &gt; 核内同步</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p2882133915277"><a name="p2882133915277"></a><a name="p2882133915277"></a>SetFlag/WaitFlag、PipeBarrier、DataSyncBarrier</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p8882133922716"><a name="p8882133922716"></a><a name="p8882133922716"></a>-</p>
</td>
</tr>
<tr id="row28501428172718"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p388253912712"><a name="p388253912712"></a><a name="p388253912712"></a>基础API &gt; 同步控制 &gt; 核间同步</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p6882143920272"><a name="p6882143920272"></a><a name="p6882143920272"></a>CrossCoreSetFlag、CrossCoreWaitFlag、SyncAll</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p9882173914275"><a name="p9882173914275"></a><a name="p9882173914275"></a>-</p>
</td>
</tr>
<tr id="row2015642713274"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p288214390273"><a name="p288214390273"></a><a name="p288214390273"></a>基础API &gt; 缓存控制</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1688293992714"><a name="p1688293992714"></a><a name="p1688293992714"></a>DataCachePreload、DataCacheCleanAndInvalid、ICachePreLoad、GetICachePreloadStatus</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1988210392278"><a name="p1988210392278"></a><a name="p1988210392278"></a>-</p>
</td>
</tr>
<tr id="row1444116151276"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p13882639172714"><a name="p13882639172714"></a><a name="p13882639172714"></a>基础API &gt; 系统变量访问</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p988253914278"><a name="p988253914278"></a><a name="p988253914278"></a>GetBlockNum、GetBlockIdx、GetDataBlockSizeInBytes、GetArchVersion、InitSocState、GetSpr、ClearSpr、GetProgramCounter、GetSubBlockNum、GetSubBlockIdx、GetSystemCycle、SetCtrlSpr、GetCtrlSpr、ResetCtrlSpr</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p2882173992716"><a name="p2882173992716"></a><a name="p2882173992716"></a>-</p>
</td>
</tr>
<tr id="row1298811359417"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p10988193511419"><a name="p10988193511419"></a><a name="p10988193511419"></a>基础API &gt; 原子操作</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1479292993514"><a name="p1479292993514"></a><a name="p1479292993514"></a>SetAtomicAdd、SetAtomicType、DisableDmaAtomic、SetAtomicMax、SetAtomicMin、SetStoreAtomicConfig、GetStoreAtomicConfig、AtomicAdd、AtomicMin、AtomicMax、AtomicCas、AtomicExch</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p89881358416"><a name="p89881358416"></a><a name="p89881358416"></a>-</p>
</td>
</tr>
<tr id="row11988153510410"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p4989143554117"><a name="p4989143554117"></a><a name="p4989143554117"></a>基础API &gt; 矩阵计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p398953516417"><a name="p398953516417"></a><a name="p398953516417"></a>Fill、LoadData、LoadDataWithTranspose、LoadDataWithSparse、SetFmatrix、SetLoadDataBoundary、SetLoadDataRepeat、SetLoadDataPaddingValue、Fixpipe、SetFixPipeConfig、SetFixpipeNz2ndFlag、SetFixpipePreQuantFlag、SetAippFunctions、LoadImageToLocal、Mmad、SetHF32Mode、SetHF32TransMode</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p7989123518419"><a name="p7989123518419"></a><a name="p7989123518419"></a>-</p>
</td>
</tr>
<tr id="row2098911359417"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p6989173517418"><a name="p6989173517418"></a><a name="p6989173517418"></a>Utils API &gt; C++标准库 &gt; 算法</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p79891335164117"><a name="p79891335164117"></a><a name="p79891335164117"></a>max、min、index_sequence</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p5989193564117"><a name="p5989193564117"></a><a name="p5989193564117"></a>-</p>
</td>
</tr>
<tr id="row3989143504110"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p119891335204116"><a name="p119891335204116"></a><a name="p119891335204116"></a>Utils API &gt; C++标准库 &gt; 容器函数</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p29891935104116"><a name="p29891935104116"></a><a name="p29891935104116"></a>tuple、get、make_tuple</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p3989113510418"><a name="p3989113510418"></a><a name="p3989113510418"></a>-</p>
</td>
</tr>
<tr id="row59891735184118"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1298918353418"><a name="p1298918353418"></a><a name="p1298918353418"></a>Utils API &gt; C++标准库 &gt; 类型特性</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1698943519414"><a name="p1698943519414"></a><a name="p1698943519414"></a>is_convertible、is_same、enable_if、conditional</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p9989113544116"><a name="p9989113544116"></a><a name="p9989113544116"></a>-</p>
</td>
</tr>
</tbody>
</table>
<!-- end id4 -->

## 使用预留UB空间的API范围<a name="section_reserved_ubuf_api"></a>

以下API内部会使用Ascend C预留的UB空间。当开启[--cce-disable-asc-reserved-ubuf](../../../编译与运行/算子编译/AI-Core算子编译基本用法.md#常用的编译选项)编译选项后，在对应产品版本下调用相关API会触发编译期报错。由于同一API使用预留UB空间的情况在不同NPU架构下有差异，开启该编译选项后，需要手动调整API调用方式或替换为不依赖预留UB空间的实现，才能完成兼容性迁移。

>[!CAUTION]注意
> 同一接口若提供多个重载原型，可能只有部分原型会使用UB预留空间。

<!-- npu="A3,910b" id5 -->
**表5**  针对 [NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md#npu-arch)，使用预留UB空间的API范围

| API类别 | API名 |
| --- | --- |
| 基础API | AddDeqRelu、Select |
| 高阶API | Digamma、Lgamma、Power、Sign、LayerNorm、DropOut、SelectWithBytesMask |
<!-- end id5 -->

<!-- npu="950" id6 -->
**表6**  针对 [NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md#npu-arch)，使用预留UB空间的API范围

| API类别 | API名 |
| --- | --- |
| 基础API > Memory数据搬运 | Copy |
| 基础API > 基础算术 | Exp、Ln、Abs、Reciprocal、Sqrt、Rsqrt、Relu、Add、Sub、Mul、Div、Max、Min、Adds、Muls、Maxs、Mins、LeakyRelu、Subs、Divs |
| 基础API > 逻辑计算 | Not、And、Or、ShiftLeft、ShiftRight、Ands、Ors |
| 基础API > 复合计算 | Axpy、CastDequant、AddRelu、AddReluCast、AddDeqRelu、SubRelu、SubReluCast、MulAddDst、MulCast、FusedMulAdd、MulAddRelu |
| 基础API > 比较与选择 | Compare、Compares、Select、GatherMask |
| 基础API > 类型转换 | Cast |
| 基础API > 归约计算 | ReduceRepeat、ReduceDataBlock、ReducePairElem |
| 基础API > 数据填充 | Duplicate |
| 基础API > Kernel-Tiling | GET_TILING_DATA、GET_TILING_DATA_WITH_STRUCT、GET_TILING_DATA_MEMBER |
| Utils API > 调测接口（SIMD VF） | printf、asc_dump_reg、asc_dump、asc_dump_ubuf |
| 高阶API | AscendAntiQuant、AdjustSoftMaxRes、TopK |
<!-- end id6 -->
