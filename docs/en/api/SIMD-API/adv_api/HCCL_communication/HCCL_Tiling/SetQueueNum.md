# SetQueueNum

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:56:01.453Z pushedAt=2026-11-18T04:58:07.901Z nl=2 -->

## Description

Sets the number of BatchWrite communication queues on each core that sends tasks to the server.

## Prototype

```
uint32_t SetQueueNum(uint16_t num)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| num | Input | Number of queues. The supported value range of the number of cores participating in communication * the number of queues is [0, 40]. For details about setting the number of cores participating in communication, see [SetCommBlockNum](SetCommBlockNum.md). |

## Return Value

-   0 indicates success.
-   A non-zero value indicates failure.

## Constraints

<!-- npu="A3" id1 -->
This API takes effect only on Atlas A3 products when the communication type is HCCL\_CMD\_BATCH\_WRITE.
<!-- end id1 -->

## Examples

```
const char* groupName = "testGroup";
uint32_t opType = HCCL_CMD_BATCH_WRITE;
std::string algConfig = "BatchWrite=level0:fullmesh";
uint32_t reduceType = 0; // BatchWrite has no reduction semantics; this is a placeholder.
AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, opType, algConfig, reduceType);
mc2CcTilingConfig.SetQueueNum(2U);
```
