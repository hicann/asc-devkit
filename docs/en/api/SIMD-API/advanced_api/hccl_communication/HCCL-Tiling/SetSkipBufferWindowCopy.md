# SetSkipBufferWindowCopy

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:33:03.095Z -->

## Description

Sets the location from which the communication algorithm obtains input data.

## Prototype

```
uint32_t SetSkipBufferWindowCopy(uint8_t skipBufferWindowCopy)
```

## Parameters

**Table 1**  Parameter Name

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| skipBufferWindowCopy | Input | Indicates the location from which the communication algorithm obtains the input data. The type is uint8_t.<br><br>For Atlas A2 training products/Atlas A2 inference products, the parameter values are as follows:<br>0: The communication input is not placed in windows. If this API is not called to set the location of the communication input, the communication input is not placed in windows by default. Here, windows refers to the shared buffer accessible to other cards.<br>1: The communication input is not placed in windows. Currently, the value 1 has the same function as the value 0.<br>2: The communication input is placed in windows. This applies only to the AllReduce algorithm and the AlltoAll algorithm.<br><br>For Atlas A3 training products/Atlas A3 inference products, this parameter is a reserved field and does not take effect after configuration. |

## Return Value

-   0 indicates success.
-   A non-zero value indicates failure.

## Constraints

None

## Examples

For an example of calling this API, see [Examples](SetOpType.md#examples).
