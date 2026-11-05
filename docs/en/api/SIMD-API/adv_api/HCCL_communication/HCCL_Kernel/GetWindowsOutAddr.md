# GetWindowsOutAddr

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:47:00.129Z pushedAt=2026-10-27T11:43:53.848Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/GetWindowsOutAddr_res.md#id1 -->

## Description

Obtains the start address of the WindowsOut data for inter-card communication, which can be used directly as the input/output address for computation to reduce copying. This API works on all cores by default. You can also call [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) before invoking it to specify that it runs on a particular core.

## Prototype

```
__aicore__ inline GM_ADDR GetWindowsOutAddr(uint32_t rankId)
```

## Parameters

**Table 1**  API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| rankId | Input | ID of the card to be queried. |

## Return Value

Returns the start address of the WindowsOut inter-card communication data of the corresponding card. Returns **nullptr** when **rankId** is invalid.

## Constraints

None

## Examples

See the example in [GetWindowsInAddr](GetWindowsInAddr.md).
