# Matmul高阶API开启Tiling全量常量化<a name="ZH-CN_TOPIC_0000002374865629"></a>

## 案例介绍<a name="section12231144316533"></a>

本案例呈现了在使用Matmul高阶API进行矩阵乘法计算时，开启Matmul Tiling全量常量化对算子性能的提升效果。Matmul API在初始化和迭代过程中有大量Scalar计算，Matmul初始化时的Scalar计算影响指令头开销，Matmul迭代间的Scalar计算可能阻塞MTE2流水。在调用Matmul API实现矩阵乘法时，使用[MatmulApiStaticTiling](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Matmul_template_params.md)参数替代TCubeTiling变量参数，将Scalar计算提前到编译期进行，以减少运行时的Scalar计算开销，实现算子性能的提升。

-   Matmul Tiling常量化的适用场景：
    -   Matmul初始化时的Scalar计算较多，影响指令头开销。
    -   Matmul迭代之间的Scalar计算较多，阻塞MTE2流水。

-   Matmul Tiling常量化需要在编译期确定部分Tiling参数，根据确定参数的不同，分为全量常量化和部分常量化两种场景，使用Matmul Tiling常量化需要满足两种场景中任一场景的条件：

    -   <a name="li159920538152"></a>全量常量化：能够确定常量singleCore Shape（singleCoreM/singleCoreN/singleCoreK）和常量base Shape（basicM/basicN/basicK，也称baseM/baseN/baseK）。

    -   <a name="li98655581816"></a>部分常量化：能够确定常量base Shape（basicM/basicN/basicK，也称baseM/baseN/baseK）。

    其中，全量常量化场景比部分常量化场景可以减少更多的Scalar计算开销。

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
<tr id="row10176102512525"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p161761325185216"><a name="p161761325185216"></a><a name="p161761325185216"></a>b</p>
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

当前案例使用的AI处理器共24个核，每个核中包含1个AIC核和2个AIV核。

Tiling参数如下：

-   原始shape：M=128, N=30720, K=64。
-   单核shape：按24个AIC核进行切分，singleCoreM=128，singleCoreN=1280，singleCoreK=64。

    对于B矩阵，沿着N轴进行切分，切分成24份的singleCoreN，单核上处理K \* singleCoreN大小的数据。对于A矩阵，M轴不进行切分即singleCoreM=M，单核上处理singleCoreM \* K大小的数据。总共24个核参与计算。

-   基本块shape：baseM=128，baseN=256，baseK=64。
-   L1相关Tiling参数：stepM=1，stepN=1，stepKa=4，stepKb=4，depthA1=8，depthB1=8。

## 获取性能数据<a name="section1910315281533"></a>

