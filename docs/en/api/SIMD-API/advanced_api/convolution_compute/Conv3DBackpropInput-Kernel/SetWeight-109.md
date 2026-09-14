# SetWeight

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T14:34:02.327Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Sets the input matrix **Weight** for convolution backpropagation computation.

## Prototype

```
__aicore__ inline void SetWeight(const AscendC::GlobalTensor<SrcT> &weight)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| weight | Input | Start address of the Weight matrix in Global Memory. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). **SrcT** indicates the data type of the Weight matrix. Currently supported data types are **half** and **bfloat16_t**. |

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
gradInput_.SetWeight(weightGm_[offsetB_]);
```
