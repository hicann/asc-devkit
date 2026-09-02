# 核内同步能力概述

## 为什么需要核内同步

AI Core内部的执行单元（如MTE2搬运单元、Vector计算单元等）以异步并行的方式运行，在读写同一存储资源时可能存在数据依赖关系。为确保数据一致性及计算正确性，需通过同步控制协调操作时序。

<!-- npu="950" id2 -->

针对[NPU架构3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)，硬件架构图如下，高亮部分展示了并行执行的计算单元和搬运单元。

**图1**  NPU架构3510架构图    
![](../../../figures/npu_3510_hw_arch_sync.png)

<!-- end id2 -->

<!-- npu="A3,910b" id1 -->

针对[NPU架构2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)，硬件架构图如下，高亮部分展示了并行执行的计算单元和搬运单元。

**图2**  NPU架构2201架构图    
![](../../../figures/atlas_a2_a3_architecture.png)

<!-- end id1 -->

下图示例描述了一个常见的Vector计算数据流：

1. 先通过DMA执行单元将数据从GM搬入到Local Memory；
2. 进行计算；
3. 然后再通过DMA执行单元将计算结果从Local Memory搬出到GM。

**图3**  Vector计算数据流示意图    
![](../../../figures/vector_compute_data_flow.png)

四个执行单元Scalar、Vector、DMA（MTE2）、DMA（MTE3）并行执行，若访问同一片Local Memory，需要同步机制来控制它们的访问时序：保证先搬入Local Memory后再计算，计算完成后再搬出。

**图4**  核内并行流水执行时序示意图    
![](../../../figures/intra_core_parallel_pipeline_execution_timing.png)

## 硬件流水类型

AI Core内部并行的指令流水类型和解释如下所示：

> [!NOTE]说明
>不同的硬件架构，每一种硬件流水类型包含的具体流水会有所差异，详细介绍请参考[硬件实现](../../../../guide/programming_guide/advanced_programming/hardware_implementation/hardware_implementation.md)章节。

**表1**  指令流水类型和相关说明

| 流水类型 | 含义 |
| --- | --- |
| PIPE_S | 标量流水线，使用标量访存语句或标量计算接口访问GM、片上存储地址时为此流水 |
| PIPE_V | 矢量计算流水及部分硬件架构下的L0C Buffer->UB数据搬运流水 |
| PIPE_M | 矩阵计算流水 |
| PIPE_MTE1 | L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer数据搬运流水 |
| PIPE_MTE2 | GM->L1 Buffer、GM->UB等数据搬运流水 |
| PIPE_MTE3 | UB->GM等数据搬运流水 |
| PIPE_FIX | L0C Buffer->GM、L0C Buffer->L1 Buffer等数据搬运流水 |

## 核内同步分类

对上述核内并行流水的同步控制分为以下几种：

