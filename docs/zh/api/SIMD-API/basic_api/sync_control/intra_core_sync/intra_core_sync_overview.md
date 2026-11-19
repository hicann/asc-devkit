# 核内同步能力概述<a name="ZH-CN_TOPIC_0000002555621170"></a>

## 为什么需要核内同步<a name="zh-cn_topic_0000002542725361_section199075142046"></a>

AI Core内部的执行单元（如MTE2搬运单元、Vector计算单元等）以异步并行的方式运行，在读写同一存储资源时可能存在数据依赖关系。为确保数据一致性及计算正确性，需通过同步控制协调操作时序。

<!-- npu="910b,A3" id1 -->

针对[NPU架构2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)，硬件架构图如下，高亮部分展示了并行执行的计算单元和搬运单元。

**图 1**  架构图<a name="zh-cn_topic_0000002542725361_fig11732135516285"></a>  
![](../../../../figures/atlas_a2_a3_architecture.png "Atlas_A2_A3_硬件架构图")

<!-- end id1 -->

<!-- npu="950" id2 -->

针对[NPU架构3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)，硬件架构图如下，高亮部分展示了并行执行的计算单元和搬运单元。

**图 2**  架构图<a name="zh-cn_topic_0000002542725361_fig31512113433"></a>  
![](../../../../figures/npu_3510_hw_arch_sync.png "Ascend_950PR_950DT_硬件架构图")

<!-- end id2 -->

下图示例描述了一个常见的Vector计算数据流：

1. 先通过DMA执行单元将数据从Global Memory（GM）搬入到Local Memory；
2. 进行计算；
3. 然后再通过DMA执行单元将计算结果从Local Memory搬出到GM。

![](../../../../figures/vector_compute_data_flow.png "Vector_计算_数据流示意图")

四个执行单元Scalar、Vector、DMA（MTE2）、DMA(MTE3)并行执行，若访问同一片Local Memory，需要同步机制来控制它们的访问时序：保证先搬入Local Memory后再计算，计算完成后再搬出。

![](../../../../figures/intra_core_parallel_pipeline_execution_timing.png "intra_core_parallel_pipeline_execution_timing")

## 硬件流水类型<a name="zh-cn_topic_0000002542725361_section1272612276459"></a>

AI Core内部并行的指令流水类型和解释如下所示：

> [!NOTE]说明 
>不同的硬件架构，每一种硬件流水类型包含的具体流水会有所差异，详细介绍请参考[硬件实现](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/hardware_implementation.md)章节。

**表1**  指令流水类型和相关说明

| 流水类型 | 含义 |
| --- | --- |
| PIPE_S | 标量流水线，使用GlobalTensor/LocalTensor的GetValue函数时为此流水 |
| PIPE_V | 矢量计算流水及部分硬件架构下的L0C Buffer-&gt;UB数据搬运流水 |
| PIPE_M | 矩阵计算流水 |
| PIPE_MTE1 | L1 Buffer -&gt;L0A Buffer、L1 Buffer-&gt;L0B Buffer数据搬运流水 |
| PIPE_MTE2 | GM-&gt;L1 Buffer、GM-&gt;UB等数据搬运流水 |
| PIPE_MTE3 | UB-&gt;GM、L1 Buffer-&gt;GM等数据搬运流水 |
| PIPE_FIX | L0C Buffer-&gt;GM、L0C Buffer -&gt;L1等数据搬运流水 |

## 核内同步分类<a name="zh-cn_topic_0000002542725361_section2167161594419"></a>

对上述核内并行流水的同步控制可按表2选择。

**表2**  核内同步接口分类说明

