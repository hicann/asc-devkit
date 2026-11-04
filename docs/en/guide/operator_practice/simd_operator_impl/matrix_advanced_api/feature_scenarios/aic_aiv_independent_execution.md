# AIC and AIV Independent Execution Mechanism<a name="ZH-CN_TOPIC_0000002532388137"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:00:36.570Z pushedAt=2026-11-03T07:05:13.297Z nl=2 -->

## Feature Overview<a name="zh-cn_topic_0000002299608693_section310824820358"></a>

The AIC and AIV independent execution mechanism is also known as the dual-primary mode. In [separation mode](../../../../programming_guide/advanced_programming/hardware_implementation/basic_architecture.md#li188191010204418), unlike the MIX mode (which includes cube computation and vector computation) that drives AIC execution through a message mechanism, the dual-primary mode runs AIC and AIV code independently without relying on message-driven execution. Enabling the dual-primary mode can improve Matmul computation performance. By default, the dual-primary mode is disabled and must be enabled through the enableMixDualMaster parameter in MatmulConfig.

## Scenarios<a name="zh-cn_topic_0000002299608693_section118051016163613"></a>

When the cube computation and vector computation code in an operator run independently and do not rely on message-driven execution, you can enable the dual-primary mode to improve Matmul computation performance.

## Constraints<a name="zh-cn_topic_0000002299608693_section14160134220363"></a>

-   This feature supports only the [Norm template](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulConfig.md) and [MDL template](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/MatmulConfig.md).
-   The operator kernel is of the MIX type, and the AIC core count : AIV core count is 1:1.
-   The operator kernel is of the MIX type, the AIC core count : AIV core count is 1:2, and both cube A and cube B enable the [IBSHARE](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Matmul_usage.md) parameter.
-   The value of this parameter must be consistent across all Matmul objects in the same operator.
-   Cubes A, B, and Bias support input only from Global Memory.
-   To obtain the cube computation result, you can only call the [IterateAll](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateAll.md) API to output it to [GlobalTensor](../../../../../api/SIMD-API/basic_api/data_structures/GlobalTensor/GlobalTensor.md), that is, the computation result is placed at an address in Global Memory. You cannot call APIs such as [GetTensorC](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetTensorC.md) to obtain the result.

## Example<a name="zh-cn_topic_0000002299608693_section15486294368"></a>

For the complete operator sample, see [Operator Sample with Dual-Primary Mode Enabled](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_mixdualmaster).

```
// Modify the template parameter enableMixDualMaster=true. The Norm template enables dual-primary mode, and the MDL template uses the GetMDLConfig API to obtain the template parameters.
constexpr static MatmulConfig MM_CFG = GetNormalConfig(false, false, false, BatchMode::BATCH_LESS_THAN_L1, true, IterateOrder::ORDER_M, ScheduleType::OUTER_PRODUCT, false, true/*enableMixDualMaster*/);
Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG> mm;

// Regular Matmul computation
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
mm.SetBias(gm_bias);
mm.IterateAll(gm_c);
```
