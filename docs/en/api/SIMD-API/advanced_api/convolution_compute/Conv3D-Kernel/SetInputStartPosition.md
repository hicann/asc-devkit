# SetInputStartPosition

<!-- md-trans-meta sourceCommit=e59e1725727133ce76073dba21b022a1b0c9fd56 translatedAt=2026-09-05T14:07:42.855Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Sets the start position for loading data into the **Input** feature matrix on a single core.

## Prototype

```
__aicore__ inline void SetInputStartPosition(int64_t diStartPos, int64_t mStartPos)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **diStartPos** | Input | Start position in the Din direction on a single core. |
| **mStartPos** | Input | Start position in the M direction on a single core. |

## Return Value

None

## Constraints

None

## Examples

```
conv3dApi.SetInputStartPosition(diIdxStart, mIdxStart);
```

