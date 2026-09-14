# SetInput

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T14:21:05.133Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Sets the feature matrix Input.

## Prototype

```
__aicore__ inline void SetInput(const AscendC::GlobalTensor<SrcT> &input)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| input | Input | First address of Input in Global Memory. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). The data type SrcT supported by the feature matrix Input is: half, bfloat16_t. |

## Return Value

None

## Constraints

None

## Examples

```
const Conv3DBackpropFilterTilingData* tilingData;
// ...Initialize tilingData.
ConvBackpropApi::Conv3DBackpropFilter <inputType, weightSizeType, gradOutputType, gradWeightType > gradWeight_;
gradWeight_.Init(&(tilingData->dwTiling));
// Set Input.
gradWeight_.SetInput(inputGm_[offsetB_]);
...
```
