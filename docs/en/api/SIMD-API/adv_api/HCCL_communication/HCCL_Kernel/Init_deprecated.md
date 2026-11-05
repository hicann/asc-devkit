# Init (Deprecated)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:49:03.669Z pushedAt=2026-10-27T11:44:16.327Z nl=2 -->

> [!NOTE]
> This API is deprecated and will be removed in a later version. Do not use this API. Use the [InitV2](InitV2.md) API for initialization instead.

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
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
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/Init_deprecated_res.md#id1 -->

## Description

**HCCL** client initialization API. By default, this API works on all cores. Users can also specify a specific core for it to run on by calling [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) before invoking it.

## Prototype

```
__aicore__ inline void Init(GM_ADDR context, __gm__ void* initTiling = nullptr)
```

## Parameters

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| context | Input | Communication context, containing related information such as rankDim and rankID. |
| initTiling | Input | Optional parameter, the address of the communicator initialization [Mc2InitTiling](../HCCL_Tiling/TilingData_struct.md#table4835205712588). [Mc2InitTiling](../HCCL_Tiling/TilingData_struct.md#table4835205712588) is computed on the host side. For details, see [Table 1 Mc2InitTiling parameter description](../HCCL_Tiling/TilingData_struct.md#table4835205712588). It is passed by the framework to the kernel function for use. For a complete example, see [8.13.1.2-Examples](HCCL_template_params.md#examples). |

## Return Value

None

## Constraints

-   If the **initTiling** parameter is passed when calling this API, it must be used together with the [SetCcTiling](SetCcTiling_deprecated.md) API.
-   In the same program, APIs that pass the optional **initTiling** parameter and APIs that do not pass the optional **initTiling** parameter cannot be called at the same time. It is recommended to use the calling method that passes the **initTiling** parameter.
-   If the **initTiling** parameter is passed when calling this API, you must use the development method of defining the TilingData structure using standard C++ syntax.
-   This API does not support initializing multiple HCCL objects with the same context. Each HCCL object should obtain its own communication context.
