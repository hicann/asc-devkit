# 内存一致性

本文介绍NPU编程中的内存一致性问题，说明不同执行单元、不同AI Core、不同Stream以及Host侧和Device侧之间如何建立可预期的读写顺序。本文重点讨论“操作顺序和可见性”，缓存副本写回、失效等问题请参考[缓存一致性](cache_coherence.md)。

## 内存一致性基本概念

### 什么是内存一致性

内存一致性关注的是多个访问者对同一片内存的读写顺序是否符合程序预期。典型内存一致性例子是：生产者先写数据再写标志，消费者看到标志后，是否一定能读取到生产者写入的数据：

```text
初始状态：
GM[data] = 0
GM[flag] = 0

时间点    生产者                                  消费者
======    ====================================    ====================================
T0        STORE data = 1
T1        STORE flag = 1
                                                T2  LOAD flag -> 1
                                                T3  LOAD data -> ?
```

用户期望内存序和代码顺序保持一致，在flag赋值之前，data已经赋值完成，期望消费者在`T2`看到`flag = 1`后，`T3`一定读取到`data = 1`。但是在NPU上，生产者和消费者可能运行在不同流水、不同AI Core、不同Stream，甚至位于Host侧和Device侧之间。不同执行域之间通常是乱序并行的，不会自动建立全局顺序，用户需要根据访问范围插入同步、MEMBAR或通信机制，自行管理内存一致性。

### 缓存一致性与内存一致性的区别

缓存一致性和内存一致性是两类不同问题：

| 类型 | 关注点 | 典型问题 | 常用处理方式 |
| --- | --- | --- | --- |
| 缓存一致性 | Cache副本和下一级存储中的值是否一致。 | DCache中保留旧值，或Dirty副本尚未写回GM。 | DCCI |
| 内存一致性 | 多个读写操作之间的先后关系是否符合预期。 | 消费者看到`flag = 1`后，是否一定能读取到生产者写入的`data`。 | 同步、DSB等 |

缓存一致性是“副本与最新值是否一致”，内存一致性是“指令实际执行顺序是否和代码顺序一致”。实际编程时，两类问题经常同时出现，例如生产者写入数据后，通过`flag`发布同步标志，消费者等待`flag`后再读取数据：

```text
初始状态：
GM[data] = 0
GM[flag] = 0
消费者DCache[data] = 0

时间点    生产者                                  消费者
======    ====================================    ====================================
T0        Scalar.STORE data = 1
T1        DCCI data
T2        Scalar.DSB
          // 保证data写回GM完成

T3        STORE flag = 1
          // 发布标志，flag本身就是同步通知

                                                T4  WAIT flag == 1
                                                    // 等待flag变为1

                                                T5  DCCI data
                                                T6  Scalar.DSB
                                                    // 处理消费者侧存在的旧DCache副本

                                                T7  Scalar.LOAD data -> 1
```

上例中，`flag`是同步标志。生产者写`flag = 1`表示发布通知，消费者等待`flag == 1`表示接收通知。DCCI用于维护`data`对应的Cache副本，属于缓存一致性处理；`Scalar.DSB`用于等待写回或失效相关操作完成；`STORE flag`和`WAIT flag`用于建立生产者和消费者之间的执行先后关系，属于内存一致性处理。

- 只做同步但不处理DCache副本，可能仍读到旧值；
- 只做DCCI但不建立执行顺序，消费者也可能过早读取。

缓存一致性或内存一致性任意一类问题没有处理好，都可能导致最终读取到的数据不符合预期。

## NPU内存一致性模型

### NPU内存序

#### NPU硬件执行层级与一致性边界

NPU硬件架构中有多个层级，它们具有异步并行的特征：
- Host侧通过Stream向Device侧异步下发任务；
- Device侧的一个Kernel可以由多个AI Core并行执行；
- 每个AI Core内部又存在多条可以异步并行执行的流水。

这些硬件层级决定了NPU编程中的内存一致性维护通常需要按访问范围分层处理：

- 同一AI Core内，不同流水访问同一地址或通过UB/GM传递数据时，需要核内同步。
- 不同AI Core之间，通过GM共享数据或传递标志时，需要核间同步。
- Host侧和Device侧之间，由于Kernel执行、异步拷贝和Stream任务下发都是异步的，需要Host-Device同步。

#### 流水间一致性

AI Core内部存在Scalar流水、MTE流水、Fixpipe流水<!-- npu="950" id1 -->和SIMT流水<!-- end id1 -->，它们负责不同类型的计算或访存任务，可以通过各自的数据通路访问GM。

不同流水之间默认可以并发执行，导致代码中的书写顺序不一定等于不同流水观察到的内存访问顺序。如果没有建立必要的同步关系，不同流水上的读写操作可能按照硬件调度和资源状态交错执行，导致RAW、WAW、WAR等场景下出现内存一致性问题。

因此，跨流水存在数据依赖时，不能只依赖代码书写顺序，需要在软件上建立先后关系。若跨流水访问同时涉及Scalar<!-- npu="950" id2 -->或SIMT<!-- end id2 -->通过Cache访问GM，还需要额外考虑DCache<!-- npu="950" id3 -->或SIMT DCache<!-- end id3 -->中的旧副本、Dirty副本等[缓存一致性](cache_coherence.md)问题。

#### 流水内一致性

##### 单流水内的一致性保证

同一流水中的指令通常按代码书写顺序进入流水，按程序顺序执行，这是分析流水内顺序的基础。

但是这不代表后序指令开始执行时，前序指令的内存访问已经完成。如果前后指令访问同一地址或重叠地址，仍可能需要单流水同步。

##### Scalar流水

Scalar负责标量控制、地址计算、标量计算、Scalar访存以及发射其他流水指令等操作。Scalar流水内的GM访问需要按GM地址、访问路径以及DCache的Cache Line粒度区分：

