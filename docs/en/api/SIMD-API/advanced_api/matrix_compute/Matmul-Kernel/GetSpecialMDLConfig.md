# GetSpecialMDLConfig

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:30:03.861Z pushedAt=2026-09-12T09:55:18.099Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Configures the parameters of the SpecialMDL template to obtain a custom SpecialMDL template. For details about the SpecialMDL template, see [Table Template features](MatmulConfig.md#table6981133810309).

## Prototype

```
__aicore__ constexpr MatmulConfig GetSpecialMDLConfig(const bool intrinsicsLimit = false, const bool batchLoop = false, const uint32_t doMTE2Preload = 0, const bool isVecND2NZ = false, bool isPerTensor = false, bool hasAntiQuantOffset = false)
```

## Parameters

All parameters of this API are used to set the parameters in the [MatmulConfig structure](MatmulConfig.md#matmulconfig-params), where the corresponding parameters have the same functions.

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| intrinsicsLimit | Input | Used to set the **intrinsicsCheck** parameter.<br><br>Whether to enable loop-based data transfer from Global Memory to the L1 Buffer when the inner axis (that is, the last axis) of the left matrix or the right matrix on a single core is greater than or equal to 65535 (number of elements). For example, for the left matrix A[M, K], if the inner-axis data **singleCoreK** on a single core is greater than 65535, after this parameter is set to **true**, the API internally transfers data through loops. The parameter values are as follows:<br>**false**: When the inner axis of the left matrix or the right matrix on a single core is greater than or equal to 65535, loop-based data transfer is not enabled (default value).<br>**true**: When the inner axis of the left matrix or the right matrix on a single core is greater than or equal to 65535, loop-based data transfer is enabled.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is not supported in the MxMatmul scenario. |
| batchLoop | Input | Used to set the **isNBatch** parameter.<br><br>Whether to enable multi-batch input and multi-batch output. This parameter is valid only for BatchMatmul. After multi-batch is enabled, only the Norm template is supported, and [IterateNBatch](IterateNBatch.md) must be called to implement multi-batch input and multi-batch output. The parameter values are as follows:<br>**false**: Multi-batch is not enabled (default value).<br>**true**: Multi-batch is enabled. |
| doMTE2Preload | Input | Used to set the **doMTE2Preload** parameter.<br><br>When the MTE2 pipeline gap is large and the M/N values are large, this parameter can be used to enable preloading in the corresponding M/N direction. After it is enabled, the MTE2 gap is reduced and performance is improved. The preloading function is valid only for the MDL template (the SpecialMDL template is not supported). The parameter values are as follows:<br>0: Not enabled (default value).<br>1: Enable preloading in the M direction.<br>2: Enable preloading in the N direction.<br><br>Note: When enabling preloading in the M/N direction, ensure that K is fully loaded and DoubleBuffer is enabled in the M/N direction. The condition for full loading of K in the M direction is: singleCoreK/baseK <= stepKa. The condition for full loading of K in the N direction is: singleCoreK/baseK <= stepKb. |
| isVecND2NZ | Input | Reserved parameter. Keep the default value **false**. |
| isPerTensor | Input | Used to set the **isPerTensor** parameter.<br><br>In the scenario where the A matrix is of the half type and the B matrix is of the int8_t type, whether per-tensor quantization is used when B-matrix quantization is enabled.<br>**true**: per-tensor quantization.<br>**false**: per-channel quantization.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is not supported in the MxMatmul scenario. |
| hasAntiQuantOffset | Input | Used to set the **hasAntiQuantOffset** parameter.<br><br>In the scenario where the A matrix is of the half type and the B matrix is of the int8_t type, whether the offset coefficient is used when B-matrix quantization is enabled.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is not supported in the MxMatmul scenario. |

## Return Value

[MatmulConfig structure](MatmulConfig.md#matmulconfig-params)

## Constraints

None

## Examples

```
// Configure the parameters of the SpecialMDL template to obtain a custom SpecialMDL template.
constexpr MatmulConfig MM_CFG = GetSpecialMDLConfig(
    /* intrinsicsLimit      */ false,
    /* batchLoop            */ false,
    /* doMTE2Preload        */ 0,
    /* isVecND2NZ           */ false,
    /* isPerTensor          */ false,
    /* hasAntiQuantOffset   */ false
);
// Perform regular Matmul computation, and use the computation result of the custom SpecialMDL template for the final output.
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
