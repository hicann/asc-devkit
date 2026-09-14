# GetTensorC

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:29:57.518Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Call this API after the Iterate operation is complete to obtain the result matrix block and complete the data transfer from L0C to GM. This API is used together with the [Iterate](Iterate-111.md) API to obtain the result matrix after Iterate performs the iterative computation.

## Prototype

```
template <bool sync = true>
__aicore__ inline void GetTensorC(const AscendC::GlobalTensor<DstT> &output, uint8_t enAtomic = 0, bool enSequentialWrite = false)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| sync | Reserved parameter. Users do not need to be aware of it. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| output | Input | GM address in Global Memory to which the computation result is moved. |
| enAtomic | Input | Reserved parameter. Users do not need to be aware of it. |
| enSequentialWrite | Input | Reserved parameter. Users do not need to be aware of it. |

## Return Value

None

## Constraints

The **GetTensorC** API must be called after **Iterate** to complete the convolution backpropagation implementation. The calling sequence is as follows.

```
while (Iterate()) {
    GetTensorC();
}
```

## Examples

```
while (gradInput_.Iterate()) {
    gradInput_.GetTensorC(gradInputGm_[offsetC_]);
}
```

