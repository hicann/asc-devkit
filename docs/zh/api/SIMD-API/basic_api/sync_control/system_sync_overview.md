# 系统同步能力概述<a name="ZH-CN_TOPIC_0000002586323695"></a>

在Ascend C编程中，同步用于约束AI Core内外不同执行单元的执行先后关系，确保后续指令在依赖的数据、状态或同步信号就绪后再继续执行。由于数据搬运、计算、不同核函数（Kernel）或不同AI Core之间可以并行，如果缺少必要的同步，后续执行单元可能读取到尚未写入完成的数据，或在前序任务未完成时提前执行，导致计算结果错误或执行行为不符合预期。

AI Core的同步总共分成3类：核内同步、核间同步、任务间同步。核间通信用于管理不同AI Core之间的消息传递和通信资源，与同步能力相关但属于独立的接口分类。

以下内容展示三类同步和核间通信的使用场景。

- 核内同步用于约束同一核内不同硬件流水或同一硬件流水内指令的执行时序。

    例如在AIV中，MTE2单元通过`PIPE_MTE2`将数据搬入片上存储后，Vector单元才可以通过`PIPE_V`读取该数据进行计算，此时需要插入`PIPE_MTE2`到`PIPE_V`的同步，使`PIPE_V`等待`PIPE_MTE2`的前序搬入完成。
- 核间同步用于约束不同核之间的执行进度，参与同步的对象可以是多个AI Core，也可以是同一AI Core内的AIC和AIV。
    <!-- npu="950" id1 -->
    如图1所示，针对[NPU架构3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)，同一个AI Core内AIV通过`PIPE_MTE3`将数据从UB搬运到L1 Buffer后，AIC侧`PIPE_MTE1`才能将L1 Buffer中的数据搬入L0A Buffer和L0B Buffer并继续执行矩阵计算，此时需要让AIC等待AIV的数据搬运完成通知。

    核内同步和核间同步的区别在于：核内同步解决同一核内部硬件流水之间的数据依赖，关注`PIPE_MTE2`、`PIPE_V`等流水的先后关系；核间同步解决不同核或不同逻辑核之间的进度依赖，关注AIC、AIV或多个AI Core之间的先后关系。

    **图1**  [NPU架构3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)架构图
    ![](../../../figures/npu_3510_hw_arch_sync.png)
    <!-- end id1 -->

- 核间通信用于不同AI Core之间的消息传递和通信资源管理。例如，AIV可以通过`CubeResGroupHandle`向AIC发送消息，AIC根据消息内容执行对应的计算任务；`KfcWorkspace`用于管理消息通信空间。核间通信负责传递消息和管理资源，核间同步负责约束任务的执行顺序，两者可以根据业务需要配合使用。

- 任务间同步用于SuperKernel场景下多个子核函数（Kernel）之间的调度与并行控制。SuperKernel将多个子核函数（Kernel）融合为一个超级核函数（Kernel）执行，通过任务间同步接口可以让相邻子核函数（Kernel）的部分指令并行，减少Task调度等待时间。更多原理和适配说明请参考[SuperKernel](../../../../guide/programming_guide/advanced_programming/super_kernel/principles.md)。

各类同步对应的接口如下。

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
| [GroupBarrier](inter_core_sync/GroupBarrier/GroupBarrier.md) | 用于同一个CubeResGroupHandle中的AIV任务之间的组同步，通过Arrive和Wait控制任务依赖关系。 | 软件同步 |

**表3**  任务间同步接口

| 接口名称 | 功能简述 | 同步类别 |
| --- | --- | --- |
| [SetNextTaskStart](./inter_task_sync/SetNextTaskStart.md) | 在SuperKernel的子核函数（Kernel）中调用，调用后的指令可以和后续其他的子核函数（Kernel）实现并行，提升整体性能。 | 硬件同步 |
| [WaitPreTaskEnd](./inter_task_sync/WaitPreTaskEnd.md) | 在SuperKernel的子核函数（Kernel）中调用，调用前的指令可以和前序其他的子核函数（Kernel）实现并行，提升整体性能。 | 硬件同步 |

核间通信接口负责在不同AI Core之间传递消息，以及管理消息队列和通信空间。它不直接定义任务的到达或等待关系；当业务同时需要数据传递和执行顺序控制时，应分别使用核间通信接口和核间同步接口。

**表4**  核间通信接口

| 接口名称 | 功能简述 |
| --- | --- |
| [CubeResGroupHandle](inter_core_communication/CubeResGroupHandle/CubeResGroupHandle.md) | 对AIC和AIV计算资源进行分组，并通过消息队列完成核间消息的申请、发送、处理和释放。 |
| [KfcWorkspace](inter_core_communication/KfcWorkspace/KfcWorkspace.md) | 管理CubeResGroupHandle使用的消息通信空间。 |
