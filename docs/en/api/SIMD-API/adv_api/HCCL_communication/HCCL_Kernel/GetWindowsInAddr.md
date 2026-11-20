# GetWindowsInAddr

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:30:34.881Z pushedAt=2026-11-18T04:56:37.900Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/GetWindowsInAddr_res.md#id1 -->

## Description

Obtains the WindowsIn start address of inter-card communication data, which can be used directly as the input/output address for computation to reduce copying. By default, this API works on all cores. Users can also specify it to run on a single core by calling [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) before invoking it.

## Prototype

```
__aicore__ inline GM_ADDR GetWindowsInAddr(uint32_t rankId)
```

## Parameters

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| rankId | Input | ID of the card to be queried. |

## Return Value

Start address of the WindowsIn inter-card communication data of the corresponding card. Returns **nullptr** when **rankId** is invalid.

## Constraints

None

## Examples

```
REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); // ReduceScatterCustomTilingData is the structure defined in the corresponding operator header file.
GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);
Hccl hccl;
GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In an AscendC custom operator kernel function (Kernel), obtain the HCCL context in this way.
hccl.InitV2(contextGM, &tilingData);

auto winInAddr = hccl.GetWindowsInAddr(0);
auto winOutAddr = hccl.GetWindowsOutAddr(0);
auto rankId = hccl.GetRankId();
auto rankDim = hccl.GetRankDim(); // 4 cards.
```
