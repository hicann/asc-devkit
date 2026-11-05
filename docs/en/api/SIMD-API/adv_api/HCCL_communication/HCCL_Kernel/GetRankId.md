# GetRankId

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:46:15.937Z pushedAt=2026-10-27T11:43:32.889Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/GetRankId_res.md#id1 -->

## Description

Obtains the **RankId** of the current card. By default, this API works on all cores. Users can also call [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) to specify that it runs on a specific core before calling this API.

## Prototype

```
__aicore__ inline uint32_t GetRankId()
```

## Parameters

None

## Return Value

Returns the **RankId** of the current card.

## Constraints

None

## Examples

See the example in [GetWindowsInAddr](GetWindowsInAddr.md).
