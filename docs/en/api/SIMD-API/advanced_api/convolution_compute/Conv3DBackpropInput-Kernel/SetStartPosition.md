# SetStartPosition

<!-- md-trans-meta sourceCommit=e59e1725727133ce76073dba21b022a1b0c9fd56 translatedAt=2026-09-05T14:32:56.078Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Sets the start position for loading **GradOutput** data on a single core.

## Prototype

```
__aicore__ inline void SetStartPosition(uint32_t curDinStartIdx, int32_t curHoStartIdx)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **curDinStartIdx** | Input | Start position of the current core in the D direction. |
| **curHoStartIdx** | Input | Start position of the current core in the H direction. |

## Return Value

None

## Constraints

None

## Examples

```
gradInput_.SetStartPosition(dinStartIdx_, curHoStartIdx_); // Set the start position for loading GradOutput on a single core.
```

