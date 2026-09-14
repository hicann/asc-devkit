# Init

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:20:24.682Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

**Init** is mainly used to initialize the Tiling data in the **Conv3DBackpropFilter** object and divide resources based on the Tiling parameters. For details about the Tiling parameters, see [Conv3DBackpropFilter Tiling APIs](../Conv3DBackpropFilter-Tiling/conv3dbackpropfilter_tiling_api.md).

## Prototype

```
__aicore__ inline void Init(const TConv3DBpFilterTiling *__restrict tiling)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Input | Tiling parameters of the **Conv3DBackpropFilter** object. For the definition of the **Conv3DBackpropFilterTilingData** structure, see [TConv3DBpFilterTiling structure](../Conv3DBackpropFilter-Tiling/tconv3dbpfiltertiling_structure.md).<br><br>The Tiling parameters can be obtained through the Host-side [GetTiling](../Conv3DBackpropFilter-Tiling/GetTiling-128.md) API and passed to the Kernel side for use. |

## Return Value

None

## Constraints

The **Init** API must be called before the **Iterate**, **GetTensorC**, and **End** APIs, and it can be called only once. The calling sequence is as follows.

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
// ...Initialize tilingData, create the Conv3DBackpropFilter object, and call the init API.
ConvBackpropApi::Conv3DBackpropFilter <inputType, weightSizeType, gradOutputType, gradWeightType > gradWeight_;
gradWeight_.Init(&(tilingData->dwTiling));
```

