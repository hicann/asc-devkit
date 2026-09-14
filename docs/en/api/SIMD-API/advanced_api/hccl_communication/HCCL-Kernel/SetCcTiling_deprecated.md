# SetCcTiling (Deprecated)

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-10T02:25:26.870Z -->

> [!NOTE] Note
> This API is deprecated and will be removed in a later version. Do not use it. Use the [SetCcTilingV2](SetCcTilingV2.md) API to set the Tiling address of the communication algorithm.

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Sets the Tiling address of the HCCL client communication algorithm.

## Prototype

```
__aicore__ inline int32_t SetCcTiling(__gm__ void *ccOpTilingData)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ccOpTilingData | Input | Address of the [Mc2CcTiling](../HCCL-Tiling/tilingdata_structure.md#table678914014562) parameter of the communication algorithm. Mc2CcTiling is computed on the Host side. For details, see [Table 2 Mc2CcTiling parameters](../HCCL-Tiling/tilingdata_structure.md#table678914014562). It is passed by the framework to the Kernel function for use. |

## Return Value

-   HCCL\_SUCCESS, indicating success.
-   HCCL\_FAILED, indicating failure.

## Constraints

-   For the same communication algorithm with identical parameters, this API needs to be called only once before the **Prepare** API is called; otherwise, it needs to be called multiple times.
-   Only one **ccOpTilingData** address can be set for the same communication algorithm. For the same communication algorithm, calling this API repeatedly overwrites the **ccOpTilingData** address of that communication algorithm.
-   If this API is called, it must be used together with the [Init](Init_deprecated.md) API that passes the **initTiling** address, and the **Init** API must be called before this API.
-   If this API is called, the development method of defining the **TilingData** structure using standard C++ syntax must be used.

