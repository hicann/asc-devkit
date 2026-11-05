# GetMMConfig

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:22:48.633Z pushedAt=2026-10-27T11:20:57.699Z nl=2 -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetMMConfig_res.md#id1 -->

## Description

Flexible custom Matmul template parameter configuration. By setting [MatmulConfigMode](#table17837129144319), [MatmulShapeParams](#table16317184295116), [MatmulQuantParams](#table8313111211573), [MatmulBatchParams](#table15129204644), and [MatmulFuncParams](#table66217141862), obtain the custom [MatmulConfig](MatmulConfig.md#matmulconfig-params).

**MatmulConfigMode** specifies the MatmulConfig template to be obtained and modified. For details about each template, see [template features](MatmulConfig.md#template-features). Based on usage requirements, users set variable parameters, that is, one or more **MatmulShapeParams**, **MatmulQuantParams**, **MatmulBatchParams**, and **MatmulFuncParams** in any order, to modify the corresponding parameter configuration of the MatmulConfig template. Compared with APIs that obtain templates such as [GetNormalConfig](GetNormalConfig.md) and [GetMDLConfig](GetMDLConfig.md), this API provides a more flexible way to configure custom Matmul template parameters.

## Prototype

```
template <MatmulConfigMode configMode, typename... ArgTypes>
__aicore__ inline constexpr MatmulConfig GetMMConfig(ArgTypes&&... args)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| **configMode** | MatmulConfig template to be obtained. |
| **ArgTypes** | Variadic template parameters. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **args** | Input | Variadic parameters, passing one or more of **MatmulShapeParams**, **MatmulQuantParams**, **MatmulBatchParams**, and **MatmulFuncParams** to be set in any order. |

**Table 3**  MatmulConfigMode parameters

<a name="table17837129144319"></a>

| Parameter | Description |
| --- | --- |
| **CONFIG_NORM** | Indicates setting the default value of MatmulConfig to the Norm template. |
| **CONFIG_MDL** | Indicates setting the default value of MatmulConfig to the MDL template. |
| **CONFIG_SPECIALMDL** | Indicates setting the default value of MatmulConfig to the SpecialMDL template. |
| **CONFIG_IBSHARE** | Indicates setting the default value of MatmulConfig to the IBShare template. |

**Table 4**  MatmulShapeParams parameters

<a name="table16317184295116"></a>

| Parameter | Data Type | Description |
| --- | --- | --- |
| **singleCoreM** | uint32_t | M-axis shape size within a single core, in elements. |
| **singleCoreN** | uint32_t | N-axis shape size within a single core, in elements. |
| **singleCoreK** | uint32_t | K-axis shape size within a single core, in elements. |
| **basicM** | uint32_t | Same meaning as the **baseM** parameter in the [TCubeTiling structure](../Matmul_Tiling/TCubeTiling_struct.md). Base block M-axis length during Matmul computation, in elements. |
| **basicN** | uint32_t | Same meaning as the **baseN** parameter in the [TCubeTiling structure](../Matmul_Tiling/TCubeTiling_struct.md). Base block N-axis length during Matmul computation, in elements. |
| **basicK** | uint32_t | Same meaning as the **baseK** parameter in the [TCubeTiling structure](../Matmul_Tiling/TCubeTiling_struct.md). Base block K-axis length during Matmul computation, in elements. |

**Table 5**  MatmulQuantParams parameters

<a name="table8313111211573"></a>

| Parameter | Data Type | Description |
| --- | --- | --- |
| isPerTensor | bool | In the scenario where cube A is of half type input and cube B is of int8_t type input, whether per tensor is used when quantization of cube B is enabled.<br>true: per tensor quantization. false: per channel quantization.<!-- npu="950" id9 --><br><br>For Ascend 950PR&950DT products, the MxMatmul scenario does not support this parameter.<!-- end id9 --> |
| hasAntiQuantOffset | bool | In the scenario where cube A is of half type input and cube B is of int8_t type input, whether the offset coefficient is used when quantization of cube B is enabled.<!-- npu="950" id10 --><br><br>For Ascend 950PR&950DT products, the MxMatmul scenario does not support this parameter.<!-- end id10 --> |

**Table 6** MatmulBatchParams parameters

<a name="table15129204644"></a>

| Parameter | Data Type | Description |
| --- | --- | --- |
| isNBatch | bool | Whether to use multi-batch input and multi-batch output. This parameter is valid only for BatchMatmul. After multi-batch is enabled, only the Norm template is supported, and [IterateNBatch](IterateNBatch.md) must be called to implement multi-batch input and multi-batch output. The parameter values are as follows:<br>false: Multi-batch is not enabled (default value).<br>true: Multi-batch is enabled. |
| batchMode | BatchMode | In the BatchMatmul scenario, when the Layout type is NORMAL, set the relationship between the total size of the multi-batch data of the input A/B matrices and the L1 Buffer size. The parameter values are as follows:<br>BatchMode::BATCH_LESS_THAN_L1: total data size of multiple batches < L1 Buffer Size;<br>BatchMode::BATCH_LARGE_THAN_L1: total data size of multiple batches > L1 Buffer Size;<br>BatchMode::SINGLE_LARGE_THAN_L1: total data size of a single batch > L1 Buffer Size. |
| isBiasBatch | bool | In the batch multi-batch Matmul scenario, that is, the BatchMatmul scenario, whether the Bias size has a Batch axis. The parameter values are as follows:<br>true: Bias has a Batch axis, and the Bias size is Batch * N (default value).<br>false: Bias does not have a Batch axis, and the Bias size is N. When Matmul is computed for multiple batches, Bias is reused.<br>Note: The BatchMode::SINGLE_LARGE_THAN_L1 scenario supports only the value true.<!-- npu="950" id14 --><br><br>Except for the MxMatmul scenario, Ascend 950PR&950DT products support this parameter.<!-- end id14 --><!-- npu="910b" id15 --><br><br>Atlas A2 products support this parameter.<!-- end id15 --><!-- npu="A3" id16 --><br><br>Atlas A3 products support this parameter.<!-- end id16 --><!-- npu="310p" id17 --><br><br>The Atlas inference products AI Core does not support setting it to false.<!-- end id17 --><!-- npu="310b" id18 --><br><br>Atlas 200I/500 A2 inference products do not support setting it to false.<!-- end id18 --> |
| bmmOutMode | BatchOutMode | Reserved parameter. |

**Table 7** MatmulFuncParams parameters

<a name="table66217141862"></a>

| Parameter | Data Type | Description |
| --- | --- | --- |
| intrinsicsLimit | bool | When the inner axis (that is, the last axis) of the left cube or right cube on a single core is greater than or equal to 65535 (number of elements), whether to enable loop execution of data transfer from Global Memory to L1 Buffer. For example, for the left cube A[M, K], if the inner axis data singleCoreK on a single core is greater than 65535, after this parameter is set to true, the API internally transfers data through loop execution. The parameter values are as follows:<br>false: When the inner axis of the left cube or right cube on a single core is greater than or equal to 65535, loop execution of data transfer is not enabled (default value).<br>true: When the inner axis of the left cube or right cube on a single core is greater than or equal to 65535, loop execution of data transfer is enabled.<!-- npu="950" id19 --><br><br>For Ascend 950PR&950DT products, the MxMatmul scenario does not support this parameter.<!-- end id19 --> |
| enVecND2NZ | bool | Enables ND2NZ through vector instructions. When enabled, [SetLocalWorkspace](SetLocalWorkspace.md) must be set. The parameter values are as follows:<br>false: ND2NZ through vector instructions is not enabled (default value).<br>true: ND2NZ through vector instructions is enabled.<!-- npu="310p" id20 --><br><br>For the Atlas inference products AI Core, when the Unified Buffer (UB) space is sufficient (the UB space is greater than twice the [transLength](../Matmul_Tiling/TCubeTiling_struct.md#p1620315053211) parameter of TCubeTiling), it is recommended to enable it first for better transfer performance.<!-- end id20 --><!-- npu="950" id21 --><br><br>For Ascend 950PR&950DT products, the MxMatmul scenario does not support this parameter.<!-- end id21 --> |
| enableDoubleCache | bool | After the IBShare template is enabled, whether to cache two blocks of data in the L1 Buffer at the same time. The parameter values are as follows:<br>false: One block of data is cached in the L1 Buffer at the same time (default value).<br>true: Caching two blocks of data in the L1 Buffer at the same time is enabled.<br><br>Note: When this parameter is set to true, the basic block size must be controlled to prevent the caching of two blocks of data from exceeding the L1 Buffer size limit.<!-- npu="950" id22 --><br><br>Including the MxMatmul scenario, Ascend 950PR&950DT products do not support this parameter.<!-- end id22 --> |
| enableL1CacheUB | bool | Whether to enable L1 Buffer caching of the UB computation block. It is recommended to use this in scenarios with many serialized MTE3 and MTE2 pipelines. The parameter values are as follows:<br>true: L1 Buffer caching of the UB computation block is enabled.<br>false: L1 Buffer caching of the UB computation block is not enabled.<br><br>To enable L1 Buffer caching of the UB computation block, the [SetMatmulConfigParams](../Matmul_Tiling/SetMatmulConfigParams.md) API must be called in the Tiling implementation to set the parameter enableL1CacheUBIn to true.<!-- npu="950" id23 --><br><br>Including the MxMatmul scenario, Ascend 950PR&950DT products do not support this parameter.<!-- end id23 --><!-- npu="A3" id24 --><br><br>Atlas A3 products do not support this parameter.<!-- end id24 --><!-- npu="910b" id25 --><br><br>Atlas A2 products do not support this parameter.<!-- end id25 --><!-- npu="310p" id26 --><br><br>The Atlas inference products AI Core supports this parameter.<!-- end id26 --><!-- npu="310b" id27 --><br><br>Atlas 200I/500 A2 inference products do not support this parameter.<!-- end id27 --> |
| doMTE2Preload | uint32_t | When the MTE2 pipeline gap is large and the M/N values are large, this parameter can be used to enable the preload function in the corresponding M/N direction. After it is enabled, the MTE2 gap is reduced and performance is improved. The preload function is valid only for the MDL template (the SpecialMDL template is not supported). The parameter values are as follows:<br>0: Not enabled (default value).<br>1: M-direction preload is enabled.<br>2: N-direction preload is enabled.<br><br>Note: When enabling the preload function in the M/N direction, ensure that K is fully loaded and DoubleBuffer is enabled in the M/N direction. The K full-load condition in the M direction is: singleCoreK/baseK <= stepKa; the K full-load condition in the N direction is: singleCoreK/baseK <= stepKb. |
| iterateOrder | IterateOrder | The loop iteration order of Matmul cube computation, which has the same parameter meaning as the iterateOrder parameter in [Table 1](../Matmul_Tiling/TCubeTiling_struct.md#tcubetiling-struct). This parameter takes effect when the ScheduleType parameter is set to ScheduleType::OUTER_PRODUCT. The parameter values are as follows:<br><br>ORDER_M: Offset in the M-axis direction first, then in the N-axis direction.<br><br>ORDER_N: Offset in the N-axis direction first, then in the M-axis direction.<br><br>UNDEF: Currently invalid.<br><br>Note: In the Norm template Matmul scenario and when the MDL template is used, if IterateOrder is set to ORDER_M, stepN in the [TCubeTiling structure](../Matmul_Tiling/TCubeTiling_struct.md#tcubetiling-struct) must be greater than 1; if IterateOrder is set to ORDER_N, stepM in the TCubeTiling structure must be greater than 1. MxMatmul supports only the MDL template.<!-- npu="A3" id28 --><br><br>Atlas A3 products support this parameter.<!-- end id28 --><!-- npu="910b" id29 --><br><br>Atlas A2 products support this parameter.<!-- end id29 --><!-- npu="310p" id30 --><br><br>The Atlas inference products AI Core does not support this parameter.<!-- end id30 --><!-- npu="310b" id31 --><br><br>Atlas 200I/500 A2 inference products do not support this parameter.<!-- end id31 --> |
| scheduleType | ScheduleType | Configures the Matmul data transfer mode. The parameter values are as follows:<br>ScheduleType::INNER_PRODUCT: Default mode, which performs MTE1 loop transfer in the K direction;<br>ScheduleType::OUTER_PRODUCT: Performs MTE1 loop transfer in the M or N direction. After this value is set, it must be used together with the IterateOrder parameter. This configuration currently takes effect only in the BatchMatmul scenario (with the Norm template enabled) or the Matmul scenario (with the MDL template or Norm template enabled).<br>If IterateOrder is set to ORDER_M, loop transfer is performed in the N direction (performance may be improved when singleCoreN is greater than baseN), that is, the MTE1 transfer of cube B is parallelized;<br>If IterateOrder is set to ORDER_N, loop transfer is performed in the M direction (performance may be improved when singleCoreM is greater than baseM), that is, the MTE1 transfer of cube A is parallelized;<br>Loop transfer in the M direction and N direction cannot be enabled at the same time;<br><br>Note:<br>In the Norm template Batch Matmul scenario or the MDL template, when singleCoreK > baseK, the value ScheduleType::OUTER_PRODUCT cannot be set, and the default mode must be used.<br>In the Norm template or MDL template Matmul scenario, ScheduleType::OUTER_PRODUCT can be configured only in pure Cube mode (only cube computation).<br>The MDL template supports configuring ScheduleType::OUTER_PRODUCT only in the scenario computed by calling [IterateAll](IterateAll.md).<br>ScheduleType::OUTER_PRODUCT can be configured only when the C cube is output to GM.<!-- npu="950" id32 --><br><br>Ascend 950PR&950DT products support this parameter.<!-- end id32 --><!-- npu="A3" id33 --><br><br>Atlas A3 products support this parameter.<!-- end id33 --><!-- npu="910b" id34 --><br><br>Atlas A2 products support this parameter.<!-- end id34 --><!-- npu="310p" id35 --><br><br>The Atlas inference products AI Core does not support this parameter.<!-- end id35 --><!-- npu="310b" id36 --><br><br>Atlas 200I/500 A2 inference products do not support this parameter.<!-- end id36 --> |
| enableReuse | bool | Whether the dataPtr in the callback function set by the [SetSelfDefineData](SetSelfDefineData.md) function directly passes the computation data. If SetSelfDefineData is not called to set dataPtr, this parameter supports only the default value true. The parameter values are as follows:<br>true: Directly passes the computation data, limited to a single value.<br>false: Passes the address information of the data stored on GM.<!-- npu="950" id37 --><br><br>For Ascend 950PR&950DT products, the MxMatmul scenario does not support this parameter.<!-- end id37 --> |
| enableUBReuse | bool | Whether to enable UB reuse. When the UB space is sufficient (the UB space is greater than four times the [transLength](../Matmul_Tiling/TCubeTiling_struct.md#p1620315053211) parameter of TCubeTiling), after UB reuse is enabled, the UB space is divided into two non-overlapping parts, which respectively store the data of two adjacent iterations of Matmul computation. The transfer of the data of the next iteration does not need to wait for the release of the UB space of the previous iteration, thereby optimizing the pipeline. The parameter values are as follows:<br>true: UB reuse is enabled.<br>false: UB reuse is not enabled.<!-- npu="950" id38 --><br><br>Including the MxMatmul scenario, Ascend 950PR&950DT products do not support this parameter.<!-- end id38 --><!-- npu="A3" id39 --><br><br>Atlas A3 products do not support this parameter.<!-- end id39 --><!-- npu="910b" id40 --><br><br>Atlas A2 products do not support this parameter.<!-- end id40 --><!-- npu="310p" id41 --><br><br>The Atlas inference products AI Core supports this parameter.<!-- end id41 --><!-- npu="310b" id42 --><br><br>Atlas 200I/500 A2 inference products do not support this parameter.<!-- end id42 --> |
| isPartialOutput | bool | Whether to enable the PartialOutput function, that is, to control the basic block computation method of the K direction output by Matmul in sequence: whether the K axis of one Matmul Iterate computation performs accumulation. The parameter values are as follows:<br>true: The PartialOutput function is enabled. The K axis of one Iterate does not perform accumulation, and each Matmul computation outputs a cube fragment of baseM * baseN size of the local baseK.<br>false: The PartialOutput function is not enabled. The K axis of one Iterate performs accumulation, and each Matmul computation outputs a cube fragment of baseM * baseN size of the SingleCoreK length.<!-- npu="950" id43 --><br><br>Ascend 950PR&950DT products support this parameter.<!-- end id43 --><!-- npu="A3" id44 --><br><br>Atlas A3 products support this parameter.<!-- end id44 --><!-- npu="910b" id45 --><br><br>Atlas A2 products support this parameter.<!-- end id45 --><!-- npu="310p" id46 --><br><br>The Atlas inference products AI Core does not support this parameter.<!-- end id46 --><!-- npu="310b" id47 --><br><br>Atlas 200I/500 A2 inference products do not support this parameter.<!-- end id47 --> |
| isA2B2Shared | bool | Whether to enable global management of A2 and B2, that is, to control whether all Matmul objects share the double buffer mechanism of A2 and B2. This is a global configuration, and the values of all Matmul objects must remain consistent. Note that when it is enabled, the basic block sizes of cube A and cube B must not exceed 32KB.<br><br>The parameter values are as follows:<br>true: Enabled. false: Disabled (default value).<!-- npu="A3" id48 --><br><br>Atlas A3 products support this parameter.<!-- end id48 --><!-- npu="910b" id49 --><br><br>Atlas A2 products support this parameter.<!-- end id49 --><!-- npu="310p" id50 --><br><br>The Atlas inference products AI Core does not support this parameter.<!-- end id50 --><!-- npu="950" id51 --><br><br>Except for the MxMatmul scenario, Ascend 950PR&950DT products support this parameter.<!-- end id51 --><br><br>When this parameter is set to true, it is recommended to also set the enUnitFlag parameter to true so that transfer and computation pipelines run in parallel to improve performance. |
| isEnableChannelSplit | bool | Whether to enable the channel_split function. Normally, the fractal of the C cube in CubeFormat::NZ format computed by Matmul is 16\*16. Assuming the number of fractals at this time is x, the channel_split function makes the obtained C cube fractal 16\*8, and the number of fractals becomes 2x. Note that currently this parameter can be enabled only in the scenario where the Format of the C cube computed by Matmul is CubeFormat::NZ, the TYPE is float, the cube multiplication result L0C Buffer (CO1) is of the float type, and the result is output to Global Memory. The parameter values are as follows:<br>false: Default value. The channel_split function is not enabled, and the output fractal is 16\*16.<br>true: The channel_split function is enabled, and the output fractal is 16\*8.<!-- npu="950" id52 --><br><br>Ascend 950PR&950DT products support this parameter.<!-- end id52 --><!-- npu="A3" id53 --><br><br>Atlas A3 products support this parameter.<!-- end id53 --><!-- npu="910b" id54 --><br><br>Atlas A2 products support this parameter.<!-- end id54 --><!-- npu="310p" id55 --><br><br>The Atlas inference products AI Core does not support this parameter.<!-- end id55 --><!-- npu="310b" id56 --><br><br>Atlas 200I/500 A2 inference products do not support this parameter.<!-- end id56 --> |
| enableKdimReorderLoad | bool | Whether to enable K-axis staggered loading of data. Based on the same Tiling parameters, when Matmul computation is performed, if the left cube or right cube of multiple cores is the same and stored in Global Memory, multiple cores generally access the same address at the same time to load cube data, causing same-address access conflicts and affecting performance. After K-axis staggered loading of data is enabled, when multiple cores perform Matmul, they try to access different Global Memory addresses of the cube at the same time, reducing the probability of address access conflicts and improving performance. This parameter function supports only the MDL template. It is recommended to enable this function in scenarios where the K axis is large and neither the left cube nor the right cube is fully loaded. The parameter values are as follows.<br>false: Default value. The K-axis staggered loading function is disabled.<br>true: The K-axis staggered loading function is enabled.<!-- npu="950" id57 --><br><br>Except for the MxMatmul scenario, Ascend 950PR&950DT products support this parameter.<!-- end id57 --><!-- npu="A3" id58 --><br><br>Atlas A3 products support this parameter.<!-- end id58 --><!-- npu="910b" id59 --><br><br>Atlas A2 products support this parameter.<!-- end id59 --><!-- npu="310p" id60 --><br><br>The Atlas inference products AI Core does not support this parameter.<!-- end id60 --><!-- npu="310b" id61 --><br><br>Atlas 200I/500 A2 inference products do not support this parameter.<!-- end id61 --> |
| enableL1BankConflictOptimise | bool | Whether to enable Bank conflict optimization on L1. On the Tiling side, call the [EnableL1BankConflictOptimise](../Matmul_Tiling/EnableL1BankConflictOptimise.md) API to obtain the result of whether this optimization can be enabled, and use it together with the TilingKey mechanism to add a code implementation branch on the kernel function (Kernel) side. If this optimization is enabled, when Matmul computation is performed based on the same Tiling parameters, the L1 Buffer space is no longer allocated continuously for the A and B matrices and the ScaleA and ScaleB matrices in the MxMatmul scenario. In the DoubleBuffer scenario, the data computed in parallel is allocated in the upper half and lower half of the L1 Buffer respectively. In the non-DoubleBuf scenario, the data is allocated in the upper half of the L1 Buffer. In addition, Bias is allocated in the upper half of the L1 Buffer, and the quantization coefficients in the vector quantization/dequantization scenario are allocated in the lower half of the L1 Buffer. The parameter values are as follows.<br>false: Default value. L1 Bank conflict optimization is disabled.<br>true: L1 Bank conflict optimization is enabled.<!-- npu="950" id62 --><br><br>Except for the MxMatmul scenario, Ascend 950PR&950DT products support this parameter.<!-- end id62 --><!-- npu="A3" id63 --><br><br>Atlas A3 products do not support this parameter.<!-- end id63 --><!-- npu="910b" id64 --><br><br>Atlas A2 products do not support this parameter.<!-- end id64 --><!-- npu="310p" id65 --><br><br>The Atlas inference products AI Core does not support this parameter.<!-- end id65 --><!-- npu="310b" id66 --><br><br>Atlas 200I/500 A2 inference products do not support this parameter.<!-- end id66 --> |

## Return Value

[MatmulConfig structure](MatmulConfig.md#matmulconfig-params).

## Constraints

None

## Examples

```
// Obtain the MatmulConfig template as the Norm template.
constexpr static MatmulConfigMode configMode = MatmulConfigMode::CONFIG_NORM;
// singleCoreM, singleCoreN, singleCoreK, basicM, basicN, basicK
constexpr static MatmulShapeParams shapeParams = {128, 128, 128, 64, 64, 64};
// When the B cube is quantized, use per channel and do not use the offset coefficient.
constexpr static MatmulQuantParams quantParams = {false, false};
// Do not enable multiple batches.
constexpr static MatmulBatchParams batchParams{false};
// Do not perform chip instruction transfer address offset validation; enable ND2NZ through vector.
constexpr static MatmulFuncParams funcParams{false, true};
constexpr static MatmulConfig mmConfig = GetMMConfig<configMode>(shapeParams, quantParams, batchParams, funcParams);
```
