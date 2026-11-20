# asc\_prof\_stop

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-13T09:05:48.322Z pushedAt=2026-11-16T07:07:41.594Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT product: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference product AI Core: Not supported
- Atlas inference product Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/tuning_interface/asc_prof_stop_res.md#id1 -->

## Description

In SIMD scenarios, this API sets the signal to stop performance data collection and is used together with asc\_prof\_start. When using the msOpProf tool for operator on-board tuning, asc\_prof\_start and asc\_prof\_stop can be called before and after the kernel-side code segment, respectively, to specify the range of the code segment to be tuned.

## Prototype

```
__aicore__ inline void asc_prof_stop()
```

## Parameters

None

## Return Value

None

## Constraints

None

## Examples

```
asc_prof_stop();
```
