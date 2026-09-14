# SetStartPosition

<!-- md-trans-meta sourceCommit=e59e1725727133ce76073dba21b022a1b0c9fd56 translatedAt=2026-09-05T14:22:20.609Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Sets the start position for loading GradOutput data on a single core.

## Prototype

```
__aicore__ inline void SetStartPosition(uint32_t hoStartIdx)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **hoStartIdx** | Input | Start position in the Height direction of GradOutput on the current core. |

## Return Value

None

## Constraints

None

## Examples

```
...
gradWeight_.SetSingleShape(singleShapeM, singleShapeN, singleShapeK);
gradWeight_.SetStartPosition(hoStartIdx); // Set the start position for loading GradOutput on a single core.
...
```

