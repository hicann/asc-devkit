# Matmul高阶API开启MTE2 Preload<a name="ZH-CN_TOPIC_0000002360914053"></a>

## 案例介绍<a name="section17413194624510"></a>

本案例呈现了在矩阵乘算子场景中，使用Matmul高阶API进行矩阵乘法计算，开启MTE2 Preload对算子性能的提升效果。通过MatmulConfig中的doMTE2Preload参数开启矩阵M或N方向的预加载功能，预加载即在MTE2间隙提前加载A矩阵/B矩阵数据，开启预加载功能后，可以减少MTE2间隙，提升算子性能。doMTE2Preload参数的详细介绍请参考[MatmulConfig](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulConfig.md)。

-   开启MTE2 Preload的适用场景

    MTE2流水间隙较大，且M或N数值较大时。

-   开启MTE2 Preload的约束条件
    -   仅在使用MDL模板和SpecialMDL模板时，MTE2 Preload有效。
    -   开启M或N方向预加载功能时，需保证K方向数据全载，且M或N方向开启DoubleBuffer。
    -   K方向数据全载的条件是singleK <= baseK \* stepK。
    -   M方向开启DoubleBuffer的条件是depthA1 = stepM \* stepK \* 2。
    -   N方向开启DoubleBuffer的条件是depthB1 = stepN \* stepK \* 2。

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
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p141751425165213"><a name="p141751425165213"></a><a name="p141751425165213"></a>128, 512</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1517522515218"><a name="p1517522515218"></a><a name="p1517522515218"></a>float16</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p16176725105210"><a name="p16176725105210"></a><a name="p16176725105210"></a>ND</p>
</td>
</tr>
<tr id="row10176102512525"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p161761325185216"><a name="p161761325185216"></a><a name="p161761325185216"></a>b</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p4176112555211"><a name="p4176112555211"></a><a name="p4176112555211"></a>512, 24576</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1176132515212"><a name="p1176132515212"></a><a name="p1176132515212"></a>float16</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p817692575212"><a name="p817692575212"></a><a name="p817692575212"></a>ND</p>
</td>
</tr>
</tbody>
</table>

当前案例使用的AI处理器共24个核，算子中开启高阶API Matmul的纯Cube模式。使用MDL模板，Tiling参数如下：

-   原始shape：M=128, N= 24576, K=512。
-   单核shape：singleCoreM=128，singleCoreN=1024，singleCoreK=512。
-   基本块shape：baseM=128，baseN=128，baseK=64。
-   L1缓存相关Tiling参数：stepM=1，stepN=1，stepKa=8，stepKb=8，depthA1=8，depthB1=16。

## 获取性能数据<a name="section851404010469"></a>

使用msOpProf工具获取[算子仿真流水图](../../performance_analysis/collect_performance_data.md#section17259539153513)和[上板Profiling](../../performance_analysis/collect_performance_data.md#section17953123893415)数据，重点分析Cube，Fixpipe的流水情况。

## 分析主要瓶颈点<a name="section221431704714"></a>

-   优化前的流水图如下，M和K方向全载，因此A矩阵只搬运一次。由于N较大，B矩阵会搬运多次，可以看到单次MTE2间存在间隙。![](../../../figures/zh-cn_image_0000002394102925.png)
-   优化前的Profiling数据如下，aic\_time平均耗时30.88us。![](../../../figures/zh-cn_image_0000002394115169.png)

## 设计优化方案<a name="section33901368431"></a>

开启MTE2 Preload功能：在创建Matmul对象时，开启doMTE2Preload开关。开启MTE2 Preload的完整样例请参考[M方向预加载Matmul算子样例](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_preload)。具体步骤如下：

1.  配置MDL模板参数，将其中的doMTE2Preload参数设置为2，开启N方向Preload功能。

    ```
     // preloadMode = 2
    static constexpr MatmulConfig MM_CFG = GetMDLConfig(false, false, preloadMode); 
    ```

2.  基于自定义MatmulConfig模板参数，创建Matmul对象。

    ```
    AscendC::Matmul<AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, aType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, bType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, cType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, biasType>, MM_CFG> matmulObj;
    ```

## 验证优化方案性能收益<a name="section19022397498"></a>

-   优化后的流水图如下，Tiling参数不变，可以看到，下一次计算使用的B矩阵数据提前加载，MTE2间的间隙缩短。![](../../../figures/zh-cn_image_0000002394160225.png)
-   优化后的Profiling数据如下，aic\_time平均耗时28.50us，较优化前的30.88us有所下降。![](../../../figures/zh-cn_image_0000002394120757.png)

## 总结<a name="section8281219125011"></a>

当MTE2流水间隙较大，且M或N数值较大时，可以考虑开启MTE2 Preload功能，提前加载A矩阵或B矩阵数据。
