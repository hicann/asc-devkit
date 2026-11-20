# GetOffsetC

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T06:14:20.836Z pushedAt=2026-11-18T04:52:40.901Z nl=2 -->

## Description

Reserved API for future functionality.

Obtains the position of the current tile in the entire C cube for this computation.

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

