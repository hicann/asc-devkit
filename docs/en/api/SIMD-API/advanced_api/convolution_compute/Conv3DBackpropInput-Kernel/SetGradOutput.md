# SetGradOutput

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T14:32:03.185Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Sets the input matrix **GradOutput** for convolution backward computation.

## Prototype

```
__aicore__ inline void SetGradOutput(const AscendC::GlobalTensor<SrcT> &gradOutput)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gradOutput | Input | Start address of the GradOutput matrix in Global Memory. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). SrcT indicates the data type of the GradOutput matrix. The currently supported data types are half and bfloat16_t. |

## Return Value

None

## Constraints

None

## Examples

```
ConvBackpropApi::Conv3DBackpropInput<weightDxType, inputSizeDxType, gradOutputDxType, gradInputDxType> gradInput_;
// Set the address of the GlobalTensor in GradOutput.
GlobalTensor<gradOutputType> gradOutputGm_;
gradOutputGm_.SetGlobalBuffer((__gm__ gradOutputType *)gradOutput);
gradInput_.SetGradOutput(gradOutputGm_);
```
