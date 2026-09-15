# Matmul Advanced API Enabling Cube-Only Mode<a name="ZH-CN_TOPIC_0000002326915590"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:09:04.071Z -->

## Case Introduction<a name="section1698917333"></a>

This case demonstrates the performance improvement of the operator achieved by enabling the cube-only mode of the Matmul advanced API in the matrix multiplication operator scenario. As shown in the following figure, the Matmul API uses the MIX mode by default. That is, the user initiates a message from the AIV side, and after the message is relayed through the message communication framework, the Matmul computation is executed on the AIC side. This message processing mechanism introduces additional Scalar performance overhead. Compared with the MIX mode, the cube-only mode can directly bypass the message communication framework to complete the Matmul computation, thereby improving operator performance.

**Figure 1** Matmul process diagram in the default MIX mode<a name="fig0672118378"></a>  
![](../../../figures/default_mix_mode_matmul_process_diagram.png "Matmul process diagram in the default MIX mode")

-   Scenarios for enabling the cube-only mode

    Non-fused operators that involve only matrix computation. That is, compared with the MIX mode (which includes matrix computation and vector computation), there is no vector computation. The operator specifications in this case are as follows:

**Table 1** Operator case specifications

<a name="table15465191317123"></a>
<table><thead align="left"><tr id="row184651013131217"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p24653132122"><a name="p24653132122"></a><a name="p24653132122"></a>Input</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p13465111311213"><a name="p13465111311213"></a><a name="p13465111311213"></a>Shape</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p14465171371212"><a name="p14465171371212"></a><a name="p14465171371212"></a>Data Type</p></th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p74651713141213"><a name="p74651713141213"></a><a name="p74651713141213"></a>Format</p></th>
</tr>
</thead>
<tbody><tr id="row446561351212"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p517515253529"><a name="p517515253529"></a><a name="p517515253529"></a>a</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p141751425165213"><a name="p141751425165213"></a><a name="p141751425165213"></a>128, 64</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1517522515218"><a name="p1517522515218"></a><a name="p1517522515218"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p16176725105210"><a name="p16176725105210"></a><a name="p16176725105210"></a>ND</p></td>
</tr>
<tr id="row44651313101220"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p161761325185216"><a name="p161761325185216"></a><a name="p161761325185216"></a>b</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p4176112555211"><a name="p4176112555211"></a><a name="p4176112555211"></a>64, 30720</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1176132515212"><a name="p1176132515212"></a><a name="p1176132515212"></a>float16</p></td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p817692575212"><a name="p817692575212"></a><a name="p817692575212"></a>ND</p></td>
</tr>
</tbody>
</table>

The AI processor used in this case has 24 cores in total, each containing one AIC core and two AIV cores.

The tiling parameters are as follows:

-   Original shape: M=128, N=30720, K=64.
-   Single-core shape:
    -   MIX scenario: split across 48 AIV cores, with singleCoreM=128, singleCoreN=640, and singleCoreK=64.
    -   Pure Cube scenario: split across 24 AIC cores, with singleCoreM=128, singleCoreN=1280, and singleCoreK=64.

-   Basic block shape: baseM=128, baseN=256, baseK=64.
-   L1-related Tiling parameters: stepM=1, stepN=1, stepKa=4, stepKb=4, depthA1=8, depthB1=8.

## Obtaining Performance Data<a name="section1910315281533"></a>

