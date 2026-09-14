# GetNormalConfig

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:31:48.320Z pushedAt=2026-09-12T09:55:18.102Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

This API is used to set the parameters of the **Norm** template and obtain a custom **Norm** template. For details about the **Norm** template, see [Table Template features](MatmulConfig.md#table6981133810309).

## Prototype

```
__aicore__ constexpr MatmulConfig GetNormalConfig(const bool intrinsicsLimit = false, const bool batchLoop = false, const bool isVecND2NZ = false, const BatchMode bmmMode = BatchMode::BATCH_LESS_THAN_L1, const bool isMsgReuse = true, const IterateOrder iterateOrder = IterateOrder::UNDEF, const ScheduleType scheduleType = ScheduleType::INNER_PRODUCT, const bool enUnitFlag = true, const bool enableMixDualMaster = false, const BatchOutMode bmmOutMode = BatchOutMode::SINGLE_BATCH)
```

## Parameters

All parameters of this API are used to set the parameters in the [MatmulConfig structure](MatmulConfig.md#matmulconfig-params), where the corresponding parameters have the same function.

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| intrinsicsLimit | Input | Used to set the **intrinsicsCheck** parameter.<br><br>Whether to enable cyclic data transfer from the Global Memory to the L1 Buffer when the inner axis (that is, the tail axis) of the left or right matrix on a single core is greater than or equal to 65535 (in elements). For example, for the left matrix A[M, K], if the inner axis data **singleCoreK** on a single core is greater than 65535, after this parameter is set to **true**, the API internally transfers data cyclically. The parameter values are as follows:<br>**false**: When the inner axis of the left or right matrix on a single core is greater than or equal to 65535, cyclic data transfer is not enabled (default value).<br>**true**: When the inner axis of the left or right matrix on a single core is greater than or equal to 65535, cyclic data transfer is enabled.<br><br>For Ascend 950PR/Ascend 950DT, the **MxMatmul** scenario does not support this parameter. |
| batchLoop | Input | Used to set the **isNBatch** parameter.<br><br>Whether to enable multi-batch input and multi-batch output. This parameter is valid only for **BatchMatmul**. After multi-batch is enabled, only the **Norm** template is supported, and [IterateNBatch](IterateNBatch.md) must be called to implement multi-batch input and multi-batch output. The parameter values are as follows:<br>**false**: Multi-batch is not enabled (default value).<br>**true**: Multi-batch is enabled. |
| isVecND2NZ | Input | Used to set the **enVecND2NZ** parameter.<br><br>This parameter indicates whether to enable ND2NZ using the vector. When this parameter is enabled, [SetLocalWorkspace](SetLocalWorkspace.md) must be set. The parameter values are as follows:<br>**false**: ND2NZ is not enabled using the vector (default value).<br>**true**: ND2NZ is enabled using the vector.<br><br>For the AI Core of Atlas inference products, when the Unified Buffer space is sufficient (the Unified Buffer space is greater than twice the [transLength](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct) parameter of TCubeTiling), it is recommended to enable this parameter first for better data transfer performance.<br><br>For Ascend 950PR/Ascend 950DT, the MxMatmul scenario does not support this parameter. |
| bmmMode | Input | Used to set the parameter **batchMode**. This parameter is used in the **BatchMatmul** scenario.<br><br>In the **BatchMatmul** scenario, when the Layout type is **NORMAL**, this parameter is used to set the relationship between the total size of the multi-batch data of the input A/B matrices and the L1 Buffer size. The parameter values are as follows:<br>**BatchMode::BATCH_LESS_THAN_L1**: The total size of multi-batch data < L1 Buffer Size.<br>**BatchMode::BATCH_LARGE_THAN_L1**: The total size of multi-batch data > L1 Buffer Size.<br>**BatchMode::SINGLE_LARGE_THAN_L1**: The total size of single-batch data > L1 Buffer Size. |
| isMsgReuse | Input | Used to set the **enableReuse** parameter.<br><br>Whether the **dataPtr** in the callback function set by the [SetSelfDefineData](SetSelfDefineData.md) function directly passes the compute data. If **SetSelfDefineData** is not called to set **dataPtr**, this parameter can only be set to **true** (default value). The parameter values are as follows:<br>**true**: Directly pass the compute data, and only a single value can be passed.<br>**false**: Pass the address of the data stored on GM.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is not supported in the MxMatmul scenario. |
| iterateOrder | Input | Used to set the **iterateOrder** parameter.<br><br>This parameter indicates the loop iteration order of matrix operations performed by Matmul, which has the same meaning as the **iterateOrder** parameter in [Table 1](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct). This parameter takes effect when **ScheduleType** is  set to **ScheduleType::OUTER_PRODUCT**. The parameter values are as follows:<br><br>**ORDER_M**: Offsetting is performed along the M axis first, and then along the N axis.<br><br>**ORDER_N**: Offsetting is performed along the N axis first, and then along the M axis.<br><br>**UNDEF**: Currently invalid.<br><br>Note: When the Norm template is used in the Matmul scenario or the MDL template is used, if **IterateOrder** is set to **ORDER_M**, **stepN** in the [TCubeTiling structure](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct) must be greater than 1; if **IterateOrder** is set to **ORDER_N**, **stepM** in the TCubeTiling structure must be greater than 1. MxMatmul supports only the MDL template.<br><br>Atlas A3 training products/Atlas A3 inference products support this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products support this parameter.<br><br>AI Core of Atlas inference products does not support this parameter.<br><br>Atlas 200I/500 A2 inference product does not support this parameter. |
| scheduleType | Input | Used to set the **scheduleType** parameter.<br><br>This parameter is used to configure the Matmul data moving mode. The parameter values are as follows:<br>**ScheduleType::INNER_PRODUCT**: Default mode, which performs cyclic data movement using MTE1 along the K axis;<br>**ScheduleType::OUTER_PRODUCT**: Performs cyclic data movement using MTE1 along the M or N axis. If the parameter is set to this value, it must be used together with the **IterateOrder** parameter. This configuration currently takes effect only in the BatchMatmul scenario (with the Norm template enabled) or the Matmul scenario (with the MDL template or Norm template enabled).<br>If **IterateOrder** is set to **ORDER_M**, cyclic data movement is performed along the N axis (performance may improve when **singleCoreN** is greater than **baseN**), that is, data in matrix B is moved in parallel using MTE1.<br>If **IterateOrder** is set to **ORDER_N**, cyclic data movement is performed along the M axis (performance may improve when **singleCoreM** is greater than **baseM**), that is, data in matrix A is moved in parallel using MTE1.<br>Loop-based moving along the M and N axes cannot be enabled at the same time.<br><br>Note:<br>In the batch Matmul scenario of the Norm template or in the MDL template, when **singleCoreK** is greater than **baseK**, **ScheduleType::OUTER_PRODUCT** cannot be set, and the default mode must be used.<br>In the Matmul scenario of the Norm template or MDL template, **ScheduleType::OUTER_PRODUCT** can be configured only in Cube mode (only matrix computation).<br>The MDL template supports configuring **ScheduleType::OUTER_PRODUCT** only when [IterateAll](IterateAll.md) is called for computation.<br>**ScheduleType::OUTER_PRODUCT** can be configured only when the C matrix is output to GM.<br><br>Ascend 950PR/Ascend 950DT supports this parameter.<br><br>Atlas A3 training products/Atlas A3 inference products support this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products support this parameter.<br><br>AI Core of Atlas inference products does not support this parameter.<br><br>Atlas 200I/500 A2 inference products do not support this parameter. |
| enUnitFlag | Input | Used to set the **enUnitFlag** parameter.<br><br>If UnitFlag is enabled, the computation and transfer are conducted in parallel, improving the performance. It is enabled by default for **Norm** and **IBShare**, and disabled by default for **MDL**. The parameter values are as follows:<br>**false**: The **UnitFlag** function is not enabled.<br>**true**: The **UnitFlag** function is enabled.<br><br>Note: For the MxMatmul scenario of Ascend 950PR/Ascend 950DT, enabling the **UnitFlag** function brings performance benefits only when the **NORM**/**MDL** template is used, A and scaleA are not transposed, B and scaleB are transposed, C is in the ND format, and the output is written to GM. |
| enableMixDualMaster | Input | Used to set the **enableMixDualMaster** parameter.<br><br>Whether to enable **MixDualMaster** (dual-master mode). Unlike the **MIX** mode (which includes matrix computation and vector computation) that drives **AIC** execution through the message mechanism, the dual-master mode runs code independently on **AIC** and **AIV** without relying on message driving to improve performance. The default value of this parameter is false, and it can be set to true only in the following scenarios:<br>The kernel function type is **MIX**, and the **AIC** core count : **AIV** core count is 1:1.<br>The kernel function type is **MIX**, the **AIC** core count : **AIV** core count is 1:2, and the [IBSHARE](matmul_usage.md#table1188045714378) parameter is enabled for both matrix A and matrix B.<br><br>Note: When **MixDualMaster** is enabled, the following requirements must be met:<br>The value of this parameter must be consistent across all **Matmul** objects in the same operator.<br>The A/B/Bias matrices support data transfer only from GM.<br>To obtain the matrix computation result, you can only call the [IterateAll](IterateAll.md) API to output it to **GlobalTensor** or **LocalTensor**, that is, place the computation result at an address in Global Memory or Local Memory. You cannot call APIs such as [GetTensorC](GetTensorC.md) to obtain the result.<br><br>Except for the MxMatmul scenario, Ascend 950PR/Ascend 950DT supports this parameter.<br><br>Atlas A3 training products/Atlas A3 inference products support this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products support this parameter.<br><br>AI Core of Atlas inference products does not support this parameter.<br><br>Atlas 200I/500 A2 inference products do not support this parameter. |
| bmmOutMode | Input | Reserved parameter. |

## Return Value

[MatmulConfig structure](MatmulConfig.md#matmulconfig-params).

## Constraints

None

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
    /* bmmOutMode        */ BatchOutMode::SINGLE_BATCH
);
// Perform regular Matmul computation, and use the custom Norm template for the final output.
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
