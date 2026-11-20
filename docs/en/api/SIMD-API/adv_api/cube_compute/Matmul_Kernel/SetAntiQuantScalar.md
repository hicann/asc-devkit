# SetAntiQuantScalar

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-02T06:39:45.113Z pushedAt=2026-11-18T04:53:22.903Z nl=2 -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Not supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Not supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90 processor products: Not supported
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030 processor products: Not supported
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetAntiQuantScalar_res.md#id1 -->

## Description

During Matmul computation, the A cube supports half-type input and the B cube supports int8-type input. In this scenario, the anti-quantization API must be called to perform anti-quantization. After the anti-quantization API is called, when data is moved from GM to L1, the anti-quantization operation is performed to convert the B cube to half type. The anti-quantization API in this section applies the same quantization coefficient to all data of the B cube for anti-quantization.

Call this API before [Iterate](Iterate.md) or [IterateAll](IterateAll.md).

## Prototype

```
__aicore__ inline void SetAntiQuantScalar(const SrcT offsetScalar, const SrcT scaleScalar)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| offsetScalar | Input | Anti-quantization coefficient used for addition. **SrcT** is the data type corresponding to **A_TYPE**. |
| scaleScalar | Input | Anti-quantization coefficient used for multiplication. **SrcT** is the data type corresponding to **A_TYPE**. |

## Return Value

None

## Constraints

None
