# GetBasicConfig

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:24:58.218Z pushedAt=2026-09-12T09:55:18.092Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Configures the parameters of the **BasicBlock** template to obtain a custom template. For details about the **BasicBlock** template, see [Table Template features](MatmulConfig.md#table6981133810309).

When using this API, you can compile parameters to constants on the template level. Compared with the **BasicBlock** template for which only **baseM**, **baseN**, and **baseK** are complied to constants, **singleCoreM**, **singleCoreN**, **singleCoreK**, **baseM**, **baseN**, and **baseK** can also be compiled to constants through template-level compilation with constants. For details about how to implement template-level compilation with constants, see [GetMatmulApiTiling](GetMatmulApiTiling.md#matmul-tiling-constant).

## Prototype

```
__aicore__ constexpr MatmulConfig GetBasicConfig(const uint32_t basicM, const uint32_t basicN, const uint32_t basicK, const bool intrinsicsLimit = false, const bool batchLoop = false, const BatchMode bmmMode = BatchMode::BATCH_LESS_THAN_L1)
```

## Parameters

All parameters of this API are used to set the parameters in [MatmulConfig](MatmulConfig.md#matmulconfig-params), where the corresponding parameters have the same functions.

**Table 1** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| basicM | Input | Used to set the **basicM** parameter.<br><br>It has the same meaning as the **baseM** parameter in [tcubetiling_structure](../Matmul-Tiling/tcubetiling_structure.md), which is the M-axis length of the base block in Matmul computation, in elements. |
| basicN | Input | Used to set the **basicN** parameter.<br><br>It has the same meaning as the **baseN** parameter in [tcubetiling_structure](../Matmul-Tiling/tcubetiling_structure.md), which is the N-axis length of the base block in Matmul computation, in elements. |
| basicK | Input | Used to set the **basicK** parameter.<br><br>It has the same meaning as the **baseK** parameter in [tcubetiling_structure](../Matmul-Tiling/tcubetiling_structure.md), which is the K-axis length of the base block in Matmul computation, in elements. |
| intrinsicsLimit | Input | Used to set the **intrinsicsCheck** parameter.<br><br>When the inner axis (which is the tail axis) of the left or right matrix on a single core is greater than or equal to 65535 (in elements), this parameter specifies whether to enable looping data movement from the Global Memory to the L1 Buffer. For example, for the left matrix A[M, K], if the inner-axis data **singleCoreK** on a single core is greater than 65535, setting this parameter to **true** enables the API to move data in a loop internally. Valid values:<br>**false**: When the inner axis of the left or right matrix on a single core is greater than or equal to 65535, looping data movement is disabled (default).<br>**true**: When the inner axis of the left or right matrix on a single core is greater than or equal to 65535, looping data movement is enabled.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is not supported in the **MxMatmul** scenario. |
| batchLoop | Input | Used to set the **isNBatch** parameter.<br><br>This parameter specifies whether to use multiple-batch input and multiple-batch output. It is valid only for **BatchMatmul**. When multiple batches are enabled, only the **Norm** template is supported, and [IterateNBatch](IterateNBatch.md) must be called to implement multiple-batch input and multiple-batch output. Valid values:<br>**false**: Multiple batches are disabled (default).<br>**true**: Multiple batches are enabled. |
| bmmMode | Input | Used to set the **batchMode** parameter. This parameter is used in the **BatchMatmul** scenario.<br><br>In the **BatchMatmul** scenario, when the layout type is **NORMAL**, this parameter is used set the relationship between the total data volume of multiple batches of the **BatchMatmul** input A/B matrices and the L1 Buffer size. Valid values:<br>**BatchMode::BATCH_LESS_THAN_L1**: total data volume of multiple batches < L1 Buffer size;<br>**BatchMode::BATCH_LARGE_THAN_L1**: total data volume of multiple batches > L1 Buffer size;<br>**BatchMode::SINGLE_LARGE_THAN_L1:** data volume of a single batch > L1 Buffer size. |

## Return Value

[MatmulConfig](MatmulConfig.md#matmulconfig-params).

## Constraints

-   When using this API, the basic block sizes **baseM** and **baseN** must satisfy the following conditions: **singleCoreM** is divisible by **baseM**, and **singleCoreN** is divisible by **baseN**.
-   The parameters **basicM**, **basicN**, and **basicK** of this API must be consistent with the **baseM**, **baseN**, and **baseK** settings of the [TCubeTiling struct](../Matmul-Tiling/tcubetiling_structure.md#tcubetiling-struct).

## Examples

```
// Configure the parameters of the BasicBlock template to obtain a custom template.
constexpr MatmulConfig MM_CFG = GetBasicConfig(
    /* basicM              */ 128,
    /* basicN              */ 256,
    /* basicK              */ 64,
    /* intrinsicsLimit     */ false,
    /* batchLoop           */ false,
    /* batchMode           */ BatchMode::BATCH_LESS_THAN_L1
);
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