使用msOpProf工具获取[算子仿真流水图](../../performance_analysis/collect_performance_data.md#section17259539153513)和[上板Profiling](../../performance_analysis/collect_performance_data.md#section17953123893415)数据。相较于基础场景，Tiling常量化在编译期期间将部分或全部Tiling参数由变量转化为常数值，在算子执行时直接使用常量化的Tiling参数，可以减少Scalar性能开销，所以重点分析Scalar流水。

## 分析主要瓶颈点<a name="section93975169548"></a>

-   优化前的流水图如下，默认不开启Tiling常量化，Tiling参数需要从Host侧拷贝到核函数（Kernel）侧，导致Matmul初始化时的Scalar计算较多，第一个MTE2指令开始于3.536us左右，MTE2前的指令头开销在算子整个流水中占比较大，因此需要优化Scalar计算。

    ![](../../../figures/zh-cn_image_0000002411422713.png)

-   优化前的Profiling数据如下，从C列的aic\_time数据来看，多个核中最大算子执行耗时为10.62us，从G列的aic\_scalar\_time数据来看，Scalar平均耗时6.32us。

    ![](../../../figures/zh-cn_image_0000002411018580.png)

## 设计优化方案<a name="section10569929145417"></a>

如下图所示，默认不开启Tiling常量化功能时，开发者在host侧创建Tiling对象，通过调用API自动获取Tiling参数。然后将Tiling参数从Host侧传递到核函数（Kernel）侧，在核函数（Kernel）侧初始化操作时传入。在算子执行时，使用Tiling变量参数完成矩阵乘操作。

**图1**  默认不开启Tiling常量化的Matmul计算流程示意图<a name="fig1911911419426"></a>  
![](../../../figures/no_const_matmul.png "默认不开启Tiling常量化的Matmul计算流程示意图")

如下图所示，开启Tiling常量化功能时，开发者只需要在核函数（Kernel）侧创建Matmul对象时，调用GetMatmulApiTiling接口在编译期获取常量化Tiling信息，即可完成Tiling常量化。在算子执行时，使用常量化的Tiling参数完成矩阵乘操作，减少Scalar计算开销。

**图2**  开启Tiling常量化的Matmul计算流程示意图<a name="fig146371949194314"></a>  
![](../../../figures/const_matmul.png "开启Tiling常量化的Matmul计算流程示意图")

Matmul API开启Tiling全量常量化的完整样例请参考[Matmul Tiling常量化的算子样例](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_constant_tiling)。开启Tiling全量常量化功能的步骤如下：

1.  调用获取MatmulConfig模板的接口GetMMConfig时，使用常数值设置MatmulShapeParams，得到带有常量化参数的自定义MatmulConfig模板CUSTOM\_CFG。

    ```
    constexpr int32_t MAX_M = 10000; // custom matmul kernel support max value of M Dim shape
    constexpr int32_t MAX_N = 10000; // custom matmul kernel support max value of N Dim shape
    constexpr int32_t MAX_K = 10000; // custom matmul kernel support max value of K Dim shape
    constexpr int32_t BASE_M = 128;  // BASE_M * BASE_K * sizeof(typeA) <=L0A size
    constexpr int32_t BASE_N = 256;  // BASE_N * BASE_K * sizeof(typeB) <=L0B size
    constexpr int32_t BASE_K = 64;   // BASE_M * BASE_N * sizeof(typeC) <=L0C size
    constexpr MatmulShapeParams shapeParams = { MAX_M,
                                                MAX_N,
                                                MAX_K,
                                                BASE_M,
                                                BASE_N,
                                                BASE_K };
    constexpr MatmulConfig CUSTOM_CFG = GetMMConfig<MatmulConfigMode::CONFIG_MDL>(shapeParams);
    ```

2.  创建Matmul对象。首先调用GetMatmulApiTiling接口，将Tiling信息常量化，得到常量化模板参数CONSTANT\_CFG，包括常量化的Matmul Tiling信息和MatmulConfig模板。创建Matmul对象时，使用常量化模板参数CONSTANT\_CFG。

    ```
    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, aType>;
    using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, bType>;
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, cType>;
    using BIAS_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, biasType>;
    constexpr static auto CONSTANT_CFG = AscendC::GetMatmulApiTiling<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE>(CUSTOM_CFG);
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CONSTANT_CFG> matmulObj;
    ```

3.  初始化操作。全量常量化时，可以在REGIST\_MATMUL\_OBJ接口的入参传递Tiling参数的位置，使用空指针替代。部分常量化时，在核函数（Kernel）侧使用REGIST\_MATMUL\_OBJ接口初始化Matmul对象时，仍需要使用Tiling。

    ```
    // 全量常量化场景，初始化操作示例
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmulObj, (TCubeTiling*)nullptr);
    
    // 部分常量化场景，初始化操作示例
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmulObj, &tiling);
    ```

## 验证优化方案性能收益<a name="section7519174385413"></a>

-   优化后的流水图如下，通过开启Tiling全量常量化，无需将Tiling参数从Host侧拷贝到核函数（Kernel）侧，在编译期完成Tiling常量化，减少了Matmul初始化时的Scalar计算。从0us起到第一个MTE2指令发起，这之间的时间为Matmul初始化时间，Matmul初始化时间从优化前的3.536us减少到2.185us，性能有所提升。

    ![](../../../figures/zh-cn_image_0000002377921500.png)

-   优化后的Profiling数据如下，从C列的aic\_time数据来看，多个核中最大算子执行耗时为7.87us，相较于优化前的10.62us提升了25.9%。从G列的aic\_scalar\_time数据来看，Scalar平均耗时3.38us，相较于优化前的6.32us提升了46.5%。

    ![](../../../figures/zh-cn_image_0000002411019880.png)

## 总结<a name="section252011820413"></a>

算子在调用Matmul API完成矩阵乘计算时，若Matmul初始化时的Scalar计算较多，影响了指令头开销，或Matmul迭代间的Scalar计算较多，阻塞了MTE2流水。在这两类场景下，满足上文提及的Tiling常量化开启条件（[全量常量化](#li159920538152)或[部分常量化](#li98655581816)），可以考虑开启Tiling常量化，减少Scalar计算开销，提升算子性能。
