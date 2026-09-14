# Matmul Tiling Class Usage

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:16:50.518Z pushedAt=2026-09-12T09:55:18.178Z -->

Ascend C provides a set of Matmul Tiling APIs to help you obtain the Tiling parameters required for [Matmul kernel computation](../Matmul-Kernel/matmul_usage.md#li5878185413338). You only need to pass in the Position, Format, and DType information of the A/B/C matrices, call the API, and then obtain the related parameters in the TCubeTiling structure in [Init](../Matmul-Kernel/Init-85.md).

The Matmul Tiling APIs are divided into the Matmul single-core Tiling API, the multi-core Tiling API, and the BatchMatmul Tiling API, which are used for Matmul single-core computation, multi-core computation, and BatchMatmul computation scenarios, respectively. The process of obtaining Tiling parameters is as follows:

1.  Create a single-core Tiling object, a multi-core Tiling object, or a BatchMatmul Tiling object.
2.  Set the parameter type information of A, B, C, and Bias, as well as the shape information such as M, N, Ka, and Kb.
3.  Call the [GetTiling](GetTiling.md) API to obtain the Tiling information.

The following shows examples of obtaining Tiling parameters using the Matmul single-core Tiling API, the multi-core Tiling API, and the BatchMatmul Tiling API:

-   Matmul single-core Tiling

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
    // Set the Position, Format, and DType information of the A, B, C, and Bias matrices.
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(1024, 1024, 1024); // Set the M, N, and K sizes for single-core computation.
    tiling.SetOrgShape(1024, 1024, 1024); // Set the original input M, N, and K sizes. For single-core Tiling, this is the same as SetShape. If Ka and Kb have different lengths, set tiling.SetOrgShape(1024, 1024, 1024, 1280).
    tiling.EnableBias(true); // Set the matmul computation to include bias.
    tiling.SetBufferSpace(-1, -1, -1);  // Set the allowed space. By default, all space of the AI processor is used.
    optiling::TCubeTiling tilingData;
    int64_t ret = tiling.GetTiling(tilingData);    // if ret = -1, get tiling failed
    ```

-   Matmul multi-core Tiling

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::MultiCoreMatmulTiling tiling(ascendcPlatform);
    tiling.SetDim(1); // Set the number of cores participating in the computation to 1.
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    tiling.SetShape(1024, 1024, 1024);
    tiling.SetSingleShape(1024, 1024, 1024);
    tiling.SetOrgShape(1024, 1024, 1024);
    tiling.EnableBias(true);
    tiling.SetBufferSpace(-1, -1, -1);  // Set the allowed space. By default, all space of the AI processor is used.
    optiling::TCubeTiling tilingData;
    int64_t ret = tiling.GetTiling(tilingData);    // if ret = -1, get tiling failed
    ```

-   BatchMatmul Tiling

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::BatchMatmulTiling bmmTiling(ascendcPlatform);

    bmmTiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    bmmTiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    bmmTiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    bmmTiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
    bmmTiling.EnableBias(true);
    bmmTiling.SetShape(64, 48, 32);
    bmmTiling.SetSingleShape(64, 48, 32);
    bmmTiling.SetOrgShape(64, 48, 32);
    // When the Layout type is NORMAL, set the Layout axis information of matrices A, B, and C through SetBatchInfoForNormal.
    bmmTiling.SetBatchInfoForNormal(2, 2, 64, 48, 32);
    // When the Layout type is BSNGD, SBNGD, or BNGS1S2, set the Layout axis information of matrices A, B, and C through SetALayout, SetBLayout, and SetCLayout.
    // bmmTiling.SetALayout(3, 64, 2, 2, 32);
    // bmmTiling.SetBLayout(3, 32, 2, 2, 48);
    // bmmTiling.SetCLayout(3, 64, 2, 2, 48);
    bmmTiling.SetBatchNum(2);
    bmmTiling.SetBufferSpace(-1, -1, -1);  // Set the allowed space. By default, all space of the AI processor is used.
    optiling::TCubeTiling tilingData;
    int64_t ret = bmmTiling.GetTiling(tilingData);    // if ret = -1, get tiling failed
    ```

The API list is as follows:

**Table 1** APIs shared by MatmulApiTiling/MultiCoreMatmulTiling/BatchMatmulTiling

| API | Description |
| --- | --- |
| SetAType | Sets the position, data format, data type, and whether to transpose of matrix A. |
| SetBType | Sets the position, data format, data type, and whether to transpose of matrix B. |
| SetCType | Sets the position, data format, and data type of matrix C. |
| SetDequantType | Sets the dequantization mode. |
| SetBiasType | Sets the position, data format, and data type of Bias. |
| SetShape | Sets the shapes singleM, singleN, and singleK of a single Matmul computation, in element count. |
| SetOrgShape | Sets the original complete shapes M, N, Ka, and Kb used in Matmul computation, in element count. |
| SetALayout | Sets the Layout axis information of matrix A. |
| SetBLayout | Sets the Layout axis information of matrix B. |
| SetCLayout | Sets the Layout axis information of matrix C. |
| SetBatchInfoForNormal | Sets the M/N/K axis information of matrices A/B and the Batch count of each of matrices A/B. |
| SetBatchNum | Sets the maximum Batch count for multi-Batch computation. |
| EnableBias | Sets whether Bias participates in the computation. |
| SetBias | Sets whether Bias participates in the computation. The EnableBias API is recommended. |
| SetFixSplit | Sets the fixed baseM, baseN, and baseK, in element count. |
| SetBufferSpace | Sets the available L1/L0C/UB space size for Matmul computation, in bytes. |
| SetTraverse | Sets the traversal mode, M-axis first or N-axis first. |
| SetMadType | Sets whether to enable the HF32 mode. Not supported in the current version. |
| SetSplitRange | Sets the maximum and minimum values of baseM/baseN/baseK. |
| SetMatmulConfigParams | Customizes the MatmulConfig parameters. |
| SetDoubleBuffer | Sets whether to enable the double buffer function for A/B/C/Bias and whether ND2NZ or NZ2ND conversion is required. This API is reserved and not supported in the current version. |
| GetBaseM | Obtains the baseM value. |
| GetBaseN | Obtains the baseN value. |
| GetBaseK | Obtains the baseK value. |
| GetTiling | Obtains the Tiling parameters. |

**Table 2** Other APIs of MultiCoreMatmulTiling

| API | Description |
| --- | --- |
| SetDim | Sets the number of cores that can participate in multi-core Matmul computation. |
| SetSingleRange | Sets the maximum and minimum values of singleCoreM/singleCoreN/singleCoreK, in element count. |
| SetSingleShape | Sets the shapes singleCoreM, singleCoreN, and singleCoreK of a single-core Matmul computation, in element count. |
| GetSingleShape | Obtains the computed singleCoreM/singleCoreN/singleCoreK. |
| SetAlignSplit | Sets the alignment values of singleCoreM/singleCoreN/singleCoreK during multi-core splitting. |
| GetCoreNum | Obtains the numBlocks used after multi-core splitting. |
| SetSplitK | Enables K-axis splitting in the multi-core scenario. The EnableMultiCoreSplitK API is recommended. |
| EnableMultiCoreSplitK | Enables K-axis splitting in the multi-core scenario. |

**Table 3** Other APIs of BatchMatmulTiling

| API | Description |
| --- | --- |
| GetCoreNum | Obtains the numBlocks used after multi-core splitting. |

## Header Files to Include

-   Matmul single-core Tiling

    ```
    #include "lib/matmul/matmul_tiling.h"
    ```

-   Matmul multi-core Tiling

    ```
    #include "lib/matmul/bmm_tiling.h"
    ```

-   BatchMatmul Tiling

    ```
    #include "lib/matmul/bmm_tiling.h"
    ```
