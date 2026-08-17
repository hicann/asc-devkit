# Matmul高阶API开启UnitFlag<a name="ZH-CN_TOPIC_0000002353876489"></a>

## 案例介绍<a name="section17413194624510"></a>

本案例呈现了在矩阵乘算子场景中，使用Matmul高阶API进行矩阵乘法计算，开启UnitFlag功能对算子性能的提升效果。UnitFlag功能为AIC核中MMAD计算指令和FIXPIPE数据搬运指令提供了基于内存访问的细粒度同步，使计算与搬运流水并行。开启UnitFlag功能的方式为将MatmulConfig中的enUnitFlag参数设置为true。enUnitFlag参数的详细介绍请参考[MatmulConfig](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulConfig.md)。

-   开启UnitFlag的适用场景

    算子的MMAD流水和FIXPIPE流水之间串行执行，FIXPIPE等待MMAD计算完成才搬出结果，这个指令同步等待的时间在算子整体执行耗时中占比较高。这种场景可以开启UnitFlag功能，以获得MMAD和FIXPIPE流水并行的性能收益。如果算子原本的MMAD、FIXPIPE流水可以被其他流水掩盖（比如MTE2 Bound），这时开启UnitFlag功能总体收益很小。

-   开启UnitFlag的约束条件
    -   UnitFlag功能仅支持Norm、IBShare、MDL三个模板。
    -   开启UnitFlag功能时，不支持算子内同时存在L0C Buffer（CO1）搬出到Global Memory和L1 Buffer（A1）搬出到Global Memory的两种流水。
    -   开启UnitFlag功能时，若同时开启[L0C累加](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Iterate.md)功能，不支持多次[Iterate](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Iterate.md)计算、一次[GetTensorC](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetTensorC.md)输出。

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
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p141751425165213"><a name="p141751425165213"></a><a name="p141751425165213"></a>128, 64</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1517522515218"><a name="p1517522515218"></a><a name="p1517522515218"></a>float16</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p16176725105210"><a name="p16176725105210"></a><a name="p16176725105210"></a>ND</p>
</td>
</tr>
<tr id="row226312045217"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p161761325185216"><a name="p161761325185216"></a><a name="p161761325185216"></a>b</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p4176112555211"><a name="p4176112555211"></a><a name="p4176112555211"></a>64, 30720</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1176132515212"><a name="p1176132515212"></a><a name="p1176132515212"></a>float16</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p817692575212"><a name="p817692575212"></a><a name="p817692575212"></a>ND</p>
</td>
</tr>
</tbody>
</table>

当前案例使用的AI处理器共20个核，每个核包含1个AIC核和2个AIV核。

算子的Tiling参数如下：

-   原始shape：M=128, N=30720, K=64。
-   单核shape：按20个AIC核进行切分，singleCoreM=128，singleCoreN=1536，singleCoreK=64。

    对于B矩阵，沿着N轴进行切分，切分成20份singleCoreN，单核上处理K \* SingleCoreN大小的数据。对于A矩阵，M轴不进行切分即singleCoreM=M，单核上处理singleCoreM \* K大小的数据。总共20个核参与计算。

-   基本块shape：baseM=128，baseN=256，baseK=64。
-   L1相关Tiling参数：stepM=1，stepN=1，stepKa=4，stepKb=4，depthA1=8，depthB1=8。

## 获取性能数据<a name="section851404010469"></a>

使用msOpProf工具获取[算子仿真流水图](../../performance_analysis/collect_performance_data.md#section17259539153513)和[上板Profiling](../../performance_analysis/collect_performance_data.md#section17953123893415)数据。因为UnitFlag功能主要优化MMAD和FIXPIPE流水串行问题，所以获取性能数据后重点分析Cube、FIXPIPE的流水情况。

## 分析主要瓶颈点<a name="section221431704714"></a>

-   优化前的流水图如下。如下图中红框所示，每一轮MMAD计算流水和FIXPIPE数据搬出流水之间都是串行执行的，完成MMAD计算后才开始FIXPIPE数据搬出，考虑实现MMAD与FIXPIPE之间流水并行来优化算子性能。

    ![](../../../figures/zh-cn_image_0000002413944550.png)

-   优化前的Profiling数据如下，从C列的aic\_time数据可以看出，多个核中最大算子执行耗时为37.39us。

    ![](../../../figures/zh-cn_image_0000002414101622.png)

## 设计优化方案<a name="section990382124919"></a>

如下图所示，未开启UnitFlag功能时，MMAD和FIXPIPE是指令级别的同步，FIXPIPE指令需要等MMAD指令执行完成才进行结果搬出，MMAD和FIXPIPE之间流水串行。

**图1**  未开启UnitFlag功能<a name="fig108452036134412"></a>  
![](../../../figures/no_unitflag.png "未开启UnitFlag功能")

如下图所示，开启UnitFlag功能时，MMAD和FIXPIPE指令是512B大小的细粒度同步。在一条MMAD指令执行过程中，每当完成一个512B数据结果的计算，FIXPIPE立即开始搬出该512B的数据，从而实现MMAD和FIXPIPE之间的流水并行，提升算子性能。

**图2**  开启UnitFlag功能<a name="fig58335714416"></a>  
![](../../../figures/unitflag.png "开启UnitFlag功能")

Matmul API开启UnitFlag功能的完整样例请参考[开启UnitFlag功能和MDL模板的Matmul样例](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_unitflag)。开启UnitFlag功能的主要步骤如下：

1.  自定义MatmulConfig模板参数，将其中的enUnitFlag参数设置为true，开启UnitFlag功能。

    ```
    __aicore__ inline constexpr MatmulConfig GetCustomMDLCFG()
    {
        auto mmCfg = CFG_MDL;
        mmCfg.enUnitFlag = true;
        return mmCfg;
    }
    constexpr static MatmulConfig CUSTOM_CFG_MDL = GetCustomMDLCFG();
    ```

2.  基于自定义的MatmulConfig模板参数，创建Matmul对象。

    ```
    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType>;
    using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType>;
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
    using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CUSTOM_CFG_MDL > matmulObj;
    ```

## 验证优化方案性能收益<a name="section19022397498"></a>

-   优化后的流水图如下，MMAD计算流水和FIXPIPE数据搬出流水之间实现了流水并行。

    ![](../../../figures/zh-cn_image_0000002413945226.png)

-   优化后的Profiling数据如下，从C列的aic\_time数据可以看出，多个核中最大算子执行耗时为34.66us，较优化前的37.39us有约7.3%的性能提升。

    ![](../../../figures/zh-cn_image_0000002447501485.png)

## 总结<a name="section8281219125011"></a>

在算子的MMAD计算流水和FIXPIPE数据搬出流水串行且未被其他流水掩盖（比如MTE2 Bound）时，考虑开启UnitFlag功能，实现MMAD计算流水和FIXPIPE数据搬出流水的流水并行，提升算子性能。