- 多流水同步：同一核内具有数据依赖的不同类型流水指令之间的同步。
    - 通过[asc_sync_notify](asc_sync_notify.md)/[asc_sync_wait](asc_sync_wait.md)接口进行不同流水线间的同步控制。在`asc_sync_notify`/`asc_sync_wait`的指令中，可以指定一对指令流水（源流水与目的流水）先后执行的关系，表示两个指令流水之间完成一组“锁”机制，其作用原理为：
        - `asc_sync_notify`：当源流水的前序指令的所有读写操作都完成之后，当前指令开始执行，并将硬件中的对应标志位设置为1。
        - `asc_sync_wait`：当目的流水执行到该指令时，如果发现硬件中对应标志位为0，目的流水的后续指令将一直被阻塞；如果发现硬件中对应标志位为1，则将硬件中对应标志位设置为0，同时目的流水的后续指令开始执行。
    <!-- npu="950" id3 -->
    - 针对Ascend 950PR/Ascend 950DT，新增[asc_lock](asc_lock.md)/[asc_unlock](asc_unlock.md)接口进行不同流水线间的同步控制。通过`Lock`锁定指定流水（阻塞后续指令），再通过`Unlock`释放流水，来完成流水间的同步依赖。
        - `asc_lock`：根据`mutex_id`获取Mutex，若Mutex已被锁定，将阻塞后续指定流水指令队列，直到前序指令中对应`mutex_id`的Mutex被`asc_unlock`。
        - `asc_unlock`：当前流水的前置指令退出后，根据`mutex_id`释放对应Mutex。
    - 相较于`asc_sync_notify`/`asc_sync_wait`同步机制，使用`asc_lock`/`asc_unlock`接口有以下优势：
        - 内聚性更强，使用时与其它流水线解耦，可以简化反向同步。以`PIPE_MTE2`与`PIPE_V`之间同步为例，对比如下：

            ```cpp
            // asc_sync_notify/asc_sync_wait机制
            For i=0:100
               if i > 0:
                     asc_sync_wait(PIPE_V, PIPE_MTE2, EVENT_ID0)
               endif
               // MTE2指令
               asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0)
               asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0)
               // PIPE_V指令
               if i < 99:
                     asc_sync_notify(PIPE_V, PIPE_MTE2, EVENT_ID0)
               endif
            endFor

            // asc_lock/asc_unlock机制
            For i=0:100
               asc_lock(PIPE_MTE2, 0)
               // MTE2指令
               asc_unlock(PIPE_MTE2, 0)
               asc_lock(PIPE_V, 0)
               // PIPE_V指令
               asc_unlock(PIPE_V, 0)
            endFor
            ```

        - 可以使用更多的同步信号量：`asc_sync_notify`/`asc_sync_wait`的入参`EVENT_ID0`为同步ID，取值范围为[0, 7]；而`asc_lock`/`asc_unlock`的入参`mutex_id`取值范围为[0, 31]。
    <!-- end id3 -->
- 单流水同步：同一核内具有数据依赖的相同类型流水指令之间的同步。通过[asc_sync_pipe](asc_sync_pipe.md)接口进行相同流水线间的同步控制。同一流水中虽然指令是顺序执行，但并不意味着后一条指令开始执行时前一条指令执行结束。`asc_sync_pipe`指令可以保证前序指令中所有数据读写全部完成，后序指令才开始执行。注意该接口不支持PIPE_S单流水的同步。
- 通过[asc_sync_data_barrier](asc_sync_data_barrier.md)接口阻塞后续指令的执行，直到此前已发出但尚未完成的内存访问指令全部执行完成。开发者通过`arg`参数指定屏障作用的内存范围，确保屏障前后的内存访问指令按预期顺序完成。

