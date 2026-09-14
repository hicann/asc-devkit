# SetInput

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T14:06:57.562Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Sets the feature matrix **Input**.

## Prototype

```
__aicore__ inline void SetInput(const AscendC::GlobalTensor<InputT>& input)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| input | Input | First address of **Input** in Global Memory. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). The supported data types of the feature matrix **Input** are half and bfloat16_t. |

## Return Value

None

## Constraints

None

## Examples

```
GlobalTensor<half> inputGm;
inputGm.SetGlobalBuffer(reinterpret_cast<__gm__ half *>(input));
conv3dApi.SetInput(inputGm);
```
