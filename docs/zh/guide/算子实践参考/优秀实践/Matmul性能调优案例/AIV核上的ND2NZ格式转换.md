# AIV核上的ND2NZ格式转换<a name="ZH-CN_TOPIC_0000002340907814"></a>

## 案例介绍<a name="section17413194624510"></a>

本案例展示了在矩阵乘算子场景中，使用Matmul高阶API进行计算，对内轴（内轴即矩阵的行方向）非256字节对齐的输入矩阵，在AIV核上进行ND2NZ格式转换对算子性能提升的效果。为提升Cube单元的计算效率，ND格式的输入矩阵在执行Cube计算前会先转换为NZ格式，ND格式和NZ格式的具体内容可参考[数据格式](../../SIMD算子实现/矩阵编程（高阶API）/基础知识.md#zh-cn_topic_0000001622194138_section1453415011)。Matmul API内部使用随路ND2NZ指令同时进行格式转换以及数据搬运。但在数据非256字节对齐时，[随路ND2NZ指令](../../../../api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（GMToL1随路转换-ND2NZ搬运）.md)存在带宽利用率低的问题。因此输入矩阵的内轴非256字节对齐时，在进行Matmul计算前，利用AIV核上Vector计算单元完成ND格式到NZ格式的转换，可以避免随路非对齐数据搬运存在的效率低的问题，从而提升算子性能。

-   AIV核上的ND2NZ格式转换的适用场景

    输入矩阵内轴非256字节对齐，且数据量较大影响随路格式转换的效率。

本案例的算子规格如下：

**表1**  算子规格

<a name="table101751125175213"></a>
<table><thead align="left"><tr id="row8175525185219"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p1417582516529"><a name="p1417582516529"></a><a name="p1417582516529"></a>输入</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p417532575212"><a name="p417532575212"></a><a name="p417532575212"></a>Shape</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p1017582585214"><a name="p1017582585214"></a><a name="p1017582585214"></a>Data type</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p317502512522"><a name="p317502512522"></a><a name="p317502512522"></a>Format</p>
</th>
</tr>
</thead>
<tbody><tr id="row217562525215"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p517515253529"><a name="p517515253529"></a><a name="p517515253529"></a>a</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p141751425165213"><a name="p141751425165213"></a><a name="p141751425165213"></a>1024, 1024</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1517522515218"><a name="p1517522515218"></a><a name="p1517522515218"></a>float16</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p16176725105210"><a name="p16176725105210"></a><a name="p16176725105210"></a>ND</p>
</td>
</tr>
<tr id="row10176102512525"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p161761325185216"><a name="p161761325185216"></a><a name="p161761325185216"></a>b</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p4176112555211"><a name="p4176112555211"></a><a name="p4176112555211"></a>1024, 4095</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1176132515212"><a name="p1176132515212"></a><a name="p1176132515212"></a>float16</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p817692575212"><a name="p817692575212"></a><a name="p817692575212"></a>ND</p>
</td>
</tr>
</tbody>
</table>

当前案例使用的AI处理器共24个核，算子中开启高阶API Matmul的纯Cube模式。使用MDL模板，Tiling参数如下：

-   原始shape：M=1024, N= 4095, K=1024。
-   单核shape：singleCoreM=128，singleCoreN=1408，singleCoreK=1024。
-   基本块shape：baseM=128，baseN=256，baseK=64。
-   L1缓存相关Tiling参数：stepM=1，stepN=1，stepKa=4，stepKb=4。

## 获取性能数据<a name="section851404010469"></a>

使用msOpProf工具获取[算子仿真流水图](../../性能分析/获取性能数据.md#section17259539153513)和[上板Profiling](../../性能分析/获取性能数据.md#section17953123893415)数据，重点分析MTE2的流水。

## 分析主要瓶颈点<a name="section221431704714"></a>

-   优化前的Cube流水图如下，由于使用了随路ND2NZ指令，在MTE2数据搬运过程中进行数据格式的转换，导致MTE2整体占比较高。![](../../../figures/zh-cn_image_0000002409729950.png)
-   优化前的Profiling数据如下，可以看到只使用Cube单元执行计算，aic\_time最大耗时149.04us，其中aic\_mte2\_ratio占比很高。![](../../../figures/zh-cn_image_0000002381161446.png)

## 设计优化方案<a name="section33901368431"></a>

对于ND格式的输入矩阵，不再使用随路ND2NZ指令进行格式转换，而是利用Vector计算单元的能力完成数据格式转换。首先使用DataCopyPad接口，将非对齐的矩阵数据搬入Unified Buffer，使用Duplicate接口填充需要补为对齐位置的数据，再逐行调用Copy接口实现数据从ND到NZ格式的重排，将重排后的NZ数据写入workspace内存，最后直接读取workspace上的NZ数据，进行Matmul计算。

实现AIV核上的ND2NZ格式转换的主要步骤如下：

1.  创建Matmul对象时，定义内轴非256字节对齐的B矩阵的Format为NZ格式。

    ```
    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, ATYPE, true>;
    // 使用CubeFormat::NZ定义矩阵B的类型信息
    using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::NZ, BType, true>;
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
    using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MDL> matmulObj;
    ```

2.  利用Vector计算单元实现ND2NZ格式转换。如下代码中MatrixBtoNZ为将B矩阵的ND格式转换为NZ格式的函数，该函数的具体实现请参考完整样例代码。

    ```
    // Vector ND2NZ
    if ASCEND_IS_AIV {
        pipe->InitBuffer(ubBuf, TOTAL_UB_SIZE);
        MatrixBtoNZ<typename B_TYPE::T>(tempGM, bGMNZ, tiling, isTransB, ubBuf, tiling.baseK,
            tiling.baseN); // ND2NZ格式转换函数
        SyncAll();
        // CV SYNC
        NotifyEvent<PIPE_MTE3>(4);
        return;
    }
    if ASCEND_IS_AIC {
        WaitEvent(4); // 等待Vector完成ND2NZ格式转换
    }
    ```

3.  设置左矩阵A、右矩阵B、Bias，完成矩阵乘操作。

    ```
    matmulObj.SetTail(tailM, tailN, shapes.k);
    matmulObj.SetTensorA(aGlobal, false);
    matmulObj.SetTensorB(bGlobal, false);
    if (shapes.isBias) {
        matmulObj.SetBias(biasGlobal);
    }
    matmulObj.IterateAll(cGlobal);
    ```

## 验证优化方案性能收益<a name="section19022397498"></a>

-   优化后的Vector流水图如下所示，利用Vector计算单元的能力，完成B矩阵的数据格式转换。![](../../../figures/zh-cn_image_0000002409891270.png)
-   优化后的Cube流水图如下所示，不使用随路ND2NZ指令对B矩阵进行格式转换后，MTE2的占比明显下降。![](../../../figures/zh-cn_image_0000002443251985.png)
-   优化后的Profiling数据如下，可以看到同时使用Cube单元和Vector单元，aic\_time最大耗时90.95us，其中aic\_mte2\_ratio占比明显降低。![](../../../figures/zh-cn_image_0000002381162386.png)![](../../../figures/zh-cn_image_0000002414561861.png)

**表2**  端到端性能对比

<a name="table135810433437"></a>
<table><thead align="left"><tr id="row65820433437"><th class="cellrowborder" valign="top" width="22.61%" id="mcps1.2.5.1.1"><p id="p75827438435"><a name="p75827438435"></a><a name="p75827438435"></a>优化方法</p>
</th>
<th class="cellrowborder" valign="top" width="21.88%" id="mcps1.2.5.1.2"><p id="p3582204311439"><a name="p3582204311439"></a><a name="p3582204311439"></a>总耗时(us)</p>
</th>
<th class="cellrowborder" valign="top" width="25.729999999999997%" id="mcps1.2.5.1.3"><p id="p16582243124317"><a name="p16582243124317"></a><a name="p16582243124317"></a>AIC_MTE2平均耗时(us)</p>
</th>
<th class="cellrowborder" valign="top" width="29.78%" id="mcps1.2.5.1.4"><p id="p15582144314316"><a name="p15582144314316"></a><a name="p15582144314316"></a>AIV_MTE2平均耗时(us)</p>
</th>
</tr>
</thead>
<tbody><tr id="row65821543124312"><td class="cellrowborder" valign="top" width="22.61%" headers="mcps1.2.5.1.1 "><p id="p185821343184318"><a name="p185821343184318"></a><a name="p185821343184318"></a>随路ND2NZ</p>
</td>
<td class="cellrowborder" valign="top" width="21.88%" headers="mcps1.2.5.1.2 "><p id="p4582743164312"><a name="p4582743164312"></a><a name="p4582743164312"></a>149.82</p>
</td>
<td class="cellrowborder" valign="top" width="25.729999999999997%" headers="mcps1.2.5.1.3 "><p id="p0582194314314"><a name="p0582194314314"></a><a name="p0582194314314"></a>130.77</p>
</td>
<td class="cellrowborder" valign="top" width="29.78%" headers="mcps1.2.5.1.4 "><p id="p1958224374312"><a name="p1958224374312"></a><a name="p1958224374312"></a>0</p>
</td>
</tr>
<tr id="row155821743154311"><td class="cellrowborder" valign="top" width="22.61%" headers="mcps1.2.5.1.1 "><p id="p1458210436437"><a name="p1458210436437"></a><a name="p1458210436437"></a>Vector侧ND2NZ</p>
</td>
<td class="cellrowborder" valign="top" width="21.88%" headers="mcps1.2.5.1.2 "><p id="p758284324319"><a name="p758284324319"></a><a name="p758284324319"></a>93.76</p>
</td>
<td class="cellrowborder" valign="top" width="25.729999999999997%" headers="mcps1.2.5.1.3 "><p id="p1658234344311"><a name="p1658234344311"></a><a name="p1658234344311"></a>22.85</p>
</td>
<td class="cellrowborder" valign="top" width="29.78%" headers="mcps1.2.5.1.4 "><p id="p4582134310436"><a name="p4582134310436"></a><a name="p4582134310436"></a>10.31</p>
</td>
</tr>
</tbody>
</table>

从上表中执行时间的对比，可以看出：不使用随路ND2NZ指令后，总耗时大幅下降，端到端性能提升明显。

## 总结<a name="section8281219125011"></a>

对于矩阵乘计算中矩阵内轴非256字节对齐的场景，随路ND2NZ指令的带宽利用率低，影响算子性能，通过在AIV核上进行ND2NZ的数据重排，提升算子整体性能。值得注意的是，带宽利用率与数据量有关，如果矩阵数据总量太小，即使是在AIV核上进行的ND2NZ转换也无法明显提升有效带宽，反而会因为引入了多核同步，导致算子端到端的性能劣化。
