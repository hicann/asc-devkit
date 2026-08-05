# SetReduceType

## 功能说明

设置Reduce操作类型，仅对有归约操作的通信任务生效。

## 函数原型

```
uint32_t SetReduceType(uint32_t reduceType, uint8_t dstDataType = 0, uint8_t srcDataType = 0)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| reduceType | 输入 | 归约操作类型，仅对有归约操作的通信任务生效。uint32_t类型，取值详见[表2](../HCCL_Kernel/HCCL_usage.md#table2469980529)参数说明。 |
| dstDataType | 输入 | 通信任务中输出数据的数据类型。uint8_t类型，该参数的取值范围请参考[表1](../HCCL_Kernel/HCCL_usage.md#table116710585514)。<!-- npu="950" id1 --><br><br>Ascend 950PR/Ascend 950DT，CCU作为服务端时，不同通信任务支持的输出数据类型不同，具体为：<br>对于AllReduce、AllGather、AllToAll、AllToAllV、AllToAllVWrite通信任务：输出的数据类型必须与输入的数据类型一致。各通信任务支持的输入数据类型请参考srcDataType。<br>对于ReduceScatter通信任务，当输入的数据类型为int16_t、int32_t、half、float、bfloat16_t时，输出的数据类型必须与其一致；当输入的数据类型为int8_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t时，输出的数据类型必须为half、bfloat16_t、float三者之一。<!-- end id1 --> |
| srcDataType | 输入 | 通信任务中输入数据的数据类型。uint8_t类型，该参数的取值范围请参考[表1](../HCCL_Kernel/HCCL_usage.md#table116710585514)。<!-- npu="950" id2 --><br><br>Ascend 950PR/Ascend 950DT，CCU作为服务端时，不同通信任务支持的输入数据类型如下：<br>AllReduce通信任务：支持的输入类型为int16_t、half、bfloat16_t、int32_t、float。<br>AllGather、AllToAll、AllToAllV、AllToAllVWrite通信任务：支持的输入类型为int8_t、uint8_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、double。<br>ReduceScatter通信任务：支持的输入类型为int8_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、half、bfloat16_t、int32_t、float。<!-- end id2 --> |

## 返回值说明

-   0表示设置成功。
-   非0表示设置失败。

## 约束说明
<!-- npu="950" id3 -->
对于Ascend 950PR/Ascend 950DT，AI CPU作为服务端时，该接口暂不支持使用。
<!-- end id3 -->
<!-- npu="A3" id4 -->
对于Atlas A3 训练系列产品/Atlas A3 推理系列产品，该接口暂不支持使用。
<!-- end id4 -->
<!-- npu="910b" id5 -->
对于Atlas A2 训练系列产品/Atlas A2 推理系列产品，该接口暂不支持使用。
<!-- end id5 -->

## 调用示例

本接口的调用示例请见[调用示例](SetOpType.md#调用示例)。
