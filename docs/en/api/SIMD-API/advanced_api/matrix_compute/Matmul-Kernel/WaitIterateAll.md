# WaitIterateAll

<!-- md-trans-meta sourceCommit=4f4939e215c7dcbcebb0b665d1de1aeb4c46f587 translatedAt=2026-09-09T07:09:25.974Z pushedAt=2026-09-12T09:55:18.158Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Waits for the **WaitIterateAll** asynchronous API to return, supporting continuous output to Global Memory.

## Function Prototype

```
__aicore__ inline void WaitIterateAll()
```

## Parameters

None

## Return Value

None

## Constraints

-   Used with the [IterateAll](IterateAll.md) asynchronous API.
-   Only continuous output to Global Memory is supported.

## Examples

```
AscendC::Matmul<aType, bType, cType, biasType> mm;
mm.SetTensorA(gm_a[offsetA]);
mm.SetTensorB(gm_b[offsetB]);
if (tiling.isBias) {
    mm.SetBias(gm_bias[offsetBias]);
}
mm.template IterateAll<false>(gm_c[offsetC], 0, false, true);
// do some others compute
mm.WaitIterateAll(); // Wait for IterateAll to complete.
```
