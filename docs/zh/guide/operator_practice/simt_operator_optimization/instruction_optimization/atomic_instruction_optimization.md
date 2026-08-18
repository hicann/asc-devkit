# 原子操作指令优化

【优先级】高

【描述】SIMT编程模式下，多个线程更新同一个内存地址时需要使用[原子操作接口](../../../../api/SIMT-API/atomic_operations/atomic_operations_intro.md)保证更新结果正确。原子操作属于高开销指令，其耗时由两方面决定：一是同一地址上的原子操作只能排队依次执行，二是单条原子指令的处理开销。


原子操作需要完整完成“读出旧值—计算新值—写回新值”三个步骤。若多个线程同时更新同一地址且缺乏保护，可能出现多个线程读取到相同旧值、后写回的结果覆盖先写回的结果，导致部分更新丢失。原子操作为避免此类丢失，保证三个步骤作为一个不可分割的整体完成，因此同一地址上的多个原子操作无法并行执行，只能排队依次执行。同一地址上排队的原子操作越多，总耗时越高。

因此原子操作的优化围绕“减少同一地址上排队的原子操作次数”与“降低单条原子指令的处理开销”两点展开，具体可从以下方面进行优化：

- **通过分层归约将原子操作从GM迁移至Unified Buffer（UB）**：Global Memory（GM）位于AI Core外部，访问路径较长、延迟较高；UB位于AI Core内部，访问路径较短、延迟较低。且UB为线程块私有，不同线程块的UB原子操作互不竞争。先在各线程块的UB中局部归约、再向GM汇总，可同时降低单次处理开销并减少同一地址上排队的原子操作次数。
- **非必要不使用原子操作的返回值**：是否使用返回值会影响编译器生成的原子指令，不使用返回值时编译器可生成处理开销更低的优化指令。
- **分散GM上原子操作的目标地址**：当原子操作因业务需要必须在GM上执行时，由于GM原子操作以128B Sector（扇区）为处理粒度，目标地址集中在同一个Sector内时处理效率较低，应通过padding等方式使原子目标覆盖更多Sector。该方法仅适用于GM，UB上的原子操作不适用。

【样例介绍】本节以两个样例为载体说明上述优化方向：

- [asc_atomic_add接口性能对比样例](../../../../../../examples/03_simt_api/02_features/01_api_features/02_atomic_operation/atomic_add_perf/README.md)：包含16个场景、归为5组对照实验，量化内存层级、原子累加组织方式、是否使用返回值、同一地址上的竞争程度与目标地址的Sector分布对原子加性能的影响。每组对照仅改变一个属性，其余配置保持一致。
- [Histogram原子操作性能调优样例](../../../../../../examples/03_simt_api/03_best_practices/03_instruction_optimizations/atomic_histogram/README.md)：对形状为[262144]的uint8_t字节流做直方图统计，输出256个桶的uint32_t计数结果。样例启动64个线程块、每个线程块2048个线程，通过对比“全部线程直接更新GM直方图”与“先在各线程块UB上统计再汇总至GM”两种实现，说明分层归约的优化效果。

<!-- npu="950" id1 -->
下文性能数据均使用msOpProf工具在Ascend 950PR产品上采集，指标为Task Duration（Task整体耗时，即算子端到端耗时）。
<!-- end id1 -->

## 通过分层归约将原子操作从GM迁移至UB

【反例】全部线程直接对GM上的全局直方图执行原子累加。

**图 1**  GM全局计数示意图

![](../../../figures/atomic_operation_gm_global_count.png)

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void histogram_gm_global(
    uint32_t* histogram, const uint8_t* input, uint32_t byte_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < byte_count; index += stride) {
        asc_atomic_add(&histogram[input[index]], 1U);
    }
}
```

上述实现中，各线程按grid-stride方式遍历输入字节（即每个线程以总线程数为固定步长，分次处理输入中不同位置的字节），直接对GM直方图的对应桶执行原子加1。262144次原子更新全部集中在256个GM地址上，平均每个地址需排队处理1024次原子加，且每次原子加均作用于访问延迟较高的GM。该实现的性能数据如下：

| 计数位置 | Task Duration（μs） |
| :---: | :---: |
| GM | 424.22 |

【正例】每个线程块先在UB上维护私有直方图，最后汇总至GM。

**图 2**  UB分块计数示意图

![](../../../figures/atomic_operation_ub_block_local_count.png)

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void histogram_ub_block_local(
    uint32_t* histogram, const uint8_t* input, uint32_t byte_count)
{
    __ubuf__ uint32_t block_histogram[HISTOGRAM_BIN_COUNT]; // __ubuf__表示该数组位于UB片上存储。

    // 线程块内线程协作清零UB直方图。
    for (uint32_t bin_index = threadIdx.x; bin_index < HISTOGRAM_BIN_COUNT; bin_index += blockDim.x) {
        block_histogram[bin_index] = 0;
    }
    asc_syncthreads();

    // grid-stride遍历，对本线程块私有的UB直方图做原子累加。
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < byte_count; index += stride) {
        asc_atomic_add(&block_histogram[input[index]], 1U);
    }
    asc_syncthreads();

    // 前256个线程各负责一个桶，将线程块局部结果原子累加到最终GM直方图。
    for (uint32_t bin_index = threadIdx.x; bin_index < HISTOGRAM_BIN_COUNT; bin_index += blockDim.x) {
        asc_atomic_add(histogram + bin_index, block_histogram[bin_index]);
    }
}
```

