# 概述

## 同步控制概述

同步控制C API提供核内流水同步，以及核间、子核间的通知与等待接口。

### 同步类型与使用场景

在Ascend C编程中，同步用于约束AI Core内外不同执行单元的执行先后关系，确保后续指令在依赖的数据、状态或同步信号就绪后再继续执行。由于数据搬运、计算、不同核函数（Kernel）或不同AI Core之间可以并行，如果缺少必要的同步，后续执行单元可能读取到尚未写入完成的数据，或在前序任务未完成时提前执行，导致计算结果错误或执行行为不符合预期。

AI Core的同步，总共分成2类：[核内同步](#核内同步原理)、[核间同步](#核间同步原理)。

以下内容展示两类同步的使用场景。

- 核内同步用于约束同一核内不同硬件流水或同一硬件流水内指令的执行时序。

    例如在AIV中，MTE2单元通过`PIPE_MTE2`将数据搬入片上存储后，Vector单元才可以通过`PIPE_V`读取该数据进行计算，此时需要插入`PIPE_MTE2`到`PIPE_V`的同步，使`PIPE_V`等待`PIPE_MTE2`的前序搬入完成。
- 核间同步用于约束不同核之间的执行时序，参与同步的对象可以是多个AI Core，也可以是同一AI Core内的AIC和AIV。
    <!-- npu="950" id1 -->
    如图1所示，针对[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)，同一个AI Core内AIV通过`PIPE_MTE3`将数据从UB搬运到L1 Buffer后，AIC侧`PIPE_MTE1`才能将L1 Buffer中的数据搬入L0A Buffer和L0B Buffer并继续执行矩阵计算，此时需要让AIC等待AIV的数据搬运完成通知。

    核内同步和核间同步的区别在于：核内同步解决同一核内部硬件流水之间的数据依赖，关注`PIPE_MTE2`、`PIPE_V`等流水的先后关系；核间同步解决不同核或不同逻辑核之间的进度依赖，关注AIC、AIV或多个AI Core之间的先后关系。

    **图1**  [NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)架构图

    ![](../../../figures/npu_3510_hw_arch_sync.png)
    <!-- end id1 -->

### 核内同步原理

#### 为什么需要核内同步

AI Core内部的执行单元（如MTE2搬运单元、Vector计算单元等）以异步并行的方式运行，在读写同一存储资源时可能存在数据依赖关系。为确保数据一致性及计算正确性，需通过同步控制协调操作时序。

<!-- npu="950" id2 -->
针对[NPU架构版本3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)，硬件架构如图1所示，高亮部分展示了并行执行的计算单元和搬运单元。

<!-- end id2 -->

<!-- npu="A3,910b" id3 -->

针对[NPU架构版本2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)，硬件架构图如下，高亮部分展示了并行执行的计算单元和搬运单元。

**图2**  NPU架构版本2201架构图

![](../../../figures/atlas_a2_a3_architecture.png)

<!-- end id3 -->

下图示例描述了一个常见的Vector计算数据流：

1. 先通过DMA执行单元将数据从GM搬入到Local Memory；
2. 进行计算；
3. 然后再通过DMA执行单元将计算结果从Local Memory搬出到GM。

**图3**  Vector计算数据流示意图

![](../../../figures/vector_compute_data_flow.png)

四个执行单元Scalar、Vector、DMA（MTE2）、DMA（MTE3）并行执行，若访问同一片Local Memory，需要同步机制来控制它们的访问时序：保证先搬入Local Memory后再计算，计算完成后再搬出。

**图4**  核内并行流水执行时序示意图

![](../../../figures/intra_core_parallel_pipeline_execution_timing.png)

#### 硬件流水类型

AI Core内部并行的指令流水类型和解释如下所示：

> [!NOTE]说明
> 不同的硬件架构，每一种硬件流水类型包含的具体流水会有所差异，详细介绍请参考[硬件实现](../../../../guide/programming_guide/advanced_programming/hardware_implementation/hardware_implementation.md)章节。

**表1** 指令流水类型和相关说明

| 流水类型 | 含义 |
| --- | --- |
| `PIPE_S` | 标量流水，使用标量访存语句或标量计算接口访问GM、片上存储地址时为此流水 |
| `PIPE_V` | 矢量计算流水及部分硬件架构下的L0C Buffer->UB数据搬运流水 |
| `PIPE_M` | 矩阵计算流水 |
| `PIPE_MTE1` | L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer数据搬运流水 |
| `PIPE_MTE2` | GM->L1 Buffer、GM->UB等数据搬运流水 |
| `PIPE_MTE3` | UB->GM等数据搬运流水 |
| `PIPE_FIX` | L0C Buffer->GM、L0C Buffer->L1 Buffer等数据搬运流水 |

此外，`PIPE_ALL`表示全部流水，用于需要等待所有流水前序指令完成的场景。

同步接口中`pipe_t`枚举类型的取值如下：<a id="硬件流水类型"></a>

```c
typedef enum {
    PIPE_S = 0,
    PIPE_V = 1,
    PIPE_M = 2,
    PIPE_MTE1 = 3,
    PIPE_MTE2 = 4,
    PIPE_MTE3 = 5,
    PIPE_ALL = 6,
    PIPE_FIX = 10
} pipe_t;
```

#### 核内同步组合

<!-- npu="A3,910b" id4 -->
以[NPU架构版本2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)为例，该硬件架构下所有合法的核内同步组合如[表2](#aic_intra_core_sync_combinations)和[表3](#aiv_intra_core_sync_combinations)所示。其中，“不涉及”表示硬件层面不存在此种同步组合，“暂无应用场景”表示存在此种同步组合，但在实际开发场景中暂不需要使用。

<a id="aic_intra_core_sync_combinations"></a>

**表2** AIC中所有合法的核内同步组合

| 源流水/目的流水 | `PIPE_S` | `PIPE_M` | `PIPE_MTE1` | `PIPE_MTE2` | `PIPE_MTE3` | `PIPE_FIX` |
| --- | --- | --- | --- | --- | --- | --- |
| `PIPE_S` | 不涉及 | 不涉及 | 不涉及 | 不涉及 | 不涉及 | 不涉及 |
| `PIPE_M` | 不涉及 | `asc_sync_pipe(PIPE_M)` | `asc_sync_notify(PIPE_M, PIPE_MTE1, EVENT_ID0)`<br>`asc_sync_wait(PIPE_M, PIPE_MTE1, EVENT_ID0)` | `asc_sync_notify(PIPE_M, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_M, PIPE_MTE2, EVENT_ID0)` | 不涉及 | `asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0)`<br>`asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0)` |
| `PIPE_MTE1` | 不涉及 | `asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0)` | `asc_sync_pipe(PIPE_MTE1)` | `asc_sync_notify(PIPE_MTE1, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE1, PIPE_MTE2, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE1, PIPE_MTE3, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE1, PIPE_MTE3, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE1, PIPE_FIX, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE1, PIPE_FIX, EVENT_ID0)` |
| `PIPE_MTE2` | 不涉及 | `asc_sync_notify(PIPE_MTE2, PIPE_M, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_M, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0)` | `asc_sync_pipe(PIPE_MTE2)` | `asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0)` | 暂无应用场景 |
| `PIPE_MTE3` | 不涉及 | 不涉及 | `asc_sync_notify(PIPE_MTE3, PIPE_MTE1, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE3, PIPE_MTE1, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE3, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE3, PIPE_MTE2, EVENT_ID0)` | `asc_sync_pipe(PIPE_MTE3)` | 暂无应用场景 |
| `PIPE_FIX` | 不涉及 | `asc_sync_notify(PIPE_FIX, PIPE_M, EVENT_ID0)`<br>`asc_sync_wait(PIPE_FIX, PIPE_M, EVENT_ID0)` | `asc_sync_notify(PIPE_FIX, PIPE_MTE1, EVENT_ID0)`<br>`asc_sync_wait(PIPE_FIX, PIPE_MTE1, EVENT_ID0)` | 暂无应用场景 | 暂无应用场景 | `asc_sync_pipe(PIPE_FIX)` |

<a id="aiv_intra_core_sync_combinations"></a>

**表3** AIV中所有合法的核内同步组合

| 源流水/目的流水 | `PIPE_S` | `PIPE_V` | `PIPE_MTE2` | `PIPE_MTE3` |
| --- | --- | --- | --- | --- |
| `PIPE_S` | 不涉及 | `asc_sync_notify(PIPE_S, PIPE_V, EVENT_ID0)`<br>`asc_sync_wait(PIPE_S, PIPE_V, EVENT_ID0)` | `asc_sync_notify(PIPE_S, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_S, PIPE_MTE2, EVENT_ID0)` | `asc_sync_notify(PIPE_S, PIPE_MTE3, EVENT_ID0)`<br>`asc_sync_wait(PIPE_S, PIPE_MTE3, EVENT_ID0)` |
| `PIPE_V` | `asc_sync_notify(PIPE_V, PIPE_S, EVENT_ID0)`<br>`asc_sync_wait(PIPE_V, PIPE_S, EVENT_ID0)` | `asc_sync_pipe(PIPE_V)` | `asc_sync_notify(PIPE_V, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_V, PIPE_MTE2, EVENT_ID0)` | `asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0)`<br>`asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0)` |
| `PIPE_MTE2` | `asc_sync_notify(PIPE_MTE2, PIPE_S, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_S, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0)` | `asc_sync_pipe(PIPE_MTE2)` | `asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0)` |
| `PIPE_MTE3` | `asc_sync_notify(PIPE_MTE3, PIPE_S, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE3, PIPE_S, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE3, PIPE_V, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE3, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE3, PIPE_MTE2, EVENT_ID0)` | `asc_sync_pipe(PIPE_MTE3)` |

<!-- end id4 -->

#### 什么时候需要开发者手动插入同步

C API编程方式下所有的同步均需开发者手动管理。

### 核间同步原理

核间同步的使用场景通常是：一个核计算所依赖的数据，需要等待其他相关核的计算全部完成后才能获取。

以图5为例，AIC需要依赖AIV的`asc_reduce`计算结果。由于整体矢量较大，必须拆分为多个部分，由每个AIV分别完成部分计算。每个AIV将其部分计算结果通过原子累加写入GM。AIC必须读取所有AIV均完成累加后的最终结果，因此需要通过核间同步来保证AIC读取`asc_reduce`结果时的时序正确性。

**图5**  核间同步业务场景示例图（AIC与AIV的比例为1：2）

![](../../../figures/inter_core_sync_scenario_example.png)

AIC/AIV多核结构如下图6所示，AIC/AIV按group划分。一个group内包含1个block和N个subblock（N≥1），其中block表示“主核”，每个subblock表示一个“从核”。

算子按计算特征可分为三类：Cube算子（矩阵计算）、Vector算子（矢量计算）和Mix算子（同时包含矩阵和矢量计算）。如表4所示，算子类型决定了其所需的核间同步方式和可选的group配置模式。其中Mix算子通过函数执行空间限定符指定AIC与AIV的block/subblock关系；Cube算子和Vector算子为单类核执行，不涉及block/subblock划分。AIV为block且AIC为subblock的配置（即Vector核为主、Cube核为辅的模式）当前不支持。表6总结了核间同步接口及其支持的同步场景。

**图6**  block和subblock之间关系（灰色部分表示一个group，即1个block和N个subblock）<a id="fig_block_subblock_relationship"></a>

![](../../../figures/block_subblock_relationship_3510.png)

**表4** group配置

| 算子类型 | 函数执行空间限定符 | 核函数（Kernel）类型 | block/subblock | 1：N |
| --- | --- | --- | --- | --- |
| Cube算子 | `__cube__` | `KERNEL_TYPE_AIC_ONLY` | AIC为block，无subblock | 不涉及 |
| Vector算子 | `__vector__` | `KERNEL_TYPE_AIV_ONLY` | AIV为block，无subblock | 不涉及 |
| Mix算子 | `__mix__(1, 0)` | `KERNEL_TYPE_MIX_AIC_1_0` | AIC为block，无subblock | 不涉及 |
| Mix算子 | `__mix__(0, 1)` | `KERNEL_TYPE_MIX_AIV_1_0` | AIV为block，无subblock | 不涉及 |
| Mix算子 | `__mix__(1, 1)` | `KERNEL_TYPE_MIX_AIC_1_1` | AIC为block，AIV为subblock | 1：1 |
| Mix算子 | `__mix__(1, 2)` | `KERNEL_TYPE_MIX_AIC_1_2` | AIC为block，AIV为subblock | 1：2 |

#### 核间同步控制模式

图7展示了四种同步控制模式，各模式的功能描述如下。该图基于核函数（Kernel）使用`__mix__(1, 2)`修饰、逻辑核数`numBlocks=4`的场景配置。

- 模式0：AI Core核间的同步控制。对于AIC全核场景，同步所有的AIC核，直到所有的AIC核都执行到`asc_sync_inter_arrive`时，`asc_sync_inter_wait`后续的指令才会执行；对于AIV全核场景，同步所有的AIV核，直到所有的AIV核都执行到`asc_sync_inter_arrive`时，`asc_sync_inter_wait`后续的指令才会执行。
- 模式1：AI Core内部，AIV核之间的同步控制。如果两个AIV核都运行了`asc_sync_subblock_arrive`，`asc_sync_subblock_wait`后续的指令才会执行。
- 模式2：AI Core内部，AIC与AIV之间的同步控制。在AIC核执行`asc_sync_block_arrive`之后，两个AIV上`asc_sync_block_wait`后续的指令才会继续执行；两个AIV都执行`asc_sync_block_arrive`后，AIC上`asc_sync_block_wait`后续的指令才能执行。
- 模式4：AI Core内部，AIC与单个AIV之间的同步控制。在单个AIV核执行`asc_sync_intra_arrive`之后，AIC上`asc_sync_intra_wait`后续的指令才会继续执行；AIC执行`asc_sync_intra_arrive`后，单个AIV上`asc_sync_intra_wait`后续的指令才能执行。AIV0与AIV1可单独触发AIC等待。

**图7**  四种核间同步控制模式示意图<a id="fig_sync_control_mode"></a>

![](../../../figures/3510_sync_control_mode_diagram.png)

## API列表

### 核内同步

核内同步接口的分类、适用场景和差异如表5所示。

**表5** 核内同步接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_sync_notify](intra_core_sync/asc_sync_notify.md)/[asc_sync_wait](intra_core_sync/asc_sync_wait.md) | **多流水同步**：同一核内具有数据依赖的不同类型流水指令之间的同步。<br>需要开发者显式指定一对源流水和目的流水。`asc_sync_notify`在源流水前序指令的所有读写操作完成后设置标志位；`asc_sync_wait`阻塞目的流水，直至检测到该标志位，解除阻塞并清除标志位。 | `c_api/sync/sync.h` |
| [asc_lock](intra_core_sync/asc_lock.md)/[asc_unlock](intra_core_sync/asc_unlock.md) | **多流水同步**：同一核内不同流水之间的同步控制。<br>通过`mutex_id`锁定和释放指定流水。`asc_lock`根据`mutex_id`获取Mutex，若Mutex已被锁定，将阻塞指定流水上后续指令的执行；`asc_unlock`在指定流水的前序指令执行完成后释放对应Mutex。与`asc_sync_notify`/`asc_sync_wait`相比，该组合内聚性更强、与其他流水解耦，可简化反向同步逻辑。 | `c_api/sync/sync.h` |
| [asc_sync_mte2](intra_core_sync/asc_sync_mte2.md)、[asc_sync_mte3](intra_core_sync/asc_sync_mte3.md)、[asc_sync_vec](intra_core_sync/asc_sync_vec.md) | **多流水同步**：需要同时阻塞多条目的流水，直到指定源流水的前序指令完成。<br>**易用性接口**，内部封装了多对`asc_sync_notify`与`asc_sync_wait`。<br>`asc_sync_mte2`等价于分别执行`PIPE_MTE2`->`PIPE_MTE3`和`PIPE_MTE2`->`PIPE_V`两组同步。<br>`asc_sync_mte3`等价于分别执行`PIPE_MTE3`->`PIPE_MTE2`和`PIPE_MTE3`->`PIPE_V`两组同步。<br>`asc_sync_vec`等价于分别执行`PIPE_V`->`PIPE_MTE2`、`PIPE_V`->`PIPE_MTE3`和`PIPE_V`->`PIPE_S`三组同步。<br>以上三个接口只能在AIV中调用。 | `c_api/sync/sync.h` |
| [asc_sync_pipe](intra_core_sync/asc_sync_pipe.md) | **单流水同步**：同一核内具有数据依赖的相同类型流水指令之间的同步。<br>同一流水中虽然指令是顺序执行，但后一条指令开始执行时前一条指令不一定执行结束。`asc_sync_pipe`保证前序指令中所有数据读写全部完成后，后序指令才开始执行。该接口不支持`PIPE_S`单流水同步。 | `c_api/sync/sync.h` |
| [asc_sync](intra_core_sync/asc_sync.md) | **全流水同步**：需要阻塞调用点后所有硬件流水的后序指令，直到调用点之前所有硬件流水的前序指令全部完成。<br>功能与`asc_sync_pipe(PIPE_ALL)`等价，适用于无法只约束单条流水的场景。 | `c_api/sync/sync.h` |
| [asc_sync_data_barrier](intra_core_sync/asc_sync_data_barrier.md) | **内存访问屏障**：需要等待此前已发出但尚未完成的内存访问指令全部执行完成。<br>通过`arg`参数指定屏障作用的内存范围，确保屏障前后的内存访问指令按预期顺序完成。 | `c_api/sync/sync.h` |

### 核间同步

核间同步接口及其支持的同步场景如表6所示。

**表6** 核间同步接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_sync_inter_arrive](inter_core_sync/asc_sync_inter_arrive.md)/[asc_sync_inter_wait](inter_core_sync/asc_sync_inter_wait.md) | **同步范围**：组间同步，即不同group之间所有block（所有AIC或所有AIV）之间的同步。<br>**对应模式**：模式0（同步等待全部AIC或全部AIV执行结束）<br>`asc_sync_inter_arrive`为生产者，负责发送通知；`asc_sync_inter_wait`为消费者，负责阻塞等待通知到达。 | `c_api/sync/sync.h` |
| [asc_sync_subblock_arrive](inter_core_sync/asc_sync_subblock_arrive.md)/[asc_sync_subblock_wait](inter_core_sync/asc_sync_subblock_wait.md) | **同步范围**：组内subblock间的同步，即同一group内不同subblock（AIV）之间的同步。<br>**对应模式**：模式1（单个AI Core内，全部AIV之间的同步）<br>`asc_sync_subblock_arrive`为生产者，负责发送通知；`asc_sync_subblock_wait`为消费者，负责阻塞等待通知到达。 | `c_api/sync/sync.h` |
| [asc_sync_block_arrive](inter_core_sync/asc_sync_block_arrive.md)/[asc_sync_block_wait](inter_core_sync/asc_sync_block_wait.md) | **同步范围**：同一group内block（AIC）与所有subblock（AIV）之间的同步。<br>**对应模式**：模式2（单个AI Core内，AIC与全部AIV之间的同步）<br>`asc_sync_block_arrive`为生产者，负责发送通知；`asc_sync_block_wait`为消费者，负责阻塞等待通知到达。 | `c_api/sync/sync.h` |
| [asc_sync_intra_arrive](inter_core_sync/asc_sync_intra_arrive.md)/[asc_sync_intra_wait](inter_core_sync/asc_sync_intra_wait.md) | **同步范围**：同一group内block（AIC）与单个subblock（AIV）之间的同步。<br>**对应模式**：模式4（单个AI Core内，AIC核与单个AIV之间同步）<br>`asc_sync_intra_arrive`为生产者，负责发送通知；`asc_sync_intra_wait`为消费者，负责阻塞等待通知到达。 | `c_api/sync/sync.h` |
