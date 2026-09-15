# Nop

<!-- md-trans-meta sourceCommit=b75a0dc7dc91a8e352541f98f46fc736d8863e1a translatedAt=2026-08-27T11:15:49.275Z -->

## Applicable Products

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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

The user inputs the number of cycles to delay, and **Nop** delays execution by the specified number of cycles.

## Prototype

```cpp
template <int count = 1>
__aicore__ inline void Nop()
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| count | Input | Delays **count** cycles. |

## Constraints

None

## Example

In the following example, 10 cycles are delayed.

```cpp
Nop<10>();
```

