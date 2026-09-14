# SetAntiQuantScalar

<!-- md-trans-meta sourceCommit=4f4939e215c7dcbcebb0b665d1de1aeb4c46f587 translatedAt=2026-09-09T06:48:29.573Z pushedAt=2026-09-12T09:55:18.120Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

During Matmul computation, the A matrix supports **half** type input and the B matrix supports **int8** type input. In this scenario, call the fake-quantization API to perform fake-quantization. After the fake-quantization API is called, the fake-quantization operation is performed when data is moved from GM to L1, converting the B matrix to the **half** type. The fake-quantization API in this section provides the function of applying the same quantization coefficient to all data of the B matrix for fake-quantization.

Call this API before [Iterate](Iterate.md) or [IterateAll](IterateAll.md).

## 
Prototype

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
