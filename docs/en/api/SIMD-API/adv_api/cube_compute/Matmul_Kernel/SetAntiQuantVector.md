# SetAntiQuantVector

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:44:46.455Z pushedAt=2026-10-21T10:54:43.986Z -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetAntiQuantVector_res.md#id1 -->

## Description

During Matmul computation, the A cube supports half type input and the B cube supports int8 type input. In this scenario, the pseudo-quantization API must be called to perform pseudo-quantization. After the pseudo-quantization API is called, when data is moved from GM to L1, the pseudo-quantization operation is performed to convert the B cube to the half type. The pseudo-quantization API in this section provides a quantization parameter vector whose shape is \[1, N\], where N is the N value among M/N/K in Matmul cube computation. Each column of the B cube is pseudo-quantized using the pseudo-quantization coefficient of the corresponding column in this vector.

Call this API before [Iterate](Iterate.md) or [IterateAll](IterateAll.md).

## Prototype

```
__aicore__ inline void SetAntiQuantVector(const LocalTensor<SrcT>& offsetTensor, const LocalTensor<SrcT>& scaleTensor)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **offsetTensor** | Input | Parameter vector used for addition in the pseudo-quantization operation. **SrcT** is the data type corresponding to **A_TYPE**. |
| **scaleTensor** | Input | Parameter vector used for multiplication in the pseudo-quantization operation. **SrcT** is the data type corresponding to **A_TYPE**. |

## Return Value

None

## Constraints

None
