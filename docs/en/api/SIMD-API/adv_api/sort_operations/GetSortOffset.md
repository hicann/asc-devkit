# GetSortOffset

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T05:10:10.573Z pushedAt=2026-11-18T04:58:53.297Z nl=2 -->

## Description

Obtains the corresponding offset (in bytes) in the Sort data based on the element position.

## Prototype

```
template <typename T>
__aicore__ inline uint32_t GetSortOffset(const uint32_t elemOffset)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |

**Table 2** API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| elemOffset | Input | Element position. |

## Return Value

Corresponding offset in the Sort data.

## Constraints

None

## Examples

```
AscendC::GetSortOffset<half>(128);
```
