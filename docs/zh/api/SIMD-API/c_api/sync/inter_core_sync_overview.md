# 核间同步能力概述

核间同步的使用场景通常是：一个核计算所依赖的数据，需要等待其他相关核的计算全部完成后才能获取。  

以图1为例，AIC需要依赖AIV的`asc_reduce`计算结果。由于整体矢量较大，必须拆分为多个部分，由每个AIV分别完成部分计算。每个AIV将其部分计算结果通过原子累加写入GM。AIC必须读取所有AIV均完成累加后的最终结果，因此需要通过核间同步来保证AIC读取`asc_reduce`结果时的时序正确性。

**图1**  核间同步业务场景示例图（AIC与AIV的比例为1：2）    
![](../../../figures/inter_core_sync_scenario_example.png)

AIC/AIV多核结构如下图2所示，AIC/AIV按group划分。一个group内细分block及subblock，block与subblock比例为1：N（N≥1），block表示多少个“主核”，subblock表示一个“主核”带多少个“从核”。

算子按计算特征可分为三类：Cube算子（矩阵计算）、Vector算子（矢量计算）和Mix算子（同时包含矩阵和矢量计算）。如表1所示，算子类型决定了其所需的核间同步方式和可选的group配置模式。其中Mix算子通过函数修饰符指定AIC与AIV的block/subblock关系；Cube算子和Vector算子为单类核执行，不涉及block/subblock划分。AIV为block且AIC为subblock的配置（即Vector核为主、Cube核为辅的模式）当前不支持。表2总结了核间同步接口及其支持的同步场景。

**图2**  block和subblock之间关系（灰色部分表示一个group，即1个block和N个subblock）<a name="fig_block_subblock_relationship"></a>    
![](../../../figures/block_subblock_relationship_3510.png)

**表1**  group配置

| 算子类型 | 函数修饰符 | Kernel类型 | block/subblock | 1：N |
| --- | --- | --- | --- | --- |
| Cube算子 | `__cube__` | `KERNEL_TYPE_AIC_ONLY` | AIC为block，无subblock | 不涉及 |
| Vector算子 | `__vector__` | `KERNEL_TYPE_AIV_ONLY` | AIV为block，无subblock | 不涉及 |
| Mix算子 | `__mix__(1, 0)` | `KERNEL_TYPE_MIX_AIC_1_0` | AIC为block，无subblock | 不涉及 |
| Mix算子 | `__mix__(0, 1)` | `KERNEL_TYPE_MIX_AIV_1_0` | AIV为block，无subblock | 不涉及 |
| Mix算子 | `__mix__(1, 1)` | `KERNEL_TYPE_MIX_AIC_1_1` | AIC为block，AIV为subblock | 1：1 |
| Mix算子 | `__mix__(1, 2)` | `KERNEL_TYPE_MIX_AIC_1_2` | AIC为block，AIV为subblock | 1：2 |

**表2**  支持的核间同步场景

| 接口名称 | 同步范围 | 对应模式 | 功能简述 |
| --- | --- | --- | --- |
| [asc_sync_inter_arrive](asc_sync_inter_arrive.md)/[asc_sync_inter_wait](asc_sync_inter_wait.md) | 组间同步，即不同group之间所有block（所有AIC或所有AIV）之间的同步。 | 模式0（同步等待全部AIC或全部AIV执行结束） | `asc_sync_inter_arrive`为生产者，负责发送通知；`asc_sync_inter_wait`为消费者，负责阻塞等待通知到达。 |
| [asc_sync_subblock_arrive](asc_sync_subblock_arrive.md)/[asc_sync_subblock_wait](asc_sync_subblock_wait.md) | 组内subblock间的同步，即同一group内不同subblock（AIV）之间的同步。 | 模式1（单个AI Core内，全部AIV之间的同步） | `asc_sync_subblock_arrive`为生产者，负责发送通知；`asc_sync_subblock_wait`为消费者，负责阻塞等待通知到达。 |
| [asc_sync_block_arrive](asc_sync_block_arrive.md)/[asc_sync_block_wait](asc_sync_block_wait.md) | 同一group内block（AIC）与所有subblock（AIV）之间的同步。 | 模式2（单个AI Core内，AIC与全部AIV之间的同步） | `asc_sync_block_arrive`为生产者，负责发送通知；`asc_sync_block_wait`为消费者，负责阻塞等待通知到达。 |
| [asc_sync_intra_arrive](asc_sync_intra_arrive.md)/[asc_sync_intra_wait](asc_sync_intra_wait.md) | 同一group内block（AIC）与单个subblock（AIV）之间的同步。 | 模式4（单个AI Core内，AIC核与单个AIV之间同步） | `asc_sync_intra_arrive`为生产者，负责发送通知；`asc_sync_intra_wait`为消费者，负责阻塞等待通知到达。 |
