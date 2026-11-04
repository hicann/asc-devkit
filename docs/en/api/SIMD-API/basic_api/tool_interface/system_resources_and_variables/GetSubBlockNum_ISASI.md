# GetSubBlockNum(ISASI)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:25:07.805Z pushedAt=2026-10-26T07:41:44.873Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSubBlockNum_ISASI_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

In the [separate mode architecture](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), obtain the number of Cube Cores (AIC) or Vector Cores (AIV) on a logical AI Core in the current configuration.

## Prototype

```cpp
__aicore__ inline int64_t GetSubBlockNum()
```

## Parameters

None

## Return Value

In the Kernel launch project scenario, the return values of this API called on AIC and AIV for different [operator type](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#section1074418132518) are as follows:

**Table 1** Return values when the Kernel type is set through the function execution space qualifier

| Operator Type | Qualifier | AIC | AIV |
| --- | --- | --- | --- |
| Vector operator | \_\_vector\_\_ | - | 1 |
| Cube operator | \_\_cube\_\_ | 1 | - |
| Mix operator | \_\_mix\_\_(0, 1) | - | 1 |
| Mix operator | \_\_mix\_\_(1, 0) | 1 | - |
| Mix operator | \_\_mix\_\_(1, 1) | 1 | 1 |
| Mix operator | \_\_mix\_\_(1, 2) | 1 | 2 |

In the custom operator project and Kernel direct commissioning project scenarios, the return values of this API called on AIC and AIV for different Kernel types (set through [Setting the Kernel Type](../../Kernel-Tiling/set_Kernel_type.md)) are as follows:

**Table 2** Return values when the Kernel type is set through the Kernel Type API

| Kernel Type | KERNEL_TYPE_AIV_ONLY | KERNEL_TYPE_AIC_ONLY | KERNEL_TYPE_MIX_AIC_1_2 | KERNEL_TYPE_MIX_AIC_1_1 | KERNEL_TYPE_MIX_AIC_1_0 | KERNEL_TYPE_MIX_AIV_1_0 |
| --- | --- | --- | --- | --- | --- | --- |
| AIV | 1 | - | 2 | 1 | - | 1 |
| AIC | - | 1 | 1 | 1 | 1 | - |

**Note that the return values have the same meaning in both scenarios; only the method of setting the type differs. When writing operators, the Kernel direct commissioning project is recommended.**

## Constraints

None

## Example

```cpp
int64_t subBlockNum = AscendC::GetSubBlockNum();
```
