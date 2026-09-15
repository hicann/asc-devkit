# AIC and AIV Independent Operation Mechanism<a name="ZH-CN_TOPIC_0000002532388137"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-26T13:40:19.706Z -->

## Description<a name="zh-cn_topic_0000002299608693_section310824820358"></a>

The independent operation mechanism of AIC and AIV is also called dual-master mode. In [separate mode](../../../../programming_guide/advanced_programming/hardware_implementation/basic_architecture.md#li188191010204418), unlike MIX mode (which includes Cube computation and vector computation) that drives AIC through the message mechanism, dual-master mode allows AIC and AIV to run code independently without relying on message-driven execution. Enabling dual-master mode improves Matmul computation performance. By default, dual-master mode is disabled and must be enabled through the enableMixDualMaster parameter in MatmulConfig.

## Scenarios<a name="zh-cn_topic_0000002299608693_section118051016163613"></a>

When the Cube computation and vector computation code in an operator run independently without relying on message-driven execution, you can enable the dual-master mode to improve Matmul computation performance.

## Constraints<a name="zh-cn_topic_0000002299608693_section14160134220363"></a>

-   This feature supports only the [Norm template](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md) and [MDL template](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/MatmulConfig.md).
-   The type of the operator kernel function is MIX, and the ratio of AIC cores to AIV cores is 1:1.
-   The type of the operator kernel function is MIX, the ratio of AIC cores to AIV cores is 1:2, and the [IBSHARE](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/matmul_usage.md) parameter is enabled for both matrix A and matrix B.
-   The value of this parameter must be consistent across all Matmul objects in the same operator.
-   Matrices A, B, and Bias support input only from Global Memory.
-   To obtain the Cube computation result, you can only call the [IterateAll](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/IterateAll.md) API to output it to [GlobalTensor](../../../../../api/SIMD-API/basic_api/data_structures/GlobalTensor/GlobalTensor.md), that is, the computation result is placed at an address in Global Memory. You cannot call APIs such as [GetTensorC](../../../../../api/SIMD-API/advanced_api/matrix_compute/Matmul-Kernel/GetTensorC.md) to obtain the result.

## Calling Example<a name="zh-cn_topic_0000002299608693_section15486294368"></a>

For the complete operator sample, see [Operator Sample with Dual-Master Mode Enabled](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_mixdualmaster).

```
// Set the template parameter enableMixDualMaster=true. The Norm template enables dual-master mode, and the MDL template obtains the template parameters through the GetMDLConfig API.
constexpr static MatmulConfig MM_CFG = GetNormalConfig(false, false, false, BatchMode::BATCH_LESS_THAN_L1, true, IterateOrder::ORDER_M, ScheduleType::OUTER_PRODUCT, false, true/*enableMixDualMaster*/);
Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, MM_CFG> mm;

// Perform regular Matmul computation.
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
mm.SetBias(gm_bias);
mm.IterateAll(gm_c);
```
