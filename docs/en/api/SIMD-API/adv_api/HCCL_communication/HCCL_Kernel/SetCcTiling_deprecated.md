# SetCcTiling (Deprecated)

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:38:28.716Z pushedAt=2026-11-18T04:57:05.533Z nl=2 -->

> [!NOTE] Note
>This API is deprecated and will be removed in a later version. Do not use it. Use the [SetCcTilingV2](SetCcTilingV2.md) API to set the Tiling address of the communication algorithm.

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/SetCcTiling_deprecated_res.md#id1 -->

## Description

Sets the Tiling address of the HCCL client communication algorithm.

## Prototype

```
__aicore__ inline int32_t SetCcTiling(__gm__ void* ccOpTilingData)
```

## Parameters

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ccOpTilingData | Input | Address of the [Mc2CcTiling](../HCCL_Tiling/TilingData_struct.md#table678914014562) parameter of the communication algorithm. Mc2CcTiling is computed on the host side. For details, see [Table 2 Mc2CcTiling parameter description](../HCCL_Tiling/TilingData_struct.md#table678914014562). It is passed by the framework to the kernel function for use. |

## Return Value

-   HCCL\_SUCCESS, indicating success.
-   HCCL\_FAILED, indicating failure.

## Constraints

-   For the same communication algorithm with identical parameters, this API needs to be called only once before calling the **Prepare** API; otherwise, it needs to be called multiple times.
-   Only one ccOpTilingData address can be set for the same communication algorithm. For the same communication algorithm, calling this API repeatedly overwrites the ccOpTilingData address of that communication algorithm.
-   If this API is called, it must be used together with the [Init](Init_deprecated.md) API that passes the initTiling address, and the **Init** API must be called before this API.
-   If this API is called, the TilingData structure must be defined using standard C++ syntax.
