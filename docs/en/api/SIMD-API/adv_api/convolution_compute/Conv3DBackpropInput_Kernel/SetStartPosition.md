# SetStartPosition

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:42:33.572Z pushedAt=2026-10-29T02:14:34.329Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3DBackpropInput_Kernel/SetStartPosition_res.md#id1 -->

## Description

Sets the start position for loading GradOutput data on a single core.

## Prototype

```
__aicore__ inline void SetStartPosition(uint32_t curDinStartIdx, int32_t curHoStartIdx)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| curDinStartIdx | Input | Start position of the current core in the D direction. |
| curHoStartIdx | Input | Start position of the current core in the H direction. |

## Return Value

None

## Constraints

None

## Examples

```
gradInput_.SetStartPosition(dinStartIdx_, curHoStartIdx_); // Set the start position for loading GradOutput on a single core.
```
