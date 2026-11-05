# GetSpecialMDLConfig

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:24:05.106Z pushedAt=2026-10-27T11:24:20.411Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetSpecialMDLConfig_res.md#id1 -->

## Description

Configures the parameters of the SpecialMDL template to obtain a custom SpecialMDL template. For details about the SpecialMDL template, see [template features](MatmulConfig.md#template-features).

## Prototype

```
__aicore__ constexpr MatmulConfig GetSpecialMDLConfig(const bool intrinsicsLimit = false, const bool batchLoop = false, const uint32_t doMTE2Preload = 0, const bool isVecND2NZ = false, bool isPerTensor = false, bool hasAntiQuantOffset = false)
```

## Parameters

All parameters of this API are used to set the parameters in the [MatmulConfig structure](MatmulConfig.md#matmulconfig-params), where the corresponding parameters have the same functions.

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **intrinsicsLimit** | Input | Used to set the parameter **intrinsicsCheck**.<br><br>When the inner axis (i.e., the last axis) of the left cube or right cube on a single core is greater than or equal to 65535 (number of elements), whether to enable loop-based data loading from Global Memory to L1 Buffer. For example, for the left cube A[M, K], when the inner axis data **singleCoreK** on a single core is greater than 65535, after this parameter is set to **true**, the API internally loads data through loop execution. The parameter values are as follows:<br>**false**: When the inner axis of the left cube or right cube on a single core is greater than or equal to 65535, loop-based data loading is not enabled (default value).<br>**true**: When the inner axis of the left cube or right cube on a single core is greater than or equal to 65535, loop-based data loading is enabled.<!-- npu="950" id9 --><br><br>For Ascend 950PR&950DT products, this parameter is not supported in the MxMatmul scenario.<!-- end id9 --> |
| **batchLoop** | Input | Used to set the parameter **isNBatch**.<br><br>Whether to enable multiple-Batch input and multiple-Batch output. This is valid only for BatchMatmul. After multiple-Batch is enabled, only the Norm template is supported, and [IterateNBatch](IterateNBatch.md) must be called to implement multiple-Batch input and multiple-Batch output. The parameter values are as follows:<br>**false**: Multiple-Batch is not enabled (default value).<br>**true**: Multiple-Batch is enabled. |
| **doMTE2Preload** | Input | Used to set the parameter **doMTE2Preload**.<br><br>When the MTE2 pipeline gap is large and the M/N values are large, you can enable the preload function in the corresponding M/N direction through this parameter, which reduces the MTE2 gap and improves performance. The preload function is valid only for the MDL template (not supported for the SpecialMDL template). The parameter values are as follows:<br>**0**: Not enabled (default value).<br>**1**: Enable preload in the M direction.<br>**2**: Enable preload in the N direction.<br><br>Note: When enabling the preload function in the M/N direction, ensure that K is fully loaded and that DoubleBuffer is enabled in the M/N direction. The K fully loaded condition for the M direction is: singleCoreK/baseK <= stepKa; the K fully loaded condition for the N direction is: singleCoreK/baseK <= stepKb. |
| **isVecND2NZ** | Input | Reserved parameter. Keep the default value **false**. |
| **isPerTensor** | Input | Used to set the parameter **isPerTensor**.<br><br>In the scenario where the A cube is of half type and the B cube is of int8_t type, whether it is per-tensor quantization when B cube quantization is enabled.<br>**true**: per-tensor quantization.<br>**false**: per-channel quantization.<!-- npu="950" id10 --><br><br>For Ascend 950PR&950DT products, this parameter is not supported in the MxMatmul scenario.<!-- end id10 --> |
| **hasAntiQuantOffset** | Input | Used to set the parameter **hasAntiQuantOffset**.<br><br>In the scenario where the A cube is of half type and the B cube is of int8_t type, whether to use the offset coefficient when B cube quantization is enabled.<!-- npu="950" id11 --><br><br>For Ascend 950PR&950DT products, this parameter is not supported in the MxMatmul scenario.<!-- end id11 --> |

## Return Value

[MatmulConfig structure](MatmulConfig.md#matmulconfig-params).

## Constraints

None

## Examples

```
// Configure the parameters of the SpecialMDL template and obtain the custom SpecialMDL template.
constexpr MatmulConfig MM_CFG = GetSpecialMDLConfig(
    /* intrinsicsLimit      */ false,
    /* batchLoop            */ false,
    /* doMTE2Preload        */ 0,
    /* isVecND2NZ           */ false,
    /* isPerTensor          */ false,
    /* hasAntiQuantOffset   */ false);
// Perform regular Matmul computation, and finally output the computation result using the custom SpecialMDL template.
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
