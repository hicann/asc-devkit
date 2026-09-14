# GetMMConfig

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:31:02.150Z pushedAt=2026-09-12T09:55:18.105Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported

- Atlas A3 training products/Atlas A3 inference products: Supported

- Atlas A2 training products/Atlas A2 inference products: Supported

- Atlas 200I/500 A2 inference products: Not supported

- AI Core of Atlas inference products: Supported

- Vector Core of Atlas inference products: Not supported

- Atlas training products: Not supported

## Description

Flexiblely customizes  the Matmul template parameters. By setting [MatmulConfigMode](#table17837129144319), [MatmulShapeParams](#table16317184295116), [MatmulQuantParams](#table8313111211573), [MatmulBatchParams](#table15129204644), and [MatmulFuncParams](#table66217141862), you can obtain the custom [MatmulConfig](MatmulConfig.md#matmulconfig-params).

**MatmulConfigMode** specifies the MatmulConfig template to be obtained and modified. For details about each template, see [Template features](MatmulConfig.md#table6981133810309). Based on usage requirements, you can modify the corresponding parameter configuration of the MatmulConfig template by setting variable parameters, that is, one or more **MatmulShapeParams**, **MatmulQuantParams**, **MatmulBatchParams**, and **MatmulFuncParams** in any order. Compared with template acquisition APIs such as [GetNormalConfig](GetNormalConfig.md) and [GetMDLConfig](GetMDLConfig.md), this API provides a more flexible way to configure custom Matmul template parameters.

## Prototype

```
template <MatmulConfigMode configMode, typename... ArgTypes>
__aicore__ inline constexpr MatmulConfig GetMMConfig(ArgTypes&&... args)
```

## Parameter

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| **configMode** | MatmulConfig template to obtain. |
| **ArgTypes** | Variable template parameter. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **args** | Input | Variable parameters. Pass one or more of **MatmulShapeParams**, **MatmulQuantParams**, **MatmulBatchParams**, and **MatmulFuncParams** to be set in any order. |

**Table 3** MatmulConfigMode parameters

<a name="table17837129144319"></a>

| Parameter | Description |
| --- | --- |
| **CONFIG_NORM** | Indicates setting the **MatmulConfig** default value to the Norm template. |
| **CONFIG_MDL** | Indicates setting the **MatmulConfig** default value to the MDL template. |
| **CONFIG_SPECIALMDL** | Indicates setting the **MatmulConfig** default value to the SpecialMDL template. |
| **CONFIG_IBSHARE** | Indicates setting the **MatmulConfig** default value to the IBShare template. |

**Table 4** MatmulShapeParams parameters

<a name="table16317184295116"></a>

| Parameter | Data Type | Description |
| --- | --- | --- |
| singleCoreM | uint32_t | Size of the M-axis shape within a single core, in elements. |
| singleCoreN | uint32_t | Size of the N-axis shape within a single core, in elements. |
| singleCoreK | uint32_t | Size of the K-axis shape within a single core, in elements. |
| basicM | uint32_t | This parameter has the same meaning as the baseM parameter in [tcubetiling_structure](../Matmul-Tiling/tcubetiling_structure.md). It indicates the length of the M axis of the base block during Matmul computation, in elements. |
| basicN | uint32_t | This parameter has the same meaning as the baseN parameter in [tcubetiling_structure](../Matmul-Tiling/tcubetiling_structure.md). It indicates the length of the N axis of the base block during Matmul computation, in elements. |
| basicK | uint32_t | This parameter has the same meaning as the baseK parameter in [tcubetiling_structure](../Matmul-Tiling/tcubetiling_structure.md). It indicates the length of the K axis of the base block during Matmul computation, in elements. |

**Table 5**  MatmulQuantParams parameters

<a name="table8313111211573"></a>

| Parameter | Data Type | Description |
| --- | --- | --- |
| isPerTensor | bool | In the scenario where matrix A is half type input and matrix B is int8_t type input, whether B matrix quantization is per tensor.<br>true: per tensor quantization. false: per channel quantization.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is not supported in the MxMatmul scenario. |
| hasAntiQuantOffset | bool | In the scenario where matrix A is half type input and matrix B is int8_t type input, whether the offset coefficient is used when B matrix quantization is enabled.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is not supported in the MxMatmul scenario. |

**Table 6**  MatmulBatchParams parameters

<a name="table15129204644"></a>

| Parameter | Data Type | Description |
| --- | --- | --- |
| isNBatch | bool | Whether to enable input and output of multiple batches. This parameter is valid only for BatchMatmul. After this feature is enabled, only the Norm template is supported, and [IterateNBatch](IterateNBatch.md) must be called to implement input and output of multiple batches. Parameter values are as follows:<br>**false**: Multiple batches are not enabled (default value).<br>**true**: Multiple batches are enabled. |
| batchMode | BatchMode | In the BatchMatmul scenario where the layout type is **NORMAL**, sets the relationship between the sum of multi-batch data of the input A/B matrix of BatchMatmul and the value of L1 Buffer. Parameter values are as follows:<br>**BatchMode::BATCH_LESS_THAN_L1**: total volume of multi-batch data < L1 Buffer Size;<br>**BatchMode::BATCH_LARGE_THAN_L1**: total volume of multi-batch data > L1 Buffer Size;<br>**BatchMode::SINGLE_LARGE_THAN_L1**: total volume of single-batch data > L1 Buffer Size. |
| isBiasBatch | bool | Whether the bias size involves batch axes in the BatchMatmul scenario. Parameter values are as follows:<br>**true**: The bias size involves batch axes, and the bias size is Batch * N (default value).<br>**false**: The bias size does not involve batch axes, and the bias size is N. When multiple batches are used for Matmul computation, the bias is reused.<br>Note: In the BatchMode::SINGLE_LARGE_THAN_L1 scenario, this parameter can be set to **true** only.<br><br>Except for the MxMatmul scenario, Ascend 950PR/Ascend 950DT supports this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products support this parameter.<br><br>Atlas A3 training products/Atlas A3 inference products support this parameter.<br><br>AI Core of Atlas inference products does not support setting this parameter to **false**.<br><br>Atlas 200I/500 A2 inference products do not support setting this parameter to **false**. |
| bmmOutMode | BatchOutMode | Reserved parameter. |

**Table 7**  MatmulFuncParams parameters

<a name="table66217141862"></a>

| Parameter | Data Type | Description |
| --- | --- | --- |
| intrinsicsLimit | bool | When the inner axis (which is the tail axis) of the left matrix or right matrix on a single core is greater than or equal to 65535 (in elements), whether to enable loop-based data transfer from the Global Memory to the L1 Buffer. For example, for left matrix A[M, K], if the inner axis data singleCoreK on a single core is greater than 65535, after setting this parameter to true, the API internally transfers data in a loop. The parameter values are as follows:<br>false: When the inner axis of the left matrix or right matrix on a single core is greater than or equal to 65535, loop-based data transfer is not enabled (default value).<br>true: When the inner axis of the left matrix or right matrix on a single core is greater than or equal to 65535, loop-based data transfer is enabled.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is not supported in the MxMatmul scenario. |
| enVecND2NZ | bool | Whether to enable ND2NZ through vector instructions. When enabled, [SetLocalWorkspace](SetLocalWorkspace.md) must be set. The parameter values are as follows:<br>false: ND2NZ through vector instructions is not enabled (default value).<br>true: ND2NZ through vector instructions is enabled.<br><br>For Atlas inference products AI Core, when the Unified Buffer space is sufficient (the Unified Buffer space is greater than twice the [transLength](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct) parameter of TCubeTiling), it is recommended to enable this preferentially for better transfer performance.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is not supported in the MxMatmul scenario. |
| enableDoubleCache | bool | After enabling the IBShare template, whether to cache two blocks of data simultaneously in the L1 Buffer. The parameter values are as follows:<br>false: One block of data is cached in the L1 Buffer (default value).<br>true: Two blocks of data are cached in the L1 Buffer simultaneously.<br><br>Note: When this parameter is set to true, the basic block size must be controlled to prevent the cache of two blocks of data from exceeding the L1 Buffer size limit.<br><br>Including the MxMatmul scenario, Ascend 950PR/Ascend 950DT does not support this parameter. |
| enableL1CacheUB | bool | Whether to enable caching of Unified Buffer compute blocks in the L1 Buffer. Recommended for scenarios with more serial MTE3 and MTE2 pipelines. The parameter values are as follows:<br>true: Enable caching of Unified Buffer compute blocks in the L1 Buffer.<br>false: Do not enable caching of Unified Buffer compute blocks in the L1 Buffer.<br><br>To enable caching of Unified Buffer compute blocks in the L1 Buffer, you must call the [SetMatmulConfigParams](../Matmul-Tiling/SetMatmulConfigParams.md) API in the Tiling implementation and set the parameter enableL1CacheUBIn to true.<br><br>Including the MxMatmul scenario, Ascend 950PR/Ascend 950DT does not support this parameter.<br><br>Atlas A3 training products/Atlas A3 inference products do not support this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products do not support this parameter.<br><br>Atlas inference products AI Core supports this parameter.<br><br>Atlas 200I/500 A2 inference products do not support this parameter. |
| doMTE2Preload | uint32_t | When the MTE2 pipeline gap is large and the M/N values are large, this parameter can be used to enable the preload function in the corresponding M/N direction, which reduces the MTE2 gap and improves performance. The preload function is valid only for the MDL template (the SpecialMDL template is not supported). The parameter values are as follows:<br>0: Not enabled (default value).<br>1: Enable preload in the M direction.<br>2: Enable preload in the N direction.<br><br>Note: When enabling the preload function in the M/N direction, ensure that K is fully loaded and DoubleBuffer is enabled in the M/N direction. The K full-load condition for the M direction is: singleCoreK/baseK <= stepKa; the K full-load condition for the N direction is: singleCoreK/baseK <= stepKb. |
| iterateOrder | IterateOrder | The loop iteration order of matrix operations in Matmul, with the same meaning as the iterateOrder parameter in [Table 1](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct). This parameter takes effect when the ScheduleType parameter is set to ScheduleType::OUTER_PRODUCT. The parameter values are as follows:<br><br>ORDER_M: Offset in the M axis direction first, then in the N axis direction.<br><br>ORDER_N: Offset in the N axis direction first, then in the M axis direction.<br><br>UNDEF: Currently invalid.<br><br>Note: When the Matmul scenario of the Norm template or the MDL template is used, if IterateOrder is ORDER_M, stepN in the [TCubeTiling structure](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct) must be greater than 1; if IterateOrder is ORDER_N, stepM in the TCubeTiling structure must be greater than 1. MxMatmul supports only the MDL template.<br><br>Atlas A3 training products/Atlas A3 inference products supports this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products supports this parameter.<br><br>Atlas inference products AI Core does not support this parameter.<br><br>Atlas 200I/500 A2 inference products do not support this parameter. |
| scheduleType | ScheduleType | Configures the Matmul data transfer mode. The parameter values are as follows:<br>ScheduleType::INNER_PRODUCT: Default mode, performs MTE1 loop transfer in the K direction;<br>ScheduleType::OUTER_PRODUCT: Performs MTE1 loop transfer in the M or N direction; after setting this value, it must be used together with the IterateOrder parameter. This configuration currently takes effect only in the BatchMatmul scenario (with the Norm template enabled) or the Matmul scenario (with the MDL template or Norm template enabled).<br>If IterateOrder is ORDER_M, loop transfer is performed in the N direction (possible performance improvement when singleCoreN is greater than baseN), that is, MTE1 transfer of the B matrix is parallelized;<br>If IterateOrder is ORDER_N, loop transfer is performed in the M direction (possible performance improvement when singleCoreM is greater than baseM), that is, MTE1 transfer of the A matrix is parallelized;<br>Loop transfer in the M direction and the N direction cannot be enabled simultaneously;<br><br>Note:<br>In the Batch Matmul scenario of the Norm template or the MDL template, when singleCoreK > baseK, ScheduleType::OUTER_PRODUCT cannot be set; the default mode must be used.<br>In the Matmul scenario of the Norm template or MDL template, ScheduleType::OUTER_PRODUCT can be configured only in pure Cube mode (matrix computation only).<br>The MDL template supports ScheduleType::OUTER_PRODUCT only when computed via [IterateAll](IterateAll.md).<br>ScheduleType::OUTER_PRODUCT is supported only when the C matrix is output to GM.<br><br>Ascend 950PR/Ascend 950DT supports this parameter.<br><br>Atlas A3 training products/Atlas A3 inference products supports this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products supports this parameter.<br><br>Atlas inference products AI Core does not support this parameter.<br><br>Atlas 200I/500 A2 inference products do not support this parameter. |
| enableReuse | bool | Whether the dataPtr in the callback function set by the [SetSelfDefineData](SetSelfDefineData.md) function directly passes the compute data. If SetSelfDefineData is not called to set dataPtr, this parameter supports only the default value true. The parameter values are as follows:<br>true: Directly pass the compute data, limited to a single value.<br>false: Pass the data address information stored in GM.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is not supported in the MxMatmul scenario. |
| enableUBReuse | bool | Whether to enable Unified Buffer reuse. When the Unified Buffer space is sufficient (the Unified Buffer space is greater than four times the [transLength](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct) parameter of TCubeTiling), after enabling Unified Buffer reuse, the Unified Buffer space is divided into two non-overlapping parts that respectively store the data of two adjacent Matmul iterations; the transfer-in of data for the next iteration no longer needs to wait for the release of the Unified Buffer space of the previous iteration, thereby optimizing the pipeline. The parameter values are as follows:<br>true: Enable Unified Buffer reuse.<br>false: Do not enable Unified Buffer reuse.<br><br>Including the MxMatmul scenario, Ascend 950PR/Ascend 950DT does not support this parameter.<br><br>Atlas A3 training products/Atlas A3 inference products do not support this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products do not support this parameter.<br><br>Atlas inference products AI Core supports this parameter.<br><br>Atlas 200I/500 A2 inference products do not support this parameter. |
| isPartialOutput | bool | Whether to enable the PartialOutput function, that is, controlling the basic block computation mode of Matmul's sequential K-direction output: whether the K axis of one Iterate computation of Matmul performs accumulation. The parameter values are as follows:<br>true: Enable the PartialOutput function. The K axis of one Iterate does not perform accumulation, and each Matmul computation outputs a matrix fragment of baseM * baseN size with a local baseK.<br>false: Do not enable the PartialOutput function. The K axis of one Iterate performs accumulation, and each Matmul computation outputs a matrix fragment of baseM * baseN size with SingleCoreK length.<br><br>Ascend 950PR/Ascend 950DT supports this parameter.<br><br>Atlas A3 training products/Atlas A3 inference products supports this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products supports this parameter.<br><br>Atlas inference products AI Core does not support this parameter.<br><br>Atlas 200I/500 A2 inference products do not support this parameter. |
| isA2B2Shared | bool | Whether to enable global management of A2 and B2, that is, controlling whether all Matmul objects share the double buffer mechanism of A2 and B2. This is a global configuration, and all Matmul objects must have the same value. Note that when enabled, the basic block sizes of the A matrix and B matrix must not exceed 32KB.<br><br>The parameter values are as follows:<br>true: Enable. false: Disable (default value).<br><br>Atlas A3 training products/Atlas A3 inference products supports this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products supports this parameter.<br><br>Atlas inference products AI Core does not support this parameter.<br><br>Except for the MxMatmul scenario, Ascend 950PR/Ascend 950DT supports this parameter.<br><br>When this parameter is set to true, it is recommended to also set the enUnitFlag parameter to true so that the transfer and computation pipelines run in parallel to improve performance. |
| isEnableChannelSplit | bool | Whether to enable the channel_split function. Normally, the fractal of the C matrix in CubeFormat::NZ format computed by Matmul is 16\*16, and the number of fractals is x. The channel_split function changes the fractal of the obtained C matrix to 16\*8, and the number of fractals to 2x. Note that this parameter can be enabled only when the Format of the C matrix of the Matmul computation result is CubeFormat::NZ, TYPE is float, the matrix multiplication result CO1 is float, and the output is to Global Memory. The parameter values are as follows:<br>false: Default value, the channel_split function is not enabled, and the output fractal is 16\*16.<br>true: The channel_split function is enabled, and the output fractal is 16\*8.<br>Ascend 950PR/Ascend 950DT supports this parameter.<br><br>Atlas A3 training products/Atlas A3 inference products supports this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products supports this parameter.<br><br>Atlas inference products AI Core does not support this parameter.<br><br>Atlas 200I/500 A2 inference products do not support this parameter. |
| enableKdimReorderLoad | bool | Whether to enable staggered K-axis data loading. When performing Matmul computation based on the same Tiling parameters, if the left matrix or right matrix of multiple cores is the same and stored in Global Memory, multiple cores generally access the same address simultaneously to load matrix data, causing address access conflicts and affecting performance. After enabling staggered K-axis data loading, when multiple cores perform Matmul, they will try to access different Global Memory addresses of the matrix at the same time, reducing the probability of address access conflicts and improving performance. This parameter function supports only the MDL template, and it is recommended to enable this function when the K axis is large and neither the left matrix nor the right matrix is fully loaded. The parameter values are as follows.<br>false: Default value, the staggered K-axis data loading function is disabled.<br>true: The staggered K-axis data loading function is enabled.<br><br>Except for the MxMatmul scenario, Ascend 950PR/Ascend 950DT supports this parameter.<br><br>Atlas A3 training products/Atlas A3 inference products supports this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products supports this parameter.<br><br>Atlas inference products AI Core does not support this parameter.<br><br>Atlas 200I/500 A2 inference products do not support this parameter. |
| enableL1BankConflictOptimise | bool | Whether to enable Bank conflict optimization on L1. On the Tiling side, call the [EnableL1BankConflictOptimise](../Matmul-Tiling/EnableL1BankConflictOptimise.md) API to obtain the result of whether this optimization can be enabled, and use it together with the TilingKey mechanism to add a code implementation branch on the Kernel side. If this optimization is enabled, when performing Matmul computation based on the same Tiling parameters, the L1 Buffer space is no longer allocated continuously for the A and B matrices and the ScaleA and ScaleB matrices of the MxMatmul scenario. In the DoubleBuffer scenario, the data for parallel computation is allocated in the upper half and lower half of the L1 Buffer respectively; in the non-DoubleBuf scenario, the data is allocated in the upper half of the L1 Buffer. In addition, Bias is allocated in the upper half of the L1 Buffer, and the quantization coefficients of the vector quantization/dequantization scenario are allocated in the lower half of the L1 Buffer. The parameter values are as follows.<br>false: Default value, L1 Bank conflict optimization is disabled.<br>true: L1 Bank conflict optimization is enabled.<br><br>Except for the MxMatmul scenario, Ascend 950PR/Ascend 950DT supports this parameter.<br><br>Atlas A3 training products/Atlas A3 inference products do not support this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products do not support this parameter.<br><br>Atlas inference products AI Core does not support this parameter.<br><br>Atlas 200I/500 A2 inference products do not support this parameter. |

## Return Value

[MatmulConfig structure](MatmulConfig.md#matmulconfig-params).

## Constraints

None.

## Examples

```
// Obtain the MatmulConfig template and use it as the Norm template.
constexpr static MatmulConfigMode configMode = MatmulConfigMode::CONFIG_NORM;
// singleCoreM, singleCoreN, singleCoreK, basicM, basicN, and basicK
constexpr static MatmulShapeParams shapeParams = {128, 128, 128, 64, 64, 64};
// Quantization for matrix B is conducted per per channel, and the offset coefficient is not used.
constexpr static MatmulQuantParams quantParams = {false, false};
// The multi-batch feature is not enabled.
constexpr static MatmulBatchParams batchParams{false};
// Do not verify the address offset for chip instruction movement, and enable ND2NZ using the vector.
constexpr static MatmulFuncParams funcParams{false, true};
constexpr static MatmulConfig mmConfig = GetMMConfig<configMode>(shapeParams, quantParams, batchParams, funcParams);
```