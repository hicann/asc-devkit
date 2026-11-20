# SetSingleShape

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T06:27:25.511Z pushedAt=2026-11-18T11:07:26.652Z nl=2 -->

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
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3DBackpropFilter_Kernel/SetSingleShape_res.md#id1 -->

## Description

Sets the shape computed by Conv3DBackpropFilter on a single core. Used to modify the shape at runtime.

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
