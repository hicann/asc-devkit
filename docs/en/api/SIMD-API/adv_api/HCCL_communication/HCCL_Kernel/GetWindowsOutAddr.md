# GetWindowsOutAddr

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:31:07.130Z pushedAt=2026-11-18T04:56:39.950Z nl=2 -->

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

Start address of the WindowsOut inter-card communication data of the corresponding card. Returns **nullptr** when **rankId** is invalid.

## Constraints

None

## Examples

See the example in [GetWindowsInAddr](GetWindowsInAddr.md).
