# Init

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:31:26.449Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

**Init** is mainly used to initialize the tiling data in the **Conv3DBackpropInput** object and divide resources based on the tiling parameters. For details about the tiling parameters, see [Conv3DBackpropInput Tiling APIs](../Conv3DBackpropInput-Tiling/conv3dbackpropinput_tiling_api.md).

## Prototype

```
__aicore__ inline void Init(const TConv3DBackpropInputTiling *__restrict tiling)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Input | Tiling parameters of the **Conv3DBackpropInput** object. For the definition of the **TConv3DBackpropInputTiling** structure, see [TConv3DBackpropInputTiling Structure](../Conv3DBackpropInput-Tiling/tconv3dbackpropinputtiling_structure.md).<br><br>The Tiling parameters can be obtained through the Host-side [GetTiling](../Conv3DBackpropInput-Tiling/GetTiling-114.md) API and passed to the Kernel side for use. |

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
const Conv3DBackpropInputTilingData* tilingData;
// ...Initialize the tilingData, create the Conv3DBackpropInput object, and call the init API.
ConvBackpropApi::Conv3DBackpropInput<weightDxType, inputSizeDxType, gradOutputDxType, gradInputDxType> gradInput_;
gradInput_.Init(&(tilingData->conv3DDxTiling));
```