| 同步类型 | 接口 | 适用场景 | 差异点 |
| --- | --- | --- | --- |
| 多流水同步 | [TQueSync](TQueSync.md) | 同一核内具有数据依赖的不同类型流水指令之间的同步。 | 跨硬件版本兼容接口。通过模板参数指定源流水和目的流水，再通过成员函数`SetFlag`/`WaitFlag`完成一对流水之间的同步。 |
| 多流水同步 | [SetFlag/WaitFlag\(ISASI\)](SetFlag_WaitFlag_ISASI.md) | 同一核内具有数据依赖的不同类型流水指令之间的同步。 | 需要开发者显式指定一对源流水和目的流水。`SetFlag`在源流水前序指令的所有读写操作完成后设置标志位；`WaitFlag`阻塞目的流水，直至检测到该标志位，解除阻塞并清除标志位。该接口属于ISASI类接口，不能保证跨硬件版本兼容。 |
| 多流水同步 | [Lock](Lock.md)/[Unlock](Unlock.md) | 同一核内不同流水之间的同步控制。 | 通过MutexID锁定和释放指定流水。`Lock`根据MutexID获取Mutex，若Mutex已被锁定，将阻塞指定流水上后续指令的执行；`Unlock`在指定流水的前序指令执行完成后释放对应Mutex。与`SetFlag`/`WaitFlag`相比，该组合内聚性更强、与其他流水解耦，可简化反向同步逻辑，支持情况以接口文档为准。 |
| 单流水同步 | [PipeBarrier](PipeBarrier_ISASI.md) | 同一核内具有数据依赖的相同类型流水指令之间的同步。 | 同一流水中虽然指令是顺序执行，但后一条指令开始执行时前一条指令不一定执行结束。`PipeBarrier`保证前序指令中所有数据读写全部完成后，后序指令才开始执行。该接口不支持`PIPE_S`单流水同步；如果需要阻塞所有流水，可以使用`PipeBarrier<PIPE_ALL>()`。 |
| 内存访问屏障 | [DataSyncBarrier](DataSyncBarrier_ISASI.md) | 需要等待此前已发出但尚未完成的内存访问指令全部执行完成。 | 用于解决Scalar的内存依赖，通过模板参数指定需要等待的内存位置，确保屏障前后的内存访问指令按预期顺序完成。 |

<!-- npu="910b,A3" id4 -->

