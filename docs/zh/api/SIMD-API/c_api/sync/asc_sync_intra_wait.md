# asc_sync_intra_wait

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
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

本接口与[asc_sync_intra_arrive](asc_sync_intra_arrive.md)配对使用，实现单AI Core内AIC与单个AIV之间的同步（[四种核间同步模式](system_sync_overview.md#fig_sync_control_mode)中的模式4），核间同步实现的原理如下：

- 单个AIV等待AIC的场景：
  - 单AI Core内AIC执行`asc_sync_intra_arrive`后向调度模块发送通知，接着调度模块将该AI Core内单个AIV对应`sync_id`的计数器增加1。
  - 单个AIV上配对的`asc_sync_intra_wait`检测到对应`sync_id`的计数器非0后解除阻塞并将计数器减1。
- AIC等待单个AIV的场景：
  - 单AI Core内单个AIV执行`asc_sync_intra_arrive`后向调度模块发送通知，接着调度模块将AIC对应`sync_id`的计数器增加1。
  - AIC上配对的`asc_sync_intra_wait`检测到对应`sync_id`的计数器非0后解除阻塞并将计数器减1。

## 函数原型

```c
__aicore__ inline void asc_sync_intra_wait(pipe_t pipe,
                                           uint8_t sync_id)
__aicore__ inline void asc_sync_intra_wait(pipe_t pipe,
                                           uint64_t sync_id)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :---  | :--- | :--- |
| pipe | 输入 | 标识阻塞哪条流水的后续指令，直到对应`sync_id`的计数器非0。 |
| sync_id | 输入 | 核间同步的标记，用于标识同一组同步信号。每个`sync_id`各自拥有独立的4位计数器。<br>一个AI Core由1个AIC与2个AIV构成，AIC侧拥有32个`sync_id`（0~31），每个AIV侧各拥有16个`sync_id`（0~15）。<br>跨核ID映射规则如下：<br>• 第一个AIV的ID 0~15映射到AIC的ID 0~15；第二个AIV的ID 0~15映射到AIC的ID 16~31。<br>• AIC的ID 0~15映射到第一个AIV的ID 0~15；AIC的ID 16~31映射到第二个AIV的ID 0~15。 |

## 返回值说明

无

## 流水类型

`PIPE_S`

## 约束说明

- 用户需要确保配套使用`asc_sync_intra_arrive`和`asc_sync_intra_wait`，配对的`sync_id`必须符合跨核ID映射关系，否则会出现未定义行为。
- 每个计数器最多连续累加15次（此时计数器的值为15），必须保证计数器的值不超过15，否则触发异常。
- 本接口会阻塞`pipe`流水中的后续指令；对应`sync_id`的计数器非0时解除阻塞，并将计数器减1。
- AIC的`sync_id`取值范围为[0, 31]，超出范围值会被按位宽截断处理为低5位（例如，syncId=32时，截取后为0；syncId=33时，截取后为1）。
- AIV的`sync_id`取值范围为[0, 15]，超出范围值会被按位宽截断处理为低4位（例如，syncId=16时，截取后为0；syncId=17时，截取后为1）。
- 调用本接口的核函数必须使用`__mix__(1, 2)`修饰。

## 调用示例

`asc_sync_intra_wait`需与`asc_sync_intra_arrive`配对使用，具体调用示例请参考[asc_sync_intra_arrive调用示例](asc_sync_intra_arrive.md#调用示例)。
