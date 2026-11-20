# SetMMLayoutTransform

<!-- md-trans-meta sourceCommit=971bc6393be4cb3e0f8244913e605c5710bb2cc9 translatedAt=2026-11-19T02:39:20.989Z pushedAt=2026-11-19T05:58:49.098Z nl=2 -->

## Supported Products

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
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Function Description

Controls the preferred direction (M or N) of the Mmad/MmadWithSparse computation result. This API is deprecated. Use [SetMMRowMajor/SetMMColumnMajor](SetMMColumnMajor-SetMMRowMajor.md) to replace it.

## Prototype

```cpp
__aicore__ inline void SetMMLayoutTransform(bool mmLayoutMode)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| mmLayoutMode | Input | Preferred direction control parameter for Mmad/MmadWithSparse, of bool type.<br>&nbsp;&nbsp;&bull; true: CUBE prefers to generate the result through the N direction, then through the M direction (Column Major);<br>&nbsp;&nbsp;&bull; false: CUBE prefers to generate the result through the M direction, then through the N direction (Row Major). |

## Return Value

None

## Constraints

- This API is deprecated. It is recommended to use [SetMMRowMajor](SetMMColumnMajor-SetMMRowMajor.md)/[SetMMColumnMajor](SetMMColumnMajor-SetMMRowMajor.md) to replace it.

## Example

```cpp
AscendC::SetMMLayoutTransform(true); // CUBE prefers to generate the result through the N direction, then through the M direction.
// It is recommended to use the following method as a replacement:
// AscendC::SetMMColumnMajor();
```
