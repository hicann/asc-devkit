# 开启Double Buffer<a name="ZH-CN_TOPIC_0000001893038945"></a>

【优先级】中

【描述】执行于AI Core上的指令队列主要包括如下几类：[Vector（矢量）](../../../technical_appendix/concepts_and_terms/glossary.md#p203923211517)指令队列V、[Cube（矩阵）](../../../technical_appendix/concepts_and_terms/glossary.md#p1038918214519)指令队列M、[Scalar（标量）](../../../technical_appendix/concepts_and_terms/glossary.md#p1739112219516)指令队列S和搬运指令队列[Memory Transfer Engine 1（MTE1）](../../../technical_appendix/concepts_and_terms/glossary.md#p339019220518)/[Memory Transfer Engine 2（MTE2）](../../../technical_appendix/concepts_and_terms/glossary.md#p53901325511)/[Memory Transfer Engine 3（MTE3）](../../../technical_appendix/concepts_and_terms/glossary.md#p183901322511)。不同指令队列间的相互独立性和可并行执行特性，是Double Buffer优化机制的基石。

以纯Vector计算为例，矢量计算前后的CopyIn、CopyOut过程使用搬运指令队列（MTE2/MTE3），Compute过程使用Vector指令队列（V）。不同指令队列可并行执行，意味着CopyIn、CopyOut过程和Compute过程可以并行。如下图所示，考虑一个完整的数据搬运和计算过程，CopyIn过程将数据从[Global Memory（GM）](../../../technical_appendix/concepts_and_terms/glossary.md#p1439018225112)搬运到[Local Memory（LM）](../../../technical_appendix/concepts_and_terms/glossary.md#p193901420518)，Vector计算单元完成Compute计算后，经过CopyOut过程将计算结果搬回GM。

**图1** 数据搬运与Vector计算过程<a name="fig_data_vector_compute"></a>  
![](../../../figures/data_vec_54.png "数据搬运与Vector计算过程")

**图2** 未开启Double Buffer的流水图<a name="fig_disable_double_buffer_pipeline"></a>  
![](../../../figures/no_doublebuf_pipe.png "未开启Double Buffer的流水图")

在此过程中，数据搬运与Vector计算串行执行，Vector计算单元不可避免地存在资源闲置问题。假设CopyIn、Compute、CopyOut三个阶段的耗时均为$t$，则Vector的利用率仅为三分之一，等待时间过长，Vector利用率严重不足。

为减少Vector等待时间，Double Buffer为待处理数据准备Ping和Pong两组缓冲区。如下表所示，t0时，MTE2将数据块0的输入搬入Ping；t1时，MTE2将数据块1的输入搬入Pong，Vector同时使用Ping计算数据块0；t2时，MTE2将数据块2的输入搬入Ping，Vector使用Pong计算数据块1，MTE3同时从Ping搬出数据块0的结果。由此，数据搬入、Vector计算和数据搬出可以跨数据块并行执行，缓解Vector计算单元闲置问题。

**表1** Ping/Pong Double Buffer的三个时间点  
| 执行阶段 | t0 | t1 | t2 |
| --- | --- | --- | --- |
| MTE2搬入 | 数据块0输入到Ping输入区 | 数据块1输入到Pong输入区 | 数据块2输入到Ping输入区 |
| Vector计算 | 无 | 计算数据块0（Ping） | 计算数据块1（Pong） |
| MTE3搬出 | 无 | 无 | 数据块0结果从Ping输出区搬出 |

t2时，Ping中的输入区和输出区相互独立，因此数据块2搬入和数据块0搬出可以同时执行。

总体来说，Double Buffer是基于MTE指令队列与Vector指令队列的独立性和可并行性，通过将数据搬运与Vector计算并行执行以隐藏大部分的数据搬运时间，并降低Vector指令的等待时间，最终提高Vector单元的利用效率。开启Double Buffer后的整体流水如下图所示。

**图3** 开启Double Buffer的流水图<a name="fig_enable_double_buffer_pipeline"></a>  
![](../../../figures/dbuf_pipe.png "开启Double Buffer的流水图")

## 使用说明

多数情况下，采用Double Buffer能有效提升Vector的利用率，缩减算子执行时间。然而，Double Buffer机制缓解Vector闲置问题，并不代表它总能带来明显的整体性能提升。例如：

- 当数据搬运时间较短，而Vector计算时间较长时，由于数据搬运在整个计算过程中的时间占比较低，Double Buffer机制带来的性能收益会偏小。
- 当原始数据较小且Vector可以一次性完成所有数据量的计算时，无法形成跨数据块的流水重叠，额外的缓冲区和同步处理可能使效果适得其反。

使用Double Buffer还需要满足以下条件：

- 单核数据能够切分为至少两个数据块，使相邻数据块可以交替使用两组缓冲区。
- [Unified Buffer（UB）](../../../technical_appendix/concepts_and_terms/glossary.md#p17392122195118)能够容纳两组输入和输出缓冲区以及算子所需的其他临时数据。
- 同一缓冲区的写入、读取和再次复用顺序能够通过相应编程方式的同步机制正确管理。
- 缓冲区地址和数据搬运长度满足目标产品及搬运接口的对齐要求，尾块按照实际数据长度进行处理。

因此，Double Buffer的使用需综合考虑Vector算力、数据量、UB容量、数据块数量以及搬运与计算时间占比等因素。

## 不同编程方式下的实现

本文分别介绍基于`TPipe/TQue`的编程方式和基于指针的C语言编程方式。静态Tensor编程方式同样可以使用Double Buffer，但需要手动划分Ping/Pong缓冲区并管理同步事件，其实现和约束请参见[静态Tensor编程的流水优化](../../../programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md#section121239188376)，本文不再单独展开。

### 基于TPipe/TQue的编程方式

采用`TPipe/TQue`管理资源时，通过在算子类的`Init`函数中调用`InitBuffer`初始化队列，并设置内存块数量，控制是否开启Double Buffer。`num`参数设置为1时，每个队列分配一个内存块，不开启Double Buffer；设置为2时，每个队列分配两个内存块，开启Double Buffer。

【反例】

`InitBuffer`的`num`参数设置为1，输入和输出队列均只分配一个内存块，数据搬入、计算和数据搬出不能跨数据块重叠。

```cpp
__aicore__ inline void Init(__gm__ uint8_t* src0Gm, __gm__ uint8_t* src1Gm, __gm__ uint8_t* dstGm)
{
    src0Global.SetGlobalBuffer((__gm__ half*)src0Gm);
    src1Global.SetGlobalBuffer((__gm__ half*)src1Gm);
    dstGlobal.SetGlobalBuffer((__gm__ half*)dstGm);
    // 不开启Double Buffer，占用的物理空间是1 * sizeSrc0 * sizeof(half)。
    // 三个InitBuffer执行后总空间为：
    // 1 * (sizeSrc0 * sizeof(half) + sizeSrc1 * sizeof(half) + sizeDst0 * sizeof(half))。
    pipe.InitBuffer(inQueueSrc0, 1, sizeSrc0 * sizeof(half));
    pipe.InitBuffer(inQueueSrc1, 1, sizeSrc1 * sizeof(half));
    pipe.InitBuffer(outQueueDst, 1, sizeDst0 * sizeof(half));
}

__aicore__ inline void Process()
{
    // 需要round * 2次循环才能处理完数据。
    for (uint32_t index = 0; index < round * 2; ++index) {
        CopyIn(index);
        Compute();
        CopyOut(index);
    }
}
```

【正例】

在`Init`函数初始化输入和输出队列时，将`InitBuffer`的`num`参数设置为2。`Process`函数按照数据块顺序调用`CopyIn`、`Compute`和`CopyOut`，队列根据内存块的入队、出队和释放顺序管理缓冲区复用及流水同步。

```cpp
__aicore__ inline void Init(__gm__ uint8_t* src0Gm, __gm__ uint8_t* src1Gm, __gm__ uint8_t* dstGm)
{
    src0Global.SetGlobalBuffer((__gm__ half*)src0Gm);
    src1Global.SetGlobalBuffer((__gm__ half*)src1Gm);
    dstGlobal.SetGlobalBuffer((__gm__ half*)dstGm);
    // InitBuffer中使用2表示开启Double Buffer，占用的物理空间是2 * sizeSrc0 * sizeof(half)。
    // 三个InitBuffer执行后总空间为：
    // 2 * (sizeSrc0 * sizeof(half) + sizeSrc1 * sizeof(half) + sizeDst0 * sizeof(half))。
    pipe.InitBuffer(inQueueSrc0, 2, sizeSrc0 * sizeof(half));
    pipe.InitBuffer(inQueueSrc1, 2, sizeSrc1 * sizeof(half));
    pipe.InitBuffer(outQueueDst, 2, sizeDst0 * sizeof(half));
}

__aicore__ inline void Process()
{
    // 开启Double Buffer的前提是循环次数大于等于2。
    for (uint32_t index = 0; index < round; ++index) {
        CopyIn(index);
        Compute();
        CopyOut(index);
    }
}
```

采用该方式时，需要注意以下事项：

- `InitBuffer`应在算子类的`Init`函数中调用，三个参数依次表示队列、内存块数量和单个内存块的字节数。
- 开启Double Buffer后，输入和输出队列占用的UB空间增加为原来的两倍，需要重新计算单个数据块的长度和循环次数。
- `Process`中的循环次数取决于Tiling切分结果。上述`round`仅用于展示开启前后的代码位置，不能作为所有算子的固定计算方式。
- 如果尾块不能满足主块的Double Buffer切分条件，应单独处理尾块。

完整的Tiling、队列初始化和主尾块处理方法请参见[Double Buffer场景](../../simd_operator_impl/vector_programming/double_buffer_scenario.md)。

### 基于指针的C语言编程方式

采用基于指针的C语言编程方式时，Double Buffer不是单一开关，需要在Kernel中手动申请Ping和Pong两组UB缓冲区，根据数据块序号交替选择缓冲区，并使用[asc_lock](../../../../api/SIMD-API/c_api/sync/asc_lock.md)和[asc_unlock](../../../../api/SIMD-API/c_api/sync/asc_unlock.md)接口约束同一缓冲区的读取、写入和再次复用。

下面的代码省略了核间切分、搬运参数配置和Host侧逻辑，只保留Double Buffer所在的Kernel及其关键逻辑。完整的单缓冲区基线和Double Buffer实现请参见[融合向量计算Double Buffer样例](../../../../../../examples/02_simd_c_api/02_features/01_reg_vector_compute/00_add_double_buffer/README.md)。`BLOCK_LENGTH`表示单个数据块的最大元素个数，`currentLength`表示当前数据块的实际元素个数。

```cpp
__global__ __vector__ void add_double_buffer(__gm__ half* xGm, __gm__ half* yGm, __gm__ half* zGm,
    uint32_t totalElements)
{
    asc_init();

    // 每组包含x、y、z三个Buffer，两组分别为Ping和Pong。
    __ubuf__ half ubBuffer[BLOCK_LENGTH * 6];
    __ubuf__ half* xPing = ubBuffer;
    __ubuf__ half* yPing = xPing + BLOCK_LENGTH;
    __ubuf__ half* zPing = yPing + BLOCK_LENGTH;
    __ubuf__ half* xPong = zPing + BLOCK_LENGTH;
    __ubuf__ half* yPong = xPong + BLOCK_LENGTH;
    __ubuf__ half* zPong = yPong + BLOCK_LENGTH;

    uint32_t blockCount = (totalElements + BLOCK_LENGTH - 1) / BLOCK_LENGTH;
    for (uint32_t blockIdx = 0; blockIdx < blockCount; ++blockIdx) {
        uint32_t offset = blockIdx * BLOCK_LENGTH;
        uint32_t currentLength = (totalElements - offset) > BLOCK_LENGTH ?
            BLOCK_LENGTH : (totalElements - offset);

        // 偶数块使用Ping，奇数块使用Pong。
        uint8_t inputMutexId = static_cast<uint8_t>(blockIdx & 1);
        uint8_t outputMutexId = static_cast<uint8_t>(inputMutexId + 2);
        __ubuf__ half* xLocal = (blockIdx & 1) == 0 ? xPing : xPong;
        __ubuf__ half* yLocal = (blockIdx & 1) == 0 ? yPing : yPong;
        __ubuf__ half* zLocal = (blockIdx & 1) == 0 ? zPing : zPong;

        // MTE2搬入当前块输入，输入Mutex保护同组x/y的再次写入。
        asc_lock(PIPE_MTE2, inputMutexId);
        asc_copy_gm2ub_align(xLocal, xGm + offset, currentLength * sizeof(half));
        asc_copy_gm2ub_align(yLocal, yGm + offset, currentLength * sizeof(half));
        asc_unlock(PIPE_MTE2, inputMutexId);

        // Vector等待当前x/y就绪，并等待同组z可写。
        asc_lock(PIPE_V, inputMutexId);
        asc_lock(PIPE_V, outputMutexId);
        asc_add(zLocal, xLocal, yLocal, currentLength);
        asc_unlock(PIPE_V, inputMutexId);
        asc_unlock(PIPE_V, outputMutexId);

        // MTE3等待当前z计算完成后搬出。
        asc_lock(PIPE_MTE3, outputMutexId);
        asc_copy_ub2gm_align(zGm + offset, zLocal, currentLength * sizeof(half));
        asc_unlock(PIPE_MTE3, outputMutexId);
    }

    // Kernel退出前排空已提交但尚未完成的异步操作。
    asc_sync_pipe(PIPE_ALL);
}
```

`blockIdx`为0、1、2时，代码分别选择Ping、Pong、Ping。循环按照代码顺序提交数据搬入、计算和数据搬出操作，不同流水在满足同步依赖后异步执行，从而使相邻数据块形成重叠。

每组输入和输出缓冲区需要满足以下同步关系：

| UB区域 | 生产者和消费者 | 需要保证的顺序 | 同步方式 |
| --- | --- | --- | --- |
| Ping/Pong `x/y` | MTE2写入 -> Vector读取 | 同组输入搬入完成后才能计算；下一次写入同组输入前，前一次读取必须结束。 | 使用该组的输入`mutex_id`。 |
| Ping/Pong `z` | Vector写入 -> MTE3读取 | 计算完成后才能搬出结果。 | 使用该组的输出`mutex_id`。 |
| Ping/Pong `z` | MTE3读取 -> 下一次Vector写入 | 前一个数据块的结果搬出完成后，才能复用同组输出缓冲区。 | 下一次Vector获取同一输出`mutex_id`时等待。 |

MTE2向Ping的`x/y`搬入新数据时，MTE3可以同时从Ping的`z`搬出上一个数据块的结果，因为`x/y/z`是同一组中彼此独立的子缓冲区。再次写入同组`x/y`前必须等待前一次Vector读取结束，再次写入同组`z`前必须等待前一次MTE3搬出结束。

采用该方式时，需要注意以下事项：

- 需要根据可用UB容量静态确定Ping/Pong缓冲区的大小、偏移和对齐方式。
- 尾块需要通过`currentLength`按照实际长度搬运和计算。
- 注意`asc_lock/asc_unlock`接口的产品支持情况；若在其他产品上开启Double Buffer，应根据同步接口支持情况，使用`asc_sync_notify/asc_sync_wait`等接口建立等价的数据依赖，不能直接照搬本节代码。
- Kernel退出前需要等待已提交的异步操作完成。示例使用`asc_sync_pipe(PIPE_ALL)`排空流水。
