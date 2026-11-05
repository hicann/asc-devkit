# InitV2

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:50:48.092Z pushedAt=2026-10-27T11:45:15.895Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/InitV2_res.md#id1 -->

## Description

**HCCL** client initialization API. By default, this API works on all cores. Users can also specify it to run on a specific core by calling [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) before invoking it.

## Prototype

```
__aicore__ inline void InitV2(GM_ADDR context, const void* initTiling)
```

## Parameters

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| context | Input | Communication context, containing information such as **rankDim** and **rankID**. Obtain the context through the framework-provided API [GetHcclContext](../HCCL-Context/GetHcclContext.md). |
| initTiling | Input | Address of [Mc2InitTiling](../HCCL_Tiling/TilingData_struct.md#table4835205712588) for communicator initialization. [Mc2InitTiling](../HCCL_Tiling/TilingData_struct.md#table4835205712588) is computed on the host side. For details, see [Table 1 Mc2InitTiling parameter description](../HCCL_Tiling/TilingData_struct.md#table4835205712588). It is passed by the framework to the kernel function for use. |

## Return Value

None

## Constraints

-   This API must be used together with the [SetCcTilingV2](SetCcTilingV2.md) API.
-   When calling this API, you must use the standard C++ syntax to define the **TilingData** structure.
-   The **initTiling** parameter passed to this API cannot use a Global Memory address. It is recommended to obtain the stack address of **TilingData** through the [GET\_TILING\_DATA\_WITH\_STRUCT](../../../basic_api/Kernel-Tiling/GET_TILING_DATA_WITH_STRUCT.md) API.
-   This API does not support initializing multiple HCCL objects with the same context.

## Examples

User-defined **TilingData** structure:

```
class UserCustomTilingData {
    AscendC::tiling::Mc2InitTiling initTiling;
    AscendC::tiling::Mc2CcTiling tiling;
    CustomTiling param;
};
```

Create an HCCL object on all cores and call the **InitV2** API to initialize it:

```
extern "C" __global__ __aicore__ void userKernel(GM_ADDR aGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
{
    REGISTER_TILING_DEFAULT(UserCustomTilingData);
    GET_TILING_DATA_WITH_STRUCT(UserCustomTilingData, tilingData, tilingGM);

    GM_ADDR contextGM = AscendC::GetHcclContext<0>();
    Hccl hccl;
    hccl.InitV2(contextGM, &tilingData);
    hccl.SetCcTilingV2(offsetof(UserCustomTilingData, tiling));

    // Call the Prepare, Commit, Wait, and Finalize APIs of HCCL.
}
```
