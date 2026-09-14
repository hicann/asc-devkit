# SetCommBlockNum

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-10T02:30:11.404Z -->

## Description

Sets the number of cores that participate in BatchWrite communication.

## Prototype

```
uint32_t SetCommBlockNum(uint16_t num)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| num | Input | Number of cores. |

## Return Value

-   0 indicates that the setting is successful.
-   A non-zero value indicates that the setting fails.

## Constraints

This API takes effect only on Atlas A3 training products/Atlas A3 inference products when the communication type is HCCL\_CMD\_BATCH\_WRITE.

## Examples

```
const char *groupName = "testGroup";
uint32_t opType = HCCL_CMD_BATCH_WRITE;
std::string algConfig = "BatchWrite=level0:fullmesh";
uint32_t reduceType = HCCL_REDUCE_SUM;
AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, opType, algConfig, reduceType);
mc2CcTilingConfig.SetCommBlockNum(24U);
```

