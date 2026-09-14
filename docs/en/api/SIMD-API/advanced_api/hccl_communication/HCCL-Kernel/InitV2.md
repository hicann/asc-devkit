# InitV2

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:22:06.863Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

HCCL client initialization interface. This interface works on all cores by default. Users can also specify that it runs on a particular core by calling [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) before invocation.

## Function Prototype

```
__aicore__ inline void InitV2(GM_ADDR context, const void *initTiling)
```

## Parameters

**Table 1** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| context | Input | Communication context, containing information such as rankDim and rankID. You can obtain context through the API [GetHcclContext](../HCCL-Context/GetHcclContext.md) provided by the framework. |
| initTiling | Input | Address of the communication domain initialization [Mc2InitTiling](../HCCL-Tiling/tilingdata_structure.md#table4835205712588). [Mc2InitTiling](../HCCL-Tiling/tilingdata_structure.md#table4835205712588) is computed on the Host side. For details, see [Table 1 Mc2InitTiling parameter description](../HCCL-Tiling/tilingdata_structure.md#table4835205712588). It is passed by the framework to the Kernel function for use. |

## Return Value

None

## Constraints

-   This API must be used together with [SetCcTilingV2](SetCcTilingV2.md).
-   When calling this API, you must define the TilingData structure using standard C++ syntax.
-   The **initTiling** parameter passed to this API cannot use a Global Memory address. It is recommended that you obtain the stack address of TilingData through the [GET\_TILING\_DATA\_WITH\_STRUCT](../../../basic_api/Kernel-Tiling/GET_TILING_DATA_WITH_STRUCT.md) API.
-   This API does not support initializing multiple HCCL objects with the same context.

## Examples

Define a custom **TilingData** structure:

```
class UserCustomTilingData {
    AscendC::tiling::Mc2InitTiling initTiling;
    AscendC::tiling::Mc2CcTiling tiling;
    CustomTiling param;
};
```

Create an HCCL object on all cores and call the **InitV2** API to initialize it:

```
extern "C" __global__ __aicore__ void userKernel(GM_ADDR aGM, GM_ADDR workspaceGM, GM_ADDR tilingGM) {
    REGISTER_TILING_DEFAULT(UserCustomTilingData);
    GET_TILING_DATA_WITH_STRUCT(UserCustomTilingData,tilingData,tilingGM);

    GM_ADDR contextGM = AscendC::GetHcclContext<0>();
    Hccl hccl;
    hccl.InitV2(contextGM, &tilingData);
    hccl.SetCcTilingV2(offsetof(UserCustomTilingData, tiling));

    // Call the Prepare, Commit, Wait, and Finalize APIs of HCCL.
}
```

