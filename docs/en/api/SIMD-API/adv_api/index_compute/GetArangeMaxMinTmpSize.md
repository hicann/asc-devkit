# GetArangeMaxMinTmpSize

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-10T06:31:26.477Z pushedAt=2026-11-18T11:07:26.786Z nl=2 -->

## Description

Obtains the Arange Tiling parameters: the maximum temporary space size (max) and the minimum temporary space size (min) required for the Arange API to complete computation.

Because the Arange API does not require temporary space internally, both max and min return 0.

## Prototype

> [!NOTE]
>The GetArithProgressionMaxMinTmpSize API is deprecated and will be removed in future versions. Do not use it. Use the GetArangeMaxMinTmpSize API instead.

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
| maxValue | Output | Maximum temporary space size required for the Arange API to complete computation, in bytes.<br> Note: maxValue is for reference only and may be larger than the remaining UB space. In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Minimum temporary space size required for the Arange API to complete computation, in bytes. |

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
