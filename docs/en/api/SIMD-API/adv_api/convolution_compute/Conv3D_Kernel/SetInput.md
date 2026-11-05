# SetInput

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:14:59.041Z pushedAt=2026-10-29T02:14:34.163Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3D_Kernel/SetInput_res.md#id1 -->

## Description

Sets the feature matrix Input.

## Prototype

```
__aicore__ inline void SetInput(const AscendC::GlobalTensor<InputT>& input)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| input | Input | Start address of Input in Global Memory. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). The feature matrix Input supports the following data types: half and bfloat16_t. |

## Return Value

None

## Constraints

None

## Examples

```
GlobalTensor<half> inputGm;
inputGm.SetGlobalBuffer(reinterpret_cast<__gm__ half*>(input));
conv3dApi.SetInput(inputGm);
```
