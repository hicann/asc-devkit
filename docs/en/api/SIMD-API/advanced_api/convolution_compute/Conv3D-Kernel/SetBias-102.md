# SetBias

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T14:06:49.030Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Sets the bias matrix **Bias**.

## Prototype

```
__aicore__ inline void SetBias(const AscendC::GlobalTensor<BiasT>& bias)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| bias | Input | Address of **Bias** in Global Memory. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). The supported data types of the bias matrix **Bias** are **half** and **bfloat16_t**. |

## Return Value

None

## Constraints

In convolution computation, if the bias matrix **Bias** is involved, this API must be called; if the convolution computation does not involve **Bias**, this API should not be called.

## Examples

```
GlobalTensor<float> biasGm;
biasGm.SetGlobalBuffer(reinterpret_cast<__gm__ half *>(bias));
if (biasFlag) {
    conv3dApi.SetBias(biasGm);
}
```
