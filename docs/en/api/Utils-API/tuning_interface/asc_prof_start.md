# asc\_prof\_start

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T07:46:09.518Z pushedAt=2026-10-26T11:12:09.546Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
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
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/tuning_interface/asc_prof_start_res.md#id1 -->

## Description

In SIMD scenarios, this API sets the signal to start performance data collection and is used together with asc\_prof\_stop. When using the msOpProf tool for operator on-board tuning, you can call asc\_prof\_start and asc\_prof\_stop before and after the kernel-side code segment to specify the range of the code segment to be tuned.

## Prototype

```
__aicore__ inline void asc_prof_start()
```

## Parameters

None

## Return Value

None

## Constraints

None

## Examples

```
asc_prof_start();
```