<!-- npu="A3,910b" id4 -->
以[NPU架构2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)为例，该硬件架构下所有合法的核内同步组合如[表2](#aic_intra_core_sync_combinations)和[表3](#aiv_intra_core_sync_combinations)所示。其中，“不涉及”表示硬件层面不存在此种同步组合，“暂无应用场景”表示存在此种同步组合，但在实际开发场景中暂不需要使用。

<a name="aic_intra_core_sync_combinations"></a>

**表2**  AIC中所有合法的核内同步组合

| 源流水/目的流水 | `PIPE_S` | `PIPE_M` | `PIPE_MTE1` | `PIPE_MTE2` | `PIPE_MTE3` | `PIPE_FIX` |
| --- | --- | --- | --- | --- | --- | --- |
| `PIPE_S` | 不涉及 | 不涉及 | 不涉及 | 不涉及 | 不涉及 | 不涉及 |
| `PIPE_M` | 不涉及 | `asc_sync_pipe(PIPE_M)` | `asc_sync_notify(PIPE_M, PIPE_MTE1, EVENT_ID0)`<br>`asc_sync_wait(PIPE_M, PIPE_MTE1, EVENT_ID0)` | `asc_sync_notify(PIPE_M, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_M, PIPE_MTE2, EVENT_ID0)` | 不涉及 | `asc_sync_notify(PIPE_M, PIPE_FIX, EVENT_ID0)`<br>`asc_sync_wait(PIPE_M, PIPE_FIX, EVENT_ID0)` |
| `PIPE_MTE1` | 不涉及 | `asc_sync_notify(PIPE_MTE1, PIPE_M, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE1, PIPE_M, EVENT_ID0)` | `asc_sync_pipe(PIPE_MTE1)` | `asc_sync_notify(PIPE_MTE1, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE1, PIPE_MTE2, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE1, PIPE_MTE3, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE1, PIPE_MTE3, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE1, PIPE_FIX, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE1, PIPE_FIX, EVENT_ID0)` |
| `PIPE_MTE2` | 不涉及 | `asc_sync_notify(PIPE_MTE2, PIPE_M, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_M, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE2, PIPE_MTE1, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_MTE1, EVENT_ID0)` | `asc_sync_pipe(PIPE_MTE2)` | `asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0)` | 暂无应用场景 |
| `PIPE_MTE3` | 不涉及 | 不涉及 | `asc_sync_notify(PIPE_MTE3, PIPE_MTE1, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE3, PIPE_MTE1, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE3, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE3, PIPE_MTE2, EVENT_ID0)` | `asc_sync_pipe(PIPE_MTE3)` | 暂无应用场景 |
| `PIPE_FIX` | 不涉及 | `asc_sync_notify(PIPE_FIX, PIPE_M, EVENT_ID0)`<br>`asc_sync_wait(PIPE_FIX, PIPE_M, EVENT_ID0)` | `asc_sync_notify(PIPE_FIX, PIPE_MTE1, EVENT_ID0)`<br>`asc_sync_wait(PIPE_FIX, PIPE_MTE1, EVENT_ID0)` | 暂无应用场景 | 暂无应用场景 | `asc_sync_pipe(PIPE_FIX)` |

<a name="aiv_intra_core_sync_combinations"></a>

**表3**  AIV中所有合法的核内同步组合

| 源流水/目的流水 | `PIPE_S` | `PIPE_V` | `PIPE_MTE2` | `PIPE_MTE3` |
| --- | --- | --- | --- | --- |
| `PIPE_S` | 不涉及 | `asc_sync_notify(PIPE_S, PIPE_V, EVENT_ID0)`<br>`asc_sync_wait(PIPE_S, PIPE_V, EVENT_ID0)` | `asc_sync_notify(PIPE_S, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_S, PIPE_MTE2, EVENT_ID0)` | `asc_sync_notify(PIPE_S, PIPE_MTE3, EVENT_ID0)`<br>`asc_sync_wait(PIPE_S, PIPE_MTE3, EVENT_ID0)` |
| `PIPE_V` | `asc_sync_notify(PIPE_V, PIPE_S, EVENT_ID0)`<br>`asc_sync_wait(PIPE_V, PIPE_S, EVENT_ID0)` | `asc_sync_pipe(PIPE_V)` | `asc_sync_notify(PIPE_V, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_V, PIPE_MTE2, EVENT_ID0)` | `asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0)`<br>`asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0)` |
| `PIPE_MTE2` | `asc_sync_notify(PIPE_MTE2, PIPE_S, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_S, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0)` | `asc_sync_pipe(PIPE_MTE2)` | `asc_sync_notify(PIPE_MTE2, PIPE_MTE3, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE2, PIPE_MTE3, EVENT_ID0)` |
| `PIPE_MTE3` | `asc_sync_notify(PIPE_MTE3, PIPE_S, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE3, PIPE_S, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE3, PIPE_V, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID0)` | `asc_sync_notify(PIPE_MTE3, PIPE_MTE2, EVENT_ID0)`<br>`asc_sync_wait(PIPE_MTE3, PIPE_MTE2, EVENT_ID0)` | `asc_sync_pipe(PIPE_MTE3)` |

<!-- end id4 -->

## 什么时候需要开发者手动插入同步

C API编程方式下所有的同步均需开发者手动管理。
