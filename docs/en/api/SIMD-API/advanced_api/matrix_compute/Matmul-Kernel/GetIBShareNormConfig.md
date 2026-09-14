# GetIBShareNormConfig

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:26:22.152Z pushedAt=2026-09-12T09:55:18.094Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Configures the parameters of the **IBShare** template to obtain a custom template. For details about the IBShare template, see [Table Template Features](MatmulConfig.md#table6981133810309).

## Prototype

```
__aicore__ constexpr MatmulConfig GetIBShareNormConfig(const bool intrinsicsLimit = false, const bool batchLoop = false, const bool isVecND2NZ = false, const BatchMode bmmMode = BatchMode::BATCH_LESS_THAN_L1, const bool isDoubleCache = false, const bool enUnitFlag = true)
```

## Parameter

All parameters of this API are used to set the parameters in [MatmulConfig](MatmulConfig.md#matmulconfig-params), where the corresponding parameters have the same functions.

**Table 1**  API Parameter Description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| intrinsicsLimit | Input | Used to set the parameter **intrinsicsCheck**.<br><br>When the inner axis (which is the tail axis) of the left or right matrix on a single core is greater than or equal to 65535 (number of elements), whether to enable loop-based data transfer from the Global Memory to the L1 Buffer. For example, for the left matrix A[M, K], if the inner-axis data **singleCoreK** on a single core is greater than 65535, after setting this parameter to **true**, the API internally transfers data through loops. The parameter values are as follows:<br>**false**: When the inner axis of the left or right matrix on a single core is greater than or equal to 65535, loop-based data transfer is not enabled (default value).<br>**true**: When the inner axis of the left or right matrix on a single core is greater than or equal to 65535, loop-based data transfer is enabled.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is not supported in the **MxMatmul** scenario. |
| batchLoop | Input | Used to set the parameter **isNBatch**.<br><br>Whether to enable multi-Batch input and multi-Batch output. This is valid only for **BatchMatmul**. After enabling multi-Batch, only the Norm template is supported, and [IterateNBatch](IterateNBatch.md) must be called to implement multi-Batch input and multi-Batch output. The parameter values are as follows:<br>**false**: Multi-Batch is not enabled (default value).<br>**true**: Multi-Batch is enabled. |
| isVecND2NZ | Input | Used to set the parameter **enVecND2NZ**.<br><br>This parameter is used to enable ND2NZ using the vector. When enabled, [SetLocalWorkspace](SetLocalWorkspace.md) must be set. The parameter values are as follows:<br>**false**: ND2NZ is not enabled using the vector (default value).<br>**true**: ND2NZ is enabled using the vector.<br><br>For the AI Core of the Atlas inference products, when the Unified Buffer space is sufficient (greater than 2 times the [transLength](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct) parameter of **TCubeTiling**), it is recommended to enable ND2NZ preferentially for better data transfer performance.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is not supported in the **MxMatmul** scenario. |
| bmmMode | Input | Used to set the parameter **batchMode**. This parameter is used in the **BatchMatmul** scenario.<br><br>In the **BatchMatmul** scenario, when the Layout type is **NORMAL**, this parameter is used to set the relationship between the total size of the multi-batch data of the input A/B matrices and the L1 Buffer size. The parameter values are as follows:<br>**BatchMode::BATCH_LESS_THAN_L1**: The total size of multi-batch data < L1 Buffer Size.<br>**BatchMode::BATCH_LARGE_THAN_L1**: The total size of multi-batch data > L1 Buffer Size.<br>**BatchMode::SINGLE_LARGE_THAN_L1**: The total size of single-batch data > L1 Buffer Size. |
| isDoubleCache | Input | Used to set the parameter **enableDoubleCache**.<br><br>After enabling the **IBShare** template, whether to cache two blocks of data simultaneously on the L1 Buffer. The parameter values are as follows:<br>**false**: Only one block of data is cached on the L1 Buffer (default value).<br>**true**: Two blocks of data are cached simultaneously on the L1 Buffer.<br><br>Note: When this parameter is set to **true**, the basic block size must be controlled to prevent the cache of two blocks of data from exceeding the L1 Buffer size limit.<br><br>Including the **MxMatmul** scenario, Ascend 950PR/Ascend 950DT does not support this parameter. |
| enUnitFlag | Input | Used to set the parameter **enUnitFlag**.<br><br>This parameter is used to enable the **UnitFlag** feature to run computation and data transfer in a pipelined parallel manner, improving performance. It is enabled by default under **Norm** and **IBShare**, and disabled by default under MDL. The parameter values are as follows:<br>**false**: The UnitFlag feature is not enabled.<br>**true**: The UnitFlag feature is enabled.<br><br>Note: For the **MxMatmul** scenario on Ascend 950PR/Ascend 950DT, enabling the UnitFlag feature has performance benefits only when the NORM/MDL template is used, **A** and **scaleA** are not transposed, **B** and **scaleB** are transposed, **C** is in ND format, and the output goes to GM. |

## Return Value

[MatmulConfig structure](MatmulConfig.md#matmulconfig-params).

## Constraints

The **IBShare** template currently applies only to the MIX scenario and does not support the pure CUBE scenario.

## Examples

```
// Configure the IBShare template parameters to obtain a custom IBShare template.
constexpr MatmulConfig MM_CFG = GetIBShareNormConfig(
    /* intrinsicsLimit      */ false,
    /* batchLoop            */ false,
    /* isVecND2NZ           */ false,
    /* bmmMode              */ BatchMode::BATCH_LESS_THAN_L1,
    /* isMsgReuse           */ false,
    /* enableUBReuse        */ true
);
// Perform regular Matmul computation, and use the custom IBShare template for the final output.
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half, true/*// Enable matrix transpose.*/, LayoutMode::NONE/*// Disable BatchMatmul.*/, true/*// Enable IBShare.*/> bType;
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
