# GetTensorC

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:38:27.584Z pushedAt=2026-10-29T02:14:34.310Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3DBackpropInput_Kernel/GetTensorC_res.md#id1 -->

## Description

Call this API after the Iterate operation to obtain the result matrix block and move data from L0C to GM. This API is used together with [Iterate](Iterate.md) to obtain the result matrix after Iterate performs iterative computation.

## Prototype

```
template <bool sync = true>
__aicore__ inline void GetTensorC(const AscendC::GlobalTensor<DstT>& output, uint8_t enAtomic = 0, bool enSequentialWrite = false)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| sync | Reserved parameter, not perceivable by you. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| output | Input | GM address in Global Memory to which the computation result is moved. |
| enAtomic | Input | Reserved parameter, not perceivable by you. |
| enSequentialWrite | Input | Reserved parameter, not perceivable by you. |

## Return Value

None

## Constraints

The GetTensorC API must be called after Iterate to implement the convolution backward computation. The call sequence is as follows.

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
