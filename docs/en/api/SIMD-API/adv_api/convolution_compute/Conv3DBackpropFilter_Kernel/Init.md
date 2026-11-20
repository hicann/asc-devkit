# Init

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T06:25:08.159Z pushedAt=2026-11-18T11:07:26.604Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3DBackpropFilter_Kernel/Init_res.md#id1 -->

## Description

Init is used to initialize the Tiling data in the Conv3DBackpropFilter object and divide resources based on the Tiling parameters. For details about the Tiling parameters, see [Conv3DBackpropFilter Tiling-Side APIs](../Conv3DBackpropFilter_Tiling/Conv3DBackpropFilter_Tiling.md).

## Prototype

```
__aicore__ inline void Init(const TConv3DBpFilterTiling* __restrict tiling)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Input | Tiling parameters of the Conv3DBackpropFilter object. For the definition of the Conv3DBackpropFilterTilingData structure, see [TConv3DBpFilterTiling Structure](../Conv3DBackpropFilter_Tiling/TConv3DBpFilterTiling_struct.md).<br><br>The Tiling parameters can be obtained by calling the Host-side [GetTiling](../Conv3DBackpropFilter_Tiling/GetTiling.md) API and passed to the kernel-side for use. |

## Return Value

None

## Constraints

The Init API must be called before the Iterate, GetTensorC, and End APIs, and it can be called only once. The calling sequence is as follows.

```
Init(...);
...
Iterate(...);
GetTensorC(...);
End();
```

## Examples

```
const Conv3DBackpropFilterTilingData* tilingData;
// ...Initialize tilingData, create the Conv3DBackpropFilter object, and call the Init API.
ConvBackpropApi::Conv3DBackpropFilter <inputType, weightSizeType, gradOutputType, gradWeightType > gradWeight_;
gradWeight_.Init(&(tilingData->dwTiling));
```
