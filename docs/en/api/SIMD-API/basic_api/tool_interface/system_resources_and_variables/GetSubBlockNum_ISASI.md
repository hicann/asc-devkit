# GetSubBlockNum(ISASI)

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T12:09:10.350Z -->

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
- Atlas inference products AI Core: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not supported
<!-- end id14 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

In the [separate-mode architecture](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), obtains the number of Cube Cores (AIC) or Vector Cores (AIV) on a logical AI Core in the current configuration.

## Prototype

```cpp
__aicore__ inline int64_t GetSubBlockNum()
```

## Parameters

None

## Return Value

In the fusion compilation scenario, the return values of calling this API on AIC and AIV for different [operator types](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md#section1074418132518) are as follows:

**Table 1** Return values in the fusion compilation scenario

| Operator Type | Qualifier | AIC | AIV |
| --- | --- | --- | --- |
| Vector operator | \_\_vector\_\_ | - | 1 |
| Cube operator | \_\_cube\_\_ | 1 | - |
| Mix operator | \_\_mix\_\_(0, 1) | - | 1 |
| Mix operator | \_\_mix\_\_(1, 0) | 1 | - |
| Mix operator | \_\_mix\_\_(1, 1) | 1 | 1 |
| Mix operator | \_\_mix\_\_(1, 2) | 1 | 2 |

In the custom operator project and Kernel direct invocation project scenarios, the return values of calling this API on AIC and AIV for different Kernel types (set through [Setting the Kernel Type](../../Kernel-Tiling/Setting the Kernel Type.md)) are as follows:

**Table 2** Return values in the custom operator project and Kernel direct invocation project scenarios

| Kernel Type | KERNEL_TYPE_AIV_ONLY | KERNEL_TYPE_AIC_ONLY | KERNEL_TYPE_MIX_AIC_1_2 | KERNEL_TYPE_MIX_AIC_1_1 | KERNEL_TYPE_MIX_AIC_1_0 | KERNEL_TYPE_MIX_AIV_1_0 |
| --- | --- | --- | --- | --- | --- | --- |
| AIV | 1 | - | 2 | 1 | - | 1 |
| AIC | - | 1 | 1 | 1 | 1 | - |

**Note that the return values have the same meaning in both scenarios; only the way of setting the type differs. Fusion compilation is recommended when writing operators.**

## Constraints

None

## Example

```cpp
int64_t subBlockNum = AscendC::GetSubBlockNum();
```
