# Init

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:41:16.410Z pushedAt=2026-10-29T02:14:34.322Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3DBackpropInput_Kernel/Init_res.md#id1 -->

## Description

Init is used to initialize the Tiling data in the Conv3DBackpropInput object and divide resources based on the Tiling parameters. For details about the Tiling parameters, see [Conv3DBackpropInput Tiling-Side APIs](../Conv3DBackpropInput_Tiling/Conv3DBackpropInput_Tiling.md).

## Prototype

```
__aicore__ inline void Init(const TConv3DBackpropInputTiling* __restrict tiling)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Input | Tiling parameters of the Conv3DBackpropInput object. For the definition of the TConv3DBackpropInputTiling structure, see [TConv3DBackpropInputTiling Structure](../Conv3DBackpropInput_Tiling/TConv3DBackpropInputTiling_struct.md).<br><br>The Tiling parameters can be obtained by calling the Host-side [GetTiling](../Conv3DBackpropInput_Tiling/GetTiling.md) API and passed to the kernel function (Kernel) side for use. |

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
const Conv3DBackpropInputTilingData* tilingData;
// ...After initializing tilingData, create the Conv3DBackpropInput object and call the init API.
ConvBackpropApi::Conv3DBackpropInput<weightDxType, inputSizeDxType, gradOutputDxType, gradInputDxType> gradInput_;
gradInput_.Init(&(tilingData->conv3DDxTiling));
```
