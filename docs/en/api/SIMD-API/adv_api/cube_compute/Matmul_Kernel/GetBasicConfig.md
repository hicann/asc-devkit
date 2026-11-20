# GetBasicConfig

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-02T06:07:13.552Z pushedAt=2026-11-18T04:52:22.098Z nl=2 -->

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
- Kirin X90 processor products: Supported
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030 processor products: Not supported
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetBasicConfig_res.md#id1 -->

## Description

Used to configure the parameters of the **BasicBlock** template and obtain a custom **BasicBlock** template. For an introduction to the **BasicBlock** template, see [template features](MatmulConfig.md#template-features).

When using this API, consider using template constantization first. Compared with the **BasicBlock** template, which only implements constantization of **baseM**, **baseN**, and **baseK**, template constantization can, on this basis, implement constantization of **singleCoreM**, **singleCoreN**, **singleCoreK**, **baseM**, **baseN**, and **baseK**. For details about how to use template constantization, see [Matmul Tiling constantization](GetMatmulApiTiling.md#description).

## Prototype

```
__aicore__ constexpr MatmulConfig GetBasicConfig(const uint32_t basicM, const uint32_t basicN, const uint32_t basicK, const bool intrinsicsLimit = false, const bool batchLoop = false, const BatchMode bmmMode = BatchMode::BATCH_LESS_THAN_L1)
```

## Parameters

All parameters of this API are used to set the parameters in the [MatmulConfig structure](MatmulConfig.md#matmulconfig-params), where the corresponding parameters have the same functions.

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| basicM | Input | Used to set the **basicM** parameter.<br><br>Has the same meaning as the **baseM** parameter in [TCubeTiling structure](../Matmul_Tiling/TCubeTiling_struct.md), which is the M-axis length of the base block during Matmul computation, in elements. |
| basicN | Input | Used to set the **basicN** parameter.<br><br>Has the same meaning as the **baseN** parameter in [TCubeTiling structure](../Matmul_Tiling/TCubeTiling_struct.md), which is the N-axis length of the base block during Matmul computation, in elements. |
| basicK | Input | Used to set the **basicK** parameter.<br><br>Has the same meaning as the **baseK** parameter in [TCubeTiling structure](../Matmul_Tiling/TCubeTiling_struct.md), which is the K-axis length of the base block during Matmul computation, in elements. |
| intrinsicsLimit | Input | Used to set the parameter **intrinsicsCheck**.<br><br>When the inner axis (i.e., the last axis) of the left cube or right cube on a single core is greater than or equal to 65535 (number of elements), determines whether to enable loop-based data loading from Global Memory to L1 Buffer. For example, for the left cube A[M, K], when the inner axis data **singleCoreK** on a single core is greater than 65535, after this parameter is set to **true**, the API internally loads data through loop execution. The parameter values are as follows:<br>**false**: When the inner axis of the left cube or right cube on a single core is greater than or equal to 65535, loop-based data loading is not enabled (default value).<br>**true**: When the inner axis of the left cube or right cube on a single core is greater than or equal to 65535, loop-based data loading is enabled.<!-- npu="950" id9 --><br><br>For Ascend 950PR&950DT products, this parameter is not supported in the MxMatmul scenario.<!-- end id9 --> |
| batchLoop | Input | Used to set the parameter **isNBatch**.<br><br>Whether to enable multiple-batch input and multiple-batch output. This is valid only for BatchMatmul. After multiple-batch is enabled, only the Norm template is supported, and [IterateNBatch](IterateNBatch.md) must be called to implement multiple-batch input and multiple-batch output. The parameter values are as follows:<br>**false**: Multiple-batch is not enabled (default value).<br>**true**: Multiple-batch is enabled. |
| bmmMode | Input | Used to set the **batchMode** parameter. This parameter is used in the BatchMatmul scenario.<br><br>In the BatchMatmul scenario, when the Layout type is NORMAL, this parameter sets the relationship between the total data of multiple batches of the input A/B cubes and the L1 Buffer size. The parameter values are as follows:<br>**BatchMode::BATCH_LESS_THAN_L1**: The total data of multiple batches is less than the L1 Buffer size.<br>**BatchMode::BATCH_LARGE_THAN_L1**: The total data of multiple batches is greater than the L1 Buffer size.<br>**BatchMode::SINGLE_LARGE_THAN_L1**: The total data of a single batch is greater than the L1 Buffer size. |

## Return Value

[MatmulConfig structure](MatmulConfig.md#matmulconfig-params).

## Constraints

-   When using this API, the basic block sizes **baseM** and **baseN** must satisfy the following: **singleCoreM** must be divisible by **baseM**, and **singleCoreN** must be divisible by **baseN**.
-   The parameters **basicM**, **basicN**, and **basicK** of this API must be consistent with the **baseM**, **baseN**, and **baseK** settings in [TCubeTiling structure](../Matmul_Tiling/TCubeTiling_struct.md#p17899165811566).

## Examples

```
// Configure the parameters of the BasicBlock template to obtain a custom BasicBlock template.
constexpr MatmulConfig MM_CFG = GetBasicConfig(
    /* basicM              */ 128,
    /* basicN              */ 256,
    /* basicK              */ 64,
    /* intrinsicsLimit     */ false,
    /* batchLoop           */ false,
    /* batchMode           */ BatchMode::BATCH_LESS_THAN_L1);
// Perform regular Matmul computation, and finally output the computation result using the custom BasicBlock template.
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
