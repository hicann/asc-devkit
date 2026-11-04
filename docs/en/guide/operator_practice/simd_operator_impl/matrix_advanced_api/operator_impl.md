# Operator Implementation<a name="ZH-CN_TOPIC_0000002500548094"></a>

<!-- md-trans-meta sourceCommit=3bfd2fd2f2f5179c742a247e3eba48e2420c8c17 translatedAt=2026-10-23T11:57:40.261Z pushedAt=2026-11-03T07:05:13.296Z nl=2 -->

## Implementation Process<a name="zh-cn_topic_0000001622514006_section6935163720564"></a>

The preceding sections describe the data splitting scheme and data flow of Matmul cube multiplication. Ascend C provides a set of Matmul advanced APIs that encapsulate these common splitting, data movement, and computation algorithms, allowing users to quickly implement Matmul cube multiplication operations. On the host side, developers obtain the Tiling parameters by calling APIs. After the parameters are passed to the kernel side, they are passed in during initialization, and the cube multiplication operation can be completed through a few simple APIs. For the complete sample, click [here](../../../../../../examples/01_simd_cpp_api/00_introduction/02_matrix/matmul_advanced_api).

**Figure 1**  Cube programming process<a name="zh-cn_topic_0000001622514006_fig264116391245"></a>  
![](../../../figures/mat_prog_flow.png "Cube programming process")

