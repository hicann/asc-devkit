# DCache访问优化

【优先级】中

【描述】SIMT编程模式下，经由DCache访问Global Memory数据。当算子中存在多种不同访问模式的数据时，如仅遍历一次的输入数据与需要反复访问的热点数据，若不加区分，所有数据会共同竞争DCache空间，导致热点数据被其他数据挤出DCache，后续再次访问热点数据时需要从GM重新加载，增加访存时间。此时可以通过不同访存函数，为不同类型的数据指定不同的缓存策略，使热点数据优先驻留DCache，减少不必要的GM访问。

SIMT编程模式下提供以下访存函数：

- **`asc_ldcg`**：加载数据时从Global Memory加载，适用于仅遍历一次的输入数据，减少其对DCache空间的占用。
- **`asc_ldca`**：加载数据时优先从DCache加载，适用于需要频繁访问的热点数据（如查找表），确保热点数据常驻DCache，减少从Global Memory重新加载的次数。
- **`asc_stcg`**：存储数据时直接写入Global Memory，不经过DCache缓存，适用于写入GM后不会再从DCache读取的数据，避免输出数据占用DCache空间影响热点数据的缓存。

【样例介绍】以[sin查表算子](../../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/cache_hint)为例，使用查表法计算sin值，通过线性插值提高精度。输入数据长度为65536（256KB），sin查找表长度为8192（32KB）。算法实现中，每个线程根据输入值计算查找表索引，根据索引从sin表中读取对应位置及其后续位置的数据，通过线性插值得到结果。其中每个输入数据仅访问一次，属于非热点数据；sin查找表会被反复访问，属于热点数据。

【反例】所有数据使用默认方式加载，输入、输出和sin表数据共同竞争DCache空间。

```cpp
__global__ void sin_table_lookup_baseline(float* input, float* sin_table, float* output,
                                            uint32_t input_length, uint32_t table_length)
{
    for (int idx = threadIdx.x; idx < input_length; idx += blockDim.x) {
        float x = input[idx];
        ...
        float low_val = sin_table[n];
        float high_val = 0.0f;
        if (n + 1 >= table_length) {
            high_val = sin_table[0];
        } else {
            high_val = sin_table[n + 1];
        }
        output[idx] = sign * (low_val + frac * (high_val - low_val));
    }
}
```

上述实现中，`input[idx]`、`sin_table[n]`、`sin_table[n+1]`和`output[idx]`均使用默认方式访问Global Memory。输入数据虽然仅访问一次，但其加载时会占用DCache空间；输出数据写入后也可能驻留DCache。当输入和输出数据大量加载时，sin查找表的数据会被挤出DCache，导致后续线程查表时需要重新从Global Memory加载，增加DCache Read GM次数。

反例算子的性能数据如下：

| Task Duration(us) | DCache Read GM(次) | DCache Read Vector(次) | DCache Write Vector(次) |
|:-----------------:|:-----------------:|:---------------------:|:----------------------:|
|       56.82       |       5064        |         2048          |          6144          |

【正例】使用访存函数区分不同数据的缓存策略。

```cpp
__global__ void sin_table_lookup_optimized(float* input, float* sin_table, float* output,
                                             uint32_t input_length, uint32_t table_length)
{
    for (int idx = threadIdx.x; idx < input_length; idx += blockDim.x) {
        float x = asc_ldcg(&input[idx]);
        ...
        float low_val = asc_ldca(&sin_table[n]);
        float high_val = 0.0f;
        if (n + 1 >= table_length) {
            high_val = asc_ldca(&sin_table[0]);
        } else {
            high_val = asc_ldca(&sin_table[n + 1]);
        }
        float y = sign * (low_val + frac * (high_val - low_val));
        asc_stcg(&output[idx], y);
    }
}
```

上述实现中：
- 输入数据使用`asc_ldcg`加载，输入数据直接从Global Memory读取，不需要占用DCache空间，避免输入数据对DCache的占用；
- sin查找表使用`asc_ldca`加载，优先为sin表数据分配DCache空间，确保sin表常驻DCache；
- 输出数据使用`asc_stcg`写入，直接写入Global Memory，不经过DCache缓存，避免输出数据占用DCache影响热点数据的缓存。

正例算子的性能数据如下：

| Task Duration(us) | DCache Read GM(次) | DCache Read Vector(次) | DCache Write Vector(次) |
|:-----------------:|:-----------------:|:---------------------:|:----------------------:|
|      50.895       |       3531        |         2048          |          6144          |

从Task Duration看，优化后的执行耗时为50.895us，相比优化前的56.82us，下降约10.4%。从DCache Read GM看，优化后为3531次，相比优化前的5064次，减少约30.2%（减少1533次GM访问）。这说明优化前的主要瓶颈来自所有数据共同竞争DCache空间，导致热点数据（sin查找表）被挤出DCache后需要重新从GM加载；通过访存函数区分缓存策略后，sin查找表优先驻留DCache，减少了从GM重新加载的次数，DCache命中率提升，整体性能得到改善。

【总结】当SIMT算子中存在多种不同访问模式的数据时，应分析各类数据的访问特征，通过访存函数为不同类型的数据指定合适的缓存策略：对仅遍历一次的数据使用`asc_ldcg`降低其Cache占用优先级；对需要频繁访问的热点数据使用`asc_ldca`确保其常驻DCache；对写入后无需驻留DCache的输出数据使用`asc_stcg`优化写入后的Cache分配。这样可以减少热点数据被挤出DCache的情况，降低GM访问次数，提升算子整体性能。
