# GetSubBlockIdx(ISASI)

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-05T05:43:31.333Z pushedAt=2026-11-18T01:25:31.216Z nl=2 -->

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
<!-- npu="x90" id8 -->
- Kirin X90 processor products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSubBlockIdx_ISASI_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_sys_var_intf.h"`.

In the [separate mode architecture](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), obtain the logical index of the Cube Core (AIC) or Vector Core (AIV) on a logical AI Core.

When **GetSubBlockIdx** uses `__mix__(1, 2)` as the [function execution space qualifier](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#section1074418132518), it can be used to distinguish the two different Vector Cores on the same core, with values of 0 and 1 respectively.

## Function Prototype

```cpp
__aicore__ inline int64_t GetSubBlockIdx()
```

## Parameters

None

## Return Value

Returns the logical index of the Cube Core (AIC) or Vector Core (AIV) on the logical AI Core.

The value range of **SubBlockIdx** is \[0, SubBlockNum\), and **SubBlockNum** can be obtained through [GetSubBlockNum](GetSubBlockNum_ISASI.md).

## Constraints

None

## Example

```cpp
int64_t subBlockID = AscendC::GetSubBlockIdx();
```
