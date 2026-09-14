# AsyncGetTensorC

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:22:21.356Z pushedAt=2026-09-12T09:55:18.083Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Obtains the result matrix of the asynchronous computation performed by the **Iterate** API. The functionality of this API is overwritten by [GetTensorC](GetTensorC.md), and it is recommended to use the **GetTensorC** asynchronous API directly.

## Prototype

```
__aicore__ inline void AsyncGetTensorC(const LocalTensor<DstT>& c)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **c** | Output | Result matrix. |

## Return Value

None

## Constraints

When **MixDualMaster** (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to **true**, this API is not supported.
