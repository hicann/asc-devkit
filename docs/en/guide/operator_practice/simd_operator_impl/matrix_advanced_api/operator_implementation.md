# Operator Implementation<a name="ZH-CN_TOPIC_0000002500548094"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:32:18.409Z -->

## Implementation Process<a name="zh-cn_topic_0000001622514006_section6935163720564"></a>

The preceding sections describe the data splitting scheme and data flow of Matmul matrix multiplication. Ascend C provides a set of high-level Matmul APIs that encapsulate these common splitting, data transfer, and computation algorithm logics, allowing you to quickly implement Matmul matrix multiplication operations. On the host side, you call APIs to automatically obtain Tiling parameters. After the parameters are passed to the kernel side, they are passed in during initialization, and the matrix multiplication operation can be completed through a few simple APIs. For the complete sample, see [LINK](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/00_introduction/02_matrix/matmul_advanced_api).

**Figure 1** Matrix programming process diagram<a name="zh-cn_topic_0000001622514006_fig264116391245"></a>  
![](../../../figures/matrix_programming_process_diagram.png "matrix programming process diagram")

The key steps for automatically obtaining Tiling parameters on the host side are described as follows:

1.  **Create a Tiling object.**

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    matmul_tiling::MultiCoreMatmulTiling tilingApi(*ascendcPlatform);
    ```

    Pass in the hardware platform information to create a PlatformAscendC object, and then create a Tiling object. The hardware platform information can be obtained through GetPlatformInfo.

2.  **Set the number of cores involved in the Matmul operation, and the memory logical positions, formats, and data types of A and B.**

    ```
    tilingApi.SetDim(ascendcPlatform->GetCoreNumAic());  
    tilingApi.SetAType(AscendC::TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16, false);
    tilingApi.SetBType(AscendC::TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16, false);
    tilingApi.SetCType(AscendC::TPosition::GM, CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    ```

3.  **Set the matrix shape information.**

    ```
    tilingApi.SetOrgShape(M, N, K); // Set the original complete shapes M, N, and K
    tilingApi.SetShape(M, N, K);
    ```

4.  **Set the available space size information.**

    Set the sizes of the L1 Buffer, L0C Buffer, and Unified Buffer available for Matmul computation. -1 indicates the size of the corresponding buffer on the AI processor.

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
The specific steps for performing matrix multiplication using the Matmul API on the kernel side are as follows:

1.  **Create a Matmul object.**

    The following is an example of creating a Matmul object:

    -   In the pure Cube mode (only Cube computation) scenario, it is recommended that you define the ASCENDC\_CUBE\_ONLY macro in the code to avoid extra performance overhead. This section uses the pure Cube mode as an example.
    -   The default mode is the MIX mode (including Cube computation and Vector computation). In this scenario, the ASCENDC\_CUBE\_ONLY macro is usually not defined. If the ASCENDC\_CUBE\_ONLY macro is used in the program, you must use the ASCEND\_IS\_AIC macro and the ASCEND\_IS\_AIV macro to isolate Cube computation from Vector computation. For details, see [Fusion Operator Programming](../fusion_operator_programming/fusion_operator_programming.md).

    ```
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType; 
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType; 
    typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType; 
    AscendC::Matmul<aType, bType, cType> mm; 
    ```

    When creating an object, you need to pass in the parameter type information of A, B, and C. The type information is defined by MatmulType, including the logical memory location, data format, and data type.

2.  **Perform initialization.**

    ```
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling); // Initialize
    ```

    >[!NOTE] Note
    >The Matmul high-level API requires the system workspace (corresponding to the GetSysWorkSpacePtr API in this step) for its internal implementation. Developers need to apply for the system workspace space on their own:
    >-   During host-side Tiling implementation, set the total workspace size (including the user workspace and system workspace). The workspace space is applied for and managed by the framework. The system workspace size is obtained through [GetLibApiWorkSpaceSize](../../../../api/Utils-API/platform_info/PlatformAscendC/GetLibApiWorkSpaceSize.md).
    >    ```
    >    size_t userWorkspaceSize = 0;
    >    size_t systemWorkspaceSize = static_cast<size_t>(ascendcPlatform.GetLibApiWorkSpaceSize());
    >    size_t workspaceSize = userWorkspaceSize + systemWorkspaceSize;
    >    ```

3.  **Set left matrix A and right matrix B.**

    ```
    mm.SetTensorA(gmA);    // Set left matrix A
    mm.SetTensorB(gmB);    // Set right matrix B
    ```

4.  **Complete the matrix multiplication.**
    -   Call Iterate to complete a single iteration, and add a while loop to complete the computation of all data on a single core. With the Iterate method, you can control the number of iterations and complete the computation of the required amount of data, which is more flexible.

        ```
        while (mm.Iterate()) {   
            mm.GetTensorC(gmC); 
        }
        ```

    -   Call IterateAll to complete the computation of all data on a single core. With the IterateAll method, no loop iteration is required, which is simpler to use.

        ```
        mm.IterateAll(gmC);
        ```

5.  **End the matrix multiplication.**

    ```
    mm.End();
    ```

## Setting Shape Information<a name="zh-cn_topic_0000001622514006_section736714343331"></a>

When implementing Host Tiling, you can set shape information for Tiling computation. On the kernel side, you can also modify part of the shape information at runtime for scenarios such as tail block setting and Matmul reuse (multiple Matmul computations reusing one Matmul object). This section describes the shape concepts involved and provides guidance on setting Tiling information on both the host side and the kernel side.

-   orgShape: M, N, K
-   singleCoreShape: singleCoreM, singleCoreN, singleCoreK
-   singleShape: singleM, singleN, singleK
-   baseShape: baseM, baseN, baseK

Through the introduction of [data tiling](basic_knowledge.md#zh-cn_topic_0000001622194138_section68451031218), we have already learned the concepts of orgShape (M, N, K), singleCoreShape (singleCoreM, singleCoreN, singleCoreK), and baseShape (baseM, baseN, baseK), as shown in the following figure:

![](../../../figures/nd2nz_conversion_diagram_3_new.png)

In addition, during single-core Matmul Tiling, the shape that actually participates in the Matmul computation can be a part of the original shape. singleM, singleN, and singleK are used to express the shape that actually participates in the Matmul computation, as shown in the following figure. In the single-core case, singleM, singleN, and singleK are passed through to singleCoreM, singleCoreN, and singleCoreK.

![](../../../figures/zh-cn_image_0000002533171617.png)

-   Kernel runtime settings
    -   [SetTail](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/SetTail.md) and [SetSingleShape](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/SetSingleShape.md) modify singleCoreM, singleCoreN, and singleCoreK at runtime. Use [SetTail](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/SetTail.md) to process the tail block. In the Matmul reuse scenario (multiple Matmul computations reuse one Matmul object), you can use SetSingleShape to reset them.
    -   [SetOrgShape](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/SetOrgShape.md) modifies M, N, and K at runtime. In the Matmul reuse scenario, you can use SetOrgShape to reset them.

-   Single-core Tiling settings
    -   [SetOrgShape](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/SetOrgShape-87.md) (required): sets M, N, and K
    -   [SetShape](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/SetShape.md) (optional): sets singleM, singleN, and singleK, which is equivalent to setting singleCoreM, singleCoreN, and singleCoreK
    -   [SetFixSplit](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/SetFixSplit.md) (optional): sets baseM, baseN, and baseK

-   Multi-core Tiling settings
    -   [SetOrgShape](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/SetOrgShape-87.md) (required): sets M, N, and K
    -   [SetShape](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/SetShape.md) (optional): sets singleM, singleN, and singleK
    -   [SetFixSplit](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/SetFixSplit.md) (optional): Set baseM, baseN, and baseK

    -   [SetSingleShape](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/SetSingleShape-86.md) (optional): Set singleCoreM, singleCoreN, and singleCoreK
    -   [SetSingleRange](../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/SetSingleRange.md) (optional): Set the ranges of singleCoreM, singleCoreN, and singleCoreK

## Setting the Format<a name="zh-cn_topic_0000001622514006_section1558462120502"></a>

When creating a Matmul object, you need to pass in the parameter type information of A, B, and C. The type information is defined by MatmulType, including the logical memory location, data format, and data type. The following is an example:

```
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType; 
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType; 
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType; 
AscendC::Matmul<aType, bType, cType> mm; 
```

For the data format, there are three types: CubeFormat::ND, CubeFormat::NZ, and CubeFormat::ND\_ALIGN. The ND and NZ formats are described in the [Data Format](basic_knowledge.md#zh-cn_topic_0000001622194138_section1453415011) section. For details about the ND\_ALIGN format, see [Data Layout Format](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md#li075920427155).
