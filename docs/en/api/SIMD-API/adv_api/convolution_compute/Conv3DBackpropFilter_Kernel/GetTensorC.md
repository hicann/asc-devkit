# GetTensorC

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T06:24:43.009Z pushedAt=2026-11-18T11:07:26.588Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3DBackpropFilter_Kernel/GetTensorC_res.md#id1 -->

## Description

Call this API after the Iterate operation is complete to obtain the result matrix block and transfer data from L0C to GM. This API is used together with [Iterate](../Conv3DBackpropInput_Kernel/Iterate.md) to obtain the result matrix after Iterate performs iterative computation.

## Prototype

```
template <bool sync = true>
__aicore__ inline void GetTensorC(const AscendC::GlobalTensor<DstT>& output, uint8_t enAtomic = 1, bool enSequentialWrite = false)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| sync | Reserved parameter. You do not need to be aware of it. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| output | Input | GM address in Global Memory to which the computation result is moved. |
| enAtomic | Input | Reserved parameter. You do not need to be aware of it. |
| enSequentialWrite | Input | Reserved parameter. You do not need to be aware of it. |

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
while (gradWeight_.Iterate()) {
    gradWeight_.GetTensorC(gradWeightGm_[offsetC_]);
}
```
