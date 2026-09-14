# SetOpType

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:33:27.717Z -->

## Description

Sets the communication task type.

## Function Prototype

```
uint32_t SetOpType(uint32_t opType)
```

## Parameters

**Table 1** Parameters

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| opType | Input | Communication task type, of the uint32_t type. The HCCL API provides the [HcclCMDType](hccl_tiling_constructor.md#hcclcmdtype-parameter-description) enumeration as the value of this parameter.<br><br>For Ascend 950PR/Ascend 950DT, the currently supported communication task types are HCCL_CMD_ALLREDUCE, HCCL_CMD_ALLGATHER, HCCL_CMD_REDUCE_SCATTER, HCCL_CMD_ALLTOALL, HCCL_CMD_ALLTOALLV, and HCCL_CMD_HALF_ALLTOALLV.<br><br>For Atlas A3 training products/Atlas A3 inference products, the currently supported communication task types are HCCL_CMD_ALLREDUCE, HCCL_CMD_ALLGATHER, HCCL_CMD_REDUCE_SCATTER, HCCL_CMD_ALLTOALL, HCCL_CMD_ALLTOALLV, and HCCL_CMD_BATCH_WRITE.<br><br>For Atlas A2 training products/Atlas A2 inference products, the currently supported communication task types are HCCL_CMD_ALLREDUCE, HCCL_CMD_ALLGATHER, HCCL_CMD_REDUCE_SCATTER, HCCL_CMD_ALLTOALL, and HCCL_CMD_BATCH_WRITE. |

## Return Value

-   0 indicates success.
-   A non-zero value indicates failure.

## Constraints

None

## Examples

```
const char *groupName = "testGroup";
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

