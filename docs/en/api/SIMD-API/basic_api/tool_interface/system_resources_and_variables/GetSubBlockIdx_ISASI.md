# GetSubBlockIdx(ISASI)

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T12:08:27.105Z -->

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

In the [separated mode architecture](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), obtains the logical index of the Cube Core (AIC) or Vector Core (AIV) on a logical AI Core.

When `__mix__(1, 2)` is used as the [function execution space qualifier](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md#section1074418132518), **GetSubBlockIdx** can be used to distinguish the two different Vector Cores on the same core, with values of 0 and 1 respectively.

## Prototype

```cpp
__aicore__ inline int64_t GetSubBlockIdx()
```

## Parameters

None

## Return Value

Returns the logical index of the Cube Core (AIC) or Vector Core (AIV) on the logical AI Core.

The value range of **SubBlockIdx** is \[0, SubBlockNum\), where SubBlockNum can be obtained through [GetSubBlockNum](GetSubBlockNum_ISASI.md).

## Constraints

None

## Example

```cpp
int64_t subBlockID = AscendC::GetSubBlockIdx();
```
