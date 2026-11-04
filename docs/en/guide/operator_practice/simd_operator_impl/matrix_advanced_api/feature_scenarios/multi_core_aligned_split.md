# Multi-Core Aligned Splitting<a name="ZH-CN_TOPIC_0000002532228165"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:10:27.663Z pushedAt=2026-11-03T07:05:13.306Z nl=2 -->

## Feature Overview<a name="zh-cn_topic_0000002298767889_section16205164243819"></a>

To implement multi-core parallelism and improve computation efficiency, cube data needs to be split and distributed to different cores for processing. The main splitting strategies are splitting the K axis and not splitting the K axis.

The strategy of not splitting the K axis and splitting only the M and N axes is as follows:

-   For cube A, split it along the M axis into multiple singleCoreM blocks. Each core processes data of size SingleCoreM \* K.
-   For cube B, split it along the N axis into multiple singleCoreN blocks. Each core processes data of size K \* SingleCoreN.
-   For cube C, the cube A of size SingleCoreM \* K is multiplied by the cube B of size K \* SingleCoreN to obtain the cube C of size SingleCoreM \* SingleCoreN, which is the size of the cube C output on a single core.

For example, in the following figure, eight cores participate in the computation. Cube A is divided into four blocks along the M axis, and cube B is split into two blocks along the N axis. Each core processes only one block (for example, the green part in the figure is the data involved in the computation on core5): the Cube A block of size SingleCoreM \* K is multiplied by the cube B block of size SingleCoreN \* K to obtain the cube C block of size SingleCoreM \* SingleCoreN.

![](../../../../figures/nd2nz_new_44.png)

The strategy of splitting the M, N, and K axes is shown in the following figure:

-   For cube A, split it along the M axis into multiple singleCoreM blocks and along the K axis into multiple singleCoreK blocks. Each core processes data of size singleCoreM \* singleCoreK.
-   For cube B, split it along the K axis into multiple singleCoreK blocks, and split it along the N axis into multiple singleCoreN blocks. Each core processes data of singleCoreK \* singleCoreN in size.
-   For cube C, the Cube A block of singleCoreM \* singleCoreK in size is multiplied by the cube B block of singleCoreK \* singleCoreN in size, and the results are accumulated to obtain the cube C block of singleCoreM \* singleCoreN in size.

For example, in the following figure, the R cube block in cube C is obtained by accumulating A1\*B1+A2\*B2+A3\*B3, where A1\*B1, A2\*B2, and A3\*B3 can be computed in parallel on multiple cores.

![](../../../../figures/nd2nz_new_2.png)

The splitting strategy described above is reflected in the Tiling parameters, such as SingleCoreM, SingleCoreN, and SingleCoreK. On the host side, developers obtain the Tiling parameters automatically by calling APIs. Unlike the single-core scenario, multi-core Tiling requires using [MultiCoreMatmulTiling](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/Matmul_Tiling_constructor.md) to construct a multi-core Tiling object, and using the [SetDim](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetDim.md) API to set the number of cores used for Matmul computation. Note: The number of cores set here is the number of cores available for Matmul computation. It is set only in the multi-core scenario and is used to compute the Tiling parameters. SetSimdNumBlocks specifies the number of cores used for the entire operator computation, which is the number of cores actually loaded and must be set. For the setting rules of SetSimdNumBlocks, see [Description of numBlocks](../../../../programming_guide/advanced_programming/aclnn_operator_development/design_and_implementation/host_tiling_implementation.md#numblocks). The setting rules of SetDim are as follows:

-   In the cube-only mode (cube computation only), this section uses the cube-only mode as an example.

    [SetDim](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetDim.md) sets the number of cores available on the current AI processor. The number of cores actually used for Matmul computation is obtained through [Tiling computation](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/GetTiling.md). The number of cores actually used is less than or equal to the number of cores available on the AI processor. SetSimdNumBlocks is configured by the user based on the number of cores actually used.

-   For the setting rules of the MIX mode (including both cube computation and vector computation), see [Core Count Setting Rules in the MIX Scenario](../../fusion_operator_programming/cv_fusion/operator_impl.md#zh-cn_topic_0000001644252364_li4790115115920).

## Scenarios<a name="zh-cn_topic_0000002298767889_section86181999397"></a>

Multi-core processing of Matmul cube computation scenarios.

## Constraints<a name="zh-cn_topic_0000002298767889_section3388164212391"></a>

None

## Example<a name="zh-cn_topic_0000002298767889_section14108832203920"></a>

The key code example for this scenario is as follows. For the complete sample of the Matmul multi-core alignment scenario, see: sample of multi-core splitting of M and N: [Kernel direct call sample for the Matmul multi-core scenario](../../../../../../../examples/01_simd_cpp_api/00_introduction/02_matrix/matmul_advanced_api); sample of multi-core splitting of K: [Operator sample for the multi-core K-splitting scenario](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_splitk).

```
// Construct a multi-core Tiling object
auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
matmul_tiling::MultiCoreMatmulTiling cubeTiling(*ascendcPlatform);
// For an operator that contains only Cube computation, set the number of cores that can participate in cube multiplication to the number of Cube cores on the current AI processor
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
