# HCCL Tiling Constructor

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:30:40.871Z -->

## Description

Creates a **Mc2CcTilingConfig** object.

## Function Prototype

```
Mc2CcTilingConfig(const std::string &groupName, uint32_t opType, const std::string &algConfig, uint32_t reduceType = 0, uint8_t dstDataType = 0, uint8_t srcDataType = 0, uint8_t commEngine = 0)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **groupName** | Input | Communicator where the current communication task resides. String type, with a maximum supported length of 128 bytes. |
| **opType** | Input | Indicates the communication task type. uint32_t type. The HCCL API provides the [HcclCMDType](#) enumeration definition as the value of this parameter. For details about the supported communication task types and their values, see [Table 2](#hcclcmdtype-parameter-description). |
| **algConfig** | Input | Communication algorithm configuration. String type, with a maximum supported length of 128 bytes.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is a reserved field and takes no effect after configuration. Only the FullMesh algorithm is supported by default. The FullMesh algorithm is a full mesh between NPUs, where any two NPUs can directly send and receive data.<br><br>For Atlas A3 training products/Atlas A3 inference products, the currently supported values are:<br>"AllReduce=level0:doublering": AllReduce communication task.<br>"AllGather=level0:doublering": AllGather communication task.<br>"ReduceScatter=level0:doublering": ReduceScatter communication task.<br>"AlltoAll=level0:fullmesh;level1:pairwise": AlltoAllV and AlltoAll communication tasks.<br>"BatchWrite=level0:fullmesh": BatchWrite communication task.<br><br>For Atlas A2 training products/Atlas A2 inference products, this parameter is a reserved field and takes no effect after configuration. Only the FullMesh algorithm is supported by default. The FullMesh algorithm is a full mesh between NPUs, where any two NPUs can directly send and receive data. |
| **reduceType** | Input | Reduction operation type, which takes effect only for communication tasks with reduction operations. uint32_t type. For details about the values, see [Table 2](../HCCL-Kernel/hccl_usage.md#hcclreduceop). |
| **dstDataType** | Input | Data type of the output data in the communication task. uint8_t type. For details about the value range of this parameter, see [Table 1](../HCCL-Kernel/hccl_usage.md#table116710585514).<br><br>For Ascend 950PR/Ascend 950DT, different communication tasks support different output data types, as follows:<br>For the AllReduce, AllGather, AllToAll, AllToAllV, and AllToAllVWrite communication tasks: the output data type must be the same as the input data type. For details about the input data types supported by each communication task, see **srcDataType**.<br>For the ReduceScatter communication task, when the input data type is int16_t, int32_t, half, float, or bfloat16_t, the output data type must be the same as the input data type; when the input data type is int8_t, hifloat8_t, fp8_e5m2_t, or fp8_e4m3fn_t, the output data type must be one of half, bfloat16_t, and float.<br><br>For Atlas A3 training products/Atlas A3 inference products, this parameter is not supported and takes no effect after configuration.<br><br>For Atlas A2 training products/Atlas A2 inference products, this parameter is not supported and takes no effect after configuration. |
| **srcDataType** | Input | Data type of the input data in the communication task. uint8_t type. For details about the value range of this parameter, see [Table 1](../HCCL-Kernel/hccl_usage.md#table116710585514).<br><br>For Ascend 950PR/Ascend 950DT, the input data types supported by different communication tasks are as follows:<br>AllReduce communication task: the supported input types are int16_t, half, bfloat16_t, int32_t, and float.<br>AllGather, AllToAll, AllToAllV, and AllToAllVWrite communication tasks: the supported input types are int8_t, uint8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, and double.<br>ReduceScatter communication task: the supported input types are int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, half, bfloat16_t, int32_t, and float.<br><br>For Atlas A3 training products/Atlas A3 inference products, this parameter is not supported and takes no effect after configuration.<br><br>For Atlas A2 training products/Atlas A2 inference products, this parameter is not supported and takes no effect after configuration. |
| **commEngine** | Input | Communication engine. uint8_t type. For details about the value range of this parameter, see the value description of the **hcclOpExpansionMode** parameter in [HcclCommConfig](https://gitcode.com/cann/hcomm/blob/9.1.0/docs/en/api_ref/comm_mgr_c/data_type_definition/HcclCommConfig.md#parameters). |


 <a id="hcclcmdtype-parameter-description"></a>
**Table 2**  HcclCMDType Parameter Description

| Data Type | Description |
| --- | --- |
| HcclCMDType | Communication task type. The HcclCMDType enumeration class is defined as shown in the following code.<br><br>For Ascend 950PR/Ascend 950DT, the currently supported communication task types are HCCL_CMD_ALLREDUCE, HCCL_CMD_ALLGATHER, HCCL_CMD_REDUCE_SCATTER, HCCL_CMD_ALLTOALL, HCCL_CMD_ALLTOALLV, and HCCL_CMD_HALF_ALLTOALLV.<br><br>For Atlas A3 training products/Atlas A3 inference products, the currently supported communication task types are HCCL_CMD_ALLREDUCE, HCCL_CMD_ALLGATHER, HCCL_CMD_REDUCE_SCATTER, HCCL_CMD_ALLTOALL, HCCL_CMD_ALLTOALLV, and HCCL_CMD_BATCH_WRITE.<br><br>For Atlas A2 training products/Atlas A2 inference products, the currently supported communication task types are HCCL_CMD_ALLREDUCE, HCCL_CMD_ALLGATHER, HCCL_CMD_REDUCE_SCATTER, HCCL_CMD_ALLTOALL, and HCCL_CMD_BATCH_WRITE. |

```
enum class HcclCMDType { 
    HCCL_CMD_INVALID = 0,
    HCCL_CMD_BROADCAST = 1,
    HCCL_CMD_ALLREDUCE,
    HCCL_CMD_REDUCE,
    HCCL_CMD_SEND,
    HCCL_CMD_RECEIVE,
    HCCL_CMD_ALLGATHER,
    HCCL_CMD_REDUCE_SCATTER,
    HCCL_CMD_ALLTOALLV,
    HCCL_CMD_ALLTOALLVC,
    HCCL_CMD_ALLTOALL,
    HCCL_CMD_GATHER,
    HCCL_CMD_SCATTER,
    HCCL_CMD_BATCH_SEND_RECV,
    HCCL_CMD_BATCH_PUT,
    HCCL_CMD_BATCH_GET,
    HCCL_CMD_ALLGATHER_V,
    HCCL_CMD_REDUCE_SCATTER_V,
    HCCL_CMD_BATCH_WRITE,
    HCCL_CMD_HALF_ALLTOALLV = 20,
    HCCL_CMD_ALL
};
```

## Return Value

None

## Constraints

None

## Examples

```
const char *groupName = "testGroup";
uint32_t opType = HCCL_CMD_REDUCE_SCATTER;
std::string algConfig = "ReduceScatter=level0:fullmesh";
uint32_t reduceType = HCCL_REDUCE_SUM;
uint8_t dstDataType = HCCL_DATA_TYPE_FP16;
uint8_t srcDataType = HCCL_DATA_TYPE_FP16;
uint8_t commEngine = 0;
AscendC::Mc2CcTilingConfig mc2CcTilingConfig(groupName, opType, algConfig, reduceType, dstDataType, srcDataType, commEngine); // Constructor.
mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling);  // tiling is the TilingData structure assembled by the operator.
mc2CcTilingConfig.GetTiling(tiling->reduceScatterTiling);
```
