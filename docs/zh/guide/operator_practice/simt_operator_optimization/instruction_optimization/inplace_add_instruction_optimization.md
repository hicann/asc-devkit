# 原地加法指令优化

【优先级】中

【描述】如`a[index] += b[index]`、`a[index] += b[index] * c[index]`，原地加法是算子中常见的数据更新形式。在上述使用加法运算符的程序中，累加目标`a[index]`的旧值必须先从L2 Cache读入寄存器才能参与计算，完成加法运算后，结果值从寄存器写回L2 Cache，每个元素都经历一次旧值读入、新值写出的寄存器往返。改用`asc_atomic_add(&a[index], b[index])`这类[原子加接口](../../../../api/SIMT-API/atomic_operations/atomic_operations_intro.md)后，“读出旧值—计算新值—写回新值”三步作为一个不可分割的整体在L2 Cache侧完成，旧值不再进入寄存器，省去其进出寄存器的数据往返。

【样例介绍】本节以[inplace_add_atomic样例](../../../../../../examples/03_simt_api/03_best_practices/03_instruction_optimizations/inplace_add_atomic/README.md)为载体说明上述优化方向，样例对三个形状均为[4194304]的int32_t数组执行逐元素累加，包含两组对照，一组为三操作数乘加`a[index] += b[index] * c[index]`，另一组为两操作数加法`a[index] += b[index]`。样例启动64个线程块、每个线程块2048个线程，线程块数量为运行时查询到的AIV核数，各组对照仅改变累加语句的代码，即普通原地加或`asc_atomic_add`，其余保持一致。

<!-- npu="950" id1 -->
下文性能数据均使用msOpProf工具在Ascend 950PR系列产品上采集，指标为Task Duration，即Task整体耗时、算子端到端耗时。
<!-- end id1 -->

**优化原理**

两组对照的原理相同：普通原地加中，累加目标的旧值需经L2 Cache读入寄存器，加法完成后新值再从寄存器写回L2 Cache；改用原子加后，旧值的读出、加法与写回均在L2 Cache侧作为一个不可分割的整体完成，旧值不再经“L2 Cache→寄存器→L2 Cache”的往返。两种累加方式的数据流转分别如下图所示。

**图 1**  普通原地加的数据流转

![](../../../figures/inplace_add_atomic_dataflow_plain.png)

**图 2**  原子加的数据流转

![](../../../figures/inplace_add_atomic_dataflow_atomic.png)

以三操作数乘加为例：普通原地加中`a[index]`、`b[index]`、`c[index]`三者都经L2 Cache读入寄存器，乘加完成后`a[index]`的新值再从寄存器写回L2 Cache，每个元素产生3次读、1次写；原子加中只有`b[index]`与`c[index]`读入寄存器并算出乘积，乘积作为原子加的操作数下发，`a[index]`不进入寄存器，每个元素的读从3次降为2次。`b`、`c`的读取与乘法计算完全相同，性能差异仅来自省去的`a[index]`旧值数据往返。

**三操作数乘加**

【反例】累加目标的旧值读入寄存器，在核内完成读改写。

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void vector_muladd_plain(
    int32_t* a, const int32_t* b, const int32_t* c, uint32_t element_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < element_count; index += stride) {
        a[index] += b[index] * c[index];
    }
}
```

上述实现中，各线程按grid-stride方式遍历数组，即每个线程以总线程数为固定步长，分次处理数组中不同位置的元素。每个元素的乘加都需将`a[index]`旧值读入寄存器、加法完成后写回。该实现的性能数据如下：

| 累加方式 | Task Duration（μs） | 耗时相对基线 |
| :---: | :---: | :---: |
| a[index] += b[index] * c[index] | 51.62 | 1× |

【正例】改用原子加，将累加下发到L2 Cache侧完成。

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void vector_muladd_atomic(
    int32_t* a, const int32_t* b, const int32_t* c, uint32_t element_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < element_count; index += stride) {
        asc_atomic_add(&a[index], b[index] * c[index]);
    }
}
```

上述实现的功能与反例一致，线程块数量、线程数、输入读取方式均相同，唯一变化是累加语句的代码。本样例中各线程更新的目标地址互不重叠，不存在同地址竞争，算子结果的正确性并不依赖原子性；此处使用原子加是为了省去旧值进出寄存器的数据往返，获取性能收益。该实现的性能数据如下：

| 累加方式 | Task Duration（μs） | 耗时相对基线 |
| :---: | :---: | :---: |
| asc_atomic_add | 47.51 | **0.92×** |

根据Task Duration数据，改用原子加后执行耗时从51.62μs下降至47.51μs，降幅约8.0%。

**两操作数加法**

【反例】累加目标的旧值读入寄存器，在核内完成读改写。

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void vector_add_plain(
    int32_t* a, const int32_t* b, uint32_t element_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < element_count; index += stride) {
        a[index] += b[index];
    }
}
```

该实现的性能数据如下：

| 累加方式 | Task Duration（μs） | 耗时相对基线 |
| :---: | :---: | :---: |
| a[index] += b[index] | 39.18 | 1× |

【正例】改用原子加，将累加下发到L2 Cache侧完成。

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void vector_add_atomic(
    int32_t* a, const int32_t* b, uint32_t element_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < element_count; index += stride) {
        asc_atomic_add(&a[index], b[index]);
    }
}
```

该实现的核函数遍历方式、线程规模与反例完全一致，仅将累加语句改写为原子加。两操作数加法中每个元素的读从2次降为1次，数据往返的节省比例与三操作数乘加相当。该实现的性能数据如下：

| 累加方式 | Task Duration（μs） | 耗时相对基线 |
| :---: | :---: | :---: |
| asc_atomic_add | 36.52 | **0.93×** |

根据Task Duration数据，改用原子加后执行耗时从39.18μs下降至36.52μs，降幅约6.8%。

两组对照的性能数据汇总如下：

| 计算形式 | 累加方式 | Task Duration（μs） | 耗时相对基线 |
| :---: | :---: | :---: | :---: |
| **三操作数乘加** | **asc_atomic_add** | **47.51** | **0.92×** |
| 三操作数乘加 | a[index] += b[index] * c[index] | 51.62 | 1× |
| **两操作数加法** | **asc_atomic_add** | **36.52** | **0.93×** |
| 两操作数加法 | a[index] += b[index] | 39.18 | 1× |

**适用边界**

该优化仅适用于逐元素原地更新，使用前需确认各线程原子操作的目标地址互不重叠：

- **适用的逐元素原地更新**：每个线程只更新自己负责的元素，目标地址在线程间天然不重叠，如本样例的grid-stride遍历。
- **不适用的归约累加**：多个线程向同一地址累加，如直方图统计，此时同一地址上的原子操作只能排队串行执行，直接使用原子加会因地址竞争显著劣化性能，应采用[分层归约](./atomic_instruction_optimization.md)——先在各线程块的UB中局部累加，再向GM汇总。

此外，原子加支持的数据类型必须是支持指令优化的数据类型；对int64_t等类型，该执行路径的收益需实测确认，不使用返回值时能否生成更优原子指令与数据类型相关，参见[asc_atomic_add约束说明](../../../../api/SIMT-API/atomic_operations/asc_atomic_add.md#约束说明)。

【总结】原地累加场景中，若累加目标的旧值仅用于累加、不参与其他计算，且各线程目标地址互不重叠，可优先考虑用原子加完成累加：旧值的读出、加法与写回均在L2 Cache侧作为一个不可分割的整体完成，省去旧值进出寄存器的数据往返，实测收益约7%~8%。
