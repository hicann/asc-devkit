# GetRankId

<!-- md-trans-meta sourceCommit=6024fbdeffe5f05ee90ea6f5e52eb8c784f03267 translatedAt=2026-09-10T02:18:17.977Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Gets the RankId of this card. This API works on all cores by default; you can also specify a particular core for it to run on by calling [GetBlockIdx](../../../basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) before invoking it.

## Prototype

```
__aicore__ inline uint32_t GetRankId()
```

## Parameters

None

## Return Value

Returns the **RankId** of the local card.

## Constraints

None

## Examples

See [GetWindowsInAddr](GetWindowsInAddr.md) for examples.

