# GetSortOffset

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T17:09:50.801Z -->

## Description

Obtains the corresponding offset (in bytes) in the Sort data based on the element position.

## Prototype

```
template <typename T>
__aicore__ inline uint32_t GetSortOffset(const uint32_t elemOffset)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |

**Table 2**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| elemOffset | Input | Position of the element. |

## Return Value

Corresponding offset in the Sort data.

## Constraints

None

## Examples

```
AscendC::GetSortOffset<half>(128);
```