以[NPU架构2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)为例，该硬件架构下所有合法的核内同步组合如[表3](#zh-cn_topic_0000002542725361_table13753342164113)和[表4](#zh-cn_topic_0000002542725361_table1555712177426)所示。其中，"不涉及"表示硬件层面不存在此种同步组合，"暂无应用场景"表示存在此种同步组合，但在实际开发场景中暂不需要使用。

**表3**  AIC中所有合法的核内同步组合<a name="zh-cn_topic_0000002542725361_table13753342164113"></a>

| 源流水 | PIPE_S | PIPE_M | PIPE_MTE1 | PIPE_MTE2 | PIPE_MTE3 | PIPE_FIX |
| --- | --- | --- | --- | --- | --- | --- |
| PIPE_S | 不涉及 | 不涉及 | 不涉及 | 不涉及 | 不涉及 | 不涉及 |
| PIPE_M | 不涉及 | PipeBarrier&lt;PIPE_M&gt;() | SetFlag&lt;M_MTE1&gt;(0)<br>WaitFlag&lt;M_MTE1&gt;(0) | SetFlag&lt;M_MTE2&gt;(0)<br>WaitFlag&lt;M_MTE2&gt;(0) | 不涉及 | SetFlag&lt;M_FIX&gt;(0)<br>WaitFlag&lt;M_FIX&gt;(0) |
| PIPE_MTE1 | 不涉及 | SetFlag&lt;MTE1_M&gt;(0)<br>WaitFlag&lt;MTE1_M&gt;(0) | PipeBarrier&lt;PIPE_MTE1&gt;() | SetFlag&lt;MTE1_MTE2&gt;(0)<br>WaitFlag&lt;MTE1_MTE2&gt;(0) | SetFlag&lt;MTE1_MTE3&gt;(0)<br>WaitFlag&lt;MTE1_MTE3&gt;(0) | SetFlag&lt;MTE1_FIX&gt;(0)<br>WaitFlag&lt;MTE1_FIX&gt;(0) |
| PIPE_MTE2 | 不涉及 | SetFlag&lt;MTE2_M&gt;(0)<br>WaitFlag&lt;MTE2_M&gt;(0) | SetFlag&lt;MTE2_MTE1&gt;(0)<br>WaitFlag&lt;MTE2_MTE1&gt;(0) | PipeBarrier&lt;PIPE_MTE2&gt;() | SetFlag&lt;MTE2_MTE3&gt;(0)<br>WaitFlag&lt;MTE2_MTE3&gt;(0) | 暂无应用场景 |
| PIPE_MTE3 | 不涉及 | 不涉及 | SetFlag&lt;MTE3_MTE1&gt;(0)<br>WaitFlag&lt;MTE3_MTE1&gt;(0) | SetFlag&lt;MTE3_MTE2&gt;(0)<br>WaitFlag&lt;MTE3_MTE2&gt;(0) | PipeBarrier&lt;PIPE_MTE3&gt;() | 暂无应用场景 |
| PIPE_FIX | 不涉及 | SetFlag&lt;FIX_M&gt;(0)<br>WaitFlag&lt;FIX_M&gt;(0) | SetFlag&lt;FIX_MTE1&gt;(0)<br>WaitFlag&lt;FIX_MTE1&gt;(0) | 暂无应用场景 | 暂无应用场景 | PipeBarrier&lt;PIPE_FIX&gt;() |

**表4**  AIV中所有合法的核内同步组合<a name="zh-cn_topic_0000002542725361_table1555712177426"></a>

| 源流水 | PIPE_S | PIPE_V | PIPE_MTE2 | PIPE_MTE3 |
| --- | --- | --- | --- | --- |
| PIPE_S | 不涉及 | SetFlag&lt;S_V&gt;(0)<br>WaitFlag&lt;S_V&gt;(0) | SetFlag&lt;S_MTE2&gt;(0)<br>WaitFlag&lt;S_MTE2&gt;(0) | SetFlag&lt;S_MTE3&gt;(0)<br>WaitFlag&lt;S_MTE3&gt;(0) |
| PIPE_V | SetFlag&lt;V_S&gt;(0)<br>WaitFlag&lt;V_S&gt;(0) | PipeBarrier&lt;PIPE_V&gt;() | SetFlag&lt;V_MTE2&gt;(0)<br>WaitFlag&lt;V_MTE2&gt;(0) | SetFlag&lt;V_MTE3&gt;(0)<br>WaitFlag&lt;V_MTE3&gt;(0) |
| PIPE_MTE2 | SetFlag&lt;MTE2_S&gt;(0)<br>WaitFlag&lt;MTE2_S&gt;(0) | SetFlag&lt;MTE2_V&gt;(0)<br>WaitFlag&lt;MTE2_V&gt;(0) | PipeBarrier&lt;PIPE_MTE2&gt;() | SetFlag&lt;MTE2_MTE3&gt;(0)<br>WaitFlag&lt;MTE2_MTE3&gt;(0) |
| PIPE_MTE3 | SetFlag&lt;MTE3_S&gt;(0)<br>WaitFlag&lt;MTE3_S&gt;(0) | SetFlag&lt;MTE3_V&gt;(0)<br>WaitFlag&lt;MTE3_V&gt;(0) | SetFlag&lt;MTE3_MTE2&gt;(0)<br>WaitFlag&lt;MTE3_MTE2&gt;(0) | PipeBarrier&lt;PIPE_MTE3&gt;() |

<!-- end id4 -->

## 什么时候需要开发者手动插入同步

[TPipe-TQue框架编程范式](../../../../../guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_paradigm.md)和毕昇编译器均支持[自动同步](key_features.md)，可减少开发者手动插入同步的工作量，但部分场景下仍需开发者手动完成。
