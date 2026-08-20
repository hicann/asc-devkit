# asc_sync_inter_wait

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas 训练系列产品：不支持
<!-- end id8 -->

## 功能说明

在分离模式下，AIC（Cube Core）与AIV（Vector Core）按group划分，一个group由1个block（主核）和N个subblock（从核）组成，比例1:N。组间同步指不同group之间的同步，即所有block（所有AIC或所有AIV）之间的同步。

核间同步指令，与[asc_sync_inter_arrive](asc_sync_inter_arrive.md)配合使用（通过flag_id关联），用于组间block的信号同步。

每个flag_id均分配有计数器，执行asc_sync_inter_arrive后计数器将递增；执行asc_sync_inter_wait时如果对应的计数器数值为0则阻塞；如果对应的计数器大于0，则计数器减一，同时后续指令开始执行。

## 函数原型

```cpp
__aicore__ inline void asc_sync_inter_wait(pipe_t pipe, uint8_t flag_id)
__aicore__ inline void asc_sync_inter_wait(pipe_t pipe, int64_t flag_id)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :---  | :--- | :--- |
| pipe | 输入 | 需要执行同步指令的流水类型。|
| flag_id | 输入 | 事件标号，取值范围[0,15]。|

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- flag_id的取值范围为0至15，每个flag_id的计数器范围为0至15。
- 必须保证每个flag_id的计数器不溢出，否则将引发异常。
- 必须保证相同的ID在同一时间仅被一条流水线等待。

- 必须保证配套使用[asc_sync_inter_arrive](asc_sync_inter_arrive.md)和asc_sync_inter_wait，避免计算核一直处于阻塞阶段。
- pipe支持的流水类型为PIPE_V、PIPE_M、PIPE_MTE1、PIPE_MTE2、PIPE_MTE3、PIPE_FIX，不支持PIPE_ALL和PIPE_S。
- flag_id的值超出取值范围时，会截取最低4位为准。
- 使用该接口时，建议开启batchmode模式，使算子独占全部所需核资源，否则在多流并发场景下（≥2条执行流、≥2个算子并发、核数总和超过物理核数）可能导致死锁。

## 调用示例

```cpp
int64_t flag_id = 1;
asc_sync_inter_arrive(PIPE_V, flag_id);
asc_sync_inter_wait(PIPE_V, flag_id);  
```
