# asc_sync_wait

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

头文件路径为：`"c_api/sync/sync.h"`。

如图1所示，与[asc_sync_notify](asc_sync_notify.md)配对使用，用于实现AI Core内部不同流水之间的同步控制，`asc_sync_notify`和`asc_sync_wait`各自的功能如下：

- `asc_sync_notify`：当源流水的前序指令的所有读写操作都完成之后，当前指令开始执行，并将硬件中的对应标志位设置为1。`asc_sync_notify`只是设置硬件中的对应标志位，并不会阻塞源流水中的下一个指令。
- `asc_sync_wait`：当目的流水执行到该指令时，如果发现硬件中对应标志位为0，目的流水的后续指令将一直被阻塞；如果发现硬件中对应标志位为1，则将硬件中对应标志位设置为0，同时目的流水的后续指令开始执行。

**图1**  `asc_sync_notify`和`asc_sync_wait`接口功能示意图    
![](../figures/asc_sync_notify.png)

## 函数原型

```c
__aicore__ inline void asc_sync_wait(pipe_t pipe,
                                     pipe_t tpipe,
                                     event_t id)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| pipe | 输入 | 源流水类型，即“等待哪条流水的前序指令完成”。<br>参数的类型是`pipe_t`枚举，各个枚举取值的含义请参考[硬件流水类型](./intra_core_sync_overview.md#硬件流水类型)。 |
| tpipe | 输入 | 目标流水类型，即“解除哪条流水的`asc_sync_wait`阻塞”。取值范围与`pipe`相同，为`pipe_t`枚举。 |
| id | 输入 | 同步事件ID，每对`pipe`与`tpipe`组合各自拥有8个独立的同步事件ID。取值范围为`event_t`枚举类型。 |

`event_t`枚举定义如下：

```c
typedef enum {
    EVENT_ID0 = 0,
    EVENT_ID1 = 1,
    EVENT_ID2 = 2,
    EVENT_ID3 = 3,
    EVENT_ID4 = 4,
    EVENT_ID5 = 5,
    EVENT_ID6 = 6,
    EVENT_ID7 = 7
} event_t;
```

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- **`pipe`与`tpipe`并非任意组合，两者组合的取值存在限制**：针对不同产品，AIC与AIV中支持的组合不同，具体请参考[核内同步分类](intra_core_sync_overview.md#核内同步分类)中的[表2](intra_core_sync_overview.md#aic_intra_core_sync_combinations)和[表3](intra_core_sync_overview.md#aiv_intra_core_sync_combinations)。
- 相同源流水、相同目标流水、相同`id`下，连续使用`asc_sync_wait`会引发未定义行为。
- 本接口需与`asc_sync_notify`配对使用，配对的两条调用其`pipe`、`tpipe`、`id`三个参数必须完全一致。
- `pipe`与`tpipe`均不可取`PIPE_ALL`，否则触发异常。
- 每对`pipe`与`tpipe`组合各自拥有8个独立的同步事件ID。例如`PIPE_M`与`PIPE_V`的组合和`PIPE_V`与`PIPE_MTE3`的组合可同时使用相同的`id`值而互不干扰。
- 本接口可能阻塞`tpipe`（目标流水）的后续指令，直到对应的硬件标志位被置1。

## 调用示例

本接口需与[asc_sync_notify](asc_sync_notify.md)配对使用，调用示例请参考[asc_sync_notify](asc_sync_notify.md)中的“调用示例”章节。
