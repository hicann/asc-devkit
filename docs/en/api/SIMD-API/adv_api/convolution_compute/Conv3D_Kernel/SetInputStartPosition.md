# SetInputStartPosition

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:15:52.254Z pushedAt=2026-10-29T02:14:34.166Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
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
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3D_Kernel/SetInputStartPosition_res.md#id1 -->

## Description

Sets the start position for loading the feature matrix Input data on a single core.

## Prototype

```
__aicore__ inline void SetInputStartPosition(int64_t diStartPos, int64_t mStartPos)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| diStartPos | Input | Start position in the Din direction on a single core. |
| mStartPos | Input | Start position in the M direction on a single core. |

## Return Value

None

## Constraints

None

## Examples

```
conv3dApi.SetInputStartPosition(diIdxStart, mIdxStart);
```
