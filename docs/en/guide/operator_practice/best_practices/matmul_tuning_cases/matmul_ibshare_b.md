# Enabling the IBShare Template to Share Cube B Data for the Matmul Advanced API<a name="ZH-CN_TOPIC_0000002340907802"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T11:27:41.474Z pushedAt=2026-11-03T07:05:13.273Z nl=2 -->

## Case Introduction<a name="section17413194624510"></a>

This case demonstrates the performance improvement achieved by using the Matmul advanced API for cube multiplication and enabling IBShare for the B cube in a cube multiplication operator scenario. The IBShare feature reduces redundant MTE2 data movement overhead and improves operator performance by sharing the same A cube or B cube data on the L1 Buffer. This feature supports enabling IBShare for either the A cube or the B cube, and also supports enabling IBShare for both the A cube and the B cube simultaneously.

-   Scenarios for enabling IBShare

    In the MIX scenario (which includes both cube computation and vector computation), the GM addresses of the A cube or B cube of multiple AIVs are the same, and the A cube or B cube reused by multiple AIVs is fully loaded on the L1 Buffer.

-   Constraints for enabling IBShare
    -   When IBShare is enabled for both the A cube and the B cube, the A cube and B cube of other Matmul objects in the same operator must also have IBShare enabled simultaneously.
    -   When IBShare is enabled for both the A cube and the B cube, only the **IterateAll** API is supported for obtaining the cube computation result, and the result can only be output to Global Memory.

The operator specifications in this case are as follows:

**Table 1**  Operator specifications

<a name="table101751125175213"></a>
<table><thead align="left"><tr id="row8175525185219"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p1417582516529"><a name="p1417582516529"></a><a name="p1417582516529"></a>Input</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p417532575212"><a name="p417532575212"></a><a name="p417532575212"></a>Shape</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p1017582585214"><a name="p1017582585214"></a><a name="p1017582585214"></a>Data type</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p317502512522"><a name="p317502512522"></a><a name="p317502512522"></a>Format</p></th>
</tr>
</thead>
<tbody><tr id="row217562525215"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p61219167439"><a name="p61219167439"></a><a name="p61219167439"></a>a</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p17124161434"><a name="p17124161434"></a><a name="p17124161434"></a>64, 384</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p312131614432"><a name="p312131614432"></a><a name="p312131614432"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p81213165432"><a name="p81213165432"></a><a name="p81213165432"></a>ND</p></td>
</tr>
<tr id="row10176102512525"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p49845284319"><a name="p49845284319"></a><a name="p49845284319"></a>b</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p85970614134"><a name="p85970614134"></a><a name="p85970614134"></a>384, 256</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p6419195512430"><a name="p6419195512430"></a><a name="p6419195512430"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p12420205594320"><a name="p12420205594320"></a><a name="p12420205594320"></a>ND</p></td>
</tr>
</tbody>
</table>

The AI processor used in this case has 20 cores in total, each containing one AIC core and two AIV cores. Because the input shape is small, this case uses a single core as an example. Referring to the use of the **SetDim** API in MIX mode, the number of cores participating in the computation is set to 2 in the Tiling program. The Tiling parameters are as follows:

-   Original shape: M=64, N=256, K=384.
-   Single-core shape: singleCoreM=32, singleCoreN=256, singleCoreK=384. Cube A is split into two halves, one processed on AIV0 and the other on AIV1. AIV0 and AIV1 use the same cube B data.
-   Base block shape: baseM=32, baseN=256, baseK=64.
-   L1 cache-related Tiling parameters: stepM=1, stepN=1, stepKa=6, stepKb=6.

## Obtaining Performance Data<a name="section851404010469"></a>

