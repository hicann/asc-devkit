# asc_sync_block_wait

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

本接口与[asc_sync_block_arrive](asc_sync_block_arrive.md)配对使用，实现单AI Core内AIC与全部AIV之间的同步（[四种核间同步模式](system_sync_overview.md#fig_sync_control_mode)中的模式2），核间同步实现的原理如下：

- 全部AIV等待单个AIC的场景：
  - 单AI Core内AIC执行`asc_sync_block_arrive`后向调度模块发送通知，接着调度模块将该AI Core内M个AIV各自对应`flag_id`的计数器增加1。
  - 各AIV上配对的`asc_sync_block_wait`检测到对应`flag_id`的计数器非0后解除阻塞并将计数器减1。
- 单个AIC等待全部AIV的场景：
  - 单AI Core内M个AIV都执行`asc_sync_block_arrive`后向调度模块发送通知，接着调度模块将AIC对应`flag_id`的计数器增加1。
  - AIC上配对的`asc_sync_block_wait`检测到对应`flag_id`的计数器非0后解除阻塞并将计数器减1。

## 函数原型

```c
__aicore__ inline void asc_sync_block_wait(pipe_t pipe,
                                           uint8_t flag_id)

__aicore__ inline void asc_sync_block_wait(pipe_t pipe,
                                           int64_t flag_id)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :---  | :--- | :--- |
| pipe | 输入 | 标识阻塞哪条流水上后续指令，直到对应`flag_id`的计数器非0。 |
| flag_id | 输入 | 核间同步的标记，用于标识同一组同步信号。取值范围为[0, 15]，每个`flag_id`各自拥有独立的4位计数器。 |

## 返回值说明

无

## 流水类型

`PIPE_S`

## 约束说明

- 不同产品对参数`pipe`的生效情况如下：
    <!-- npu="950" id8 -->
    - Ascend 950PR/Ascend 950DT，硬件支持配置核间同步模式和流水类型，输入参数`pipe`**生效**，此时`asc_sync_block_wait`会阻塞**指定流水**的后续指令。
    <!-- end id8 -->
    <!-- npu="A3,910b" id9 -->
    - 针对如下产品，硬件不支持配置核间同步模式和流水类型，输入参数`pipe`**不生效**，此时`asc_sync_block_wait`会阻塞**全部流水**的后续指令。
        <!-- npu="A3" id10 -->
        - Atlas A3 训练系列产品/Atlas A3 推理系列产品
        <!-- end id10 -->
        <!-- npu="910b" id11 -->
        - Atlas A2 训练系列产品/Atlas A2 推理系列产品
        <!-- end id11 -->
    <!-- end id9 -->
- 用户需要确保配套使用（`flag_id`必须完全一致）`asc_sync_block_arrive`和`asc_sync_block_wait`，否则会出现未定义行为。
- 每个计数器最多连续累加15次（此时计数器的值为15），必须保证计数器的值不超过15，否则触发异常。
- `flag_id`取值范围为[0, 15]，超出范围值会被按位宽截断处理为低4位（例如，flagId=16时，截取后为0；flagId=17时，截取后为1）。

## 调用示例

`asc_sync_block_wait`需与`asc_sync_block_arrive`配对使用，具体调用示例请参考[asc_sync_block_arrive调用示例](asc_sync_block_arrive.md#调用示例)。