The key steps for automatically obtaining Tiling parameters on the host side are described as follows:
<a name="zh-cn_topic_0000001622514006_li19113114819525"></a>
1.  **Create a Tiling object**.

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    matmul_tiling::MultiCoreMatmulTiling tilingApi(*ascendcPlatform);
    ```

    Pass in the hardware platform information to create a PlatformAscendC object, and then create a Tiling object. The hardware platform information can be obtained through GetPlatformInfo.

2.  **Set the number of cores involved in the Matmul operation, and the memory logical location, format, and data type of A and B.**

    ```
    tilingApi.SetDim(ascendcPlatform->GetCoreNumAic());  
    tilingApi.SetAType(AscendC::TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16, false);
    tilingApi.SetBType(AscendC::TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16, false);
    tilingApi.SetCType(AscendC::TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    ```

3.  **Set the cube shape information.**

    ```
    tilingApi.SetOrgShape(M, N, K); // Set the original complete shapes M, N, and K.
    tilingApi.SetShape(M, N, K);
    ```

4.  **Set the available space size information.**

    Set the size of the L1 Buffer/L0C Buffer/Unified Buffer (UB) space available for Matmul computation. -1 indicates the size of the corresponding buffer on the AI processor.

    ```
    tilingApi.SetBufferSpace(-1, -1, -1);
    ```

5.  **Obtain the Tiling parameters.**

    ```
    int64_t res = tilingApi.GetTiling(tilingData);
    if (res == -1) {
        std::cout << "gen tiling failed" << std::endl;
    }
    ```

6.  Perform other operations such as serializing and saving the Tiling parameters.

    ```
    uint32_t tcubeTilingSize = tilingData.GetDataSize();
    tilingData.SaveToBuffer(tilingBuf, tcubeTilingSize);
    ```

<a name="zh-cn_topic_0000001622514006_li1032116474330"></a>
On the kernel side, the specific steps for using the Matmul API to perform cube operations are as follows:

1.  **Create a Matmul object.**

    The following is an example of creating a Matmul object:

    -   In the cube-only mode (only cube computation), it is recommended that you define the ASCENDC\_CUBE\_ONLY macro in the code to avoid extra performance overhead. This section uses the cube-only mode as an example.
    -   The default mode is the MIX mode (including both cube computation and vector computation). In this mode, the ASCENDC\_CUBE\_ONLY macro is usually not defined. If the ASCENDC\_CUBE\_ONLY macro is used in the program, you must use the ASCEND\_IS\_AIC macro and the ASCEND\_IS\_AIV macro to isolate Cube computation from Vector computation. For details, see [Fusion Operator Programming](../fusion_operator_programming/fusion_operator_programming.md).

    ```
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType; 
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType; 
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType; 
    AscendC::Matmul<aType, bType, cType> mm; 
    ```

    When creating an object, you need to pass in the parameter type information of A, B, and C. The type information is defined by MatmulType, including the logical memory location, data format, and data type.

2.  **Perform initialization.**

    ```
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling); // Initialize.
    ```

    >[!NOTE] Note
    >The Matmul advanced API requires the system workspace (corresponding to the GetSysWorkSpacePtr API in this step) for its internal implementation. Developers need to allocate the system workspace on their own:
    >-   When implementing Tiling on the host side, set the total workspace size (including the user workspace and system workspace). The workspace is allocated and managed by the framework. The system workspace size is obtained through [GetLibApiWorkSpaceSize](../../../../api/Utils-API/platform_info/PlatformAscendC/GetLibApiWorkSpaceSize.md).
    >    ```
    >    size_t userWorkspaceSize = 0;
    >    size_t systemWorkspaceSize = static_cast<size_t>(ascendcPlatform.GetLibApiWorkSpaceSize());
    >    size_t workspaceSize = userWorkspaceSize + systemWorkspaceSize;
    >    ```

3.  **Set the original cube shape and the left cube A and right cube B used by the current core.**

    In the multi-core scenario, set the corresponding cube start address for each core based on the data splitting direction. In this sample, cube A and cube C are split along the M axis: each core reads `singleCoreM` rows of data from cube A, while cube B is not split and is read in full by all cores. `SetOrgShape` must be called before `SetTensorA` and `SetTensorB`.

    ```
    mm.SetOrgShape(tiling.M, tiling.N, tiling.Ka, tiling.Kb);
    mm.SetTensorA(aGlobal[GetBlockIdx() * tiling.singleCoreM * tiling.Ka], false);
    mm.SetTensorB(bGlobal[0], false);
    ```

    Here, `GetBlockIdx()` indicates the current core number. Cube A is stored contiguously in the `[M, Ka]` layout, so the start offset of cube A for the current core is `GetBlockIdx() * tiling.singleCoreM * tiling.Ka`. Each core computes different rows of cube C, but all cores require the complete cube B for K-axis accumulation, so cube B is read starting from `bGlobal[0]`. The second parameter `false` of `SetTensorA` and `SetTensorB` indicates that the cube is not transposed.

4.  **Complete the cube multiplication operation.**
    -   Call Iterate to complete a single iteration, and use a while loop to complete the data computation assigned to the current core. Cube C is stored contiguously in the `[M, N]` layout, and the write-back start offset for the current core is `GetBlockIdx() * tiling.singleCoreM * tiling.N`. The Iterate method allows you to control the number of iterations, offering greater flexibility.

        ```
        while (mm.Iterate()) {
            mm.GetTensorC(cGlobal[GetBlockIdx() * tiling.singleCoreM * tiling.N]);
        }
        ```

    -   Call IterateAll to complete all data computation assigned to the current core and write back to cube C. The IterateAll method requires no loop iteration and is simpler to use. In the sample code of this section, IterateAll is called to implement cube multiplication.

        ```
        mm.IterateAll(cGlobal[GetBlockIdx() * tiling.singleCoreM * tiling.N]);
        ```

5.  **End the cube multiplication operation.**

    ```
    mm.End();
    ```

## Setting Shape Information<a name="zh-cn_topic_0000001622514006_section736714343331"></a>

When implementing host Tiling, you can set shape information for Tiling computation. On the kernel side, you can also modify part of the shape information at runtime for scenarios such as tail block setting and Matmul reuse (multiple Matmul computations reusing one Matmul object). This section introduces the shape concepts involved and provides guidance on setting Tiling information on both the host side and the kernel side.

-   orgShape: M, N, K
-   singleCoreShape: singleCoreM, singleCoreN, singleCoreK
-   singleShape: singleM, singleN, singleK
-   baseShape: baseM, baseN, baseK

Through the introduction of [data tiling](basics.md#zh-cn_topic_0000001622194138_section68451031218), we have learned the concepts of orgShape\(M, N, K\), singleCoreShape\(singleCoreM, singleCoreN, singleCoreK\), baseShape\(baseM, baseN, and baseK\), as shown in the following figure:

![](../../../figures/nd2nz_new_3.png)

In addition, during single-core Matmul Tiling, the shape that actually participates in the Matmul computation can be a part of the original shape. singleM, singleN, and singleK are used to express the shape that actually participates in the Matmul computation, as shown in the following figure. In the single-core case, singleM, singleN, and singleK are passed through to singleCoreM, singleCoreN, and singleCoreK.

![](../../../figures/zh-cn_image_0000002533171617.png)

-   Kernel function runtime settings
    -   [SetTail](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTail.md) and [SetSingleShape](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetSingleShape.md) modify singleCoreM, singleCoreN, and singleCoreK at runtime. Use [SetTail](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTail.md) to process tail blocks. In the Matmul reuse scenario (multiple Matmul computations reuse one Matmul object), SetSingleShape can be used to reset the values.
    -   [SetOrgShape](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetOrgShape.md) modifies M, N, and K at runtime. In the Matmul reuse scenario, SetOrgShape can be used to reset the values.

-   Single-core Tiling settings
    -   [SetOrgShape](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetOrgShape.md) (mandatory): sets M, N, and K
    -   [SetShape](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetShape.md) (optional): sets singleM, singleN, and singleK, which is equivalent to setting singleCoreM, singleCoreN, and singleCoreK
    -   [SetFixSplit](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetFixSplit.md) (optional): sets baseM, baseN, and baseK

-   Multi-core Tiling settings
    -   [SetOrgShape](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetOrgShape.md) (mandatory): sets M, N, and K
    -   [SetShape](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetShape.md) (optional): sets singleM, singleN, and singleK
    -   [SetFixSplit](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetFixSplit.md) (optional): Set baseM, baseN, and baseK

    -   [SetSingleShape](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetSingleShape.md)\( optional \): Set singleCoreM, singleCoreN, and singleCoreK
    -   [SetSingleRange](../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetSingleRange.md)\( optional \): Set the ranges of singleCoreM, singleCoreN, and singleCoreK

## Setting the Format<a name="zh-cn_topic_0000001622514006_section1558462120502"></a>

When creating a Matmul object, you need to pass in the parameter type information of A, B, and C. The type information is defined by MatmulType, including the memory logical location, data format, and data type. The following is an example:

```
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType; 
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType; 
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType; 
AscendC::Matmul<aType, bType, cType> mm; 
```

For the data format, there are three types: CubeFormat::ND, CubeFormat::NZ, and CubeFormat::ND\_ALIGN. The ND and NZ formats are described in the [data format](basics.md#zh-cn_topic_0000001622194138_section1453415011) section. For details about the ND\_ALIGN format, see [Data Layout Format](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md#li075920427155).
