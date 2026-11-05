# Init

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:27:29.398Z pushedAt=2026-10-29T02:14:34.240Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3DBackpropFilter_Kernel/Init_res.md#id1 -->

## Description

Init is mainly used to initialize the tiling data in the Conv3DBackpropFilter object and divide resources based on the tiling parameters. For details about the tiling parameters, see [Conv3DBackpropFilter Tiling-Side APIs](../Conv3DBackpropFilter_Tiling/Conv3DBackpropFilter_Tiling.md).

## Prototype

```
__aicore__ inline void Init(const TConv3DBpFilterTiling* __restrict tiling)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Input | Tiling parameters of the Conv3DBackpropFilter object. For the definition of the Conv3DBackpropFilterTilingData structure, see [TConv3DBpFilterTiling Structure](../Conv3DBackpropFilter_Tiling/TConv3DBpFilterTiling_struct.md).<br><br>The Tiling parameters can be obtained through the Host-side [GetTiling](../Conv3DBackpropFilter_Tiling/GetTiling.md) API and passed to the kernel function (Kernel) side for use. |

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
// ...Initialize the tilingData, create the Conv3DBackpropFilter object, and call the init API.
ConvBackpropApi::Conv3DBackpropFilter <inputType, weightSizeType, gradOutputType, gradWeightType > gradWeight_;
gradWeight_.Init(&(tilingData->dwTiling));
```
