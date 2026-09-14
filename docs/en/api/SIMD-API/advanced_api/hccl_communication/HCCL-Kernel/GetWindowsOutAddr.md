# GetWindowsOutAddr

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:18:59.820Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the start address of the WindowsOut data for inter-rank communication, which can be directly used as the input and output address for computation to reduce copying. By default, this API runs on all cores. You can also call [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) before invoking it to specify that it runs on a single core.

## Prototype

```
__aicore__ inline GM_ADDR GetWindowsOutAddr(uint32_t rankId)
```

## Parameters

**Table 1** API parameter description

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| **rankId** | Input | ID of the rank to query. |

## Return Value

Returns the start address of the WindowsOut data for inter-rank communication of the corresponding rank. If **rankId** is invalid, returns **nullptr**.

## Constraints

None

## Examples

See the examples in [GetWindowsInAddr](GetWindowsInAddr.md).

