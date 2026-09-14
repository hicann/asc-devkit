# GetOffsetC

<!-- md-trans-meta sourceCommit=25cbbdbbc0dfed820dc1aabf93d431934fee7576 translatedAt=2026-09-09T06:29:22.983Z pushedAt=2026-09-12T09:55:18.097Z -->

## Description

This API is reserved for future use.

Obtains the position of the current slice in the entire C matrix during this computation.

## Prototype

```
__aicore__ inline MatrixOffset GetOffsetC()
```

## Parameters

None

## Return Value

The **MatrixOffset** structure is as follows:

```
struct MatrixOffset {
    int32_t offset;
    int32_t row, col;
    int32_t height, width;
};
```

## Constraints

None

