# GetSortLen

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T05:08:38.318Z pushedAt=2026-11-18T04:58:48.989Z nl=2 -->

## Description

Obtains the size of the Sort data (in bytes) based on the number of elements.

## Prototype

```
template <typename T>
__aicore__ inline uint32_t GetSortLen(const uint32_t elemCount)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |

**Table 2** API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| elemCount | Input | Number of input elements. |

## Return Value

Size of the Sort data (in bytes).

## Constraints

None

## Examples

```
AscendC::GetSortLen<half>(128);
```
