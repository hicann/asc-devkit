# End

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T06:27:45.502Z pushedAt=2026-11-18T11:07:26.662Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3DBackpropInput_Kernel/End_res.md#id1 -->

## Description

After the Conv3DBackpropInput convolution backward computation is complete, you must call End once to clear the EventID and release the temporarily allocated memory.

## Prototype

```
__aicore__ inline void End()
```

## Parameters

None

## Return Value

None

## Constraints

The End API must be called after the Iterate and GetTensorC APIs.

## Examples

```
const Conv3DBackpropInputTilingData* tilingData;
// ...Initialize tilingData.
ConvBackpropApi::Conv3DBackpropInput<weightDxType, inputSizeDxType, gradOutputDxType, gradInputDxType> gradInput_;
// ...Other calls.
gradInput_.End();
```

