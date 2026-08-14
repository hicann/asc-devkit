# Matmul高阶API开启IBShare模板共享B矩阵数据<a name="ZH-CN_TOPIC_0000002340907802"></a>

## 案例介绍<a name="section17413194624510"></a>

本案例呈现了在矩阵乘算子场景中，使用Matmul高阶API进行矩阵乘法计算，B矩阵开启IBShare对算子性能的提升效果。IBShare功能通过共享L1 Buffer上相同的A矩阵或B矩阵数据，减少重复的MTE2数据搬运开销，提升算子性能。该功能支持A矩阵和B矩阵其中一个矩阵开启IBShare，也支持A矩阵和B矩阵同时开启IBShare。

-   开启IBShare的适用场景

    MIX场景（包含矩阵计算和矢量计算）下，多个AIV的A矩阵或B矩阵GM地址相同，且多个AIV复用的A矩阵或B矩阵在L1 Buffer上全载。

-   开启IBShare的约束条件
    -   A矩阵和B矩阵同时开启IBShare的场景，同一算子中其它Matmul对象的A矩阵和B矩阵也必须同时开启IBShare。
    -   A矩阵和B矩阵同时开启IBShare的场景，获取矩阵计算结果时，只支持调用IterateAll接口，且只支持输出到Global Memory。

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
<tbody><tr id="row217562525215"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p61219167439"><a name="p61219167439"></a><a name="p61219167439"></a>a</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p17124161434"><a name="p17124161434"></a><a name="p17124161434"></a>64, 384</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p312131614432"><a name="p312131614432"></a><a name="p312131614432"></a>float16</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p81213165432"><a name="p81213165432"></a><a name="p81213165432"></a>ND</p>
</td>
</tr>
<tr id="row10176102512525"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p49845284319"><a name="p49845284319"></a><a name="p49845284319"></a>b</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p85970614134"><a name="p85970614134"></a><a name="p85970614134"></a>384, 256</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p6419195512430"><a name="p6419195512430"></a><a name="p6419195512430"></a>float16</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p12420205594320"><a name="p12420205594320"></a><a name="p12420205594320"></a>ND</p>
</td>
</tr>
</tbody>
</table>

当前案例使用的AI处理器共20个核，每个核中包含1个AIC核和2个AIV核。因为输入shape较小，本案例以单核为示例，参考SetDim接口在MIX模式下的使用，在Tiling程序中设置参与运算的核数为2。Tiling参数如下：

-   原始shape：M=64, N= 256, K=384。
-   单核shape：singleCoreM=32，singleCoreN=256，singleCoreK=384。A矩阵拆成两半，一半在AIV0上处理，一半在AIV1上处理；AIV0和AIV1使用的B矩阵数据相同。
-   基本块shape：baseM=32，baseN=256，baseK=64。
-   L1缓存相关Tiling参数：stepM=1，stepN=1，stepKa=6，stepKb=6。

## 获取性能数据<a name="section851404010469"></a>

使用msOpProf工具获取[算子仿真流水图](../../性能分析/获取性能数据.md#section17259539153513)和[上板Profiling](../../性能分析/获取性能数据.md#section17953123893415)数据，因为IBShare功能主要是通过共享L1 Buffer上相同的A矩阵或B矩阵数据，减少重复的MTE2数据搬运开销，所以重点分析MTE2的流水情况。

## 分析主要瓶颈点<a name="section221431704714"></a>

-   优化前的流水图如下，不开启IBShare模板，默认使用的Norm模板。黑框标识AIV0发起的MTE2搬运流水：MTE2总共搬运了12次，其中A矩阵搬运了6次（stepM\*stepKa=6），B矩阵搬运了6次（stepN\*stepKb=6）。红框标识的AIV1发起的MTE2搬运流水，跟AIV0基本一致。在该案例中，因为AIV1使用的B矩阵跟AIV0使用的B矩阵数据相同，且singleCoreN=baseN\*stepN，singleCoreK=baseK\*stepKb，即B矩阵可以在L1全载。考虑在AIV0搬入B矩阵到L1 Buffer后，将B矩阵数据缓存在L1 Buffer上等待AIV1进行复用，进而节省B矩阵的MTE2重复搬运开销。

    ![](../../../figures/zh-cn_image_0000002404199037.png)

-   优化前的Profiling数据如下，C列的aic\_time是10.29us，K列的aic\_mte2\_time是5.56us。

    ![](../../../figures/zh-cn_image_0000002411076626.png)

## 设计优化方案<a name="section1875873847"></a>

下图是不开启IBShare模板（默认使用Norm模板）的Matmul计算流水示意图。MTE2分多次从Global Memory搬运基本块到L1 Buffer（A1或B1），即使前后两次搬运的B矩阵基本块数据是相同的数据，也会重复搬运。

**图1**  不开启IBShare模板的Matmul流水示意图<a name="fig1723213921319"></a>  
![](../../../figures/no_ibshare_mm.png "不开启IBShare模板的Matmul流水示意图")

下图是开启IBShare模板的Matmul计算流水示意图。MTE2分多次从Global Memory搬运基本块到L1 Buffer（A1或B1），若前后两次搬运的B矩阵基本块数据相同，不会重复搬运，第一次搬运到B1内的数据会被复用。

**图2**  开启IBShare模板的Matmul流水示意图<a name="fig10881182610139"></a>  
![](../../../figures/ibshare_mm.png "开启IBShare模板的Matmul流水示意图")

Matmul API开启IBShare模板共享B矩阵的完整样例请参考[仅B矩阵开启IBShare样例](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_ibshareB)。开启IBShare功能的主要步骤如下：

1.  创建Matmul对象。

    ```
    #define ASCENDC_CUBE_ONLY
    #include "lib/matmul_intf.h"
    
    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType>;
    using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType, false, LayoutMode::NONE, true>; // 设置B矩阵的IBSHARE参数为true
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
    using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_IBSHARE_NORM> matmulObj; // 使用默认的IBShare模板参数CFG_IBSHARE_NORM定义Matmul对象
    ```

## 验证优化方案性能收益<a name="section19022397498"></a>

-   优化后的流水图如下，黑框标识的AIV0发起的MTE2搬运流水，与优化前一致。红框标识的AIV1发起的MTE2搬运流水，相较于优化前的A矩阵和B矩阵一共12次MTE2数据搬运，减少到了仅6次A矩阵的MTE2数据搬运，省去了B矩阵的6次MTE2数据搬运开销。

    ![](../../../figures/zh-cn_image_0000002404200417.png)

-   优化后的Profiling数据如下，C列的aic\_time是9.93us，较优化前的10.29us提升了3.55%。K列的aic\_mte2\_time是4.71us，较优化前的5.56us提升了15.46%。

    ![](../../../figures/zh-cn_image_0000002444817149.png)

## 总结<a name="section8281219125011"></a>

MIX场景（包含矩阵计算和矢量计算）下，若多个AIV的A矩阵或B矩阵GM地址相同，且多个AIV复用的A矩阵/B矩阵在L1 Buffer上全载。可以考虑开启IBShare模板，通过共享L1 Buffer上相同的A矩阵或B矩阵数据，减少重复的MTE2数据搬运开销，提升算子性能。
