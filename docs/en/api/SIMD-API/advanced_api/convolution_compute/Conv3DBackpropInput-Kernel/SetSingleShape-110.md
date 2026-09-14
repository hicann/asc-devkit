# SetSingleShape

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:32:18.254Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Sets the shape computed by **Conv3DBackpropInput** on a single core, in number of elements.

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
gradInput_.SetSingleShape(singleShapeM_, singleShapeK_, singleShapeN_); // Set the shape for single-core computation.
```

