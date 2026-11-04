# GetTaskRatio

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:26:23.838Z pushedAt=2026-10-26T07:41:44.877Z nl=2 -->

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
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetTaskRatio_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

In [separate mode](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), obtains the ratio of quantities between the Cube Cores (AICs) or Vector Cores (AIVs) started by the task and the logical AI Cores.

- When called on an AIC, returns the ratio of quantities between the AICs and the logical AI Cores.
- When called on an AIV, returns the ratio of quantities between the AIVs and the logical AI Cores.

In [coupled mode](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), always returns 1.

## Prototype

```cpp
__aicore__ inline int64_t GetTaskRatio()
```

## Parameters

None

## Return Value

- For [decoupled mode](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), under different kernel types (set through the [Set Kernel Type](../../Kernel-Tiling/set_Kernel_type.md) API), the return values of this API when called on AIC and AIV are as follows:

    **Table 1** Return value list

    | Kernel Type | KERNEL_TYPE_AIV_ONLY | KERNEL_TYPE_AIC_ONLY | KERNEL_TYPE_MIX_AIC_1_2 | KERNEL_TYPE_MIX_AIC_1_1 | KERNEL_TYPE_MIX_AIC_1_0 | KERNEL_TYPE_MIX_AIV_1_0 |
    | --- | --- | --- | --- | --- | --- | --- |
    | AIV | 1 | - | 2 | 1 | - | 1 |
    | AIC | - | 1 | 1 | 1 | 1 | - |

- For [coupled mode](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), the return value is fixed at 1.

## Constraints

None

## Example

```cpp
int64_t ratio = AscendC::GetTaskRatio(); // Returns the ratio of the number of AIC or AIV to the number of logical AI Cores.
AscendC::printf("task ratio is %ld", ratio);
```
