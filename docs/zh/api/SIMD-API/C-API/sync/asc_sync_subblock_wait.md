# asc_sync_subblock_wait

## 产品支持情况


<!-- npu="950" id13 -->
- Ascend 950PR/Ascend 950DT: 支持
<!-- end id13 -->
<!-- npu="A3" id14 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品: 支持
<!-- end id14 -->
<!-- npu="910b" id15 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品: 支持
<!-- end id15 -->
<!-- npu="310b" id16 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id17 -->
<!-- npu="310p" id18 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id18 -->
<!-- npu="910" id19 -->
- Atlas 训练系列产品：不支持
<!-- end id19 -->

## 功能说明

核间同步寄存器同步指令，每个flag_id均分配有计数器，执行asc_sync_subblock_arrive后计数器将递增；执行asc_sync_subblock_wait时如果对应的计数器数值为0则阻塞；如果对应的计数器大于0，则计数器减一，同时后续指令开始执行。

## 函数原型

```cpp
    __aicore__ inline void asc_sync_subblock_wait(pipe_t pipe, uint8_t flag_id)
    __aicore__ inline void asc_sync_subblock_wait(pipe_t pipe, int64_t flag_id)
```

## 参数说明

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

## 调用示例

<!-- npu="A3,910b" id9 -->
- 以下示例支持如下产品型号：
  <!-- npu="A3" id10 -->
  - Atlas A3 训练系列产品/Atlas A3 推理系列产品
  <!-- end id10 -->
  <!-- npu="910b" id12 -->
  - Atlas A2 训练系列产品/Atlas A2 推理系列产品
  <!-- end id12 -->
  ```cpp
  // Host侧调用接口aclrtGetHardwareSyncAddr获取核间同步基地址ffts_addr
  uint64_t config = *(__gm__ uint64_t*)ffts_addr;
  asc_set_ffts_base_addr(config);
  int64_t flag_id = 1;
  asc_sync_subblock_arrive(PIPE_S, flag_id);
  asc_sync_subblock_wait(PIPE_S, flag_id);  
  ```
<!-- end id9 -->

<!-- npu="950" id11 -->
- 以下示例支持如下产品型号：
  <!-- npu="950" id8 -->
  - Ascend 950PR/Ascend 950DT
  <!-- end id8 -->
  ```cpp
  // 在Ascend 950PR/Ascend 950DT产品上，无需进行核间同步基地址ffts_addr的设置
  int64_t flag_id = 1;
  asc_sync_subblock_arrive(PIPE_S, flag_id);
  asc_sync_subblock_wait(PIPE_S, flag_id);  
  ```
<!-- end id11 -->
