# GetMDLConfig

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:21:01.897Z pushedAt=2026-10-27T11:20:16.586Z nl=2 -->

## Applicable Products

<!-- npu="950" id7 -->
- Ascend 950PR&950DT products: Supported
<!-- end id7 -->
<!-- npu="A3" id8 -->
- Atlas A3 products: Supported
<!-- end id8 -->
<!-- npu="910b" id9 -->
- Atlas A2 products: Supported
<!-- end id9 -->
<!-- npu="310b" id10 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id10 -->
<!-- npu="310p" id11 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id11 -->
<!-- npu="910" id12 -->
- Atlas training products: Not supported
<!-- end id12 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetMDLConfig_res.md#id1 -->

## Description

Used to configure the parameters of the MDL template to obtain a custom MDL template. For an introduction to the MDL template, see [template features](MatmulConfig.md#template-features).

## Prototype

```
__aicore__ constexpr MatmulConfig GetMDLConfig(const bool intrinsicsLimit = false, const bool batchLoop = false, const uint32_t doMTE2Preload = 0, const bool isVecND2NZ = false, bool isPerTensor = false, bool hasAntiQuantOffset = false, const bool enUnitFlag = false, const bool isMsgReuse = true, const bool enableUBReuse = true, const bool enableL1CacheUB = false, const bool enableMixDualMaster = false, const bool enableKdimReorderLoad = false)
```

## Parameters

All parameters of this API are used to set the parameters in the [MatmulConfig structure](MatmulConfig.md#matmulconfig-params), where corresponding parameters have the same function.

**Table 1** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| intrinsicsLimit | Input | Used to set the **intrinsicsCheck** parameter.<br><br>When the inner axis (that is, the last axis) of the left cube or the right cube on a single core is greater than or equal to 65535 (number of elements), whether to enable loop-based data loading from Global Memory to the L1 Buffer. For example, for the left cube A[M, K], when the inner axis data singleCoreK on a single core is greater than 65535, after this parameter is set to true, the API internally loads data through a loop. The parameter values are as follows:<br>false: When the inner axis of the left cube or the right cube on a single core is greater than or equal to 65535, loop-based data loading is not enabled (default value).<br>true: When the inner axis of the left cube or the right cube on a single core is greater than or equal to 65535, loop-based data loading is enabled.<!-- npu="950" id13 --><br><br>For the Ascend 950PR&950DT products, the MxMatmul scenario does not support this parameter.<!-- end id13 --> |
| batchLoop | Input | Used to set the **isNBatch** parameter.<br><br>Whether to support multiple-Batch input and multiple-Batch output. This parameter is valid only for BatchMatmul. After multiple-Batch is enabled, only the Norm template is supported, and [IterateNBatch](IterateNBatch.md) must be called to implement multiple-Batch input and multiple-Batch output. The parameter values are as follows:<br>false: Multiple-Batch is not enabled (default value).<br>true: Multiple-Batch is enabled. |
| doMTE2Preload | Input | Used to set the **doMTE2Preload** parameter.<br><br>When the MTE2 pipeline gap is large and the M/N values are large, this parameter can be used to enable the preload function in the corresponding M/N direction. After it is enabled, the MTE2 gap is reduced and performance is improved. The preload function is valid only for the MDL template (the SpecialMDL template is not supported). The parameter values are as follows:<br>0: Not enabled (default value). 1: Enable preload in the M direction. 2: Enable preload in the N direction.<br><br>Note: When enabling the preload function in the M/N direction, ensure that K is fully loaded and DoubleBuffer is enabled in the M/N direction. The K full-load condition in the M direction is: singleCoreK/baseK <= stepKa; the K full-load condition in the N direction is: singleCoreK/baseK <= stepKb. |
| isVecND2NZ | Input | Used to set the **enVecND2NZ** parameter.<br><br>Enable ND2NZ through vector instructions. When enabled, [SetLocalWorkspace](SetLocalWorkspace.md) must be set. The parameter values are as follows:<br>false: ND2NZ through vector instructions is not enabled (default value).<br>true: ND2NZ through vector instructions is enabled.<!-- npu="310p" id14 --><br><br>For the Atlas inference products AI Core, when the Unified Buffer (UB) space is sufficient (the UB space is greater than twice the [transLength](../Matmul_Tiling/TCubeTiling_struct.md#p1620315053211) parameter of TCubeTiling), it is recommended to enable it first for better data transfer performance.<!-- end id14 --><!-- npu="950" id15 --><br><br>For the Ascend 950PR&950DT products, the MxMatmul scenario does not support this parameter.<!-- end id15 --> |
| isPerTensor | Input | Used to set the **isPerTensor** parameter.<br><br>In the scenario where the A cube is of half type input and the B cube is of int8_t type input, whether per-tensor quantization is used when B cube quantization is enabled.<br>true: per tensor quantization.<br>false: per channel quantization.<!-- npu="950" id16 --><br><br>For the Ascend 950PR&950DT products, the MxMatmul scenario does not support this parameter.<!-- end id16 --> |
| hasAntiQuantOffset | Input | Used to set the **hasAntiQuantOffset** parameter.<br><br>In the scenario where the A cube is of half type input and the B cube is of int8_t type input, whether the offset coefficient is used when B cube quantization is enabled.<!-- npu="950" id17 --><br><br>For the Ascend 950PR&950DT products, the MxMatmul scenario does not support this parameter.<!-- end id17 --> |
| enUnitFlag | Input | Used to set the **enUnitFlag** parameter.<br><br>Enable the UnitFlag function to run the computation and data transfer pipelines in parallel, improving performance. It is enabled by default under Norm and IBShare, and not enabled by default under MDL. The parameter values are as follows:<br>false: The UnitFlag function is not enabled.<br>true: The UnitFlag function is enabled.<!-- npu="950" id18 --><br><br>Note: For the MxMatmul scenario of the Ascend 950PR&950DT products, enabling the UnitFlag function brings performance benefits only in the scenario where the NORM/MDL template is used, A and scaleA are not transposed, B and scaleB are transposed, C is in ND format, and the output is to GM.<!-- end id18 --> |
| isMsgReuse | Input | Used to set the **enableReuse** parameter.<br><br>Whether the dataPtr in the callback function set by the [SetSelfDefineData](SetSelfDefineData.md) function directly passes the computation data. If SetSelfDefineData is not called to set dataPtr, this parameter supports only the default value true. The parameter values are as follows:<br>true: Directly pass the computation data, limited to a single value.<br>false: Pass the address information of the data stored in GM.<!-- npu="950" id19 --><br><br>For the Ascend 950PR&950DT products, the MxMatmul scenario does not support this parameter.<!-- end id19 --> |
| enableUBReuse | Input | Used to set the **enableUBReuse** parameter.<br><br>Whether to enable UB reuse. When the UB space is sufficient (the UB space is greater than four times the [transLength](../Matmul_Tiling/TCubeTiling_struct.md#p1620315053211) parameter of TCubeTiling), after UB reuse is enabled, the UB space is divided into two non-overlapping parts, which respectively store the data of two adjacent iterations of Matmul computation. The data loading of the next iteration does not need to wait for the release of the UB space of the previous iteration, thereby optimizing the pipeline. The parameter values are as follows:<br>true: Enable UB reuse.<br>false: Do not enable UB reuse.<!-- npu="950" id20 --><br><br>Including the MxMatmul scenario, the Ascend 950PR&950DT products do not support this parameter.<!-- end id20 --><!-- npu="A3" id21 --><br><br>The Atlas A3 products do not support this parameter.<!-- end id21 --><!-- npu="910b" id22 --><br><br>The Atlas A2 products do not support this parameter.<!-- end id22 --><!-- npu="310p" id23 --><br><br>The Atlas inference products AI Core supports this parameter.<!-- end id23 --><!-- npu="310b" id24 --><br><br>The Atlas 200I/500 A2 inference product does not support this parameter.<!-- end id24 --> |
| enableL1CacheUB | Input | Used to set the **enableL1CacheUB** parameter.<br><br>Whether to enable the L1 Buffer to cache the UB computation block. It is recommended for scenarios where the MTE3 and MTE2 pipelines are largely serial. The parameter values are as follows:<br>true: Enable the L1 Buffer to cache the UB computation block.<br>false: Do not enable the L1 Buffer to cache the UB computation block.<br><br>To enable the L1 Buffer to cache the UB computation block, the [SetMatmulConfigParams](../Matmul_Tiling/SetMatmulConfigParams.md) API must be called in the Tiling implementation to set the **enableL1CacheUBIn** parameter to true.<!-- npu="950" id25 --><br><br>Including the MxMatmul scenario, the Ascend 950PR&950DT products do not support this parameter.<!-- end id25 --><!-- npu="A3" id26 --><br><br>The Atlas A3 products do not support this parameter.<!-- end id26 --><!-- npu="910b" id27 --><br><br>The Atlas A2 products do not support this parameter.<!-- end id27 --><!-- npu="310p" id28 --><br><br>The Atlas inference products AI Core supports this parameter.<!-- end id28 --><!-- npu="310b" id29 --><br><br>The Atlas 200I/500 A2 inference product does not support this parameter.<!-- end id29 --> |
| enableMixDualMaster | Input | Used to set the **enableMixDualMaster** parameter.<br><br>Whether to enable MixDualMaster (dual-master mode). Unlike the MIX mode (which includes cube computation and vector computation), which drives AIC execution through the message mechanism, the dual-master mode runs code independently for AIC and AIV without relying on message driving, and is used to improve performance. The default value of this parameter is false, and it can be set to true only in the following scenarios:<br>The type of the kernel function (Kernel) is MIX, and the AIC core count : AIV core count is 1:1.<br>The type of the kernel function (Kernel) is MIX, the AIC core count : AIV core count is 1:2, and the [IBSHARE](Matmul_usage.md#table1188045714378) parameter is enabled for both the A cube and the B cube.<br><br>Note that when the MixDualMaster scenario is enabled, the following requirements must be met:<br>The value of this parameter for all Matmul objects in the same operator must be consistent.<br>The A/B/Bias cubes support loading only from GM.<br>To obtain the cube computation result, only the [IterateAll](IterateAll.md) API can be called to output to GlobalTensor or LocalTensor, that is, the computation result is placed at an address in Global Memory or Local Memory. APIs such as [GetTensorC](GetTensorC.md) cannot be called to obtain the result.<!-- npu="950" id30 --><br><br>Except for the MxMatmul scenario, the Ascend 950PR&950DT products support this parameter.<!-- end id30 --><!-- npu="A3" id31 --><br><br>The Atlas A3 products support this parameter.<!-- end id31 --><!-- npu="910b" id32 --><br><br>The Atlas A2 products support this parameter.<!-- end id32 --><!-- npu="310p" id33 --><br><br>The Atlas inference products AI Core does not support this parameter.<!-- end id33 --><!-- npu="310b" id34 --><br><br>The Atlas 200I/500 A2 inference product does not support this parameter.<!-- end id34 --> |
| enableKdimReorderLoad | Input | Used to set the **enableKdimReorderLoad** parameter.<br><br>Whether to enable K-axis staggered data loading. Based on the same Tiling parameters, when Matmul computation is executed, if the left cube or the right cube of multiple cores is the same and is stored in Global Memory, multiple cores generally access the same address at the same time to load cube data, causing same-address access conflicts and affecting performance. After K-axis staggered data loading is enabled, when multiple cores execute Matmul, they try to access different Global Memory addresses of the cube at the same time, reducing the probability of address access conflicts and improving performance. This parameter function supports only the MDL template. It is recommended to enable this function when the K axis is large and neither the left cube nor the right cube is fully loaded. The parameter values are as follows.<br>false: Default value. Disable the K-axis staggered loading function.<br>true: Enable the K-axis staggered loading function.<!-- npu="950" id35 --><br><br>Except for the MxMatmul scenario, the Ascend 950PR&950DT products support this parameter.<!-- end id35 --><!-- npu="A3" id36 --><br><br>The Atlas A3 products support this parameter.<!-- end id36 --><!-- npu="910b" id37 --><br><br>The Atlas A2 products support this parameter.<!-- end id37 --><!-- npu="310p" id38 --><br><br>The Atlas inference products AI Core does not support this parameter.<!-- end id38 --><!-- npu="310b" id39 --><br><br>The Atlas 200I/500 A2 inference product does not support this parameter.<!-- end id39 --> |

## Return Value

[MatmulConfig structure](MatmulConfig.md#matmulconfig-params).

## Constraints

None

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetMDLConfig_res.md#id2 -->

## Examples

```
// Configure the parameters of the MDL template to obtain a custom MDL template.
constexpr MatmulConfig MM_CFG = GetMDLConfig(
    /* intrinsicsLimit      */ false,
    /* batchLoop            */ false,
    /* doMTE2Preload        */ 0,
    /* isVecND2NZ           */ false,
    /* isPerTensor          */ false,
    /* hasAntiQuantOffset   */ false,
    /* enUnitFlag           */ false,
    /* isMsgReuse           */ true,
    /* enableUBReuse        */ true,
    /* enableL1CacheUB      */ false,
    /* enableMixDualMaster  */ false,
    /* enableKdimReorderLoad*/ false);
// For regular Matmul computation, output the computation result using the custom MDL template.
AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG> mm;
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll(gm_c);
mm.End();
```
