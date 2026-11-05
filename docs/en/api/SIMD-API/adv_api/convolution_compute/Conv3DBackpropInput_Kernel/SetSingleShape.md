# SetSingleShape

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:41:16.860Z pushedAt=2026-10-29T02:14:34.325Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3DBackpropInput_Kernel/SetSingleShape_res.md#id1 -->

## Description

Sets the shape computed by Conv3DBackpropInput on a single core, in elements.

## Prototype

```
__aicore__ inline void SetSingleShape(uint64_t singleShapeM, uint64_t singleShapeK, uint32_t singleShapeN)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| singleShapeM | Input | Size of M on a single core, in elements. |
| singleShapeK | Input | Size of K on a single core, in elements. |
| singleShapeN | Input | Size of N on a single core, in elements. |

## Return Value

None

## Constraints

None

## Examples

```
const Conv3DBackpropInputTilingData* tilingData;
// ...Initialize tilingData.
ConvBackpropApi::Conv3DBackpropInput<weightDxType, inputSizeDxType, gradOutputDxType, gradInputDxType> gradInput_;
// ...Set other parameters.
gradInput_.SetSingleShape(singleShapeM_, singleShapeK_, singleShapeN_); // Set the shape computed on a single core.
```
