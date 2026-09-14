# GetMDLConfig

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:29:45.512Z pushedAt=2026-09-12T09:55:18.100Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

This API is used to set the parameters of the **MDL** template to obtain a custom MDL template. For details about the **MDL** templates, see [Table Template features](MatmulConfig.md#table6981133810309).

## Prototype

```
__aicore__ constexpr MatmulConfig GetMDLConfig(const bool intrinsicsLimit = false, const bool batchLoop = false, const uint32_t doMTE2Preload = 0, const bool isVecND2NZ = false, bool isPerTensor = false, bool hasAntiQuantOffset = false, const bool enUnitFlag = false, const bool isMsgReuse = true, const bool enableUBReuse = true, const bool enableL1CacheUB = false, const bool enableMixDualMaster = false, const bool enableKdimReorderLoad = false)
```

## Parameters

All parameters of this API are used to set the parameters in the [MatmulConfig structure](MatmulConfig.md#matmulconfig-params), where the corresponding parameters have the same functions.

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| intrinsicsLimit | Input | Used to set the **intrinsicsCheck** parameter.<br><br>Whether to enable cyclic data transfer from the Global Memory to the L1 Buffer when the inner axis (that is, the tail axis) of the left or right matrix on a single core is greater than or equal to 65535 (in elements). For example, for the left matrix A[M, K], if the inner axis data **singleCoreK** on a single core is greater than 65535, after this parameter is set to **true**, the API internally transfers data cyclically. The parameter values are as follows:<br>**false**: When the inner axis of the left or right matrix on a single core is greater than or equal to 65535, cyclic data transfer is not enabled (default value).<br>**true**: When the inner axis of the left or right matrix on a single core is greater than or equal to 65535, cyclic data transfer is enabled.<br><br>For Ascend 950PR/Ascend 950DT, the **MxMatmul** scenario does not support this parameter. |
| batchLoop | Input | Used to set the **isNBatch** parameter.<br><br>Whether to enable multi-batch input and multi-batch output. This parameter is valid only for **BatchMatmul**. After multi-batch is enabled, only the **Norm** template is supported, and [IterateNBatch](IterateNBatch.md) must be called to implement multi-batch input and multi-batch output. The parameter values are as follows:<br>**false**: Multi-batch is not enabled (default value).<br>**true**: Multi-batch is enabled. |
| doMTE2Preload | Input | Used to set the **doMTE2Preload** parameter.<br><br>When the MTE2 pipeline gap is large and the **M** or **N** value is large, this parameter can be used to enable preload along the M or N axis. After preload is enabled, the MTE2 gap is reduced and performance is improved. The preload function is valid only for the **MDL** template (the **SpecialMDL** template is not supported). The parameter values are as follows:<br>**0**: Not enabled (default value). **1**: Enable preload along the M axis. **2**: Enable preload along the N axis.<br><br>Note: When preload along the M or N axis is enabled, ensure that K is fully loaded and **DoubleBuffer** is enabled along the M or N axis. The condition for full K loading along the M axis is singleCoreK/baseK <= stepKa, and the condition for full K loading along the N axis is singleCoreK/baseK <= stepKb. |
| isVecND2NZ | Input | Used to set the **enVecND2NZ** parameter.<br><br>This parameter indicates whether to enable ND2NZ using the vector. When this parameter is enabled, [SetLocalWorkspace](SetLocalWorkspace.md) must be set. The parameter values are as follows:<br>**false**: ND2NZ is not enabled using the vector (default value).<br>**true**: ND2NZ is enabled using the vector.<br><br>For the AI Core of Atlas inference products, when the Unified Buffer space is sufficient (the Unified Buffer space is greater than twice the [transLength](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct) parameter of TCubeTiling), it is recommended to enable this parameter first for better data transfer performance.<br><br>For Ascend 950PR/Ascend 950DT, the MxMatmul scenario does not support this parameter. |
| isPerTensor | Input | Used to set the **isPerTensor** parameter.<br><br>In the scenario where matrix A is of the half type input and matrix B is of the int8_t type input, whether per tensor is used when quantization of matrix B is enabled.<br>true: per tensor quantization.<br>false: per channel quantization.<br><br>For Ascend 950PR/Ascend 950DT, the MxMatmul scenario does not support this parameter. |
| hasAntiQuantOffset | Input | Used to set the **hasAntiQuantOffset** parameter.<br><br>In the scenario where matrix A is of the half type input and matrix B is of the int8_t type input, whether the offset coefficient is used when quantization of matrix B is enabled.<br><br>For Ascend 950PR/Ascend 950DT, the MxMatmul scenario does not support this parameter. |
| enUnitFlag | Input | Used to set the **enUnitFlag** parameter.<br><br>If UnitFlag is enabled, the computation and transfer are conducted in parallel, improving the performance. It is enabled by default for **Norm** and **IBShare**, and disabled by default for **MDL**. The parameter values are as follows:<br>**false**: The **UnitFlag** function is not enabled.<br>**true**: The **UnitFlag** function is enabled.<br><br>Note: For the MxMatmul scenario of Ascend 950PR/Ascend 950DT, enabling the **UnitFlag** function brings performance benefits only when the **NORM**/**MDL** template is used, A and scaleA are not transposed, B and scaleB are transposed, C is in the ND format, and the output is written to GM. |
| isMsgReuse | Input | Used to set the **enableReuse** parameter.<br><br>Whether the **dataPtr** in the callback function set by the [SetSelfDefineData](SetSelfDefineData.md) function directly passes the compute data. If **SetSelfDefineData** is not called to set **dataPtr**, this parameter can only be set to **true** (default value). The parameter values are as follows:<br>**true**: Directly pass the compute data, and only a single value can be passed.<br>**false**: Pass the address of the data stored on GM.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is not supported in the MxMatmul scenario. |
| enableUBReuse | Input | Used to set the **enableUBReuse** parameter.<br><br>Whether to enable Unified Buffer reuse. When the Unified Buffer space is sufficient (the Unified Buffer space is greater than four times the [transLength](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct) parameter of TCubeTiling), after Unified Buffer reuse is enabled, the Unified Buffer space is divided into two non-overlapping parts to store the data of two adjacent iterations of Matmul computation. The data transfer of the next iteration does not need to wait for the Unified Buffer space of the previous iteration to be released, thereby optimizing the pipeline. The parameter values are as follows:<br>true: Enable Unified Buffer reuse.<br>false: Do not enable Unified Buffer reuse.<br><br>Including the MxMatmul scenario, Ascend 950PR/Ascend 950DT does not support this parameter.<br><br>Atlas A3 training products/Atlas A3 inference products do not support this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products do not support this parameter.<br><br>AI Core of Atlas inference products supports this parameter.<br><br>Atlas 200I/500 A2 inference products do not support this parameter. |
| enableL1CacheUB | Input | Used to set the **enableL1CacheUB** parameter.<br><br>Whether to enable L1 Buffer to cache Unified Buffer compute blocks. It is recommended to use this parameter in scenarios where the MTE3 and MTE2 pipelines are mostly serial. The parameter values are as follows:<br>true: Enable L1 Buffer to cache Unified Buffer compute blocks.<br>false: Do not enable L1 Buffer to cache Unified Buffer compute blocks.<br><br>To enable L1 Buffer to cache Unified Buffer compute blocks, you must call the [SetMatmulConfigParams](../Matmul-Tiling/SetMatmulConfigParams.md) API in the Tiling implementation to set the **enableL1CacheUBIn** parameter to true.<br><br>Including the MxMatmul scenario, Ascend 950PR/Ascend 950DT does not support this parameter.<br><br>Atlas A3 training products/Atlas A3 inference products do not support this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products do not support this parameter.<br><br>AI Core of Atlas inference products supports this parameter.<br><br>Atlas 200I/500 A2 inference products do not support this parameter. |
| enableMixDualMaster | Input | Used to set the **enableMixDualMaster** parameter.<br><br>Whether to enable **MixDualMaster** (dual-master mode). Unlike the **MIX** mode (which includes matrix computation and vector computation) that drives **AIC** execution through the message mechanism, the dual-master mode runs code independently on **AIC** and **AIV** without relying on message driving to improve performance. The default value of this parameter is false, and it can be set to true only in the following scenarios:<br>The kernel function type is **MIX**, and the **AIC** core count : **AIV** core count is 1:1.<br>The kernel function type is **MIX**, the **AIC** core count : **AIV** core count is 1:2, and the [IBSHARE](matmul_usage.md#table1188045714378) parameter is enabled for both matrix A and matrix B.<br><br>Note: When **MixDualMaster** is enabled, the following requirements must be met:<br>The value of this parameter must be consistent across all **Matmul** objects in the same operator.<br>The A/B/Bias matrices support data transfer only from GM.<br>To obtain the matrix computation result, you can only call the [IterateAll](IterateAll.md) API to output it to **GlobalTensor** or **LocalTensor**, that is, place the computation result at an address in Global Memory or Local Memory. You cannot call APIs such as [GetTensorC](GetTensorC.md) to obtain the result.<br><br>Except for the MxMatmul scenario, Ascend 950PR/Ascend 950DT supports this parameter.<br><br>Atlas A3 training products/Atlas A3 inference products support this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products support this parameter.<br><br>AI Core of Atlas inference products does not support this parameter.<br><br>Atlas 200I/500 A2 inference products do not support this parameter. |
| enableKdimReorderLoad | Input | Used to set the **enableKdimReorderLoad** parameter.<br><br>Whether to enable K-axis staggered data loading. With the same Tiling parameters, when Matmul computation is executed, if the left matrix or the right matrix of multiple cores is the same and stored in Global Memory, multiple cores generally access the same address simultaneously to load matrix data, causing same-address access conflicts and affecting performance. After K-axis staggered data loading is enabled, when multiple cores execute Matmul, they access different Global Memory addresses of the matrix at the same time as much as possible, reducing the probability of address access conflicts and improving performance. This parameter supports only the **MDL** template. It is recommended to enable this function in scenarios where the K axis is large and neither the left matrix nor the right matrix is fully loaded. The parameter values are as follows.<br>false: Default value. Disable the K-axis staggered data loading function.<br>true: Enable the K-axis staggered data loading function.<br><br>Except for the MxMatmul scenario, Ascend 950PR/Ascend 950DT supports this parameter.<br><br>Atlas A3 training products/Atlas A3 inference products support this parameter.<br><br>Atlas A2 training products/Atlas A2 inference products support this parameter.<br><br>AI Core of Atlas inference products does not support this parameter.<br><br>Atlas 200I/500 A2 inference products do not support this parameter. |

## Return Value

[MatmulConfig structure](MatmulConfig.md#matmulconfig-params).

## Constraints

None

## Examples

```
// Configure the MDL template parameters to obtain a custom MDL template.
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
    /* enableKdimReorderLoad*/ false
);
// Perform regular Matmul computation, and use the custom MDL template for the final output.
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