Use the msOpProf tool to obtain the [operator simulation pipeline diagram](../../performance_analysis/obtain_performance_data.md#section17259539153513) and [on-board profiling](../../performance_analysis/obtain_performance_data.md#section17953123893415) data. Since cube-only mode mainly optimizes Scalar pipeline performance, you can focus on analyzing the Scalar pipeline.

## Analyzing the Main Bottleneck<a name="section145803332032"></a>

-   The profiling data before optimization is as follows. As shown by the aic\_time data in column C, the maximum operator execution time among multiple cores is 17.85us. As shown by the aic\_scalar\_time data in column G, the average Scalar time is 15.02us, indicating that the performance bottleneck lies in the Scalar pipeline.

    ![](../../../figures/zh-cn_image_0000002411013306.png)

-   The pipeline diagram before optimization is as follows. Because the default mode is MIX mode, each Matmul computation involves the message communication framework processing messages, resulting in a heavy Scalar pipeline and large performance overhead, as shown in the red box in the following figure.

    ![](../../../figures/zh-cn_image_0000002390760922.png)

## Designing an Optimization Solution<a name="section81265422311"></a>

In the default MIX mode, the user initiates a message on the AIV side. After the message is relayed through the message communication framework, Matmul computation is executed on the AIC side. Based on this process, when writing operator code with the Matmul advanced API, the user can use the [REGIST\_MATMUL\_OBJ](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/REGIST_MATMUL_OBJ.md) macro without distinguishing between AIV and AIC. However, this message processing mechanism also introduces additional performance overhead, as shown in [Figure 1 Process diagram of Matmul in the default MIX mode](#fig0672118378).

The specific steps to implement the default MIX mode are as follows:

1.  On the kernel side, define the Matmul object.

    ```
    #include "lib/matmul_intf.h"
    
    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType>;
    using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType>;
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
    using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_NORM> matmulObj;
    ```

2.  On the host side, call the SetDim API of the Matmul multi-core Tiling object to set the number of cores involved in the computation.

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    matmul_tiling::MultiCoreMatmulTiling cubeTiling(*ascendcPlatform);
    int32_t numBlocks = ascendcPlatform->GetCoreNumAiv(); // In MIX mode, use GetCoreNumAiv to obtain the number of cores available on the AI processor.
    cubeTiling.SetDim(numBlocks);
    ```

3.  Call the kernel function. For details, see the [kernel function definition and call](../../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md#zh-cn_topic_0000001447989210_section1915102519220), and set the **numBlocks** parameter of the kernel function.

    ```
    matmul_custom_do(ascendcPlatform->GetCoreNumAic(), stream, x1, x2, bias, y, workspaceDevice, tilingDevice); // In MIX mode, the kernel is launched as a combination of AIV and AIC. numBlocks specifies the number of AI Cores to launch.
    ```

In operator scenarios without vector computation, you can skip the message communication framework mechanism and enable cube-only mode to complete Matmul computation, thereby reducing the performance overhead of message communication and improving operator performance.

**Figure 2**  Process diagram of Matmul in cube-only mode<a name="fig20558182319127"></a>  
![](../../../figures/cube_only_mode_matmul_process_diagram.png "Process diagram of Matmul in cube-only mode")

For the complete sample of enabling cube-only mode for the Matmul API, see the [Matmul sample in cube-only mode](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul). The main steps for enabling cube-only mode are as follows:

1.  On the kernel side, in the code that defines the Matmul object, set the ASCENDC\_CUBE\_ONLY macro before including the matmul\_intf.h header file.

    ```
    #define ASCENDC_CUBE_ONLY // Set the ASCENDC_CUBE_ONLY macro before #include "lib/matmul_intf.h".
    #include "lib/matmul_intf.h"
    
    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType>;
    using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType>;
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
    using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_NORM> matmulObj;
    ```

2.  On the host side, call the SetDim API in the Matmul multi-core Tiling object to set the number of cores participating in the computation.

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    matmul_tiling::MultiCoreMatmulTiling cubeTiling(*ascendcPlatform);
    int32_t numBlocks = ascendcPlatform->GetCoreNumAic(); // In cube-only mode, use the GetCoreNumAic API to obtain the number of cores available on the AI processor.
    cubeTiling.SetDim(numBlocks);
    ```

3.  Call the kernel function. For details, see the [kernel function definition and call](../../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md#zh-cn_topic_0000001447989210_section1915102519220), and set the **numBlocks** parameter of the kernel function.

    ```
    matmul_custom_do(ascendcPlatform->GetCoreNumAic(), stream, x1, x2, bias, y, workspaceDevice, tilingDevice); // For an operator that contains only Cube computation, numBlocks is used to set the number of AICs to launch.
    ```

4.  On the kernel side, add a return branch on the AIV side in the kernel function implementation.

    ```
    extern "C" __global__ __aicore__ void matmul_custom(GM_ADDR a, GM_ADDR b,
        GM_ADDR bias, GM_ADDR c, GM_ADDR workspace, GM_ADDR tilingGm)
    {
        if (g_coreType == AscendC::AIV) { // In cube-only mode, return directly on the AIV side.
            return;
        }
        ...
        // Other code
    }
    ```

## Verifying the Performance Gains of the Optimization Solution<a name="section36229589310"></a>

-   The optimized profiling data is as follows. According to the aic_time data in column C, the maximum operator execution time among multiple cores is 11.21 us, a significant improvement over the 17.85 us before optimization. According to the aic_scalar_time data in column G, the average Scalar time is reduced from 15.02 us before optimization to 5.17 us.

    ![](../../../figures/zh-cn_image_0000002411013940.png)

-   The optimized pipeline diagram is as follows. Compared with the pipeline diagram before optimization, the Scalar pipeline at the position marked by the red box is noticeably sparser. Compared with the MIX mode, the cube-only mode reduces the processing of message communication and optimizes the overall Scalar performance overhead.

    ![](../../../figures/zh-cn_image_0000002377887318.png)

## Summary<a name="section252011820413"></a>

In scenarios where only matrix computation is involved without vector computation, you can enable cube-only mode to optimize the message communication performance overhead in Matmul computation and improve operator performance.
