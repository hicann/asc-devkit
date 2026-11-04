# Nop

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:21:29.326Z pushedAt=2026-10-26T07:41:44.763Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/scalar_compute/Nop_res.md#id1 -->

## Description
The header file path is `"basic_api/kernel_operator_utils_intf.h"`.

The user inputs the number of cycles to delay.

## Function Prototype

```cpp
template <int count = 1>
__aicore__ inline void Nop()
```

## Parameters

**Table 1** Parameters

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| count | Input | Delays count cycles. |

## Constraints

None

## Example

In the following example, execution is delayed by 10 cycles.

```cpp
Nop<10>();
```
