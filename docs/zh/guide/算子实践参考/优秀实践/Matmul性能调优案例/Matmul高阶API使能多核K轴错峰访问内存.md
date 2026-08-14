# Matmul高阶API开启多核K轴错峰访问内存<a name="ZH-CN_TOPIC_0000002374905793"></a>

## 案例介绍<a name="section579816421564"></a>

本案例呈现在矩阵乘算子场景中，使用Matmul高阶API进行矩阵乘法计算，开启多核K轴错峰访问Device内存对算子性能的提升效果。在多核并行执行Matmul计算时，如果输入矩阵A或B的内存位置位于GM，并且参与多核计算的矩阵相同，那么将出现多核同时访问相同GM地址的情况，导致地址访问冲突，从而影响算子性能。若开启多核K轴错峰访问Device内存，切分的矩阵K轴方向对应的不同核将尽量从不同的GM起始地址开始访问和搬运数据，缓解地址访问冲突，提升算子性能。

**图1**  访问地址冲突示意图<a name="fig7682191511128"></a>  
![](../../../figures/addr_conflict.png "访问地址冲突示意图")

**图2**  缓解地址冲突示意图<a name="fig1499117817383"></a>  
![](../../../figures/ease_addr_conflict.png "缓解地址冲突示意图")

-   开启多核K轴错峰访问内存的适用场景：

    多核执行Matmul，且输入矩阵的K轴较大。

-   开启多核K轴错峰访问内存的约束条件：
    -   输入矩阵的K轴非全载，K轴非全载即矩阵的K方向数据不能同时搬入及保持在L1 Buffer中。
    -   仅支持MDL模板。
    -   在多核上执行Matmul计算。
    -   A矩阵或B矩阵的内存位置位于GM。

本案例的算子规格如下：

**表1**  算子用例规格

<a name="table15465191317123"></a>
<table><thead align="left"><tr id="row184651013131217"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p24653132122"><a name="p24653132122"></a><a name="p24653132122"></a>输入</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p13465111311213"><a name="p13465111311213"></a><a name="p13465111311213"></a>Shape</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p14465171371212"><a name="p14465171371212"></a><a name="p14465171371212"></a>Data type</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p74651713141213"><a name="p74651713141213"></a><a name="p74651713141213"></a>Format</p>
</th>
</tr>
</thead>
<tbody><tr id="row446561351212"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p517515253529"><a name="p517515253529"></a><a name="p517515253529"></a>a</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p141751425165213"><a name="p141751425165213"></a><a name="p141751425165213"></a>768, 6144</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1517522515218"><a name="p1517522515218"></a><a name="p1517522515218"></a>float16</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p16176725105210"><a name="p16176725105210"></a><a name="p16176725105210"></a>ND</p>
</td>
</tr>
<tr id="row44651313101220"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p161761325185216"><a name="p161761325185216"></a><a name="p161761325185216"></a>b</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p4176112555211"><a name="p4176112555211"></a><a name="p4176112555211"></a>6144, 2048</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1176132515212"><a name="p1176132515212"></a><a name="p1176132515212"></a>float16</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p817692575212"><a name="p817692575212"></a><a name="p817692575212"></a>ND</p>
</td>
</tr>
</tbody>
</table>

## 获取性能数据<a name="section851404010469"></a>

使用msOpProf工具获取[算子仿真流水图](../../性能分析/获取性能数据.md#section17259539153513)和[上板Profiling](../../性能分析/获取性能数据.md#section17953123893415)数据，重点分析MTE2的流水。

## 分析主要瓶颈点<a name="section013514116370"></a>

优化前的Profiling数据（PipeUtilization.csv）如下所示，aic\_mte2\_ratio平均达到0.93，MTE2在算子整体执行时长中占比较高，算子当前为MTE2 Bound。本案例中，矩阵按M和N方向切分，单核shape\[singleCoreM，singleCoreN，singleCoreK\]为\[128, 512, 6144\]，基本块shape\[baseM，baseN，baseK\]为\[128, 256, 64\]，每次加载A矩阵的数据时，多核有概率同时访问同一GM地址，引发地址冲突，导致MTE2搬运效率降低，MTE2执行耗时增加。

![](../../../figures/zh-cn_image_0000002414322380.png)

MTE2的搬运效率还可以通过查看其带宽利用率进行验证，如下图所示，通过分析Memory.csv，发现MTE2平均带宽利用率只有34.4%。

![](../../../figures/zh-cn_image_0000002414480828.png)

查看OpBasicInfo.csv文件，优化前算子整体耗时为98.72us。

## 设计优化方案<a name="section33901368431"></a>

开启K轴错峰访问内存：在创建Matmul对象时，将MatmulConfig中的enableKdimReorderLoad参数设置为true。enableKdimReorderLoad参数的详细介绍请参考[MatmulConfig](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulConfig.md)。

开启K轴错峰访问内存的完整样例请参考[K轴错峰加载数据的算子样例](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_k_reorder_load)。开启该功能的主要步骤如下：

1.  配置MDL模板参数，将其中的enableKdimReorderLoad参数设置为true，开启多核K轴错峰访问Device内存。

    ```
    constexpr MatmulConfig GetMDLKDimReorderConfig()
    {
        auto CFG = CFG_MDL;
        CFG.enableKdimReorderLoad = true;
        return CFG;
    }
    constexpr static MatmulConfig MM_CFG = GetMDLKDimReorderConfig();
    ```

2.  基于自定义的MatmulConfig模板参数，创建Matmul对象。

    ```
    AscendC::Matmul<AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, aType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, bType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, cType>,
        AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, biasType>, MM_CFG> matmulObj;
    ```

## 验证优化方案性能收益<a name="section19022397498"></a>

算子Tiling参数不变，优化后的Profiling数据（PipeUtilization.csv）如下所示。可以看到，MTE2耗时显著降低，MTE2的平均耗时从90us降低到69.87us，最大耗时从91.94us降低到75.82us。

![](../../../figures/zh-cn_image_0000002448122873.png)

MTE2的带宽利用率（Memory.csv）如下所示，平均带宽利用率提升到41.7%。

![](../../../figures/zh-cn_image_0000002414329628.png)

查看OpBasicInfo.csv文件，优化后算子整体耗时为85.68us，耗时从98.72us降低到85.68us，性能提升13.2%。

## 总结<a name="section8281219125011"></a>

在多核执行Matmul的场景，当输入矩阵K轴较大（一般大于4096）时，可以尝试使用MDL模板并开启K轴错峰访问内存的功能，缓解地址访问冲突，提升MTE2搬运效率，进而优化算子性能。
