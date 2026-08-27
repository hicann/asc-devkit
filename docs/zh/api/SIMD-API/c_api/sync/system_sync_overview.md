# 系统同步能力概述

在编程中，同步是指协调多个执行单元（如线程、进程等）对共享资源的访问顺序和时机来确保程序的正确性。如果没有同步来严格控制执行顺序，则会出现结果不一致、行为不可预测等多种问题。

AI Core的同步，总共分成2类：[核内同步](intra_core_sync_overview.md)、[核间同步](inter_core_sync_overview.md)。各类同步对应的接口如下。

**表1**  核内同步接口

| 接口名称 | 功能简述 |
| --- | --- |
| [asc_sync_notify](asc_sync_notify.md)/[asc_sync_wait](asc_sync_wait.md) | 多流水同步：同一核内不同流水之间的同步指令。`asc_sync_notify`负责发送通知，`asc_sync_wait`负责阻塞直到`asc_sync_notify`发送通知。 |
| [asc_sync_pipe](asc_sync_pipe.md) | 单流水同步：同一核内相同流水之间的同步指令（不支持`PIPE_S`单流水同步）。 |
| [asc_sync_data_barrier](asc_sync_data_barrier.md) | 阻塞后续的指令执行，直到所有之前的内存访问指令（需要等待的内存位置可通过参数控制）执行结束。 |
| [asc_lock](asc_lock.md)/[asc_unlock](asc_unlock.md) | 用于核内异步流水指令之间的同步处理，其功能类似于传统CPU中的锁机制。通过锁定指定流水再释放流水来完成流水间的同步依赖。 |

如图1所示，支持的四种同步控制模式的功能描述如下：

- 模式0：AI Core核间的同步控制。对于AIC全核场景，同步所有的AIC核，直到所有的AIC核都执行到`asc_sync_inter_arrive`时，`asc_sync_inter_wait`后续的指令才会执行；对于AIV全核场景，同步所有的AIV核，直到所有的AIV核都执行到`asc_sync_inter_arrive`时，`asc_sync_inter_wait`后续的指令才会执行。
- 模式1：AI Core内部，AIV核之间的同步控制。如果两个AIV核都运行了`asc_sync_subblock_arrive`，`asc_sync_subblock_wait`后续的指令才会执行。
- 模式2：AI Core内部，AIC与AIV之间的同步控制。在AIC核执行`asc_sync_block_arrive`之后，两个AIV上`asc_sync_block_wait`后续的指令才会继续执行；两个AIV都执行`asc_sync_block_arrive`后，AIC上`asc_sync_block_wait`后续的指令才能执行。
- 模式4：AI Core内部，AIC与单个AIV之间的同步控制。在单个AIV核执行`asc_sync_intra_arrive`之后，AIC上`asc_sync_intra_wait`后续的指令才会继续执行；AIC执行`asc_sync_intra_arrive`后，单个AIV上`asc_sync_intra_wait`后续的指令才能执行。AIV0与AIV1可单独触发AIC等待。

**图1**  同步控制模式示意图<a name="fig_sync_control_mode"></a>    
![](../../../figures/3510_sync_control_mode_diagram.png)

**表2**  核间同步接口

| 接口名称 | 同步范围 | 对应模式 | 功能简述 |
| --- | --- | --- | --- |
| [asc_sync_inter_arrive](asc_sync_inter_arrive.md)/[asc_sync_inter_wait](asc_sync_inter_wait.md) | 组间同步，即不同group之间所有block（所有AIC或所有AIV）之间的同步。 | 模式0（同步等待全部AIC或全部AIV执行结束） | `asc_sync_inter_arrive`为生产者，负责发送通知；`asc_sync_inter_wait`为消费者，负责阻塞等待通知到达。 |
| [asc_sync_subblock_arrive](asc_sync_subblock_arrive.md)/[asc_sync_subblock_wait](asc_sync_subblock_wait.md) | 组内subblock间的同步，即同一group内不同subblock（AIV）之间的同步。 | 模式1（单个AI Core内，全部AIV之间的同步） | `asc_sync_subblock_arrive`为生产者，负责发送通知；`asc_sync_subblock_wait`为消费者，负责阻塞等待通知到达。 |
| [asc_sync_block_arrive](asc_sync_block_arrive.md)/[asc_sync_block_wait](asc_sync_block_wait.md) | 同一group内block（AIC）与所有subblock（AIV）之间的同步。 | 模式2（单个AI Core内，AIC与全部AIV之间的同步） | `asc_sync_block_arrive`为生产者，负责发送通知；`asc_sync_block_wait`为消费者，负责阻塞等待通知到达。 |
| [asc_sync_intra_arrive](asc_sync_intra_arrive.md)/[asc_sync_intra_wait](asc_sync_intra_wait.md) | 同一group内block（AIC）与单个subblock（AIV）之间的同步。 | 模式4（单个AI Core内，AIC核与单个AIV之间同步） | `asc_sync_intra_arrive`为生产者，负责发送通知；`asc_sync_intra_wait`为消费者，负责阻塞等待通知到达。 |
