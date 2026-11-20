# SetSkipLocalRankCopy

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:57:18.144Z pushedAt=2026-11-18T04:58:15.513Z nl=2 -->

## Description

Sets whether the computation result of the communication algorithm on the current card is output to the destination data buffer address.

## Prototype

```
uint32_t SetSkipLocalRankCopy(uint8_t skipLocalRankCopy)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| skipLocalRankCopy | Input | Whether the computation result of the communication algorithm on the current card is output to **recvBuf** (the destination data buffer address).<!-- npu="910b" id1 --><br><br>For Atlas A2 products, only the **AllGather** and **AlltoAll** algorithms support configuring this parameter. The type is **uint8_t**, and the parameter values are as follows:<br>**0**: Outputs the computation result of the communication algorithm on the current card (the default behavior when this API is not called).<br>**1**: Does not output the computation result of the communication algorithm on the current card. When the communication result does not need to be output, set this parameter to **1**. In this case, the communication result data of the current card is not copied, which improves operator performance. For example, in an 8-card scenario, if the current card only takes part of the data from other cards, this parameter can be set to **1**.<!-- end id1 --><!-- npu="A3" id2 --><br><br>For Atlas A3 products, this parameter is a reserved field and does not take effect after configuration.<!-- end id2 --> |

## Return Value

-   0 indicates success.
-   A non-zero value indicates failure.

## Constraints

None

## Examples

For an example of calling this API, see [Examples](SetOpType.md#examples).
