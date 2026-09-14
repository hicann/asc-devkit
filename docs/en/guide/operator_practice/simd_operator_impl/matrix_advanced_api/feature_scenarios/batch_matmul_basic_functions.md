# Batch Matmul Basic Functions<a name="ZH-CN_TOPIC_0000002532228171"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:42:08.126Z -->

## Description<a name="zh-cn_topic_0000001726346562_section1953745712231"></a>

Batch Matmul refers to the scenario of processing Matmul computations in batches. This scenario provides the [IterateBatch](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/IterateBatch.md) call API. By calling IterateBatch once, you can compute multiple C matrices of the singleCoreM \* singleCoreN size.

A single Matmul computation requires data to be moved in and out. When multiple Matmul computations are performed and the input shape of each single Matmul computation is small, the data movement overhead accounts for a large proportion of the total time. Processing Matmul in batches through the IterateBatch API can effectively improve bandwidth utilization.

Batch Matmul currently supports four layout types: BSNGD, SBNGD, BNGS1S2, and NORMAL (the BMNK data layout format). For details about the data layout formats, see [IterateBatch](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/IterateBatch.md).

The following figure shows Batch Matmul computation in the NORMAL data layout format. The entire Matmul computation consists of four matrix multiplication operations: mat\_a1\*mat\_b1, mat\_a2\*mat\_b2, mat\_a3\*mat\_b3, and mat\_a4\*mat\_b4, which require computing four singleCoreM \* singleCoreN on a single core. In this scenario, if the shape is small, it can be treated as a Batch Matmul scenario for batch processing to improve performance. One IterateBatch call can simultaneously compute mat\_c1 = mat\_a1 \* mat\_b1, mat\_c2 = mat\_a2 \* mat\_b2, mat\_c3 = mat\_a3 \* mat\_b3, and mat\_c4 = mat\_a4 \* mat\_b4.

**Figure 1**  Batch Matmul schematic diagram in the NORMAL data layout format<a name="zh-cn_topic_0000001726346562_fig3876163755011"></a>  
![](../../../../figures/batch_matmul_diagram_in_normal_data_layout_format.png "Batch Matmul schematic diagram in the NORMAL data layout format")

## Scenarios<a name="zh-cn_topic_0000001726346562_section1672510573541"></a>

A Matmul computation needs to compute multiple C matrices of the singleCoreM \* singleCoreN size, and the shape processed in a single Matmul computation is small.

## Constraints<a name="zh-cn_topic_0000001726346562_section3618165412115"></a>

-   Only the [Norm template](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md) is supported.
-   For the BSNGD, SBNGD, and BNGS1S2 layout formats, the total size of the multi-batch data of the input A and B matrices after fractal alignment must be smaller than the L1 Buffer size. For the NORMAL layout format, there is no such restriction, but you need to configure the [batchMode](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md) parameter through [MatmulConfig](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md), that is, the relationship between the multi-batch data size of the input A and B matrices and the L1 Buffer size.
-   For the BSNGD, SBNGD, and BNGS1S2 layout formats, let the G axes of the left matrix and the right matrix be ALayoutInfoG and BLayoutInfoG, respectively. Then ALayoutInfoG / batchA = BLayoutInfoG / batchB. For the NORMAL layout format, batchA and batchB must satisfy a multiple relationship. The batch in the Bias shape (batch, n) must be equal to the batch of the C matrix.
-   If the API outputs to the Unified Buffer, the output C matrix size BaseM\*BaseN must be smaller than the allocated Unified Buffer memory size.
-   For the BSNGD and SBNGD layout formats, the input and output support only ND format data. For the BNGS1S2 and NORMAL layout formats, the input supports ND/NZ format data.
-   Batch Matmul does not support quantization/dequantization modes, that is, it does not support the [SetQuantScalar](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/SetQuantScalar.md) and [SetQuantVector](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/SetQuantVector.md) APIs.
-   In the BSNGD scenario, computing multiple SD rows at a time is not supported. The operator program needs to compute them in a loop.
-   **Asynchronous mode** does not support IterateBatch moving data to the Unified Buffer.
-   When the template parameter [enableMixDualMaster](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md) (default value: false) is set to true, that is, when the MixDualMaster (dual-master mode) scenario is enabled, Batch Matmul is not supported.
-   In the batch scenario, matrix A and matrix B support the half, float, bfloat16_t, and int8_t data types, but do not support the int4b_t data type.

## Calling Example<a name="zh-cn_topic_0000001726346562_section18018102212"></a>

The following is a Batch Matmul calling example in the NORMAL data layout format. For a complete Batch Matmul example in the BSNDG data layout format, see [BatchMatmul sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/batch_matmul).

-   Tiling implementation

    Use [SetBatchInfoForNormal](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/SetBatchInfoForNormal.md) to set the M/N/K axis information of A/B/C and the BatchNum of matrices A and B.

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::MultiCoreMatmulTiling tiling(ascendcPlatform);   
    int32_t M = 32;
    int32_t N = 256;
    int32_t K = 64;
    tiling->SetDim(1);
    tiling->SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling->SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling->SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling->SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling->SetShape(M, N, K);
    tiling->SetOrgShape(M, N, K);
    tiling->EnableBias(true);
    tiling->SetBufferSpace(-1, -1, -1);
    
    constexpr int32_t BATCH_NUM = 3;
    tiling->SetBatchInfoForNormal(BATCH_NUM, BATCH_NUM, M, N, K);  // Set the matrix layout
    tiling->SetBufferSpace(-1, -1, -1);
    
    optiling::TCubeTiling tilingData;
    int ret = tiling.GetTiling(tilingData);
    ```

-   Kernel implementation
    -   Create a Matmul object.

        Set the input and output layout format to NORMAL through MatmulType.

        ```
        #include "lib/matmul_intf.h"
        
        typedef AscendC::MatmulType <AscendC::TPosition::GM, CubeFormat::ND, half, false, LayoutMode::NORMAL> aType;
        typedef AscendC::MatmulType <AscendC::TPosition::GM, CubeFormat::ND, half, true, LayoutMode::NORMAL> bType;
        typedef AscendC::MatmulType <AscendC::TPosition::GM, CubeFormat::ND, float, false, LayoutMode::NORMAL> cType;
        typedef AscendC::MatmulType <AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
        constexpr MatmulConfig MM_CFG = GetNormalConfig(false, false, false, BatchMode::BATCH_LESS_THAN_L1);
        AscendC::Matmul<aType, bType, cType, biasType, MM_CFG> mm; 
        ```

    -   Initialize the operation.

        ```
        REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling); // Initialize the matmul object
        ```

    -   Set the left matrix A, right matrix B, and bias.

        ```
        mm.SetTensorA(gm_a);    // Set the left matrix A.
        mm.SetTensorB(gm_b);    // Set the right matrix B.
        mm.SetBias(gm_bias);    // Set the bias.
        ```

    -   Complete the matrix multiplication. The left matrix computes batchA MK data each time, and the right matrix computes batchB KN data each time.

        ```
        mm.IterateBatch(gm_c, batchA, batchB, false);
        ```

    -   End the matrix multiplication.

        ```
        mm.End();
        ```
