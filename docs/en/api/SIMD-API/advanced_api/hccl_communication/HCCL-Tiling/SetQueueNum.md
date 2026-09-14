# SetQueueNum

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:32:19.506Z -->

## Description

Sets the number of BatchWrite communication queues on each core that delivers tasks to the server.

## Prototype

```
uint32_t SetQueueNum(uint16_t num)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| num | Input | Number of queues. The supported value range of the number of cores participating in communication * the number of queues is [0, 40]. For details about how to set the number of cores participating in communication, see [SetCommBlockNum](SetCommBlockNum.md). |

## Return Value

-   0 indicates that the setting is successful.
-   A non-zero value indicates that the setting fails.

## Constraints

This API takes effect only when the communication type is HCCL\_CMD\_BATCH\_WRITE on Atlas A3 training products/Atlas A3 inference products.

## Examples

```
const char *groupName = "testGroup";
uint32_t opType = HCCL_CMD_BATCH_WRITE;
std::string algConfig = "BatchWrite=level0:fullmesh";
uint32_t reduceType = HCCL_REDUCE_SUM;
AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, opType, algConfig, reduceType);
mc2CcTilingConfig.SetQueueNum(2U);
```