- **访问不同GM地址**：不同地址之间没有天然的数据依赖，无论访问是否经过DCache，硬件不保证这些访问的完成顺序或对其他观察者的可见顺序与代码顺序一致。例如先写`data`、后写`flag`时，不能仅依赖代码顺序推断其他流水、其他AI Core或Host侧一定先看到`data`、再看到`flag`。
- **访问同一GM地址**：普通Scalar通过DCache访问同一GM地址时，同地址访问结果与程序序保持一致。虽然流水内同地址访问结果与程序序保持一致，但是其他流水或其他AI Core想要观察到该写入结果时，还需要按CacheLine处理DCache中的Dirty副本或旧副本，参考[DCache与GM的缓存一致性](./cache_coherence.md#dcache与gm的缓存一致性)。
- **同一GM地址混用不同访问路径**：同一地址访问中如果混用经过DCache和不经过DCache的路径，硬件不保证最终结果一定符合代码顺序，需要由软件额外处理访问完成和Cache副本问题。例如Scalar经过DCache读写和Scalar原子操作混用，需要用户显式插入同步保证内存一致性。

##### MTE/Fixpipe流水

MTE/Fixpipe指令无论是否访问同地址，指令间内存访问的实际完成顺序或对外可见顺序可能与代码顺序不一致，硬件不保证内存一致性。如果前后指令存在重叠地址或数据依赖，需要显式等待前序指令的数据读写完成。

<!-- npu="950" id4 -->
##### SIMT流水

SIMT流水用于SIMT编程模型中的线程级执行，适合复杂控制流和不规则访存场景。SIMT线程可以访问GM，也支持线程同步、MEMBAR和原子操作。

SIMT流水的顺序保证需要区分线程间和线程内：

- 线程间：不论访问是否为同一地址，SIMT的不同线程之间，内存访问的实际完成顺序或对外可见顺序可能与代码顺序不一致，硬件不保证线程间内存一致性。
- 线程内：SIMT的同一线程内访问不同地址时，硬件不保证不同地址访问顺序与代码顺序一致；SIMT的同一线程内访问同一地址时，无论是否经过SIMT DCache，硬件保证执行结果与代码顺序一致。
<!-- end id4 -->

### NPU内存一致性与标准一致性模型的关系

从经典并行体系结构的一致性模型看，NPU不提供全局顺序一致性，也不提供类似全存储排序的强内存序。

| 经典并行体系结构的一致性模型 | 模型特征 | NPU是否支持及NPU上的特征 |
| --- | --- | --- |
| 顺序一致性（Sequential Consistency，SC） | 所有处理单元观察到的内存访问结果，好像来自一个全局唯一顺序，且每个处理单元内部顺序与程序顺序一致。 | 不支持。NPU默认没有全局唯一读写顺序，不能假设所有流水、AI Core、Stream和Host侧按同一顺序观察到所有写入。 |
| 全存储排序（Total Store Order，TSO） | 写操作通常按全局存储顺序对外可见，模型强于弱一致性，弱于严格顺序一致性。 | 不支持。NPU不保证普通写操作按全局存储顺序对所有观察者可见，写入完成和对外可见需要结合DSB、同步或MEMBAR处理。 |
| 弱一致性（Weak Consistency） | 普通读写可以重排或延迟可见，只有同步、栅栏等特殊操作前后才建立明确顺序。 | 类似。NPU普通内存访问默认不建立跨执行域顺序，需要通过核内同步、核间同步、Stream/Event同步、Host-Device同步、DSB等机制显式约束访问完成顺序和可见性。 |
| Release/Acquire一致性 | 生产者用Release发布数据，消费者用Acquire获取数据；Release之前的写入在Acquire之后对消费者可见。 | 支持类似语义。NPU上通常通过[asc_sync_notify](../../../../api/SIMD-API/c_api/sync/asc_sync_notify.md)和[asc_sync_wait](../../../../api/SIMD-API/c_api/sync/asc_sync_wait.md)的组合建立Release/Acquire关系。 |

NPU更接近弱一致性模型：普通内存访问默认只具备有限的局部顺序，跨流水、跨核、跨Stream以及Host-Device之间的顺序需要用户显式建立。


## 核内内存一致性

### 核内一致性问题

本节讨论同一AI Core内，不同流水<!-- npu="950" id5 -->或SIMT线程<!-- end id5 -->之间如何建立可预期的访问顺序。这里的“核内”指生产者和消费者都在同一个AI Core内。

核内一致性问题通常来自两类场景：

- 不同流水之间通过同一块Buffer传递数据，例如MTE2把GM数据搬入UB后，Vector流水读取UB进行计算。
- 同一流水<!-- npu="950" id6 -->或SIMT线程<!-- end id6 -->前后访问同一地址、重叠地址，后序访问依赖前序访问真正完成。

因此，处理核内一致性时，首先要判断前序访问和后序访问分别位于哪个执行单元，再判断二者之间是RAW、WAW还是WAR依赖。若访问路径涉及DCache<!-- npu="950" id7 -->或SIMT DCache<!-- end id7 -->，还需要同时参考[缓存一致性](cache_coherence.md)处理Cache副本问题。

| 依赖类型 | 含义 | 示例 |
| --- | --- | --- |
| RAW | 先写后读，读操作必须读取前序写入的数据。 | MTE2先写UB，Vector后读UB。 |
| WAW | 先写后写，最终结果必须以后一次写为准。 | 两次MTE2搬运写入重叠UB地址。 |
| WAR | 先读后写，写操作不能覆盖前序读还未使用完的数据。 | Vector还在读UB，MTE2准备复用同一UB空间。 |


### 跨流水内存一致性

跨流水依赖指前序访问和后序访问不在同一条流水上。不同流水默认可以并行执行，代码顺序中的前序搬运或计算，不一定已经在后序流水访问数据前完成。

跨流水同步的核心是建立“生产流水完成后，消费流水再访问”的先后关系。本文示例中使用`WAIT`表示抽象同步点，实际编码时可根据场景选择[asc_sync_notify](../../../../api/SIMD-API/c_api/sync/asc_sync_notify.md)/[asc_sync_wait](../../../../api/SIMD-API/c_api/sync/asc_sync_wait.md)、[asc_lock](../../../../api/SIMD-API/c_api/sync/asc_lock.md)/[asc_unlock](../../../../api/SIMD-API/c_api/sync/asc_unlock.md)等核内多流水同步原语，接口选择请参考[核内同步原语](#核内同步原语)。

以MTE2负责把数据从GM搬入UB，Vector流水负责读取UB并计算，MTE3负责把结果从UB搬出到GM为示例，展示因为跨流水导致的内存一致性问题：

#### 正向依赖

正向依赖指本次循环内部的生产者和消费者依赖，`MTE2 -> V -> MTE3`流程如下：

```text
时间点    MTE2流水                    V流水                       MTE3流水
======    ========================    ========================    ========================
T0        MTE2.STORE_UB
          // 将GM数据搬入UB

T1                                    WAIT MTE2
                                      // 等待MTE2写UB完成

T2                                    V.LOAD_UB
                                      V.STORE_UB
                                      // 读取UB输入并写入UB结果

T3                                                                WAIT V
                                                                  // 等待Vector写UB完成

T4                                                                MTE3.LOAD_UB
                                                                  MTE3.STORE_GM
                                                                  // 读取UB结果并搬出到GM
```

上面的抽象流程对应两个同步点：

- `MTE2 -> V`：MTE2写UB完成后，Vector才能读取UB。通常通过`asc_sync_notify(PIPE_MTE2, PIPE_V, event_id)`和`asc_sync_wait(PIPE_MTE2, PIPE_V, event_id)`建立顺序。
- `V -> MTE3`：Vector写UB完成后，MTE3才能读取UB并搬出到GM。通常通过`asc_sync_notify(PIPE_V, PIPE_MTE3, event_id)`和`asc_sync_wait(PIPE_V, PIPE_MTE3, event_id)`建立顺序。

如果省略第一个同步，Vector可能读取尚未搬入完成的UB数据；如果省略第二个同步，MTE3可能把尚未计算完成的结果搬出到GM。

#### 反向依赖

反向依赖出现在多次循环复用Buffer的场景中。正向依赖约束的是“本轮后序流水等待本轮前序流水”，例如本轮Vector等待本轮MTE2搬入完成；反向依赖约束的是“后续循环重新写入某块Buffer前，必须等待前序循环中最后一个使用该Buffer的流水完成访问”。

无论使用单Buffer还是[Double Buffer](../../../technical_appendix/concepts_and_terms/performance_optimization/double_buffer.md)，只要后续循环会重新写入某块Buffer，就需要同步保证一致性。单Buffer和Double Buffer区别在于同一块Buffer被复用的间隔不同：

| 场景 | Buffer复用关系 | 需要等待的位置 | 说明 |
| --- | --- | --- | --- |
| 单Buffer | Loop 1复用Loop 0的`buf`。 | Loop 1写`buf`前，等待Loop 0最后一个使用`buf`的流水完成。 | 相邻循环之间存在反向依赖。 |
| Double Buffer | Loop 0使用`buffer1`，Loop 1使用`buffer2`，Loop 2重新使用`buffer1`。 | Loop 2写`buffer1`前，等待Loop 0最后一个使用`buffer1`的流水完成。 | 反向依赖仍存在，只是同一Buffer的复用间隔变长。 |

如果不处理反向依赖，后续循环的写入可能覆盖前序循环仍在读取或搬出的数据，导致内存一致性问题。下面分别用单Buffer和Double Buffer说明。

##### 单Buffer示例

单Buffer场景中，相邻循环复用同一块Buffer。Loop 1开始向`buf`搬入前，必须等待Loop 0的MTE3完成搬出，才能保证Loop 0的结果正确写回GM。

```text
时间点    Loop 0                                  Loop 1
======    ====================================    ====================================
T0        MTE2.STORE_UB(buf)
          // 搬入本轮输入

T1        V.LOAD_UB(buf)
          V.STORE_UB(buf)
          // 读取本轮输入并写入本轮结果

T2        MTE3.LOAD_UB(buf)
          MTE3.STORE_GM
          // 搬出本轮结果

T3                                                WAIT MTE3
                                                  MTE2.STORE_UB(buf)
                                                  // 上一轮搬出完成后，下一轮才能复用buf搬入
```

上例中的`WAIT MTE3`就是反向同步点。它不是本轮`MTE2 -> V -> MTE3`的正向依赖，而是用于保证Loop 1复用`buf`前，Loop 0对`buf`的使用已经结束。

##### Double Buffer示例

```text
时间点    Loop 0                                  Loop 1                                  Loop 2
======    ====================================    ====================================    ====================================
T0        MTE2.STORE_UB(buffer1)
          // 搬入Loop 0输入

T1        WAIT MTE2
          V.LOAD_UB(buffer1)
          V.STORE_UB(buffer1)                    MTE2.STORE_UB(buffer2)
          // Loop 0计算                         // 搬入Loop 1输入，与Loop 0计算并行

T2        WAIT V
          MTE3.LOAD_UB(buffer1)
          MTE3.STORE_GM                          WAIT MTE2
          // 搬出Loop 0结果                     V.LOAD_UB(buffer2)
                                                  V.STORE_UB(buffer2)
                                                  // Loop 1计算

T3                                                WAIT V
                                                  MTE3.LOAD_UB(buffer2)
                                                  MTE3.STORE_GM                          WAIT MTE3
                                                  // 搬出Loop 1结果                     // 等待Loop 0已经完成buffer1搬出

T4                                                                                        MTE2.STORE_UB(buffer1)
                                                                                          // 重新复用buffer1搬入Loop 2输入
```

上例中的`WAIT MTE3`是Double Buffer场景下的反向同步点。它不是为了建立Loop 2内部`MTE2 -> V -> MTE3`的正向依赖，而是为了保证`buffer1`在被Loop 2重新写入前，Loop 0对`buffer1`的使用已经结束。

反向依赖时，无论是单Buffer还是Double Buffer，维护内存一致性的原则不变：谁先使用Buffer，谁在使用完成后通知；谁后复用Buffer，谁在复用前等待。

如果使用`mutex_id`处理同步，单Buffer场景下所有循环复用同一个`mutex_id`；Double Buffer场景下`buffer1`和`buffer2`分别使用不同`mutex_id`。这样后续循环重新复用某个`Buffer`时，会等待该`Buffer`上一轮最后一个使用者释放对应`mutex_id`。具体示例请参考[asc_lock](../../../../api/SIMD-API/c_api/sync/asc_lock.md)/[asc_unlock](../../../../api/SIMD-API/c_api/sync/asc_unlock.md)。

### 同流水内存一致性

在MTE、Fixpipe、Vector、Cube流水中，同一流水中的指令可以按“顺序发射、异步执行”理解：后序指令会按代码顺序进入同一流水，但前序指令涉及的数据读写可能尚未全部完成。如果前后指令访问同一地址或重叠地址，就可能产生RAW、WAW或WAR问题。

| 观察维度 | 含义 | 对用户的影响 |
| --- | --- | --- |
| 代码顺序/发射顺序 | 前序指令先于后序指令进入同一流水。 | 只能说明提交先后关系，不等于前序数据读写已经完成。 |
| 数据读写完成顺序 | 前序指令的数据读写可能在后序指令开始时仍未完成。 | 后序指令访问同一地址或重叠地址时，可能读到未就绪数据或覆盖仍在使用的数据。 |
| 软件同步点 | 在后序相关访问前显式等待前序流水完成。 | 将“前序访问完成”约束到“后序访问开始”之前，保证RAW、WAW、WAR场景下的访问顺序。 |

例如，同一条MTE2流水连续向UB中存在重叠的地址区域搬入数据，期望重叠区域以后一次搬入结果为准。如果不插入同流水同步，可能出现如下问题：

```text
时间点    MTE2流水
======    ====================================
T0        MTE2.STORE_UB(region0)
          // 第一次搬入，写入UB区域region0

T1        MTE2.STORE_UB(region1)
          // 第二次搬入，region1与region0存在重叠

T2        region0和region1的重叠区域结果不确定
          // 前一次搬入的数据读写尚未全部完成，后一次搬入已经开始
          // 重叠区域不一定以后一次搬入结果为准
```

为了保证重叠区域以后一次搬入结果为准，后一次写入开始前，需要等待前一次MTE2写UB完成：

```text
时间点    MTE2流水
======    ====================================
T0        MTE2.STORE_UB(region0)
          // 第一次搬入，写入UB区域region0

T1        WAIT MTE2
          // 等待前序MTE2写UB完成

T2        MTE2.STORE_UB(region1)
          // 第二次搬入，region1与region0存在重叠
```

这里的`WAIT MTE2`是软件同步点，由用户插入，用于保证后一次写入发生在前一次写入完成之后。


### 核内同步原语

核内同步原语用于在同一AI Core内部建立可预期的执行先后关系。按同步对象可以分为单流水同步和多流水同步。

#### 单流水同步

单流水同步用于等待某一条流水中已经提交的前序操作完成，再允许后序相关操作继续执行。

| 接口 | 使用场景 | 简要说明 |
| --- | --- | --- |
| [asc_sync_pipe](../../../../api/SIMD-API/c_api/sync/asc_sync_pipe.md) | 同一流水内部前后操作存在同地址、重叠地址或数据依赖。 | 等待指定流水中前序指令的数据读写全部完成后，该流水后序指令才能开始执行；不支持`PIPE_S`，可传入`PIPE_ALL`等待所有流水，但会扩大同步范围。 |
| [asc_sync_data_barrier](../../../../api/SIMD-API/c_api/sync/asc_sync_data_barrier.md) | Scalar发起的GM或UB访问需要等待完成。 | 等待Scalar侧前序内存访问完成；`Scalar.DSB`等待GM访问，`DSB_UB`等待UB访问，`DSB_ALL`等待GM和UB访问。 |

使用时建议优先选择最小同步范围。例如只需要等待MTE2前序搬运完成时，使用`asc_sync_pipe(PIPE_MTE2)`；只有在确实无法精确判断依赖流水时，才使用`PIPE_ALL`或更大范围同步。

Scalar流水不通过`asc_sync_pipe(PIPE_S)`处理，Scalar确保上一个指令访存完成使用`asc_sync_data_barrier`。

#### 多流水同步

多流水同步用于在不同流水之间建立生产者到消费者的先后关系，例如`MTE2 -> V`、`V -> MTE3`或`S -> MTE2`。

| 接口 | 使用场景 | 简要说明 |
| --- | --- | --- |
| [asc_sync_notify](../../../../api/SIMD-API/c_api/sync/asc_sync_notify.md)/[asc_sync_wait](../../../../api/SIMD-API/c_api/sync/asc_sync_wait.md) | 两条流水之间存在明确依赖。 | `notify`在源流水设置同步标志，`wait`在目标流水等待该标志；通过源流水、目标流水和`event_id`配对。 |
| [asc_lock](../../../../api/SIMD-API/c_api/sync/asc_lock.md)/[asc_unlock](../../../../api/SIMD-API/c_api/sync/asc_unlock.md) | 多流水之间需要按Buffer生命周期或mutex_id管理依赖。 | 使用mutex机制阻塞或释放指定流水，适合复杂流水编排、反向依赖和Double Buffer场景；必须成对使用，避免嵌套相同`mutex_id`。该机制为[NPU架构版本3510](../../language_extension/simd_builtin_keywords.md)新增。 |
| [asc_sync](../../../../api/SIMD-API/c_api/sync/asc_sync.md) | 需要等待所有流水前序操作完成。 | 等待所有流水线操作完成，范围较大，会影响性能；若只依赖单条或少数流水，优先使用更精确的同步接口。|
| [asc_sync_mte2](../../../../api/SIMD-API/c_api/sync/asc_sync_mte2.md)/[asc_sync_mte3](../../../../api/SIMD-API/c_api/sync/asc_sync_mte3.md)/[asc_sync_vec](../../../../api/SIMD-API/c_api/sync/asc_sync_vec.md) | 需要等待MTE2、MTE3或Vector流水前序操作完成。 | 面向特定流水的便捷等待接口，用于等待指定流水前序操作完成后再启动后续相关操作；这类接口不表达一对源流水和目标流水的精确配对关系。 |

选择同步原语时，建议先判断依赖关系是否只落在同一条流水内，如果是，优先使用单流水同步。如果跨流水：

- 生产者和消费者位于不同流水，且需要精确表达“源流水完成后目标流水才能继续”，优先使用`asc_sync_notify/asc_sync_wait`。
- 在NPU架构版本3510上，也可以使用`asc_lock/asc_unlock`按Buffer生命周期管理跨流水依赖。
- 若只是等待某条流水或全部流水的前序操作完成，可以使用`asc_sync_mte2`、`asc_sync_mte3`、`asc_sync_vec`或`asc_sync`。

同步只能建立执行先后关系；如果访问路径涉及DCache<!-- npu="950" id8 -->或SIMT DCache<!-- end id8 -->，还需要参考[缓存一致性](cache_coherence.md)处理Cache副本。

### event_id管理

`asc_sync_notify`和`asc_sync_wait`通过`event_id`关联。可以把每个`event_id`理解为一块数据或一个Buffer的搬运、计算、搬出状态标记：生产流水完成某块数据的前序操作后，通过该`event_id`发出通知；消费流水等待同一个`event_id`后，才能继续处理对应数据。

`event_id`数量有限，取值范围通常为0到7，其中6和7为内部保留ID，用户不要手动设置为6或7。管理错误可能导致事件状态混乱、ID冲突、未定义行为或timeout问题。使用时建议遵循以下原则：

- 一个仍在处理中的数据块或Buffer复用周期，应使用独立的`event_id`表示其状态，避免多个未完成的数据块复用同一个`event_id`。
- `asc_sync_notify`和`asc_sync_wait`必须成对使用，源流水、目标流水和`event_id`必须一致。
- 相同源流水、相同目标流水、相同`event_id`下，不应连续执行多次`asc_sync_notify`而没有对应`asc_sync_wait`消费。
- 需要动态选择同步ID时，建议通过[FetchEventID](../../../../api/SIMD-API/basic_api/resource_management/TPipe/FetchEventID.md)或[AllocEventID](../../../../api/SIMD-API/basic_api/resource_management/TPipe/AllocEventID.md)获取合法ID；如果使用`AllocEventID`占用ID，使用完成后应立即调用[ReleaseEventID](../../../../api/SIMD-API/basic_api/resource_management/TPipe/ReleaseEventID.md)释放，避免ID耗尽。

### Scalar内存一致性

Scalar内存一致性需要分两类场景讨论：一类是单个Scalar流水内部前后访问之间的顺序；另一类是Scalar和其他观察者之间的可见性，例如同核内Scalar和MTE、Fixpipe<!-- npu="950" id9 -->、SIMT<!-- end id9 -->等其他硬件单元。

#### 单Scalar流水内访问内存一致性

##### Case 1：前后访问都是DCache同地址访问

如果前后访问同一GM地址且都经过DCache，硬件会维护该地址上的访问顺序，执行结果与代码顺序保持一致。这一类场景在单Scalar流水内不需要用户额外插入同步来保证同地址访问顺序。

```text
时间点    Scalar流水
======    ====================================
T0        Scalar.STORE_GM_DCache(addr) = 1
T1        Scalar.LOAD_GM_DCache(addr) -> 1
```

需要注意，上述保证只说明当前Scalar通过DCache访问同一地址时结果符合程序序，不表示该地址所在Cache Line已经写回GM，也不表示其他流水或其他AI Core已经可见。

##### Case 2：前后访问都是DCache不同地址访问

如果前后访问的是不同GM地址，即使都经过DCache，硬件也不保证不同地址访问的完成顺序或对外可见顺序与代码顺序一致。典型问题是先写数据、后写标志：

```text
时间点    Scalar流水
======    ====================================
T0        Scalar.STORE_GM_DCache(data) = 1
T1        Scalar.STORE_GM_DCache(flag) = 1
```

如果后续逻辑要求`data`写入先于`flag`发布，需要在发布`flag`前保证`data`相关访问已经完成；若消费者需要通过GM观察`data`，还要先处理`data`所在Cache Line的写端Dirty副本：

```text
时间点    Scalar流水
======    ====================================
T0        Scalar.STORE_GM_DCache(data) = 1
T1        Scalar.DCCI(data)
T2        Scalar.DSB
          // 等待data写回GM完成

T3        Scalar.STORE_GM_DCache(flag) = 1
          // 如果flag也需要对其他单元可见，flag本身还需要按发布路径继续处理
```

##### Case 3：前后访问都是不经过DCache的访问

不经过DCache路径直接访问GM，不经过DCache，例如[asc_store_dev](../../../../api/SIMD-API/c_api/scalar_compute/scalar_store/asc_store_dev.md)、[WriteGmBypassDCache](../../../../api/SIMD-API/basic_api/scalar_compute/WriteGmBypassDCache_ISASI.md)、Scalar原子操作等接口。前后访问都不经过DCache时，无论访问同一地址还是不同地址，如果后序操作依赖前序访问已经完成，都需要用户插入DSB等待前序GM访问完成。

同地址WAW场景中，如果期望最终结果以后一次写入为准，不能只依赖代码顺序：

```text
时间点    Scalar流水
======    ====================================
T0        Scalar.STORE_GM_no_cache(addr) = 1
T1        Scalar.STORE_GM_no_cache(addr) = 2
T2        GM[addr]最终值不保证一定为2
```

正确处理方式是在后一次写入前等待前一次不经过DCache的访问写完成：

```text
时间点    Scalar流水
======    ====================================
T0        Scalar.STORE_GM_no_cache(addr) = 1
T1        Scalar.DSB
          // 等待前一次GM写完成

T2        Scalar.STORE_GM_no_cache(addr) = 2
```

不同地址发布场景中，如果期望`data`先于`flag`对外可见，也需要在写`flag`前等待`data`写完成：

```text
时间点    Scalar流水
======    ====================================
T0        Scalar.STORE_GM_no_cache(data) = 1
T1        Scalar.DSB
T2        Scalar.STORE_GM_no_cache(flag) = 1
```

##### Case 4：DCache和不经过DCache的访问混合同地址访问

同一GM地址混用DCache路径和不经过DCache路径时，硬件不保证两条路径之间自动维护顺序和Cache副本。例如前序DCache写形成Dirty副本，后序不经过DCache的写虽然直接更新GM，但后续DCache Dirty副本写回时会覆盖不经过DCache的写入：

```text
时间点    Scalar流水
======    ====================================
T0        Scalar.STORE_GM_DCache(addr) = 1
          // DCache[addr] = 1，Dirty；GM[addr]仍可能是旧值

T1        Scalar.STORE_GM_no_cache(addr) = 2
          // GM[addr] = 2

T2        DCache Dirty副本后续写回
          // GM[addr]被覆盖为1
```

正确处理方式是在不经过DCache的写之前，先把同地址所在Cache Line中已经存在的DCache Dirty副本写回GM，并等待写回完成：

```text
时间点    Scalar流水
======    ====================================
T0        Scalar.STORE_GM_DCache(addr) = 1
T1        Scalar.DCCI(addr)
T2        Scalar.DSB
          // 等待DCache Dirty副本写回GM完成

T3        Scalar.STORE_GM_no_cache(addr) = 2
```

反过来，如果不经过DCache的访问已经更新GM，而当前核DCache中已经缓存过同一地址所在Cache Line，后续要通过DCache路径读取该地址时，需要先处理DCache中的旧副本，处理方式请参考[缓存一致性](cache_coherence.md)中DCache与GM一致性的读端维护方法。如果当前核DCache中已有同一地址所在Cache Line的Dirty副本，应在不经过DCache写之前先按前一个流程执行DCCI和`Scalar.DSB`，避免旧Dirty副本覆盖不经过DCache的写入。


#### Scalar和其他硬件单元的内存一致性

当Scalar和其他硬件单元之间存在数据依赖时，需要用户自行插入同步保证内存一致性。例如Scalar先写GM，后续MTE2、Vector、Fixpipe<!-- npu="950" id10 -->或SIMT<!-- end id10 -->再读取该数据，不能只依赖代码书写顺序推断后续硬件单元一定能看到Scalar写入。

下面以Scalar写GM后，MTE2读取GM为例：

```text
时间点    Scalar流水                              MTE2流水
======    ====================================    ====================================
T0        Scalar.STORE_GM(data) = 1
T1        Scalar.DCCI
          // Scalar写GM经过DCache时添加DCCI
T2        Scalar.DSB
          // 等待Scalar写GM完成

T3        sync S->MTE2
          // 通知MTE2可以读取

                                                T3  WAIT S->MTE2
                                                T4  MTE2.LOAD_GM(data)
```

其中，`Scalar.DSB`可通过[asc_sync_data_barrier](../../../../api/SIMD-API/c_api/sync/asc_sync_data_barrier.md)实现，用于等待Scalar侧前序GM访问完成；`sync S->MTE2`可通过[asc_sync_notify](../../../../api/SIMD-API/c_api/sync/asc_sync_notify.md)/[asc_sync_wait](../../../../api/SIMD-API/c_api/sync/asc_sync_wait.md)建立Scalar流水到MTE2流水的先后关系：

`DSB`和流水同步只处理访问完成顺序和执行先后关系，保证内存一致性，不负责写回或失效DCache副本。如果Scalar写GM经过DCache，或者后续读取路径可能命中旧Cache副本，还需要参考[缓存一致性](cache_coherence.md)处理DCache相关问题。

<!-- npu="950" id11 -->
### SIMT内存一致性

SIMT内存一致性需要区分线程内和线程间：

#### 单线程内同地址访问

SIMT的同一线程内访问同一地址时，无论是否经过SIMT DCache，硬件保证执行结果与代码顺序一致。这类场景不需要用户额外插入同步来保证同线程同地址访问顺序。

```text
时间点    Thread 0
======    ====================================
T0        STORE addr = 1
T1        LOAD addr -> 1
T2        STORE addr = 2
T3        LOAD addr -> 2
```


#### 单线程内不同地址访问

SIMT的同一线程内访问不同地址时，硬件不保证不同地址访问的对外可见顺序与代码顺序一致。如果后续逻辑要求不同地址之间形成类似“先写数据、后写标志”这类发布顺序，需要用户使用内存栅栏（MEMBAR）建立可见性顺序。

错误示例：

```text
时间点    生产者Thread0                           消费者Thread1
======    ====================================    ====================================
T0        STORE data = value
T1        STORE ready = 1
                                                T2  LOAD ready -> 1
                                                T3  LOAD data -> ?
```

上例中，`ready`和`data`是同线程内操作不同地址。消费者看到`ready = 1`时，不一定代表`data`的写入已经按预期对消费者可见。生产者需要在写数据和发布标志之间插入MEMBAR；消费者仍需要通过轮询等同步方式等待标志。

正确示例：

```text
时间点    生产者Thread0                           消费者Thread1
======    ====================================    ====================================
T0        STORE data = value
T1        MEMBAR
          // 保证data写入先于ready发布对外可见

T2        STORE ready = 1
                                                T3  WAIT ready == 1
                                                T4  LOAD data -> value
```

上例中的MEMBAR可以使用[asc_threadfence](../../../../api/SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence.md)实现，适合需要对其他所有线程可见的场景；如果可见性范围只需要覆盖当前Thread Block，可以使用[asc_threadfence_block](../../../../api/SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence_block.md)。MEMBAR只建立当前线程前后内存操作的可见性顺序，不会阻塞线程。

如果消费者访问GM时使用SIMT DCache，还需要同时结合SIMT DCache与GM的[缓存一致性](cache_coherence.md)要求处理旧副本。MEMBAR负责约束访问顺序，不负责自动失效消费者侧Cache副本。


#### 线程间访问

SIMT的不同线程之间，不论访问是否为同一地址，硬件不自动保证线程间内存一致性。一个线程写入的数据，如果要被另一个线程读取，需要用户自行插入同步。

例如Thread 0写入共享数据，Thread 1随后读取。如果没有同步，Thread 1可能在Thread 0写入完成前读取：

```text
时间点    Thread 0                                Thread 1
======    ====================================    ====================================
T0        STORE shared_data = value
                                                T1  LOAD shared_data -> ?
```

如果生产者和消费者位于同一个Thread Block内，可以使用[asc_syncthreads](../../../../api/SIMT-API/sync_and_memory_fence/sync_interface/asc_syncthreads.md)建立阶段同步：

```text
时间点    Thread 0                                Thread 1
======    ====================================    ====================================
T0        STORE shared_data = value
T1        asc_syncthreads()                       asc_syncthreads()
          // 等待当前Thread Block内所有Thread到达同步点

                                                T2  LOAD shared_data -> value
```

`asc_syncthreads`会阻塞当前Thread Block内所有Thread到达同步点，并保证同步点前的内存操作对块内线程可见。

<!-- end id11 -->


## 核间内存一致性

### 核间一致性问题与多核访问GM的顺序风险

不同AI Core之间通常通过GM共享数据。硬件不会自动为多个AI Core建立全局读写顺序：一个核写入GM、另一个核读取GM时，如果没有显式同步，读写操作的实际完成顺序或对外可见顺序可能与代码期望不一致。如果访问路径经过DCache，还会叠加写端Dirty副本、读端旧副本等[缓存一致性](cache_coherence.md)问题。

核间访问GM时，常见风险可以按RAW、WAW、WAR三类依赖理解。

RAW（Read After Write）要求后续读操作读取前序写操作的新值。如果消费者没有等待生产者发布数据，可能提前读到旧值；如果读端DCache中已有旧副本，也可能在同步后仍读到旧值。

```text
时间点    Core 0                                  Core 1
======    ====================================    ====================================
T0        STORE data = 1
                                                T1  LOAD data -> 旧值或不确定值
```

WAW（Write After Write）要求最终保留在GM中的是后一次写操作的数据。如果两个核对同一地址或同一Cache Line内的数据写入顺序没有建立，最终结果可能不符合“后写覆盖前写”的期望；如果前序写经过DCache形成Dirty副本，后续写还可能被前序Dirty写回覆盖。

```text
时间点    Core 0                                  Core 1
======    ====================================    ====================================
T0        STORE data = 1
                                                T1  STORE data = 2
T2        前序写入或Dirty副本晚于Core 1写入对GM可见
结果      GM中可能不是Core 1期望保留的值
```

WAR（Write After Read）要求前序读操作先读取到后续写操作之前的旧值。如果后写核过早发布新值，前读核可能读取到新值，破坏“先读旧值、再写新值”的阶段关系。

```text
时间点    Core 0                                  Core 1
======    ====================================    ====================================
T0                                                STORE data = 1
T1        LOAD data -> 新值
结果      Core 0没有读取到后写前的旧值
```

这些场景下的核间内存一致性全部需要用户手动维护。

### 核间同步原语

核间同步原语用于在多个AI Core之间建立到达、等待或屏障关系。选择接口时，首先要确认同步范围：是所有参与核都需要对齐，还是只需要某些group之间、同一group内部的block/subblock之间建立顺序。

在分离模式下，AIC（Cube Core）与AIV（Vector Core）按group划分。一个group由1个block（主核）和N个subblock（从核）组成，比例为1:N。因此，组间同步指不同group之间的block同步，组内同步指同一个group内block和subblock之间的同步。

| 接口 | 同步范围 | 作用边界 |
| --- | --- | --- |
| [asc_sync_inter_arrive](../../../../api/SIMD-API/c_api/sync/asc_sync_inter_arrive.md)/[asc_sync_inter_wait](../../../../api/SIMD-API/c_api/sync/asc_sync_inter_wait.md) | 组间block同步，即不同group之间的block同步。 | 通过`flag_id`传递同步信号，适合group之间的生产者-消费者同步。 |
| [asc_sync_block_arrive](../../../../api/SIMD-API/c_api/sync/asc_sync_block_arrive.md)/[asc_sync_block_wait](../../../../api/SIMD-API/c_api/sync/asc_sync_block_wait.md) | 组内block和subblock同步，即同一个group内AIC和AIV之间同步。 | 通过`flag_id`传递同步信号，适合同一group内部Cube侧和Vector侧建立顺序。 |

`arrive`/`wait`类接口只传递同步信号；`flag_id`需要配对使用，并避免计数器溢出和ID冲突。使用这些接口后，仍需要根据数据访问路径判断是否需要DCCI和DSB。

以核间生产者-消费者场景为例，展示生产者负责写入GM数据并发布同步信号，消费者等待同步信号后再读取GM数据的抽象流程：

```text
时间点    Producer Core                          Consumer Core
======    ====================================   ====================================
T0        STORE data = value
T1        DCCI data
T2        Scalar.DSB
T3        ARRIVE sync_point
                                                T4  WAIT sync_point
                                                T5  DCCI data
                                                T6  Scalar.DSB
                                                T7  LOAD data -> value
```

其中，T1和T2时刻处理生产者通过DCache写入后可能形成的Dirty副本，确保数据已经写回到GM；T3和T4时刻建立核间发布/获取顺序；T5和T6处理消费者本核可能存在的旧DCache副本。若生产者和消费者的数据访问路径均不经过DCache，可以按实际路径省略对应DCCI，但仍需要用同步原语建立跨核执行顺序。

上例中的`ARRIVE sync_point`和`WAIT sync_point`表示抽象的核间同步动作。组间block同步可以使用`asc_sync_inter_arrive`/`asc_sync_inter_wait`实现；同一group内block和subblock之间的同步可以使用`asc_sync_block_arrive`/`asc_sync_block_wait`实现。

### 原子操作与内存一致性

原子操作用于保证同一GM地址上的读-改-写过程不可分割。多个AI Core同时更新同一地址时，如果使用普通写入，更新之间可能互相覆盖；使用原子操作后，每一次更新都会基于该地址的某个旧值完成计算，并将新值写回同一地址，避免数据竞争。

例如多个核同时对同一地址执行原子累加：

```text
初始值    GM[dst] = 0

时间点    Core 0                                  Core 1
======    ====================================    ====================================
T0        ATOMIC_ADD dst, 1
                                                T1  ATOMIC_ADD dst, 1

结果      GM[dst] = 2
```

上例中的`ATOMIC_ADD`可使用[asc_atomic_add](../../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_add.md)等接口实现。无论Core 0和Core 1的原子加按哪一种先后顺序生效，最终结果都是`2`，因此这个例子只说明原子操作避免了同地址并发更新丢失，并不说明原子操作能够建立用户期望的跨核顺序。

当不同原子操作之间存在顺序要求时，仍需要显式同步。例如业务要求Core 0先执行原子求最大值，Core 1再执行原子累加：

```text
初始值    GM[dst] = 0

时间点    Core 0                                  Core 1
======    ====================================    ====================================
T0        ATOMIC_MAX dst, 10
                                                T1  ATOMIC_ADD dst, 1

可能结果  ATOMIC_MAX先完成，ATOMIC_ADD后完成，GM[dst] = 11
可能结果  ATOMIC_ADD先完成，ATOMIC_MAX后完成，GM[dst] = 10
```

这两个结果都不违反原子性，因为每一次原子操作本身仍然是不可分割的；问题在于两个核之间没有建立“先求最大值、后累加”的执行顺序。如果业务要求结果固定为`11`，需要在两个阶段之间加入核间同步：

```text
初始值    GM[dst] = 0

时间点    Core 0                                  Core 1
======    ====================================    ====================================
T0        ATOMIC_MAX dst, 10
T1        ARRIVE sync_point
                                                T2  WAIT sync_point
                                                T3  ATOMIC_ADD dst, 1

结果      GM[dst] = 11
```

因此，原子操作与内存一致性的边界可以这样理解：

| 机制 | 保证内容 | 不保证内容 |
| --- | --- | --- |
| 原子操作 | 保证同一地址上的读-改-写过程不可分割。 | 不保证前后普通读写的完成顺序和对其他核的可见性。 |
| 内存一致性维护 | 保证普通数据在生产者和消费者之间按期望顺序可见。 | 不替代原子操作对同一地址并发更新的保护。 |

如果用户对程序运行结果有顺序要求，需要自行插入同步处理一致性问题。


## Host-Device内存一致性

### 异步下发语义

Host侧通过Runtime向Device侧下发Kernel、异步内存拷贝等任务。对于异步任务，Host侧接口返回通常只表示任务已经下发到Stream，不表示任务已经在Device侧执行完成。Host线程会继续向下执行，Device任务则在对应Stream中等待调度和执行。

如果Host后续代码依赖Device任务的结果，不能只依赖Host侧代码顺序，需要显式等待Device任务完成。

```text
时间点    Host线程                                Stream
======    ====================================    ====================================
T0        下发Task A
T1        Host继续执行
T2        Host读取Task A结果
          // 此时Task A不一定已经完成
                                                T3  Task A执行完成
```

同一个Stream中的任务按照进入队列的顺序执行；不同Stream之间默认可以并行执行，不会因为Host侧下发顺序自动建立依赖。如果希望Stream之间存在数据依赖，应通过Event或Host侧显式同步接口建立顺序。

### Host侧显式同步接口

Host侧显式同步用于阻塞Host线程，等待Device侧指定范围内的任务完成。常用接口如下：

| 接口 | 等待范围 | 典型场景 |
| --- | --- | --- |
| `aclrtSynchronizeStream(stream)` | 等待指定Stream中已下发任务完成。 | Host只依赖某个Stream中的任务结果。 |
| `aclrtSynchronizeDevice()` | 等待当前Device当前Context下所有Stream任务完成。 | Host确实需要等待当前Context下所有Device任务完成。 |
| `aclrtSynchronizeEvent(event)` | 等待指定Event完成。 | Host只需要等待某个精确任务阶段完成。 |

选择同步接口时，应优先使用最小等待范围。只依赖某个Stream的结果时，使用`aclrtSynchronizeStream`即可；只依赖某个任务阶段时，可以等待对应Event；只有确实需要等待当前Context下所有Stream任务完成时，才使用`aclrtSynchronizeDevice`。

### 跨Stream任务依赖

跨Stream任务之间不能直接依赖Host侧下发顺序。即使Host先向`stream0`下发生产者任务，再向`stream1`下发消费者任务，两个Stream上的任务仍可能并行调度，消费者任务可能早于生产者任务完成。

未建立依赖的示例：

```text
时间点    Host线程                                Stream 0                               Stream 1
======    ====================================    ====================================   ====================================
T0        向stream0下发Producer Task
T1        向stream1下发Consumer Task
                                                                                    T2  Consumer Task可能先执行
                                                T3  Producer Task执行完成
```

如果`stream1`中的消费者任务依赖`stream0`中的生产者任务，应使用Event建立任务级细化依赖：

```text
时间点    Stream 0                               Stream 1
======    ====================================   ====================================
T0        Producer Task
T1        RECORD event
                                               T2  WAIT event
                                               T3  Consumer Task
```

上例中的`RECORD event`可通过`aclrtRecordEvent(event, stream0)`实现，该接口会捕获`stream0`上已下发的任务；`WAIT event`可通过`aclrtStreamWaitEvent(stream1, event)`实现，该接口会在`stream1`上下发等待任务，使等待点之后的任务等待Event捕获的任务完成。`aclrtStreamWaitEvent`只约束指定Stream上的后续任务，因此适合表达跨Stream之间精确的生产者-消费者依赖。


## 常见一致性错误与调试

| 现象 | 可能根因 | 排查方向 |
| --- | --- | --- |
| 结果错误或偶发错误 | 跨流水、跨核或跨Stream缺少同步，消费者可能在生产者完成前读取数据。 | 按数据生产者和消费者反推依赖链，确认是否已在依赖边界插入流水同步、核间同步或Event。 |
| 本核读到旧值、未完成值或异常值 | 后序流水未等待前序流水完成就访问同一数据，或同流水重叠地址访问缺少完成等待。 | 检查跨流水是否使用`notify/wait`，同流水重叠访问是否使用`sync_pipe`等待前序访问完成。 |
| timeout或挂死 | 同步信号没有成对匹配、event_id或flag_id不一致，或使用保留EventId 6和7。 | 检查`notify/wait`、`arrive/wait`的源流水、目标流水、event_id或flag_id是否匹配，确认等待方对应的生产者一定会执行；避免手动使用6和7。 |
| 多核写同一GM地址结果偏小 | 多核普通写互相覆盖，或普通读改写丢更新，没有使用原子操作保护同地址并发更新。 | 确认同一GM地址是否存在多核累加、求极值等并发更新；需要同地址读-改-写不可分割时使用原子操作。 |
| Scalar读到GM旧值 | 写端DCache Dirty副本未写回，或读端DCache中存在旧副本，这是缓存一致性问题。 | 检查写端是否执行DCCI和`Scalar.DSB`，读端读取前是否需要失效旧副本；具体维护方式参考[缓存一致性](cache_coherence.md)。 |
| Host未读到就绪结果 | Host只完成任务下发，没有等待Device任务执行完成。 | 检查Host读取结果前是否调用`aclrtSynchronizeStream`、`aclrtSynchronizeDevice`或`aclrtSynchronizeEvent`等待对应任务完成。 |
| 跨Stream数据错乱 | 不同Stream之间默认没有依赖，消费者任务可能早于生产者任务执行。 | 检查生产者Stream是否记录Event，消费者Stream是否通过`aclrtStreamWaitEvent`等待该Event后再执行依赖任务。 |

## 快速决策参考

判断是否需要用户维护内存一致性时，可以先确认访问跨过了哪一类一致性边界：

| 一致性边界 | 首选机制 | 责任方 |
| --- | --- | --- |
| 同流水内 | 同流水同步，例如`asc_sync_pipe(PIPE_xxx)`；Scalar访问完成使用`asc_sync_data_barrier`。 | 用户在同地址、重叠地址或前后访问存在依赖的位置等待前序访问完成。 |
| 核内跨流水 | 流水间同步，例如`asc_sync_notify`/`asc_sync_wait`、`asc_lock`/`asc_unlock`。 | 用户在生产者流水发布同步信号，在消费者流水等待后再访问数据。 |
| Scalar访问GM | `asc_sync_data_barrier`配合必要的DCCI。 | 用户等待Scalar侧GM访问完成；若访问经过DCache，还需要按[缓存一致性](cache_coherence.md)处理Dirty副本或旧副本。 |
| 多核间 | 核间同步、原子操作、必要时配合DCCI和DSB。 | 用户建立生产者Core和消费者Core之间的顺序；多核同地址读-改-写使用原子操作，普通数据传递仍需处理可见性。 |
| 跨Stream | Event任务级依赖。 | 用户需要通过Event处理Stream间同步。 |
| Host等Device | Host侧显式同步接口，例如`aclrtSynchronizeStream`、`aclrtSynchronizeDevice`、`aclrtSynchronizeEvent`。 | 用户需要添加同步接口保证Host侧在读取Device结果或复用相关资源前等待对应Device任务完成。 |

## 总结

NPU内存一致性更接近弱一致性模型。普通内存访问默认只具备有限的局部顺序，硬件不会为跨流水、跨AI Core、跨Stream以及Host-Device之间的访问自动建立全局一致的读写顺序。

因此，用户需要根据数据依赖主动维护内存一致性：在生产者和消费者之间建立同步关系，在需要时等待前序内存访问完成，并在涉及Cache副本时配合缓存一致性维护。
