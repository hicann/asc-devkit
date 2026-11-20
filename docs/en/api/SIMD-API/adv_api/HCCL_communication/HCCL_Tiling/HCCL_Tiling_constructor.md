# HCCL Tiling Constructor

<!-- md-trans-meta sourceCommit=ff08ce34f762ef327d03181f997e9759eb37718b translatedAt=2026-11-02T05:56:59.685Z pushedAt=2026-11-18T04:57:47.830Z nl=2 -->

## Description

Used to create an **Mc2CcTilingConfig** object.

## Prototype

```
Mc2CcTilingConfig(const std::string& groupName, uint32_t opType, const std::string& algConfig, uint32_t reduceType = 0, uint8_t dstDataType = 0, uint8_t srcDataType = 0, uint8_t commEngine = 0)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| groupName | Input | Communicator where the current communication task resides. String type, with a maximum supported length of 128 bytes. |
| opType | Input | Indicates the communication task type. uint32_t type. The HCCL API provides the [HcclCMDType](#table2469980529) enumeration definition as the value of this parameter. For the supported communication task types and their values, see [Table 2](#table2469980529). |
| algConfig | Input | Communication algorithm configuration. The value is of the string type, with a maximum supported length of 128 bytes.<!-- npu="950" id1 --><br><br>For Ascend 950PR&950DT products, this parameter configures the communication algorithm. The currently supported values are communication algorithm configurations such as "sole[mesh]" and "parallel[mesh,nhr]". For the supported values and details, see [SetAlgConfig](SetAlgConfig.md). This parameter and the SetAlgConfig API share the same configuration entry, and both take effect after configuration.<!-- end id1 --><!-- npu="A3" id2 --><br><br>For Atlas A3 products, the currently supported values are:<br>"AllReduce=level0:doublering": AllReduce communication task.<br>"AllGather=level0:doublering": AllGather communication task.<br>"ReduceScatter=level0:doublering": ReduceScatter communication task.<br>"AlltoAll=level0:fullmesh;level1:pairwise": AlltoAllV and AlltoAll communication tasks.<br>"BatchWrite=level0:fullmesh": BatchWrite communication task.<!-- end id2 --><!-- npu="910b" id3 --><br><br>For Atlas A2 products, this parameter is a reserved field and does not take effect after configuration. By default, only the FullMesh algorithm is supported. The FullMesh algorithm is a full connection between NPUs, where any two NPUs can directly send and receive data.<!-- end id3 --> |
| reduceType | Input | Reduction operation type, which takes effect only for communication tasks with reduction operations. uint32_t type. For the values, see [Table 2](../HCCL_Kernel/HCCL_usage.md#table2469980529). |
| dstDataType | Input | Data type of the output data in the communication task. uint8_t type. For the value range of this parameter, see [Table 1](../HCCL_Kernel/HCCL_usage.md#table116710585514). <!-- npu="950" id4 --><br><br>For Ascend 950PR&950DT products, different communication tasks support different output data types, as follows:<br>For AllReduce, AllGather, AllToAll, AllToAllV, and AllToAllVWrite communication tasks: the output data type must be the same as the input data type. For the input data types supported by each communication task, see srcDataType.<br>For the ReduceScatter communication task, when the input data type is int16_t, int32_t, half, float, or bfloat16_t, the output data type must be the same as it; when the input data type is int8_t, hifloat8_t, fp8_e5m2_t, or fp8_e4m3fn_t, the output data type must be one of half, bfloat16_t, and float. <!-- end id4 --><!-- npu="A3" id5 --><br><br>For Atlas A3 products, this parameter is not supported and does not take effect after configuration. <!-- end id5 --><!-- npu="910b" id6 --><br><br>For Atlas A2 products, this parameter is not supported and does not take effect after configuration. <!-- end id6 --> |
| srcDataType | Input | Data type of the input data in the communication task. uint8_t type. For the value range of this parameter, see [Table 1](../HCCL_Kernel/HCCL_usage.md#table116710585514). <!-- npu="950" id7 --><br><br>For Ascend 950PR&950DT products, the input data types supported by different communication tasks are as follows:<br>AllReduce communication task: the supported input types are int16_t, half, bfloat16_t, int32_t, and float.<br>AllGather, AllToAll, AllToAllV, and AllToAllVWrite communication tasks: the supported input types are int8_t, uint8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, and double.<br>ReduceScatter communication task: the supported input types are int8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, half, bfloat16_t, int32_t, and float. <!-- end id7 --><!-- npu="A3" id8 --><br><br>For Atlas A3 products, this parameter is not supported and does not take effect after configuration. <!-- end id8 --><!-- npu="910b" id9 --><br><br>For Atlas A2 products, this parameter is not supported and does not take effect after configuration. <!-- end id9 --> |
| commEngine | Input | Communication engine. The value is of the uint8_t type. For the value range, see the description of the hcclOpExpansionMode parameter in [HcclCommConfig](https://gitcode.com/cann/hcomm/blob/master/docs/en/api_ref/comm_mgr_c/data_type_definition/HcclCommConfig.md#parameters). |

**Table 2**  HcclCMDType parameters

<a name="table2469980529"></a>

| Data Type | Description |
| --- | --- |
| HcclCMDType | Communication task type. The HcclCMDType enumeration class is defined as shown in the following code. <!-- npu="950" id10 --><br><br>For Ascend 950PR&950DT products, the currently supported communication task types are HCCL_CMD_ALLREDUCE, HCCL_CMD_ALLGATHER, HCCL_CMD_REDUCE_SCATTER, HCCL_CMD_ALLTOALL, HCCL_CMD_ALLTOALLV, and HCCL_CMD_HALF_ALLTOALLV. <!-- end id10 --><!-- npu="A3" id11 --><br><br>For Atlas A3 products, the currently supported communication task types are HCCL_CMD_ALLREDUCE, HCCL_CMD_ALLGATHER, HCCL_CMD_REDUCE_SCATTER, HCCL_CMD_ALLTOALL, HCCL_CMD_ALLTOALLV, and HCCL_CMD_BATCH_WRITE. <!-- end id11 --><!-- npu="910b" id12 --><br><br>For Atlas A2 products, the currently supported communication task types are HCCL_CMD_ALLREDUCE, HCCL_CMD_ALLGATHER, HCCL_CMD_REDUCE_SCATTER, HCCL_CMD_ALLTOALL, and HCCL_CMD_BATCH_WRITE. <!-- end id12 --> |

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
const char* groupName = "testGroup";
uint32_t opType = HCCL_CMD_REDUCE_SCATTER;
std::string algConfig = "ReduceScatter=level0:fullmesh";
uint32_t reduceType = HCCL_REDUCE_SUM;
uint8_t dstDataType = HCCL_DATA_TYPE_FP16;
uint8_t srcDataType = HCCL_DATA_TYPE_FP16;
uint8_t commEngine = 0;
AscendC::Mc2CcTilingConfig mc2CcTilingConfig(
    groupName, opType, algConfig, reduceType, dstDataType, srcDataType, commEngine); // Constructor.
mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling); // tiling is the TilingData struct assembled by the operator.
mc2CcTilingConfig.GetTiling(tiling->reduceScatterTiling);
```
