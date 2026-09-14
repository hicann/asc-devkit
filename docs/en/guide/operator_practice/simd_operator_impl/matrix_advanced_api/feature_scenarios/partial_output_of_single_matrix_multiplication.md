# Single Matrix Multiplication with Partial Output<a name="ZH-CN_TOPIC_0000002500548100"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-26T13:32:30.768Z -->

## Description<a name="zh-cn_topic_0000002299615601_section310824820358"></a>

Single matrix multiplication partial output is also called Partial Output. As described in [Basic Knowledge](../basic_knowledge.md), during one Iterate computation, one or more basic block computations are performed along the K axis. In one basic block computation, input data of baseM\*baseK and baseK\*baseN sizes are computed to obtain a result of baseM\*baseN size. After the results of each basic block computation are accumulated, the result of baseM\*baseN size is obtained from the input data of baseM\*singleCoreK and singleCoreK\*baseN sizes, and is output as the final result of one Iterate.

After Partial Output is enabled, calling the Iterate API does not perform accumulation along the K axis but only performs a single basic block computation. You can obtain the corresponding single data block through the GetTensorC API and then perform accumulation along the K axis by yourself.

**Figure 1**  Schematic diagram of functional computation when Partial Output is not enabled<a name="zh-cn_topic_0000002299615601_fig59018597286"></a>  
![](../../../../figures/compute_diagram_with_partial_output_not_enabled.png "Schematic diagram of functional computation when Partial Output is not enabled")

**Figure 2**  Schematic diagram of functional computation when Partial Output is enabled<a name="zh-cn_topic_0000002299615601_fig990633219319"></a>  
![](../../../../figures/compute_diagram_with_partial_output_enabled.png "Schematic diagram of functional computation when Partial Output is enabled")

## Scenarios<a name="zh-cn_topic_0000002299615601_section118051016163613"></a>

The matrix multiplication result does not need to be accumulated; only the baseM\*baseN result of the baseM\*baseK and baseK\*baseN computations needs to be output. For example, you may need to first obtain the data of a single basic block computation for dequantization, and then accumulate the results to obtain the final result.

## Constraints<a name="zh-cn_topic_0000002299615601_section14160134220363"></a>

-   This feature is supported only by the [MDL template](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md).
-   When obtaining the matrix multiplication result, only the continuous write mode of the Iterate and GetTensorC APIs is supported. The non-continuous write mode and the IterateAll API are not supported for obtaining the result. For details about the continuous write mode, see [GetTensorC](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/GetTensorC.md).
-   This feature does not support Matmul computation with a Bias matrix, that is, the input of a Bias matrix is not supported.

## Calling Example<a name="zh-cn_topic_0000002299615601_section15486294368"></a>

For the complete operator sample, see the [operator sample with partial output enabled](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_partial_output).

```
// Configure the MDL template and enable Partial Output
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