Use the msOpProf tool to obtain the [operator simulation pipeline diagram](../../performance_analysis/collect_performance_data.md#section17259539153513) and [on-board Profiling](../../performance_analysis/collect_performance_data.md#section17953123893415) data. Because the IBShare feature mainly reduces the repeated MTE2 data movement overhead by sharing the same A or B cube data on the L1 Buffer, focus on analyzing the MTE2 pipeline.

## Analyzing the Main Bottleneck<a name="section221431704714"></a>

-   The pipeline diagram before optimization is as follows. The IBShare template is not enabled, and the Norm template is used by default. The black box marks the MTE2 movement pipeline initiated by AIV0: MTE2 performs 12 movements in total, of which the A cube is moved 6 times (stepM\*stepKa=6) and the B cube is moved 6 times (stepN\*stepKb=6). The red box marks the MTE2 movement pipeline initiated by AIV1, which is basically the same as that of AIV0. In this case, because the B cube data used by AIV1 is the same as that used by AIV0, and singleCoreN=baseN\*stepN, singleCoreK=baseK\*stepKb, that is, the B cube can be fully loaded into L1. Consider caching the B cube data in the L1 Buffer after AIV0 moves the B cube into the L1 Buffer, so that AIV1 can reuse it, thereby saving the repeated MTE2 movement overhead of the B cube.

    ![](../../../figures/zh-cn_image_0000002404199037.png)

-   The Profiling data before optimization is as follows. The **aic\_time** in column C is 10.29us, and the **aic\_mte2\_time** in column K is 5.56us.

    ![](../../../figures/zh-cn_image_0000002411076626.png)

## Designing an Optimization Solution<a name="section1875873847"></a>

The following figure shows the Matmul computation pipeline diagram when the IBShare template is not enabled (the Norm template is used by default). MTE2 moves basic blocks from Global Memory to the L1 Buffer (A1 or B1) multiple times. Even if the B cube basic block data moved in two consecutive operations is the same, it is still moved repeatedly.

**Figure 1**  Matmul pipeline diagram without the IBShare template enabled<a name="fig1723213921319"></a>  
![](../../../figures/no_ibshare_mm.png "Matmul pipeline diagram without the IBShare template enabled")

The following figure shows the Matmul computation pipeline diagram when the IBShare template is enabled. MTE2 moves basic blocks from Global Memory to the L1 Buffer (A1 or B1) multiple times. If the B cube basic block data moved in two consecutive operations is the same, it is not moved repeatedly, and the data first moved to B1 is reused.

**Figure 2**  Matmul pipeline diagram with the IBShare template enabled<a name="fig10881182610139"></a>  
![](../../../figures/ibshare_mm.png "Matmul pipeline diagram with the IBShare template enabled")

For a complete example of enabling the IBShare template to share the B cube through the Matmul API, see [Example of Enabling IBShare for the B Cube Only](../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_ibshareB). The main steps for enabling IBShare are as follows:

1.  Create a Matmul object.

    ```
    #define ASCENDC_CUBE_ONLY
    #include "lib/matmul_intf.h"
    
    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType>;
    using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType, false, LayoutMode::NONE, true>; // Set the IBSHARE parameter of the B cube to true.
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
    using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_IBSHARE_NORM> matmulObj; // Define a Matmul object using the default IBShare template parameter CFG_IBSHARE_NORM.
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section19022397498"></a>

-   The optimized pipeline diagram is as follows. The MTE2 movement pipeline initiated by AIV0, marked by the black box, is the same as before optimization. The MTE2 movement pipeline initiated by AIV1, marked by the red box, is reduced from a total of 12 MTE2 data movements for the A cube and B cube before optimization to only 6 MTE2 data movements for the A cube, eliminating the overhead of 6 MTE2 data movements for the B cube.

    ![](../../../figures/zh-cn_image_0000002404200417.png)

-   The optimized Profiling data is as follows. The value in column C, **aic\_time**, is 9.93us, an improvement of 3.55% over the 10.29us before optimization. The value in column K, **aic\_mte2\_time**, is 4.71us, an improvement of 15.46% over the 5.56us before optimization.

    ![](../../../figures/zh-cn_image_0000002444817149.png)

## Summary<a name="section8281219125011"></a>

In a MIX scenario (involving both cube computation and vector computation), if the GM addresses of the A cube or B cube of multiple AIVs are the same, and the A cube/B cube reused by multiple AIVs are fully loaded in the L1 Buffer, you can consider enabling the IBShare template to share the same A cube or B cube data in the L1 Buffer, thereby reducing the redundant MTE2 data movement overhead and improving operator performance.
