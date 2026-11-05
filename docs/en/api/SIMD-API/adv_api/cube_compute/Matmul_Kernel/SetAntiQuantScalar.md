# SetAntiQuantScalar

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:42:38.856Z pushedAt=2026-10-21T10:53:07.589Z -->

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
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
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
| **offsetScalar** | Input | Anti-quantization coefficient used for addition. **SrcT** is the data type corresponding to **A_TYPE**. |
| **scaleScalar** | Input | Anti-quantization coefficient used for multiplication. **SrcT** is the data type corresponding to **A_TYPE**. |

## Return Value

None

## Constraints

None
