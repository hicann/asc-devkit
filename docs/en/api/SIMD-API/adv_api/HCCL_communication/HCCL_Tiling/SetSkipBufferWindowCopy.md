# SetSkipBufferWindowCopy

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:57:01.670Z pushedAt=2026-11-18T04:58:11.970Z nl=2 -->

## Description

Sets the location from which the communication algorithm obtains input data.

## Prototype

```
uint32_t SetSkipBufferWindowCopy(uint8_t skipBufferWindowCopy)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| skipBufferWindowCopy | Input | Indicates the location from which the communication algorithm obtains input data, of the uint8_t type. <!-- npu="910b" id1 --><br><br>For Atlas A2 products, the parameter values are as follows:<br>**0**: The communication input is not placed in windows. If this API is not called to set the location of the communication input, the communication input is not placed in windows by default. Here, windows refer to the shared buffer accessible to other cards.<br>**1**: The communication input is not placed in windows. Currently, the function of value 1 is the same as that of value 0.<br>**2**: The communication input is placed in windows. This value applies only to the AllReduce and AlltoAll algorithms.<!-- end id1 --><!-- npu="A3" id2 --><br><br>For Atlas A3 products, this parameter is a reserved field and does not take effect after configuration.<!-- end id2 --> |

## Return Value

-   0 indicates success.
-   A non-zero value indicates failure.

## Constraints

None

## Examples

For an example of calling this API, see [Examples](SetOpType.md#examples).
