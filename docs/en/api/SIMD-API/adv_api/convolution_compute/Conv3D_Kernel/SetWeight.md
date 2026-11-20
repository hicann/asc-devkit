# SetWeight

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T06:22:19.821Z pushedAt=2026-11-18T11:07:26.520Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3D_Kernel/SetWeight_res.md#id1 -->

## Description

Sets the weight matrix Weight.

## Prototype

```
__aicore__ inline void SetWeight(const AscendC::GlobalTensor<WeightT>& weight)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| weight | Input | Address of Weight in Global Memory. The type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md). The supported data types of the weight matrix Weight are half and bfloat16_t. |

## Return Value

None

## Constraints

None

## Examples

```
GlobalTensor<half> weightGm;
weightGm.SetGlobalBuffer(reinterpret_cast<__gm__ half*>(weight));
conv3dApi.SetWeight(weightGm);
```
