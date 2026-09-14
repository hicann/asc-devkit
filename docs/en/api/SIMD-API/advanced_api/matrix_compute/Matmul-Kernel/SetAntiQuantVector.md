# SetAntiQuantVector

<!-- md-trans-meta sourceCommit=4f4939e215c7dcbcebb0b665d1de1aeb4c46f587 translatedAt=2026-09-09T06:49:56.048Z pushedAt=2026-09-12T09:55:18.124Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

During Matmul computation, the A matrix supports half type input and the B matrix supports int8 type input. In this scenario, you need to call the pseudo-quantization API to perform pseudo-quantization. After the pseudo-quantization API is called, when data is moved from GM to L1, the pseudo-quantization operation is performed to convert the B matrix to the half type. The pseudo-quantization API in this section provides a quantization parameter vector whose shape is \[1, N\], where N is the N value among M/N/K in Matmul matrix computation. Each column of the B matrix is pseudo-quantized using the pseudo-quantization coefficient of the corresponding column in this vector.

Call this API before [Iterate](Iterate.md) or [IterateAll](IterateAll.md).

## Prototype

```
__aicore__ inline void SetAntiQuantVector(const LocalTensor<SrcT> &offsetTensor, const LocalTensor<SrcT> &scaleTensor)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| offsetTensor | Input | Parameter vector used for addition in pseudo-quantization. SrcT is the data type corresponding to A_TYPE. |
| scaleTensor | Input | Parameter vector used for multiplication in pseudo-quantization. SrcT is the data type corresponding to A_TYPE. |

## Return Value

None

## Constraints

None
