# GetRankDim

<!-- md-trans-meta sourceCommit=6024fbdeffe5f05ee90ea6f5e52eb8c784f03267 translatedAt=2026-09-10T02:17:37.074Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Obtains the number of ranks in the communication domain. By default, this API runs on all cores. You can also call [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) before invoking this API to specify that it runs on a specific core.

## Prototype

```
__aicore__ inline uint32_t GetRankDim()
```

## Parameters

None

## Return Value

Returns the number of ranks in the communication domain.

## Constraints

None

## Examples

See the example in [GetWindowsInAddr](GetWindowsInAddr.md).

