# SetSingleShape

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:21:44.625Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Sets the shape for Conv3DBackpropFilter computation on a single core. Used to modify the shape at runtime.

## Prototype

```
__aicore__ inline void SetSingleShape(uint64_t singleCoreM, uint64_t singleCoreN, uint64_t singleCoreK)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| singleCoreM | Input | Size of M on a single core, in number of elements. |
| singleCoreN | Input | Size of N on a single core, in number of elements. |
| singleCoreK | Input | Size of K on a single core, in number of elements. |

## Return Value

None

## Constraints

None

## Examples

```
const Conv3DBackpropFilterTilingData* tilingData;
// ...Initialize tilingData.
ConvBackpropApi::Conv3DBackpropFilter <inputType, weightSizeType, gradOutputType, gradWeightType > gradWeight_;
....
gradWeight_.Init(&(tilingData->dwTiling));
gradWeight_.SetInput(inputGm_[offsetB_]);
gradWeight_.SetGradOutput(gradOutputGm_[offsetA_]);
gradWeight_.SetSingleShape(singleCoreM, singleCoreN, singleCoreK);// Set the single-core computation amount.
....
```

