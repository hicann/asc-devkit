# SetOpType

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:55:19.763Z pushedAt=2026-11-18T04:58:05.638Z nl=2 -->

## Description

Sets the communication task type.

## Prototype

```
uint32_t SetOpType(uint32_t opType)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| opType | Input | Indicates the communication task type. It is of the uint32_t type. The HCCL API provides the [HcclCMDType](HCCL_Tiling_constructor.md#table2469980529) enumeration definition as the value of this parameter.<!-- npu="950" id1 --><br><br>For the Ascend 950PR&950DT products, the currently supported communication task types are **HCCL_CMD_ALLREDUCE**, **HCCL_CMD_ALLGATHER**, **HCCL_CMD_REDUCE_SCATTER**, **HCCL_CMD_ALLTOALL**, **HCCL_CMD_ALLTOALLV**, and **HCCL_CMD_HALF_ALLTOALLV**.<!-- end id1 --><!-- npu="A3" id2 --><br><br>For the Atlas A3 products, the currently supported communication task types are **HCCL_CMD_ALLREDUCE**, **HCCL_CMD_ALLGATHER**, **HCCL_CMD_REDUCE_SCATTER**, **HCCL_CMD_ALLTOALL**, **HCCL_CMD_ALLTOALLV**, and **HCCL_CMD_BATCH_WRITE**.<!-- end id2 --><!-- npu="910b" id3 --><br><br>For the Atlas A2 products, the currently supported communication task types are **HCCL_CMD_ALLREDUCE**, **HCCL_CMD_ALLGATHER**, **HCCL_CMD_REDUCE_SCATTER**, **HCCL_CMD_ALLTOALL**, and **HCCL_CMD_BATCH_WRITE**.<!-- end id3 --> |

## Return Value

-   0 indicates success.
-   A non-zero value indicates failure.

## Constraints

None

## Examples

```
const char* groupName = "testGroup";
uint32_t opType = HCCL_CMD_REDUCE_SCATTER;
std::string algConfig = "ReduceScatter=level0:doublering";
AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, opType, algConfig, HCCL_REDUCE_RESERVED);
mc2CcTilingConfig.SetReduceType(HCCL_REDUCE_SUM);
mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling);
mc2CcTilingConfig.GetTiling(tiling->reduceScatterTiling);

algConfig = "AllGather=level0:doublering";
mc2CcTilingConfig.SetGroupName(groupName);
mc2CcTilingConfig.SetOpType(HCCL_CMD_ALLGATHER); // Set the communication task type.
mc2CcTilingConfig.SetAlgConfig(algConfig);
mc2CcTilingConfig.SetSkipLocalRankCopy(0);
mc2CcTilingConfig.SetSkipBufferWindowCopy(1);
mc2CcTilingConfig.GetTiling(tiling->allGatherTiling);
```
