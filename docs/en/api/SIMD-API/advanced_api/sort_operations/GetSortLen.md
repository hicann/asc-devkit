# GetSortLen

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T17:07:27.782Z -->

## Description

Obtains the size of the sort data (in bytes) based on the number of elements.

## Prototype

```
template <typename T>
__aicore__ inline uint32_t GetSortLen(const uint32_t elemCount)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |

**Table 2**  API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| elemCount | Input | Number of input elements. |

## Return Value

Size of the sort data (in bytes).

## Constraints

None

## Examples

```
AscendC::GetSortLen<half>(128);
```

