# Init (Deprecated)

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:22:54.765Z -->

> [!NOTE] Note
> This API is deprecated and will be removed in a later version. Do not use this API. Use the [InitV2](InitV2.md) API for initialization instead.

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

HCCL client-side API for initialization. By default, this API runs on all cores. You can also call [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) before invoking it to specify that it runs on a single core.

## Prototype

```
__aicore__ inline void Init(GM_ADDR context, __gm__ void *initTiling = nullptr)
```

## Parameters

**Table 1**  API parameter description

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| context | Input | Communication context, containing **rankDim**, **rankID**, and other related information. |
| initTiling | Input | Optional parameter. Address of [Mc2InitTiling](../HCCL-Tiling/tilingdata_structure.md#table4835205712588) for communication domain initialization. [Mc2InitTiling](../HCCL-Tiling/tilingdata_structure.md#table4835205712588) is computed on the Host side. For details, see [Table 1 Mc2InitTiling parameter description](../HCCL-Tiling/tilingdata_structure.md#table4835205712588). It is passed by the framework to the kernel function. For a complete example, see [8.13.1.2-Examples](hccl_template_parameter.md#examples). |

## Return Value

None

## Constraints

-   If **initTiling** is passed when calling this API, it must be used together with the [SetCcTiling](SetCcTiling_deprecated.md) API.
-   In the same program, you cannot call both the API that passes the optional **initTiling** parameter and the API that does not pass the optional **initTiling** parameter. It is recommended to use the calling method that passes the **initTiling** parameter.
-   If **initTiling** is passed when calling this API, the **TilingData** structure must be defined using standard C++ syntax.
-   This API does not support initializing multiple HCCL objects with the same context. Each HCCL object should obtain its own communication context.

