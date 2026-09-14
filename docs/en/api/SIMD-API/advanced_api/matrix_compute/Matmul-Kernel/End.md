# End

<!-- md-trans-meta sourceCommit=4f4939e215c7dcbcebb0b665d1de1aeb4c46f587 translatedAt=2026-09-09T06:23:33.189Z pushedAt=2026-09-12T09:55:18.089Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Supported
- AI Core of Atlas inference products: Supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

When switching between multiple Matmul computation tasks, you must call the **End** function to release the Matmul computation resources and prevent computation resource conflicts among multiple Matmul objects.

## Prototype

```
__aicore__ inline void End()
```

## Parameters

None

## Return Value

None

## Constraints

None

## Examples

```
mm1.IterateAll(gm_c1);
mm1.End();
mm2.IterateAll(gm_c2);
mm2.End();
```
