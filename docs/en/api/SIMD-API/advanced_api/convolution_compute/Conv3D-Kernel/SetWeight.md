# SetWeight

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T14:09:20.896Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Sets the weight matrix **Weight**.

## Prototype

```
__aicore__ inline void SetWeight(const AscendC::GlobalTensor<WeightT>& weight)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| weight | Input | Address of **Weight** in Global Memory. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). The data types supported by the weight matrix **Weight** are: half and bfloat16_t. |

## Return Value

None

## Constraints

None

## Examples

```
GlobalTensor<half> weightGm;
weightGm.SetGlobalBuffer(reinterpret_cast<__gm__ half *>(weight));
conv3dApi.SetWeight(weightGm);
```
