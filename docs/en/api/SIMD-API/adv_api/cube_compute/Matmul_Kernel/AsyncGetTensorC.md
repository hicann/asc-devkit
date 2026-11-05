# AsyncGetTensorC

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:14:39.982Z pushedAt=2026-10-21T08:43:42.379Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2: Supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/AsyncGetTensorC_res.md#id1 -->

## Description

Obtains the result cube of the asynchronous computation of the **Iterate** API. The functionality of this API has been covered by [GetTensorC](GetTensorC.md); it is recommended to directly use the **GetTensorC** asynchronous API.

## Prototype

```
__aicore__ inline void AsyncGetTensorC(const LocalTensor<DstT>& c)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **c** | Output | Result cube. |

## Return Value

None

## Constraints

When the MixDualMaster (dual-master mode) scenario is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.
