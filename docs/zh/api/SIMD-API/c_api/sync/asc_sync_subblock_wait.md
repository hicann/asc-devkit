# asc_sync_subblock_wait

## 产品支持情况

<!-- npu="950" id13 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id13 -->
<!-- npu="A3" id14 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id14 -->
<!-- npu="910b" id15 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
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

头文件路径为：`"c_api/sync/sync.h"`。

本接口与[asc_sync_subblock_arrive](asc_sync_subblock_arrive.md)配对使用，实现同一AI Core内全部AIV（[subblock](inter_core_sync_overview.md#fig_block_subblock_relationship)）之间的同步（[四种核间同步模式](system_sync_overview.md#fig_sync_control_mode)中的模式1），核间同步实现的原理如下：

- 同一AI Core内所有AIV（subblock）都执行`asc_sync_subblock_arrive`后向调度模块发送通知，接着调度模块将各AIV（subblock）对应`flag_id`的计数器增加1。
- 各AIV（subblock）上配对的`asc_sync_subblock_wait`检测到对应`flag_id`的计数器非0后解除阻塞并将计数器减1。

该同步模式的具体执行原理，请参考[单个AI Core中AIV全核同步（模式1）](key_features.md#single_ai_core_aiv_full_sync)中的代码片段及配套时序图。

## 函数原型

```c
__aicore__ inline void asc_sync_subblock_wait(pipe_t pipe,
                                              int64_t flag_id)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :---  | :--- | :--- |
| pipe | 输入 | 标识阻塞哪条流水上后续指令，直到对应`flag_id`的计数器非0。<br>参数的类型是`pipe_t`枚举，各个枚举取值的含义请参考[硬件流水类型](./intra_core_sync_overview.md#硬件流水类型)。<br>本接口对参数`pipe`是否生效与NPU架构有关，具体请参考[约束说明](#约束说明)。 |
| flag_id | 输入 | 核间同步的标记，用于标识同一组同步信号。取值范围为[0, 15]，每个`flag_id`各自拥有独立的4位计数器。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 调用本接口的核函数不能使用`__cube__`或`__vector__`[函数执行空间限定符](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#函数执行空间限定符)。使用这两种函数执行空间限定符时，硬件不会开启调度模块，无法正常进行核间同步。对于`asc_sync_subblock_arrive`和`asc_sync_subblock_wait`这对接口，支持的函数执行空间限定符为`__mix__(1, 1)`、`__mix__(1, 2)`。
- 针对`asc_sync_subblock_arrive`接口，传入的`pipe`参数在不同NPU架构中**均生效**；针对`asc_sync_subblock_wait`接口，传入的`pipe`参数**是否生效与NPU架构有关**，具体情况如下：
    <!-- npu="950" id8 -->
    - 针对[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)，硬件支持配置核间同步模式和流水类型，输入参数`pipe`**生效**，此时`asc_sync_subblock_wait`会阻塞**指定流水**的后续指令。

        本接口仅支持AIV调用，AIV支持的`pipe`取值如下：`PIPE_S`、`PIPE_MTE2`、`PIPE_MTE3`、`PIPE_V`。
    <!-- end id8 -->
    <!-- npu="A3,910b" id9 -->
    - 针对[NPU架构版本2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)，硬件不支持配置核间同步模式和流水类型，输入参数`pipe`**不生效**，此时`asc_sync_subblock_wait`会阻塞**全部流水**的后续指令。
    <!-- end id9 -->
- 用户需要确保配套使用（`flag_id`必须完全一致）`asc_sync_subblock_arrive`和`asc_sync_subblock_wait`，否则会出现未定义行为。
- 每个计数器最多连续累加15次（此时计数器的值为15），必须保证计数器的值不超过15，否则触发异常。
- `flag_id`取值范围为[0, 15]，超出范围值会被按位宽截断处理为低4位（例如，flagId=16时，截取后为0；flagId=17时，截取后为1）。

## 调用示例

`asc_sync_subblock_wait`需与`asc_sync_subblock_arrive`配对使用，具体调用示例请参考[asc_sync_subblock_arrive调用示例](asc_sync_subblock_arrive.md#调用示例)。
