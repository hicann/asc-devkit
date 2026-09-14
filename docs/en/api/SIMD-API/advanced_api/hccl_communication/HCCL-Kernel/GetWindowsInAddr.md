# GetWindowsInAddr

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:18:02.846Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the start address of the WindowsIn data for inter-card communication, which can be directly used as the input and output address for computation to reduce copying. By default, this API runs on all cores. You can also call [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) before invoking it to specify that it runs on a single core.

## Prototype

```
__aicore__ inline GM_ADDR GetWindowsInAddr(uint32_t rankId)
```

## Parameters

**Table 1** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **rankId** | Input | ID of the card to be queried. |

## Return Value

Returns the start address of the inter-card communication data WindowsIn of the corresponding card. When **rankId** is invalid, returns nullptr.

## Constraints

None

## Examples

```
REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); //ReduceScatterCustomTilingData is the structure defined in the corresponding operator header file.
GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);
Hccl hccl;
GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // In the AscendC custom operator kernel, obtain the HCCL context in this way.
hccl.InitV2(contextGM, &tilingData);

auto winInAddr = hccl.GetWindowsInAddr(0);
auto winOutAddr = hccl.GetWindowsOutAddr(0);
auto rankId = hccl.GetRankId();
auto rankDim = hccl.GetRankDim();  // 4 cards.
```