上述实现的功能与反例一致，线程块数量、线程数、输入读取方式和原子更新总次数均相同，唯一变化是原子操作的目标所在内存层级与竞争范围。该实现的性能数据如下：

| 计数位置 | Task Duration（μs） |
| :---: | :---: |
| UB分块 | 7.95 |

两种实现的性能数据汇总如下：

| 写法 | 计数位置 | Task Duration（μs） |
| :---: | :---: | :---: |
| **UB分块归约后汇总至GM** | **UB** | **7.95** |
| GM全局直接累加 | GM | 424.22 |

根据Task Duration数据，采用分层归约后执行耗时从424.22μs下降至7.95μs，降幅约98.1%，性能提升约53倍。收益来自两个叠加因素：

- **单次原子操作的处理开销降低**：原子操作的目标由GM迁移至UB，访问路径缩短、延迟降低。在同一地址上的原子操作需排队执行的前提下，单次开销的差异随排队次数逐次累积。
- **同一地址上排队的原子操作次数显著减少**：反例中256个GM地址由全部线程共享，平均每个地址执行1024次原子操作；正例中每个线程块持有独立的256桶UB直方图，竞争范围限于本线程块的2048个线程，平均每个地址执行16次原子操作，且64个线程块之间的UB原子操作互不竞争、可并行执行。最终作用于GM的原子操作仅为`64 × 256 = 16384`次汇总，相比反例的262144次显著减少。

两个因素共同作用，其收益显著高于UB初始化、`asc_syncthreads()`同步与GM汇总引入的额外开销。

> [!NOTE]说明
> UB为线程块私有存储，总容量为256KB。分层归约适用于局部归约结果可完整放入UB的场景（如本样例的256个桶）。若归约目标规模过大、无法在UB中容纳，可采用部分归约或对目标进行分段处理。

## 非必要不使用原子操作的返回值

原子操作接口通常会返回目标地址上的旧值。是否使用返回值会影响编译器生成的指令：不使用返回值时，编译器可选择处理开销更低的原子指令；使用返回值时生成的指令开销更高。因此在业务逻辑不依赖旧值的场景下，应避免使用返回值。

【反例】纯计数场景仍接收并使用原子加的返回值。

```cpp
// GM单地址，使用返回值。
__global__ void atomic_add_gm_single_return(int32_t* counter)
{
    volatile int32_t old_value_sink = asc_atomic_add(counter, 1);
    (void)old_value_sink;
}
```

【正例】不依赖旧值时直接丢弃返回值。

```cpp
// GM单地址，不使用返回值。
__global__ void atomic_add_gm_single_no_return(int32_t* counter)
{
    asc_atomic_add(counter, 1);
}
```

在启动1个线程块、每个线程块2048个线程且全部线程竞争同一地址的配置下，分别在GM与UB上测试两种写法，性能数据如下：

| 内存位置 | 数据类型 | 使用返回值（μs） | 不使用返回值（μs） | 耗时比值 |
| :---: | :---: | :---: | :---: | :---: |
| GM | int32_t | 421.59 | 159.88 | 2.64× |
| UB | int32_t | 3.17 | 2.76 | 1.15× |

根据Task Duration数据，GM上不使用返回值时耗时从421.59μs下降到159.88μs，降幅约62.1%；UB上从3.17μs下降到2.76μs，降幅约12.9%。指令形式带来的开销差异在GM上被显著放大，在UB上相对较小，因此在GM场景下更应避免使用返回值。

需要注意的是，该指令优化与数据类型相关，并非对所有类型都生效。在48个线程块、每个线程块256个线程、1024个紧密排布的GM目标地址的配置下，分别测试int32_t与int64_t，性能数据如下：

| 数据类型 | 使用返回值（μs） | 不使用返回值（μs） |
| :---: | :---: | :---: |
| int32_t | 110.95 | 1.89 |
| int64_t | 54.28 | 58.46 |

int32_t不使用返回值时耗时降至1.89μs，而int64_t在两种写法下均为54~58μs量级、无明显差异，说明该指令优化仅对int32_t生效。因此避免使用返回值的收益主要体现在int32_t等可触发优化的类型上。

## 分散GM上原子操作的目标地址

本条优化方法以“原子操作必须在GM上执行”为前提。若原子目标可通过分层归约转移到UB，应优先采用上文的方法；仅当业务要求原子更新直接作用于GM时，才需要进一步关注目标地址在GM上的分布。

GM原子操作会经过L2 Cache层级，在L2侧完成“读—算—写”，并以128B Sector为处理粒度。原子操作的目标地址集中在同一个Sector内时处理效率较低，分布在更多Sector内时处理效率较高。因此在原子操作总次数不变的前提下，应尽量使目标地址覆盖更多Sector。

