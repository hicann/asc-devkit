# GetNormalConfig

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:21:58.165Z pushedAt=2026-10-27T11:21:13.680Z nl=2 -->

## Applicable Products

<!-- npu="950" id6 -->
- Ascend 950PR&950DT products: Supported
<!-- end id6 -->
<!-- npu="A3" id7 -->
- Atlas A3 products: Supported
<!-- end id7 -->
<!-- npu="910b" id8 -->
- Atlas A2 products: Supported
<!-- end id8 -->
<!-- npu="310b" id9 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id9 -->
<!-- npu="310p" id10 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id10 -->
<!-- npu="910" id11 -->
- Atlas training products: Not supported
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetNormalConfig_res.md#id1 -->

## Description

Configures the parameters of the Norm template to obtain a custom Norm template. For details about the Norm template, see [template features](MatmulConfig.md#template-features).

## Prototype

```
__aicore__ constexpr MatmulConfig GetNormalConfig(const bool intrinsicsLimit = false, const bool batchLoop = false, const bool isVecND2NZ = false, const BatchMode bmmMode = BatchMode::BATCH_LESS_THAN_L1, const bool isMsgReuse = true, const IterateOrder iterateOrder = IterateOrder::UNDEF, const ScheduleType scheduleType = ScheduleType::INNER_PRODUCT, const bool enUnitFlag = true, const bool enableMixDualMaster = false, const BatchOutMode bmmOutMode = BatchOutMode::SINGLE_BATCH)
```

## Parameters

All parameters of this API are used to set the parameters in the [MatmulConfig structure](MatmulConfig.md#matmulconfig-params), where the corresponding parameters have the same function.

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| intrinsicsLimit | Input | Used to set the **intrinsicsCheck** parameter.<br><br>When the inner axis (that is, the last axis) of the left cube or right cube on a single core is greater than or equal to 65535 (number of elements), whether to enable loop-based copying of data from Global Memory to the L1 Buffer. For example, for the left cube A[M, K], if the inner axis data **singleCoreK** on a single core is greater than 65535, after this parameter is set to **true**, the API copies the data in a loop. The parameter values are as follows:<br>**false**: When the inner axis of the left cube or right cube on a single core is greater than or equal to 65535, loop-based data copying is not enabled (default value).<br>**true**: When the inner axis of the left cube or right cube on a single core is greater than or equal to 65535, loop-based data copying is enabled.<!-- npu="950" id12 --><br><br>For Ascend 950PR&950DT products, this parameter is not supported in the MxMatmul scenario.<!-- end id12 --> |
| batchLoop | Input | Used to set the **isNBatch** parameter.<br><br>Whether to support multiple-Batch input and multiple-Batch output. This parameter is valid only for BatchMatmul. After multiple-Batch is enabled, only the Norm template is supported, and [IterateNBatch](IterateNBatch.md) must be called to implement multiple-Batch input and multiple-Batch output. The parameter values are as follows:<br>**false**: Multiple-Batch is not enabled (default value).<br>**true**: Multiple-Batch is enabled. |
| isVecND2NZ | Input | Used to set the **enVecND2NZ** parameter.<br><br>Enables ND2NZ through vector instructions. When enabled, [SetLocalWorkspace](SetLocalWorkspace.md) must be set. The parameter values are as follows:<br>**false**: ND2NZ through vector instructions is not enabled (default value).<br>**true**: ND2NZ through vector instructions is enabled.<!-- npu="310p" id13 --><br><br>For the Atlas inference products AI Core, when the Unified Buffer (UB) space is sufficient (the UB space is greater than twice the [transLength](../Matmul_Tiling/TCubeTiling_struct.md#p1620315053211) parameter of TCubeTiling), it is recommended to enable this parameter first for better copy performance.<!-- end id13 --><!-- npu="950" id14 --><br><br>For Ascend 950PR&950DT products, this parameter is not supported in the MxMatmul scenario.<!-- end id14 --> |
| bmmMode | Input | Used to set the **batchMode** parameter. This parameter is used in the BatchMatmul scenario.<br><br>In the BatchMatmul scenario, when the Layout type is NORMAL, sets the relationship between the data sum of multiple batches of the input A/B matrices and the L1 Buffer size. The parameter values are as follows:<br>**BatchMode::BATCH_LESS_THAN_L1**: The data sum of multiple batches is less than the L1 Buffer size.<br>**BatchMode::BATCH_LARGE_THAN_L1**: The data sum of multiple batches is greater than the L1 Buffer size.<br>**BatchMode::SINGLE_LARGE_THAN_L1**: The data sum of a single batch is greater than the L1 Buffer size. |
| isMsgReuse | Input | Used to set the **enableReuse** parameter.<br><br>Whether the **dataPtr** in the callback function set by [SetSelfDefineData](SetSelfDefineData.md) directly passes the computation data. If **SetSelfDefineData** is not called to set **dataPtr**, this parameter supports only the default value **true**. The parameter values are as follows:<br>**true**: Directly passes the computation data, which is limited to a single value.<br>**false**: Passes the address information of the data stored in GM.<!-- npu="950" id15 --><br><br>For Ascend 950PR&950DT products, this parameter is not supported in the MxMatmul scenario.<!-- end id15 --> |
| iterateOrder | Input | Used to set the **iterateOrder** parameter.<br><br>The loop iteration order of cube operations in Matmul, which has the same meaning as the **iterateOrder** parameter in [Table 1](../Matmul_Tiling/TCubeTiling_struct.md#tcubetiling-struct). This parameter takes effect when the **ScheduleType** parameter value is **ScheduleType::OUTER_PRODUCT**. The parameter values are as follows:<br><br>**ORDER_M**: Offset in the M-axis direction first, and then in the N-axis direction.<br><br>**ORDER_N**: Offset in the N-axis direction first, and then in the M-axis direction.<br><br>**UNDEF**: Currently invalid.<br><br>Note: In the Matmul scenario of the Norm template and when the MDL template is used, if **IterateOrder** is set to **ORDER_M**, **stepN** in the [TCubeTiling structure](../Matmul_Tiling/TCubeTiling_struct.md#tcubetiling-struct) must be greater than 1; if **IterateOrder** is set to **ORDER_N**, **stepM** in the TCubeTiling structure must be greater than 1. MxMatmul supports only the MDL template.<!-- npu="A3" id16 --><br><br>Atlas A3 products support this parameter.<!-- end id16 --><!-- npu="910b" id17 --><br><br>Atlas A2 products support this parameter.<!-- end id17 --><!-- npu="310p" id18 --><br><br>The Atlas inference products AI Core does not support this parameter.<!-- end id18 --><!-- npu="310b" id19 --><br><br>Atlas 200I/500 A2 inference products do not support this parameter.<!-- end id19 --> |
| scheduleType | Input | Used to set the **scheduleType** parameter.<br><br>Configures the Matmul data copy mode. The parameter values are as follows:<br>**ScheduleType::INNER_PRODUCT**: Default mode, which performs MTE1 loop copying in the K direction.<br>**ScheduleType::OUTER_PRODUCT**: Performs MTE1 loop copying in the M or N direction. After this value is set, it must be used together with the **IterateOrder** parameter. This configuration currently takes effect only in the BatchMatmul scenario (with the Norm template enabled) or the Matmul scenario (with the MDL template or Norm template enabled).<br>If **IterateOrder** is set to **ORDER_M**, loop copying is performed in the N direction (which may improve performance when **singleCoreN** is greater than **baseN**), that is, the MTE1 copying of the B cube is parallelized.<br>If **IterateOrder** is set to **ORDER_N**, loop copying is performed in the M direction (which may improve performance when **singleCoreM** is greater than **baseM**), that is, the MTE1 copying of the A cube is parallelized.<br>Loop copying in the M direction and N direction cannot be enabled at the same time.<br><br>Note:<br>In the Batch Matmul scenario of the Norm template or in the MDL template, when **singleCoreK** is greater than **baseK**, **ScheduleType::OUTER_PRODUCT** cannot be set, and the default mode must be used.<br>In the Matmul scenario of the Norm template or MDL template, **ScheduleType::OUTER_PRODUCT** can be configured only in pure Cube mode (cube computation only).<br>The MDL template supports configuring **ScheduleType::OUTER_PRODUCT** only in the scenario where [IterateAll](IterateAll.md) is called for computation.<br>**ScheduleType::OUTER_PRODUCT** can be configured only when the C cube is output to GM.<!-- npu="950" id20 --><br><br>Ascend 950PR&950DT products support this parameter.<!-- end id20 --><!-- npu="A3" id21 --><br><br>Atlas A3 products support this parameter.<!-- end id21 --><!-- npu="910b" id22 --><br><br>Atlas A2 products support this parameter.<!-- end id22 --><!-- npu="310p" id23 --><br><br>The Atlas inference products AI Core does not support this parameter.<!-- end id23 --><!-- npu="310b" id24 --><br><br>Atlas 200I/500 A2 inference products do not support this parameter.<!-- end id24 --> |
| enUnitFlag | Input | Used to set the **enUnitFlag** parameter.<br><br>Enables the UnitFlag function to parallelize computation and copy pipelines for better performance. It is enabled by default in Norm and IBShare, and disabled by default in MDL. The parameter values are as follows:<br>**false**: The UnitFlag function is not enabled.<br>**true**: The UnitFlag function is enabled.<!-- npu="950" id30 --><br><br>Note: For the MxMatmul scenario of Ascend 950PR&950DT products, enabling the UnitFlag function brings performance benefits only in the scenario where the NORM/MDL template is used, A and scaleA are not transposed, B and scaleB are transposed, C is in ND format, and the output is to GM.<!-- end id30 --> |
| enableMixDualMaster | Input | Used to set the **enableMixDualMaster** parameter.<br><br>Whether to enable MixDualMaster (dual-master mode). Different from the MIX mode (which includes cube computation and vector computation) that drives AIC execution through the message mechanism, the dual-master mode runs AIC and AIV code independently without relying on message driving, which is used to improve performance. The default value of this parameter is **false**, and it can be set to **true** only in the following scenarios:<br>The type of the kernel function (Kernel) is MIX, and the ratio of AIC cores to AIV cores is 1:1.<br>The type of the kernel function (Kernel) is MIX, the ratio of AIC cores to AIV cores is 1:2, and the [IBSHARE](Matmul_usage.md#table1188045714378) parameter is enabled for both the A cube and B cube.<br><br>Note that to enable the MixDualMaster scenario, the following requirements must be met:<br>The value of this parameter must be consistent across all Matmul objects in the same operator.<br>The A/B/Bias matrices support copying only from GM.<br>To obtain the cube computation result, only the [IterateAll](IterateAll.md) API can be called to output to a GlobalTensor or LocalTensor, that is, the computation result is placed at an address in Global Memory or Local Memory. APIs such as [GetTensorC](GetTensorC.md) cannot be called to obtain the result.<!-- npu="950" id25 --><br><br>Except for the MxMatmul scenario, Ascend 950PR&950DT products support this parameter.<!-- end id25 --><!-- npu="A3" id26 --><br><br>Atlas A3 products support this parameter.<!-- end id26 --><!-- npu="910b" id27 --><br><br>Atlas A2 products support this parameter.<!-- end id27 --><!-- npu="310p" id28 --><br><br>The Atlas inference products AI Core does not support this parameter.<!-- end id28 --><!-- npu="310b" id29 --><br><br>Atlas 200I/500 A2 inference products do not support this parameter.<!-- end id29 --> |
| bmmOutMode | Input | Reserved parameter. |

## Return Value

[MatmulConfig structure](MatmulConfig.md#matmulconfig-params).

## Constraints

None

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetNormalConfig_res.md#id2 -->

## Examples

```
// Configure the parameters of the Norm template to obtain a custom Norm template.
constexpr MatmulConfig MM_CFG = GetNormalConfig(
    /* intrinsicsLimit   */ false,
    /* batchLoop         */ false,
    /* isVecND2NZ        */ false,
    /* bmmMode           */ BatchMode::BATCH_LESS_THAN_L1,
    /* isMsgReuse        */ true,
    /* iterateOrder      */ IterateOrder::UNDEF,
    /* scheduleType      */ ScheduleType::INNER_PRODUCT,
    /* enUnitFlag        */ true,
    /* enableMixDualMaster */ false,
    /* bmmOutMode        */ BatchOutMode::SINGLE_BATCH);
// Perform regular Matmul computation, and finally output the computation result using the custom Norm template.
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
