# asc\_prof\_stop

<!-- md-trans-meta sourceCommit=482b56785b936fd20f7354a81c8239038750acee translatedAt=2026-09-06T09:03:53.484Z pushedAt=2026-09-11T01:46:20.731Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training products/Atlas A3 inference products: supported
- Atlas A2 training products/Atlas A2 inference products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
- Atlas training products: not supported

## Description

In SIMD scenarios, this API stops the performance data collection signal, and is used together with asc\_prof\_start. When using the msOpProf tool for on-board operator tuning, you can call asc\_prof\_start and asc\_prof\_stop before and after the kernel-side code segment to specify the range of the code segment to be tuned.

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

## Example

```
asc_prof_stop();
```
