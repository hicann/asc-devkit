# Partial Output of Single Cube Multiplication<a name="ZH-CN_TOPIC_0000002500548100"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T12:12:40.648Z pushedAt=2026-11-03T07:05:13.307Z nl=2 -->

## Feature Overview<a name="zh-cn_topic_0000002299615601_section310824820358"></a>

Partial output of a single cube multiplication, also known as Partial Output. As described in [Basics](../basics.md), during one Iterate computation, one or more basic block computations are performed along the K direction. In one basic block computation, input data of size baseM\*baseK and baseK\*baseN is computed to obtain a result of size baseM\*baseN. After the results of each basic block computation are accumulated, the result baseM\*baseN obtained by computing the input data of size baseM\*singleCoreK and singleCoreK\*baseN is obtained and output as the final result of one Iterate.

After Partial Output is enabled, calling the Iterate API does not perform K-axis accumulation but only performs a single basic block computation. You can obtain the corresponding single-tile data through the GetTensorC API and then perform K-axis accumulation on your own.

**Figure 1**  Schematic diagram of computation without Partial Output enabled<a name="zh-cn_topic_0000002299615601_fig59018597286"></a>  
![](../../../../figures/no_partial_out.png "Schematic diagram of computation without Partial Output enabled")

**Figure 2**  Schematic diagram of computation with Partial Output enabled<a name="zh-cn_topic_0000002299615601_fig990633219319"></a>  
![](../../../../figures/partial_out.png "Schematic diagram of computation with Partial Output enabled")

## Scenarios<a name="zh-cn_topic_0000002299615601_section118051016163613"></a>

The cube multiplication result does not need to be accumulated; only the computation results of baseM\*baseK and baseK\*baseN need to be output baseM\*baseN. For example, the data of a single basic block computation needs to be obtained first for dequantization, and then accumulated to obtain the final result.

## Constraints<a name="zh-cn_topic_0000002299615601_section14160134220363"></a>

-   This feature is supported only for the [MDL template](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulConfig.md).
-   When obtaining the cube multiplication result, only the continuous write mode of the Iterate and GetTensorC APIs is supported. The non-continuous write mode and the IterateAll API are not supported for obtaining the result. For details about the continuous write mode, see [GetTensorC](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetTensorC.md).
-   This feature does not support Matmul computation with a Bias cube, that is, the input of a Bias cube is not supported.

## Example<a name="zh-cn_topic_0000002299615601_section15486294368"></a>

For the complete operator sample, see [Sample Operator with Partial Output Enabled](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_partial_output).

```
// Configure the MDL template to enable Partial Output.
constexpr static MatmulConfigMode configMode = MatmulConfigMode::CONFIG_MDL;
constexpr static MatmulFuncParams funcParams = {
  false, false, false, false, 0, IterateOrder::UNDEF, ScheduleType::INNER_PRODUCT, true, true,
  true /* isPartialOutput */
};
constexpr static MatmulConfig CFG_PARTIAL = GetMMConfig<configMode>(funcParams);
Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_PARTIAL> mm;
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm);
mm.Init(&tiling);
mm.SetTensorA(gmA, isTransposeA);
mm.SetTensorB(gmB, isTransposeB);
while (mm.Iterate()) {
    mm.GetTensorC(tmpGmC[dstOffset], false, true);
    dstOffset += baseM * baseN;
    // Other operations
}
```
