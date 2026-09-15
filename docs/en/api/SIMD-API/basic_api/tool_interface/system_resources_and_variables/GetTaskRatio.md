# GetTaskRatio

<!-- md-trans-meta sourceCommit=ab2c72ba23cbc7229f8b141e516d96812390984a translatedAt=2026-08-27T12:10:34.704Z -->

## Applicable Products

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not supported
<!-- end id14 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

In [Separated Mode](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), obtains the ratio of the number of Cube Cores (AIC) or Vector Cores (AIV) started by the task to the number of logical AI Cores.

- When called on an AIC, returns the ratio of the number of AICs to the number of logical AI Cores.
- When called on an AIV, returns the ratio of the number of AIVs to the number of logical AI Cores.

In [Coupled Mode](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), it always returns 1.

## Prototype

```cpp
__aicore__ inline int64_t GetTaskRatio()
```

## Parameters

None

## Return Value

- For [Separated Mode](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), under different Kernel types (set through the [Setting the Kernel Type](../../Kernel-Tiling/set_kernel_type.md) API), the return values of this API when called on AIC and AIV are as follows:

    **Table 1** Return value list

    | Kernel Type | KERNEL_TYPE_AIV_ONLY | KERNEL_TYPE_AIC_ONLY | KERNEL_TYPE_MIX_AIC_1_2 | KERNEL_TYPE_MIX_AIC_1_1 | KERNEL_TYPE_MIX_AIC_1_0 | KERNEL_TYPE_MIX_AIV_1_0 |
    | --- | --- | --- | --- | --- | --- | --- |
    | AIV | 1 | - | 2 | 1 | - | 1 |
    | AIC | - | 1 | 1 | 1 | 1 | - |

- For [Coupled Mode](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), it always returns 1.

## Constraints

None

## Example

```cpp
int64_t ratio = AscendC::GetTaskRatio(); // Return the ratio of AIC or AIV to the number of logical AI Cores.
AscendC::printf("task ratio is %ld", ratio);
```
