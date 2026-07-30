# asc_get_system_cycle

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
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

<!-- npu="950" id8 -->
- 针对Ascend 950PR/Ascend 950DT，若换算成时间需要按照1GHz的频率，时间单位为us，换算公式为：time = (cycle数/1000)us。
<!-- end id8 -->

<!-- npu="A3" id9 -->
- 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，若换算成时间需要按照50MHz的频率，时间单位为us，换算公式为：time = (cycle数/50)us。
<!-- end id9 -->

<!-- npu="910b" id10 -->
- 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，若换算成时间需要按照50MHz的频率，时间单位为us，换算公式为：time = (cycle数/50)us。
<!-- end id10 --> 

## 函数原型

```cpp
__aicore__ inline int64_t asc_get_system_cycle()
```

## 参数说明

无

## 返回值说明

返回系统Cycle数。

## 流水类型

PIPE_S

## 约束说明

该接口是PIPE_S流水，若需要测试其他流水的指令时间，需要在调用该接口前插入对应流水的同步。

## 调用示例

```cpp
int64_t system_cycle_before = asc_get_system_cycle(); // 计算操作指令前的cycle数
asc_sync_notify(PIPE_S, PIPE_V, 0);
asc_sync_wait(PIPE_S, PIPE_V, 0);

......// 进行PIPE_V流水的计算操作

asc_sync_notify(PIPE_V, PIPE_S, 0);
asc_sync_wait(PIPE_V, PIPE_S, 0);

int64_t system_cycle_after = asc_get_system_cycle(); // 计算操作指令后的cycle数
int64_t total_cycle = system_cycle_after - system_cycle_before; // 执行Add指令所用的cycle数
```
