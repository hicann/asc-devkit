# Multi-Core Aligned Partitioning<a name="ZH-CN_TOPIC_0000002532228165"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:33:01.707Z -->

## Description<a name="zh-cn_topic_0000002298767889_section16205164243819"></a>

To implement multi-core parallelism and improve computation efficiency, matrix data needs to be partitioned and distributed to different cores for processing. The main partitioning strategies are partitioning the K axis and not partitioning the K axis.

The strategy of not partitioning the K axis and partitioning only the M and N axes is as follows:

-   For matrix A, partition along the M axis into multiple singleCoreM blocks. Each core processes data of size SingleCoreM \* K.
-   For matrix B, partition along the N axis into multiple singleCoreN blocks. Each core processes data of size K \* SingleCoreN.
-   For matrix C, the A matrix of size SingleCoreM \* K is multiplied by the B matrix of size K \* SingleCoreN to obtain the C matrix of size SingleCoreM \* SingleCoreN, which is the size of the C matrix output on a single core.

For example, in the following figure, eight cores participate in the computation. Matrix A is partitioned into four blocks along the M axis, and matrix B is partitioned into two blocks along the N axis. Each core processes only one block (for example, the green part in the figure is the data involved in the computation on core5): the A matrix block of size SingleCoreM \* K is multiplied by the B matrix block of size SingleCoreN \* K to obtain the C matrix block of size SingleCoreM \* SingleCoreN.

![](../../../../figures/nd2nz_conversion_diagram_new_44.png)

The strategy of partitioning the M, N, and K axes is shown in the following figure:

-   For matrix A, partition along the M axis into multiple singleCoreM blocks, and partition along the K axis into multiple singleCoreK blocks. Each core processes data of size singleCoreM \* singleCoreK.
-   For matrix B, partition it along the K axis into multiple singleCoreK blocks, and along the N axis into multiple singleCoreN blocks. Each core processes data of the size singleCoreK \* singleCoreN.
-   For matrix C, the A matrix of the size singleCoreM \* singleCoreK is multiplied by the B matrix of the size singleCoreK \* singleCoreN, and the results are accumulated to obtain a C matrix block of the size singleCoreM \* singleCoreN.

For example, in the following figure, the R matrix block in matrix C is obtained by accumulating A1\*B1+A2\*B2+A3\*B3, where A1\*B1, A2\*B2, and A3\*B3 can be computed in parallel on multiple cores.

![](../../../../figures/nd2nz_conversion_diagram_2_new.png)

The preceding partitioning strategy is reflected in the Tiling parameters, such as SingleCoreM, SingleCoreN, and SingleCoreK. On the host side, developers automatically obtain the Tiling parameters by calling APIs. Unlike the single-core scenario, multi-core Tiling requires using [MultiCoreMatmulTiling](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/matmul_tiling_class_constructor.md) to construct a multi-core tiling object, and using [SetDim](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/SetDim.md) to set the number of cores used for Matmul computation. Note: The number of cores set here is the number of cores available for Matmul computation. It is set only in the multi-core scenario and is used to compute the Tiling parameters. SetBlockDim specifies the number of cores used for the entire operator computation, which is the number of cores actually loaded and must be set. For the configuration rule of SetBlockDim, see [the description of numBlocks](../../../../programming_guide/advanced_programming/aclnn_operator_development/host_tiling_implementation/basic_process.md#li1153191243910). The configuration rule of SetDim is as follows:

-   In the pure Cube mode (only Cube computation), this section uses the pure Cube mode as an example.

    [SetDim](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/SetDim.md) sets the number of cores available on the current AI processor. The number of cores actually used for Matmul computation is obtained through [Tiling computation](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Tiling/GetTiling.md). The number of cores actually used is less than or equal to the number of cores available on the AI processor. SetBlockDim is configured by the user based on the number of cores actually used.

-   For the configuration rule in the MIX mode (including Cube computation and vector computation), see [MIX Scenario Core Count Configuration Rule](../../fusion_operator_programming/cv_fusion/operator_implementation.md#zh-cn_topic_0000001644252364_li4790115115920).

## Scenarios<a name="zh-cn_topic_0000002298767889_section86181999397"></a>

Multi-core processing of Matmul Cube computation scenarios.

## Constraints<a name="zh-cn_topic_0000002298767889_section3388164212391"></a>

None

## Calling Example<a name="zh-cn_topic_0000002298767889_section14108832203920"></a>

The key code example for this scenario is as follows. For the complete sample of the Matmul multi-core alignment scenario, see: sample of multi-core partitioning of M and N: [Matmul Multi-core Kernel Direct Invocation Sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/00_introduction/02_matrix/matmul_advanced_api); sample of multi-core partitioning of K: [Operator Sample for the Multi-core K Partitioning Scenario](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_splitk).

```
// Construct a multi-core Tiling object
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
matmul_tiling::MultiCoreMatmulTiling cubeTiling(*ascendcPlatform);
// For an operator that contains only Cube computation, set the number of cores that can participate in matrix multiplication to the number of Cube cores on the current AI processor
cubeTiling.SetDim(ascendcPlatform.GetCoreNumAic());
cubeTiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
cubeTiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
cubeTiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
cubeTiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT);
cubeTiling.SetOrgShape(M, N, K);
cubeTiling.SetShape(M, N, K);
cubeTiling.EnableBias(isBias);
optiling::TCubeTiling tilingData;  
// Obtain the Tiling parameters
int ret = cubeTiling.GetTiling(tilingData);    // if ret = -1, gen tiling failed 
```
