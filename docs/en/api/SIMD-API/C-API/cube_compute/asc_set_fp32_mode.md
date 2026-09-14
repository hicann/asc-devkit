# asc_set_fp32_mode

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:34:12.111Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

Sets FP32 mode for Mmad computation. After this mode is enabled, FP32 data in the L0A Buffer/L0B Buffer is not rounded before participating in Mmad computation.

## Prototype

 ```cpp
__aicore__ inline void asc_set_fp32_mode()
```

## Parameters

None

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
asc_set_fp32_mode();
```
