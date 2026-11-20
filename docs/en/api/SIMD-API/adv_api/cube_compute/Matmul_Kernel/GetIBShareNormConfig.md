# GetIBShareNormConfig

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-02T06:09:06.422Z pushedAt=2026-11-18T04:52:30.504Z nl=2 -->

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
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90 processor products: Not supported
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030 processor products: Not supported
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetIBShareNormConfig_res.md#id1 -->

## Description

Used to set the parameters of the IBShare template to obtain a custom IBShare template. For details about the IBShare template, see [template features](MatmulConfig.md#template-features).

## Prototype

```
__aicore__ constexpr MatmulConfig GetIBShareNormConfig(const bool intrinsicsLimit = false, const bool batchLoop = false, const bool isVecND2NZ = false, const BatchMode bmmMode = BatchMode::BATCH_LESS_THAN_L1, const bool isDoubleCache = false, const bool enUnitFlag = true)
```

## Parameters

All parameters of this API are used to set the parameters in the [MatmulConfig structure](MatmulConfig.md#matmulconfig-params), where the corresponding parameters have the same functions.

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| intrinsicsLimit | Input | Used to set the parameter **intrinsicsCheck**.<br><br>When the inner axis (i.e., the last axis) of the left cube or right cube on a single core is greater than or equal to 65535 (number of elements), determines whether to enable loop-based data loading from Global Memory to L1 Buffer. For example, for the left cube A[M, K], when the inner axis data **singleCoreK** on a single core is greater than 65535, after this parameter is set to **true**, the API internally loads data through loop execution. The parameter values are as follows:<br>**false**: When the inner axis of the left cube or right cube on a single core is greater than or equal to 65535, loop-based data loading is not enabled (default value).<br>**true**: When the inner axis of the left cube or right cube on a single core is greater than or equal to 65535, loop-based data loading is enabled.<!-- npu="950" id9 --><br><br>For Ascend 950PR&950DT products, this parameter is not supported in the MxMatmul scenario.<!-- end id9 --> |
| batchLoop | Input | Used to set the parameter **isNBatch**.<br><br>Whether to enable multiple-Batch input and multiple-Batch output. This is valid only for BatchMatmul. After multiple-Batch is enabled, only the Norm template is supported, and [IterateNBatch](IterateNBatch.md) must be called to implement multiple-Batch input and multiple-Batch output. The parameter values are as follows:<br>**false**: Multiple-Batch is not enabled (default value).<br>**true**: Multiple-Batch is enabled. |
| isVecND2NZ | Input | Used to set the **enVecND2NZ** parameter.<br><br>Enables ND2NZ through vector instructions. When enabled, [SetLocalWorkspace](SetLocalWorkspace.md) must be set. The parameter values are as follows:<br>**false**: ND2NZ through vector instruction execution is not enabled (default value).<br>**true**: ND2NZ through vector instruction is enabled.<!-- npu="310p" id13 --><br><br>For the AI Core of Atlas inference products, when the Unified Buffer (UB) space is sufficient (the UB space is greater than 2 times the [transLength](../Matmul_Tiling/TCubeTiling_struct.md#p1620315053211) parameter of TCubeTiling), it is recommended to enable this parameter first for better transfer performance.<!-- end id13 --><!-- npu="950" id10 --><br><br>For Ascend 950PR&950DT products, this parameter is not supported in the MxMatmul scenario.<!-- end id10 --> |
| bmmMode | Input | Used to set the **batchMode** parameter. This parameter is used in the BatchMatmul scenario.<br><br>In the BatchMatmul scenario, when the Layout type is NORMAL, sets the relationship between the data sum of multiple batches of the input A/B cubes and the L1 Buffer size. The parameter values are as follows:<br>**BatchMode::BATCH_LESS_THAN_L1**: The data sum of multiple batches is less than the L1 Buffer size.<br>**BatchMode::BATCH_LARGE_THAN_L1**: The data sum of multiple batches is greater than the L1 Buffer size.<br>**BatchMode::SINGLE_LARGE_THAN_L1**: The data sum of a single batch is greater than the L1 Buffer size. |
| isDoubleCache | Input | Used to set the parameter **enableDoubleCache**.<br><br>After the IBShare template is enabled, whether to cache two blocks of data simultaneously on the L1 Buffer. The parameter values are as follows:<br>**false**: One block of data is cached on the L1 Buffer (default value).<br>**true**: Two blocks of data are cached simultaneously on the L1 Buffer.<br><br>Note: When this parameter is set to **true**, the basic block size must be controlled to prevent the cache of two blocks of data from exceeding the L1 Buffer size limit.<!-- npu="950" id11 --><br><br>This parameter is not supported for the Ascend 950PR&950DT products, including the MxMatmul scenario.<!-- end id11 --> |
| enUnitFlag | Input | Used to set the parameter **enUnitFlag**.<br><br>Enables the UnitFlag function to run the computation and transfer pipelines in parallel, improving performance. It is enabled by default under Norm and IBShare, and disabled by default under MDL. The parameter values are as follows:<br>**false**: The UnitFlag function is not enabled.<br>**true**: The UnitFlag function is enabled.<!-- npu="950" id12 --><br><br>Note: For the MxMatmul scenario of the Ascend 950PR&950DT products, enabling the UnitFlag function provides performance benefits only when the NORM/MDL template is used, A and scaleA are not transposed, B and scaleB are transposed, C is in ND format, and the output is to GM.<!-- end id12 --> |

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
// Regular Matmul computation, with the final output using the computation result of the custom IBShare template.
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
typedef AscendC::MatmulType<
    AscendC::TPosition::GM, CubeFormat::ND, half, true /*Enable cube transpose*/, LayoutMode::NONE /*Do not enable BatchMatmul*/,
    true /*Enable IBShare*/>
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
