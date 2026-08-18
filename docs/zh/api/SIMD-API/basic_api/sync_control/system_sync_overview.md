# 系统同步能力概述<a name="ZH-CN_TOPIC_0000002586323695"></a>

在编程中，同步是指协调多个执行单元（如线程、进程等）对共享资源的访问顺序和时机来确保程序的正确性。如果没有同步来严格控制执行顺序，则会出现结果不一致、行为不可预测等多种问题。

AI Core的同步，总共分成3类：核内同步、核间同步、任务间同步。各类同步对应的接口如下。

**表1**  核内同步接口

| 接口名称 | 功能简述 | 同步类别 |
| --- | --- | --- |
| [SetFlag/WaitFlag\(ISASI\)](intra_core_sync/SetFlag_WaitFlag_ISASI.md) | 多流水同步：同一核内不同流水之间的同步指令。SetFlag负责发送通知，WaitFlag负责阻塞直到SetFlag发送通知。 | 硬件同步 |
| [PipeBarrier](intra_core_sync/PipeBarrier_ISASI.md) | 单流水同步：同一核内相同流水之间的同步指令（不支持PIPE_S单流水同步）。 | 硬件同步 |
| [DataSyncBarrier](intra_core_sync/DataSyncBarrier_ISASI.md) | 阻塞后续的指令执行，直到所有之前的内存访问指令（需要等待的内存位置可通过参数控制）执行结束。 | 硬件同步 |
| [Lock/Unlock](intra_core_sync/Mutex_ISASI.md) | 用于核内异步流水指令之间的同步处理，其功能类似于传统CPU中的锁机制。通过锁定指定流水再释放流水来完成流水间的同步依赖。 | 硬件同步 |

**表2**  核间同步接口

| 接口名称 | 功能简述 | 同步类别 |
| --- | --- | --- |
| [CrossCoreSetFlag](inter_core_sync/CrossCoreSetFlag_ISASI.md)/[CrossCoreWaitFlag](inter_core_sync/CrossCoreWaitFlag_ISASI.md) |多个AIC或AIV之间的同步控制。CrossCoreSetFlag负责发送通知，CrossCoreWaitFlag负责阻塞直到CrossCoreSetFlag发送通知。<br>支持以下同步场景：<br>&bull;同步等待全部AIC或者同步等待全部AIV执行结束。<br>&bull;单个AI Core内，全部AIV之间的同步。<br>&bull;单个AI Core内，AIC与全部AIV之间的同步。 | 硬件同步 |
| [IBSet](inter_core_sync/IBSet.md)/[IBWait](inter_core_sync/IBWait.md) | 被等待核中调用IBSet，通过将值1写入全局内存中某块地址来通知等待核；而等待核中调用IBWait，持续读取全局内存中相应地址块的值，直到其值变为1。 | 软件同步 |
| [SyncAll](inter_core_sync/SyncAll.md) | 用于多个核之间的同步控制，支持以下同步场景：<br>&bull;纯Vector算子的全核同步，可参考[CrossCoreSetFlag的模式0](inter_core_sync/key_features.md)。<br>&bull;融合算子的全核同步，一次性实现三个核间同步能力（参考[CrossCoreSetFlag的关键特性说明](inter_core_sync/key_features.md)）：首先完成单个AI Core内，AIC与全部AIV之间的同步（AIC等AIV）；接着完成多个AI Core间，所有AIC的同步；最后完成单个AI Core内，所有AIV与AIC同步（AIV等AIC）。 | 硬件同步 |
| [SyncAll](inter_core_sync/SyncAll.md) | 仅支持纯Vector算子的多核（参与同步的核数可通过入参指定）同步。每一个AIV都向全局内存对应位置写入信号值1，接着每一个AIV都持续轮询全局内存中所有AIV对应的信号值，直到所有AIV核对应的信号值都变为1。 | 软件同步 |
| [InitDetermineComputeWorkspace](inter_core_sync/InitDetermineComputeWorkspace.md)/[NotifyNextBlock](inter_core_sync/NotifyNextBlock.md)/[WaitPreBlock](inter_core_sync/WaitPreBlock.md) | &bull; InitDetermineComputeWorkspace、NotifyNextBlock和WaitPreBlock三个接口必须组合使用，能够确保多个AIV核严格按照blockIdx的升序执行，适用于要求确定性计算的场景。<br>&bull; InitDetermineComputeWorkspace初始化GM共享内存的值，完成初始化后才可以调用NotifyNextBlock和WaitPreBlock。<br>&bull; NotifyNextBlock通过写全局内存来通知其他核当前核已执行完成，其他核可以继续往下执行；WaitPreBlock通过读取全局内存，判断当前核是否可以继续往下执行。 | 软件同步 |

**表3**  任务间同步接口

| 接口名称 | 功能简述 | 同步类别 |
| --- | --- | --- |
| [SetNextTaskStart](./inter_task_sync/SetNextTaskStart.md) | 在SuperKernel的子核函数（Kernel）中调用，调用后的指令可以和后续其他的子核函数（Kernel）实现并行，提升整体性能。 | 硬件同步 |
| [WaitPreTaskEnd](./inter_task_sync/WaitPreTaskEnd.md) | 在SuperKernel的子核函数（Kernel）中调用，调用前的指令可以和前序其他的子核函数（Kernel）实现并行，提升整体性能。 | 硬件同步 |
