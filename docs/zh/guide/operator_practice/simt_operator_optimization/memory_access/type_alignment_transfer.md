# 通过类型对齐提升搬运效率

【优先级】中

【描述】SIMT编程模式下，结构体类型对齐会影响Global Memory的访存效率。<!-- npu="950" id1 -->Ascend 950PR/Ascend 950DT支持1B、2B、4B、8B四种位宽的访存指令。<!-- end id1 -->在数据总量不变时，可通过类型对齐到更大的位宽，提升数据搬运效率。

【样例介绍】设置总搬运量`MEM_BYTES`为512MiB，构造多组不同大小、不同对齐方式的结构体`TData`，依次将长度为`num_elements`的输入数组`d_idata`整体复制到输出数组`d_odata`进行测试，对比非对齐与使用`alignas`类型对齐后的耗时差异。具体代码可参考[类型对齐优化搬运效率最佳实践样例](../../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/aligned_types)，其中核函数（Kernel）实现如下：

```cpp
constexpr uint32_t THREAD_COUNT = 2048;                  // threads per block
constexpr size_t MEM_BYTES = 512ULL * 1024ULL * 1024ULL; // 总数据量512 MiB
uint32_t num_elements = static_cast<uint32_t>(MEM_BYTES / sizeof(TData));

template <class TData>
__global__ __launch_bounds__(THREAD_COUNT) void aligned_type_kernel(
    TData* d_odata, TData* d_idata, uint32_t num_elements)
{
    const uint32_t tid = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t num_threads = blockDim.x * gridDim.x;
    for (uint32_t pos = tid; pos < num_elements; pos += num_threads) {
        d_odata[pos] = d_idata[pos];
    }
}
```

下面以`Align4`、`Misalign4`这两个结构体作为典型代表展示对照场景。

【反例】`Misalign4`未设置显式对齐，默认`alignof`受字段类型限制，编译器只能选用窄位宽的访存指令。

```cpp
struct Misalign4 {
    unsigned char r, g, b, a;
}; // sizeof=4, alignof=1
```

性能数据如下：

| Task Duration(us) | aiv_total_cycles |
| :---------------: |:--------------: |
|     81223.02      | 134028903.60    |

上述结构体由字段类型决定`alignof=1`，编译器只能用8-bit档位搬运每个元素，`aiv_total_cycles`和`Task Duration`都较高。

【正例】使用`alignas`提升对齐属性，编译器可以选用更宽位宽的访存指令。

```cpp
struct alignas(4) Align4 {
    unsigned char r, g, b, a;
}; // sizeof=4, alignof=4
```

性能数据如下：

| Task Duration(us) | aiv_total_cycles |
| :---------------: | :--------------: |
|     27889.33      |  46005213.32    |

`Align4`将`alignof`从1B提升到4B，编译器可以选用32-bit档位完成单元素搬运，`aiv_total_cycles`和`Task Duration`都显著减小。

对比多组结构体大小不同的样例，在类型对齐前后耗时差异如下：

| 结构体大小                  | 对齐Task Duration(us) | 非对齐Task Duration(us) | 非对齐Task Duration/对齐Task Duration |
| :------------------------: | :-------------------: | :---------------------: | :---------: |
| 4B (4×u8)                  | 27889.33 (alignof=4)  | 81223.02 (alignof=1)    | **2.91x**   |
| 4B (2×u16)                 | 27833.56 (alignof=4)  | 46656.61 (alignof=2)    | **1.68x**   |
| 4B (2×u8 + u16)            | 27854.56 (alignof=4)  | 46666.11 (alignof=2)    | **1.68x**   |
| 4B (u8 + 1B padding + u16) | 27809.72 (alignof=4)  | 46664.42 (alignof=2)    | **1.68x**   |
| 8B                         | 17417.23 (alignof=8)  | 21287.91 (alignof=4)    | **1.22x**   |
| 16B                        | 32983.77 (alignof=16) | 38353.60 (alignof=4)    | **1.16x**   |
| 32B                        | 60901.24 (alignof=32) | 77790.93 (alignof=4)    | **1.28x**   |

测试结果表明：**在数据总量不变的时候，相同大小的结构体，类型对齐可以提升搬运效率**。以4B结构体为例，随着结构体类型对齐位宽从1B对齐提升至4B，`非对齐Task Duration/对齐Task Duration`逐步下降，说明非对齐与对齐的耗时差距逐步缩小。此外，同大小的4B结构体，字段组合形式、有无填充位，都不会对搬运效率产生影响。

【总结】SIMT算子使用结构体搬运数据时，结构体类型对齐能让访存指令以更大位宽合并完成搬运，降低搬运耗时。<!-- npu="950" id2 -->Ascend 950PR/Ascend 950DT支持1B、2B、4B、8B位宽的访存指令，因此建议使用时在不超过结构体大小的情况下，尽量选择更高的位宽。<!-- end id2 -->
