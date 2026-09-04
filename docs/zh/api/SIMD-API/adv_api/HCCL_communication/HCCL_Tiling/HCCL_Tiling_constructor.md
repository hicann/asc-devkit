# HCCL Tiling构造函数

## 功能说明

用于创建一个Mc2CcTilingConfig对象。

## 函数原型

```
Mc2CcTilingConfig(const std::string& groupName, uint32_t opType, const std::string& algConfig, uint32_t reduceType = 0, uint8_t dstDataType = 0, uint8_t srcDataType = 0, uint8_t commEngine = 0)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| groupName | 输入 | 当前通信任务所在的通信域。string类型，支持的最大长度为128字节。 |
| opType | 输入 | 表示通信任务类型。uint32_t类型。HCCL API提供[HcclCMDType](#table2469980529)枚举定义作为该参数的取值，具体支持的通信任务类型及取值请参考[表2](#table2469980529)。 |
| algConfig | 输入 | 通信算法配置。string类型，支持的最大长度为128字节。<!-- npu="950" id1 --><br><br>针对Ascend 950PR/Ascend 950DT，该参数为预留字段，配置后不生效，默认仅支持FullMesh算法。FullMesh算法即NPU之间的全连接，任意两个NPU之间可以直接进行数据收发。<!-- end id1 --><!-- npu="A3" id2 --><br><br>针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，当前支持的取值为：<br>"AllReduce=level0:doublering"：AllReduce通信任务。<br>"AllGather=level0:doublering"：AllGather通信任务。<br>"ReduceScatter=level0:doublering"：ReduceScatter通信任务。<br>"AlltoAll=level0:fullmesh;level1:pairwise"：AlltoAllV和AlltoAll通信任务。<br>"BatchWrite=level0:fullmesh"：BatchWrite通信任务。<!-- end id2 --><!-- npu="910b" id3 --><br><br>针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，该参数为预留字段，配置后不生效，默认仅支持FullMesh算法。FullMesh算法即NPU之间的全连接，任意两个NPU之间可以直接进行数据收发。<!-- end id3 --> |
| reduceType | 输入 | 归约操作类型，仅对有归约操作的通信任务生效。uint32_t类型，取值详见[表2](../HCCL_Kernel/HCCL_usage.md#table2469980529)。 |
| dstDataType | 输入 | 通信任务中输出数据的数据类型。uint8_t类型，该参数的取值范围请参考[表1](../HCCL_Kernel/HCCL_usage.md#table116710585514)。<!-- npu="950" id4 --><br><br>Ascend 950PR/Ascend 950DT，不同通信任务支持的输出数据类型不同，具体为：<br>对于AllReduce、AllGather、AllToAll、AllToAllV、AllToAllVWrite通信任务：输出的数据类型必须与输入的数据类型一致。各通信任务支持的输入数据类型请参考srcDataType。<br>对于ReduceScatter通信任务，当输入的数据类型为int16_t、int32_t、half、float、bfloat16_t时，输出的数据类型必须与其一致；当输入的数据类型为int8_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t时，输出的数据类型必须为half、bfloat16_t、float三者之一。<!-- end id4 --><!-- npu="A3" id5 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品，该参数暂不支持，配置后不生效。<!-- end id5 --><!-- npu="910b" id6 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品，该参数暂不支持，配置后不生效。<!-- end id6 --> |
| srcDataType | 输入 | 通信任务中输入数据的数据类型。uint8_t类型，该参数的取值范围请参考[表1](../HCCL_Kernel/HCCL_usage.md#table116710585514)。<!-- npu="950" id7 --><br><br>Ascend 950PR/Ascend 950DT，不同通信任务支持的输入数据类型如下：<br>AllReduce通信任务：支持的输入类型为int16_t、half、bfloat16_t、int32_t、float。<br>AllGather、AllToAll、AllToAllV、AllToAllVWrite通信任务：支持的输入类型为int8_t、uint8_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、double。<br>ReduceScatter通信任务：支持的输入类型为int8_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、half、bfloat16_t、int32_t、float。<!-- end id7 --><!-- npu="A3" id8 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品，该参数暂不支持，配置后不生效。<!-- end id8 --><!-- npu="910b" id9 --><br><br>针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，该参数暂不支持，配置后不生效。<!-- end id9 --> |
| commEngine | 输入 | 通信引擎。uint8_t类型，该参数的取值范围请参考：[HcclCommConfig](https://gitcode.com/cann/hcomm/blob/master/docs/zh/api_ref/comm_mgr_c/data_type_definition/HcclCommConfig.md#参数说明)中的hcclOpExpansionMode参数的取值说明。 |

**表2**  HcclCMDType参数说明

<a name="table2469980529"></a>

| 数据类型 | 说明 |
| --- | --- |
| HcclCMDType | 通信任务类型。HcclCMDType枚举类定义如下代码所示。<!-- npu="950" id10 --><br><br>针对Ascend 950PR/Ascend 950DT，当前支持的通信任务类型为HCCL_CMD_ALLREDUCE、HCCL_CMD_ALLGATHER、HCCL_CMD_REDUCE_SCATTER、HCCL_CMD_ALLTOALL、HCCL_CMD_ALLTOALLV、HCCL_CMD_HALF_ALLTOALLV。<!-- end id10 --><!-- npu="A3" id11 --><br><br>针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，当前支持的通信任务类型为HCCL_CMD_ALLREDUCE、HCCL_CMD_ALLGATHER、HCCL_CMD_REDUCE_SCATTER、HCCL_CMD_ALLTOALL、HCCL_CMD_ALLTOALLV、HCCL_CMD_BATCH_WRITE。<!-- end id11 --><!-- npu="910b" id12 --><br><br>针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，当前支持的通信任务类型为HCCL_CMD_ALLREDUCE、HCCL_CMD_ALLGATHER、HCCL_CMD_REDUCE_SCATTER、HCCL_CMD_ALLTOALL、HCCL_CMD_BATCH_WRITE。<!-- end id12 --> |

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

## 返回值说明

无

## 约束说明

无

## 调用示例

```
const char* groupName = "testGroup";
uint32_t opType = HCCL_CMD_REDUCE_SCATTER;
std::string algConfig = "ReduceScatter=level0:fullmesh";
uint32_t reduceType = HCCL_REDUCE_SUM;
uint8_t dstDataType = HCCL_DATA_TYPE_FP16;
uint8_t srcDataType = HCCL_DATA_TYPE_FP16;
uint8_t commEngine = 0;
AscendC::Mc2CcTilingConfig mc2CcTilingConfig(
    groupName, opType, algConfig, reduceType, dstDataType, srcDataType, commEngine); // 构造函数
mc2CcTilingConfig.GetTiling(tiling->mc2InitTiling); // tiling为算子组装的TilingData结构体
mc2CcTilingConfig.GetTiling(tiling->reduceScatterTiling);
```
