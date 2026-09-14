# GetArangeMaxMinTmpSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-08T17:56:59.715Z -->

## Description

Obtains the Arange tiling parameters: the maximum temporary space size **max** and the minimum temporary space size **min** required for the Arange API to complete computation.

Because the Arange API does not require temporary space internally, both **max** and **min** return 0.

## Prototype

> [!NOTE] Description
> The **GetArithProgressionMaxMinTmpSize** API is deprecated and will be removed in a later version. Do not use this API. Use the **GetArangeMaxMinTmpSize** API instead.

```
void GetArangeMaxMinTmpSize(uint32_t& maxValue, uint32_t& minValue)
```

```
void GetArithProgressionMaxMinTmpSize(uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| maxValue | Output | Maximum temporary space size required for the Arange API to complete computation.<br> Description: **maxValue** is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, developers need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Minimum temporary space size required for the Arange API to complete computation. |

## Return Value

None

## Constraints

None

## Examples

```
uint32_t maxValue = 0;
uint32_t minValue = 0;
AscendC::GetArangeMaxMinTmpSize(maxValue, minValue);
```
