# SetHF32Mode

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:27:38.264Z pushedAt=2026-10-26T07:41:44.190Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_aux_config/SetHF32Mode_res.md#id1 -->

## Description

Sets whether to enable HF32 mode for Mmad computation. When this mode is enabled, the FP32 data in the L0A Buffer/L0B Buffer is rounded to HF32 before participating in Mmad computation.

## Prototype

```cpp
__aicore__ inline void SetHF32Mode(HF32Mode mode)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| mode | Input | Mmad HF32 mode control input parameter, of the HF32Mode enumeration type. The following two values are supported:<br>&nbsp;&nbsp;&bull; ENABLE: The FP32 data in the L0A Buffer/L0B Buffer is rounded to HF32 before matrix multiplication.<br>&nbsp;&nbsp;&bull; DISABLE: Regular FP32 matrix multiplication is performed. |

## Return Value

None

## Constraints

None

## Example

```cpp
AscendC::SetHF32Mode(HF32Mode::ENABLE); // Control whether HF32 precision is used for mmad computation.
```
