# SetSplitRange

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:30:46.197Z pushedAt=2026-09-12T09:55:18.218Z -->

## Description

Sets the maximum and minimum values of **baseM**/**baseN**/**baseK**. Currently, **Tiling** does not support this function.

## Prototype

```
int32_t SetSplitRange(int32_t maxBaseM = -1, int32_t maxBaseN = -1, int32_t maxBaseK = -1, int32_t minBaseM = -1, int32_t minBaseN = -1, int32_t minBaseK = -1)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| maxBaseM | Input | Sets the maximum value of baseM. The default value is -1, which means that no maximum value is specified for baseM and the value is computed by the Tiling function. |
| maxBaseN | Input | Sets the maximum value of baseN. The default value is -1, which means that no maximum value is specified for baseN and the value is computed by the Tiling function. |
| maxBaseK | Input | Sets the maximum value of baseK. The default value is -1, which means that no maximum value is specified for baseK and the value is computed by the Tiling function. |
| minBaseM | Input | Sets the minimum value of baseM. The default value is -1, which means that no minimum value is specified for baseM and the value is computed by the Tiling function. |
| minBaseN | Input | Sets the minimum value of baseN. The default value is -1, which means that no minimum value is specified for baseN and the value is computed by the Tiling function. |
| minBaseK | Input | Sets the minimum value of baseK. The default value is -1, which means that no minimum value is specified for baseK and the value is computed by the Tiling function. |

## Return Value

-1 indicates a setting failure; 0 indicates a successful setting.

## Constraints

If **baseM**/**baseN**/**baseK** do not meet the C0\_size alignment, the values are aligned to C0\_size when computing **Tiling**. Note that the C0\_size of the half/bfloat16\_t data type is 16, the C0\_size of the float data type is 8, the C0\_size of the int8\_t data type is 32, and the C0\_size of the int4b\_t data type is 64.

