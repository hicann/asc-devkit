# 归约计算概述

**表1** 归约计算接口与主要功能

| 接口 | 主要功能 | 输出长度 |
| --- | --- | --- |
| [ReduceDataBlock](./ReduceDataBlock.md) | 对每个DataBlock内的数据求和/求最大值/求最小值。 | 1 element / DataBlock |
| [ReduceRepeat](./ReduceRepeat.md) | 对每个repeat内的数据求和/求最大值/求最小值。<br>其中求最大值或最小值时可选择输出对应的索引。 | 求和：1 element / repeat；<br>求最大值/最小值：1 element (1 index) / repeat。 |
| [ReducePairElem](./ReducePairElem.md) | 对相邻两个元素进行求和。 | 1 element / pair |
| [ReduceSum](./ReduceSum.md) | 对所有输入数据求和。 | 1 element |
| [ReduceMax](./ReduceMax.md)/[ReduceMin](./ReduceMin.md) | 对所有输入数据求最大值/最小值。<br>可选择输出对应的索引。 | 1 element (1 index) |
| [GetReduceRepeatSumSpr(ISASI)](../归约计算辅助配置接口/GetReduceRepeatSumSpr(ISASI).md) | 获取`ReduceSum`接口的计算结果。 | 1 element |
| [GetReduceRepeatMaxMinSpr(ISASI)](../归约计算辅助配置接口/GetReduceRepeatMaxMinSpr(ISASI).md) | 获取调用`ReduceRepeat<MAX/MIN>`时所有repeat内的最值及其索引，或获取调用`ReduceMax`、`ReduceMin`得到的最值。 | 1 element (1 index) |

<!-- npu="950" id1 -->
针对Ascend 950PR/Ascend 950DT，Memory矢量计算的归约计算接口底层均通过VF调用转换为Reg矢量计算指令执行，接口内部存在额外的VF调用开销和UB中转（如sharedTmpBuffer）。因此Memory API不保证极致性能，对性能敏感的场景建议直接使用Reg矢量计算中的[归约计算](../../Reg矢量计算/归约计算/归约计算.md)（Reg::Reduce、Reg::PairReduceElem、Reg::ReduceDataBlock）。
<!-- end id1 -->

<!-- npu="A3,910b" id2 -->
针对Atlas A3 训练系列产品/Atlas A3 推理系列产品、Atlas A2 训练系列产品/Atlas A2 推理系列产品，在使用归约计算接口时，有如下使用建议。

- `ReduceDataBlock`
  - **推荐使用场景**：
    - 数据类型为`float`时：相比`ReduceRepeat`有更高的理论并行度；与`ReducePairElem`拥有相同的理论并行度，但可以归约更多数据。
    - 数据类型为`half`、数据量小于等于一个DataBlock时：`ReduceDataBlock`与`ReduceRepeat`具有相同的理论并行度，但`ReduceDataBlock`支持只对一个DataBlock进行归约，而`ReduceRepeat`需要对八个DataBlock（一个repeat）进行归约，因此`ReduceDataBlock`效率更高。
      - 示例：当数据长度为2K时，可以先进行一次`ReduceRepeat`归约得到长度为16（32字节），再进行一次`ReduceDataBlock`即可。此时一次`ReduceRepeat`+一次`ReduceDataBlock`耗时比两次`ReduceRepeat`短。
  - 支持的数据类型：`half`、`float`

- `ReduceRepeat`
  - **推荐使用场景**：
    - 需要获取最值的索引且对性能敏感的场景。
    - 数据类型为`half`、数据量大于一个DataBlock时：`ReduceRepeat`与`ReduceDataBlock`具有相同的理论并行度，但`ReduceRepeat`能一次调用对八个DataBlock（一个repeat）进行归约，而使用`ReduceDataBlock`至少需要调用两次，因此`ReduceRepeat`耗时更短。
  - 支持的数据类型：`half`、`float`

- `ReducePairElem`
  - **推荐使用场景**：
    - 适合在需要对相邻两个元素进行求和操作时使用。
    - 数据类型为`half`时，`ReducePairElem`的理论并行度是归约指令中最高的，因此在数据量较大、需要多次调用`ReduceDataBlock`及`ReduceRepeat`时，适合用于对数据进行初步归约。
  - 支持的数据类型：`half`、`float`

- `ReduceSum`
  - **推荐使用场景**：
    - 该接口内部通过软件仿真实现，性能可能不及硬件指令实现的`ReduceDataBlock`和`ReduceRepeat`接口，不推荐对性能敏感的场景使用。
    - 该接口能通过一次调用得到归约结果，且支持更长的输入数据，适合用于需要快速验证实现的场景。
  - 支持的数据类型：`half`、`float`

- `ReduceMax`/`ReduceMin`
  - **推荐使用场景**：
    - 该接口内部通过软件仿真实现，性能可能不及硬件指令实现的`ReduceDataBlock`和`ReduceRepeat`接口，不推荐对性能敏感的场景使用。
    - 该接口能通过一次调用得到归约结果，且支持更长的输入数据，适合用于需要快速验证实现的场景。
  - 支持的数据类型：`half`、`float`

- `GetReduceRepeatSumSpr`
  - **推荐使用场景**：获取tensor前n个数据计算场景的`ReduceSum`结果。
  - 支持的数据类型：`half`、`float`

- `GetReduceRepeatMaxMinSpr`
  - **推荐使用场景**：获取连续计算场景的`ReduceRepeat<MAX/MIN>`所有repeat内的最值及其索引，或获取连续计算场景的`ReduceMax`/`ReduceMin`得到的最值。
  - 支持的数据类型：`half`、`float`

归约接口的选择可以参考如下[决策树](#fig1)：

<a id="fig1"></a>

**图1**  归约接口选择决策树

![归约接口选择决策树](../../../../figures/reduce_decision_tree.png "归约接口选择决策树")

注：在数据类型为`half`时，`ReduceDataBlock`及`ReduceRepeat`接口并行度不如`ReducePairElem`及基础算术接口[Add](../基础算术/Add.md)，[Max](../基础算术/Max.md)，[Min](../基础算术/Min.md)。在数据量较大，需要多次调用`ReduceDataBlock`及`ReduceRepeat`时，可以考虑使用`ReducePairElem`或基础算术接口对数据进行初步归约后再调用归约计算接口。
<!-- end id2 -->
