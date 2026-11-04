# Batch Matmul Reuse of Bias Matrix<a name="ZH-CN_TOPIC_0000002500548102"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T12:05:08.857Z pushedAt=2026-11-03T07:05:13.299Z nl=2 -->

## Feature Overview<a name="zh-cn_topic_0000002373911153_section1953745712231"></a>

In Batch Matmul scenarios, the Matmul API can compute multiple C cubes of size singleCoreM \* singleCoreN at a time. When a Batch Matmul scenario has Bias input, the default Bias input cube contains a Batch axis, that is, the size of Bias is Batch \* N. By enabling the Bias reuse feature, when the Bias data used by each Batch computation is the same, you only need to input a Bias cube without a Batch axis. The Bias cube reuse feature of Batch Matmul is disabled by default. To enable this feature, set the isBiasBatch parameter in [MatmulConfig](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulConfig.md) to false.

**Figure 1**  Bias computation schematic with a Batch axis<a name="zh-cn_topic_0000002373911153_fig889710263325"></a>  
![](../../../../figures/batch_bias.png "Bias computation schematic with a Batch axis")

As shown in the preceding figure, in the scenario where the Bias cube is not reused in Batch Matmul, each cube C of size singleCoreM \* singleCoreN computed is added to a Bias cube of size 1 \* singleCoreN. If the Bias data used by different Batch computations is the same, multiple Batch computations can reuse the same Bias cube, as shown in the following figure. In this scenario, when calling [SetBias](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetBias.md), you only need to set a Bias cube of size 1 \* singleCoreN.

**Figure 2**  Bias reuse computation schematic<a name="zh-cn_topic_0000002373911153_fig1485726153719"></a>  
![](../../../../figures/bias_reuse.png "Bias reuse computation schematic")

## Scenarios<a name="zh-cn_topic_0000002373911153_section1672510573541"></a>

In Batch Matmul, the Matmul computation of each batch can use the same Bias cube.

## Constraints<a name="zh-cn_topic_0000002373911153_section3618165412115"></a>

When the Layout types of cubes A, B, and C are all NORMAL, setting the [batchMode](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulConfig.md) parameter to SINGLE\_LARGE\_THAN\_L1 is not supported. That is, in the Bias reuse scenario, the total data size of cubes A and B in a single Batch must not exceed the L1 Buffer size.

## Example<a name="zh-cn_topic_0000002373911153_section18018102212"></a>

For the complete operator sample, see [BatchMatmul Bias Reuse Operator Sample](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/batch_matmul_bias_reuse).

```
// Customize the MatmulConfig parameters and set isBiasBatch to false to enable the Bias reuse feature of BatchMatmul.
constexpr MatmulConfigMode configMode = MatmulConfigMode::CONFIG_NORM;
constexpr MatmulBatchParams batchParams = {
  false, BatchMode::BATCH_LESS_THAN_L1, false /* isBiasBatch */
};
constexpr MatmulConfig CFG_MM = GetMMConfig<configMode>(batchParams);
AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MM> mm;

REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling); // Initialize the matmul object.
mm.SetTensorA(gm_a);    // Set the left cube A.
mm.SetTensorB(gm_b);    // Set the right cube B.
mm.SetBias(gm_bias);    // Set the Bias, whose size is 1 * singleCoreN.
mm.IterateBatch(gm_c, batchA, batchB, false);
mm.End();
```
