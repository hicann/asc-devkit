# SetSkipLocalRankCopy

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:33:05.340Z -->

## Description

Sets whether the computation result of the communication algorithm on the local rank is output to the destination data buffer address.

## Prototype

```
uint32_t SetSkipLocalRankCopy(uint8_t skipLocalRankCopy)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| skipLocalRankCopy | Input | Whether the computation result of the communication algorithm on the local rank is output to recvBuf (the destination data buffer address).<br><br>For Atlas A2 training products/Atlas A2 inference products, only the AllGather and AlltoAll algorithms support configuring this parameter. The parameter is of the uint8_t type, with the following values:<br>0: Output the computation result of the communication algorithm on the local rank (the default behavior when this API is not called).<br>1: Do not output the computation result of the communication algorithm on the local rank. When the communication result does not need to be output, set this parameter to 1. In this case, the communication result data of the local rank is not copied, which improves operator performance. For example, in an 8-rank scenario, if the local rank only takes part of the data from other ranks, this parameter can be set to 1.<br><br>For Atlas A3 training products/Atlas A3 inference products, this parameter is a reserved field and does not take effect after being configured. |

## Return Value

-   0 indicates success.
-   A non-zero value indicates failure.

## Constraints

None

## Examples

For the example of calling this API, see [Examples](SetOpType.md#examples).
