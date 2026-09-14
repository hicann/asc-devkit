# Reusing the Bias Matrix in Batch Matmul<a name="ZH-CN_TOPIC_0000002500548102"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:41:45.016Z -->

## Description<a name="zh-cn_topic_0000002373911153_section1953745712231"></a>

In the Batch Matmul scenario, the Matmul API can compute multiple C matrices of size singleCoreM \* singleCoreN at a time. When the Batch Matmul scenario has a Bias input, the default Bias input matrix contains a Batch axis, that is, the size of the Bias is Batch \* N. By enabling the Bias reuse feature, when the Bias data used by each Batch computation is the same, you only need to input a Bias matrix without a Batch axis. The Bias matrix reuse feature of Batch Matmul is disabled by default. To enable this feature, you need to set the isBiasBatch parameter in [MatmulConfig](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md) to false.

**Figure 1**  Bias computation diagram with Batch axis<a name="zh-cn_topic_0000002373911153_fig889710263325"></a>  
![](../../../../figures/bias_computation_diagram_with_batch_axis.png "Bias computation diagram with Batch axis")

As shown in the preceding figure, in the Batch Matmul scenario where the Bias matrix is not reused, each time a C matrix of size singleCoreM \* singleCoreN is computed, it is added to a Bias matrix of size 1 \* singleCoreN. If the Bias data used by different Batch computations is the same, multiple Batch computations can reuse the same Bias matrix, as shown in the following figure. In this scenario, when calling the [SetBias](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/SetBias.md) API, you only need to set a Bias matrix of size 1 \* singleCoreN.

**Figure 2**  Bias reuse computation diagram<a name="zh-cn_topic_0000002373911153_fig1485726153719"></a>  
![](../../../../figures/bias_reuse_computation_diagram.png "Bias reuse computation diagram")

## Scenarios<a name="zh-cn_topic_0000002373911153_section1672510573541"></a>

In Batch Matmul, the Matmul computation of each batch can use the same Bias matrix.

## Constraints<a name="zh-cn_topic_0000002373911153_section3618165412115"></a>

When the Layout type of matrices A, B, and C is NORMAL, the [batchMode](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md) parameter cannot be set to SINGLE\_LARGE\_THAN\_L1. That is, in the Bias reuse scenario, the total size of matrices A and B in a single batch must not exceed the L1 Buffer size.

## Calling Example<a name="zh-cn_topic_0000002373911153_section18018102212"></a>

For the complete operator sample, see [BatchMatmul Bias Reuse Operator Sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/batch_matmul_bias_reuse).

```
// Customize the MatmulConfig parameters and set the isBiasBatch parameter to false to enable the Bias reuse feature of BatchMatmul.
constexpr MatmulConfigMode configMode = MatmulConfigMode::CONFIG_NORM;
constexpr MatmulBatchParams batchParams = {
  false, BatchMode::BATCH_LESS_THAN_L1, false /* isBiasBatch */
};
constexpr MatmulConfig CFG_MM = GetMMConfig<configMode>(batchParams);
AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MM> mm;

REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling); // Initialize the matmul object
mm.SetTensorA(gm_a);    // Set left matrix A
mm.SetTensorB(gm_b);    // Set right matrix B
mm.SetBias(gm_bias);    // Set Bias. The matrix size is 1 * singleCoreN
mm.IterateBatch(gm_c, batchA, batchB, false);
mm.End();
```
