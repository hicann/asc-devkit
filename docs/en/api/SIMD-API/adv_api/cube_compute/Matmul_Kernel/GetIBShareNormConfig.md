# GetIBShareNormConfig

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:18:19.576Z pushedAt=2026-10-27T11:19:42.907Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetIBShareNormConfig_res.md#id1 -->

## Description

Used to set the parameters of the IBShare template to obtain a custom IBShare template. For details about the IBShare template, see [template features](MatmulConfig.md#template-features).

## Prototype

```
__aicore__ constexpr MatmulConfig GetIBShareNormConfig(const bool intrinsicsLimit = false, const bool batchLoop = false, const bool isVecND2NZ = false, const BatchMode bmmMode = BatchMode::BATCH_LESS_THAN_L1, const bool isDoubleCache = false, const bool enUnitFlag = true)
```

## Parameters

All parameters of this API are used to set the parameters in the [MatmulConfig structure](MatmulConfig.md#matmulconfig-params), where the corresponding parameters have the same function.

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| intrinsicsLimit | Input | Used to set the parameter **intrinsicsCheck**.<br><br>When the inner axis (that is, the last axis) of the left cube or the right cube on a single core is greater than or equal to 65535 (number of elements), whether to enable loop-based data transfer from Global Memory to the L1 Buffer. For example, for the left cube A[M, K], if the inner-axis data **singleCoreK** on a single core is greater than 65535, after this parameter is set to **true**, the API internally transfers the data in a loop. The parameter values are as follows:<br>**false**: When the inner axis of the left cube or the right cube on a single core is greater than or equal to 65535, loop-based data transfer is not enabled (default value).<br>**true**: When the inner axis of the left cube or the right cube on a single core is greater than or equal to 65535, loop-based data transfer is enabled.<!-- npu="950" id9 --><br><br>For the Ascend 950PR&950DT products, this parameter is not supported in the MxMatmul scenario.<!-- end id9 --> |
| batchLoop | Input | Used to set the parameter **isNBatch**.<br><br>Whether to support multi-Batch input and multi-Batch output. This parameter is valid only for BatchMatmul. After multi-Batch is enabled, only the Norm template is supported, and [IterateNBatch](IterateNBatch.md) must be called to implement multi-Batch input and multi-Batch output. The parameter values are as follows:<br>**false**: Multi-Batch is not enabled (default value).<br>**true**: Multi-Batch is enabled. |
| isVecND2NZ | Input | Used to set the parameter **enVecND2NZ**.<br><br>Enables ND2NZ through vector instructions. When enabled, [SetLocalWorkspace](SetLocalWorkspace.md) must be set. The parameter values are as follows:<br>**false**: ND2NZ through vector instructions is not enabled (default value).<br>**true**: ND2NZ through vector instructions is enabled.<!-- npu="310p" id13 --><br><br>For the Atlas inference products AI Core, when the Unified Buffer (UB) space is sufficient (the UB space is greater than twice the [transLength](../Matmul_Tiling/TCubeTiling_struct.md#p1620315053211) parameter of TCubeTiling), it is recommended to enable this parameter first for better transfer performance.<!-- end id13 --><!-- npu="950" id10 --><br><br>For the Ascend 950PR&950DT products, this parameter is not supported in the MxMatmul scenario.<!-- end id10 --> |
| bmmMode | Input | Used to set the parameter **batchMode**. This parameter is used in the BatchMatmul scenario.<br><br>In the BatchMatmul scenario, when the Layout type is NORMAL, sets the relationship between the multi-batch total data size of the BatchMatmul input A/B cubes and the L1 Buffer size. The parameter values are as follows:<br>**BatchMode::BATCH_LESS_THAN_L1**: multi-batch total data size < L1 Buffer Size;<br>**BatchMode::BATCH_LARGE_THAN_L1**: multi-batch total data size > L1 Buffer Size;<br>**BatchMode::SINGLE_LARGE_THAN_L1**: single-batch total data size > L1 Buffer Size. |
| isDoubleCache | Input | Used to set the parameter **enableDoubleCache**.<br><br>After the IBShare template is enabled, whether to cache two blocks of data simultaneously on the L1 Buffer. The parameter values are as follows:<br>**false**: One block of data is cached on the L1 Buffer (default value).<br>**true**: Two blocks of data are cached simultaneously on the L1 Buffer.<br><br>Note: When this parameter is set to **true**, the basic block size must be controlled to prevent the cache of two blocks of data from exceeding the L1 Buffer size limit.<!-- npu="950" id11 --><br><br>This parameter is not supported for the Ascend 950PR&950DT products, including the MxMatmul scenario.<!-- end id11 --> |
| enUnitFlag | Input | Used to set the parameter **enUnitFlag**.<br><br>Enables the UnitFlag function to run computation and transfer pipelines in parallel, improving performance. It is enabled by default under Norm and IBShare, and disabled by default under MDL. The parameter values are as follows:<br>**false**: The UnitFlag function is not enabled.<br>**true**: The UnitFlag function is enabled.<!-- npu="950" id12 --><br><br>Note: For the MxMatmul scenario of the Ascend 950PR&950DT products, enabling the UnitFlag function brings performance benefits only when the NORM/MDL template is used, A and scaleA are not transposed, B and scaleB are transposed, C is in ND format, and the output is to GM.<!-- end id12 --> |

## Return Value

[MatmulConfig structure](MatmulConfig.md#matmulconfig-params).

## Constraints

The IBShare template is currently applicable only to the MIX scenario and does not support the pure CUBE scenario.

## Examples

```
// Configure the parameters of the IBShare template to obtain a custom IBShare template.
constexpr MatmulConfig MM_CFG = GetIBShareNormConfig(
    /* intrinsicsLimit      */ false,
    /* batchLoop            */ false,
    /* isVecND2NZ           */ false,
    /* bmmMode              */ BatchMode::BATCH_LESS_THAN_L1,
    /* isMsgReuse           */ false,
    /* enableUBReuse        */ true);
// For regular Matmul computation, the final output uses the computation result of the custom IBShare template.
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
typedef AscendC::MatmulType<
    AscendC::TPosition::GM, CubeFormat::ND, half, true /*Enable cube transpose.*/, LayoutMode::NONE /*Do not enable BatchMatmul.*/,
    true /*Enable IBShare.*/>
    bType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
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