一个128B Sector可容纳32个int32_t地址（`32 × 4B = 128B`）。据此可构造两种地址布局：紧密排布使32个目标地址连续存放、集中在同一个Sector内；padding布局将相邻目标地址的间隔扩大到128B，使32个目标地址分别位于32个Sector中。

**图 3**  紧密排布与padding地址布局对比示意图

![](../../../figures/atomic_operation_address_layout.png)

下面以48个线程块、每个线程块256个线程、32个int32_t GM目标地址、每个地址上384个线程、每个线程执行1次原子加（共12288次原子加）的配置为例，通过函数入参`target_stride`控制地址布局，对比目标地址集中与分散两种情形。两种情形的目标地址数、每个地址上的线程数与原子加总次数完全一致，仅地址布局不同，因此可单独反映Sector分布的影响。

【反例】目标地址紧密排布，全部原子目标集中在同一个Sector内。

```cpp
__global__ __launch_bounds__(MAX_THREADS_PER_BLOCK) void atomic_add_gm_case4_i32_return(
    int32_t* counters, uint64_t target_count, uint64_t threads_per_target, uint64_t target_stride)
{
    uint64_t global_tid = blockIdx.x * blockDim.x + threadIdx.x;
    uint64_t active_threads = target_count * threads_per_target;
    if (global_tid >= active_threads) {
        return;
    }

    uint64_t target_id = global_tid % target_count;
    int32_t* target = counters + target_id * target_stride;
    volatile int32_t old_value_sink = asc_atomic_add(target, 1);
    (void)old_value_sink;
}

// target_stride取1：32个连续的int32_t地址占满一个128B Sector。
atomic_add_gm_case4_i32_return<<<48, 256, 0, stream>>>(counters_device, 32, 384, 1);
```

上述实现中，目标数组起始地址按128B对齐，`target_stride = 1`使32个目标地址连续存放，全部位于同一个Sector内。该实现的性能数据如下：

| 地址布局 | target_stride | 覆盖Sector数 | Task Duration（μs） |
| :---: | :---: | :---: | :---: |
| 紧密排布 | 1 | 1 | 2164.72 |

【正例】通过padding扩大目标地址间隔，使原子目标分散到更多Sector上。

```cpp
// 核函数（Kernel）不变，仅将target_stride从1增加到32：相邻目标地址间隔128B，32个地址分别位于32个Sector。
atomic_add_gm_case4_i32_return<<<48, 256, 0, stream>>>(counters_device, 32, 384, 32);
```

上述实现的核函数（Kernel）与反例完全相同，数据类型、是否使用返回值、线程规模、目标地址数、每个地址上的线程数与原子加总次数均保持一致，仅将`target_stride`由1个int32_t元素增加到32个int32_t元素（即128B），使原子目标覆盖32个Sector。该实现的性能数据如下：

| 地址布局 | target_stride | 覆盖Sector数 | Task Duration（μs） |
| :---: | :---: | :---: | :---: |
| padding | 32 | 32 | 80.17 |

两种实现的性能数据汇总如下：

| 写法 | 地址布局 | 覆盖Sector数 | Task Duration（μs） |
| :---: | :---: | :---: | :---: |
| **padding分散目标地址** | **padding** | **32** | **80.17** |
| 目标地址紧密排布 | 紧密排布 | 1 | 2164.72 |

根据Task Duration数据，在工作负载完全相同的前提下，将目标地址由1个Sector分散到32个Sector后，执行耗时从2164.72μs下降到80.17μs，降幅约96.3%，性能提升约27倍。除增加地址间隔外，也可通过增加目标地址数或对结构体进行对齐处理实现相同效果。

> [!NOTE]说明
> 本节对照统一使用返回值。由上文可知，int32_t不使用返回值时会触发指令优化，其耗时主要由该优化决定，将掩盖Sector分布对性能的影响。

Sector覆盖数由目标地址的字节间隔决定，因此在目标地址数相同时，位宽更大的数据类型将占用更多Sector。评估数据类型对原子操作性能的影响时，需要区分位宽本身的开销与Sector覆盖变化带来的影响。

【总结】原子操作属于高开销指令，其耗时由同一地址上排队的原子操作次数与单条指令的处理开销共同决定。优化时可按以下顺序考虑：

- **优先采用分层归约，将原子操作从GM迁移至UB**：能在UB完成的原子更新应避免直接在GM上执行。多线程块更新同一目标时，应先在各线程块的UB中完成局部归约，再由每个线程块向GM汇总，既降低单次原子操作的处理开销，又将竞争范围由全局拆分至线程块内部。
- **非必要不使用返回值**：不使用返回值时编译器可生成开销更低的原子指令，该差异在GM上尤为明显。
- **原子操作必须在GM上执行时，分散其目标地址**：GM原子操作以128B Sector为处理粒度，目标地址过于集中会降低处理效率，可通过padding、结构体对齐使原子目标覆盖更多Sector。
