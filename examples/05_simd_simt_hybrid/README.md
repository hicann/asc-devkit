# SIMD与SIMT混合编程样例介绍


## 概述

SIMD与SIMT混合编程适用于同一个Kernel中同时需要向量化计算能力和线程级灵活控制的场景。通常可使用SIMD完成规则向量计算，使用SIMT处理离散访存、分支判断、线程映射和细粒度数据搬运等逻辑，从而在保留表达灵活性的同时提升规则计算效率。

SIMD与SIMT混合编程是基于SIMD编程和SIMT编程的高阶编程方式，相关样例主要分布在以下路径：

- `examples/05_simd_simt_hybrid`路径下提供混合编程入门样例、问题定位样例，以及使用SIMD和SIMT协同进行性能优化的样例。
- 由于混合编程的Kernel入口与SIMD编程方式一致，端到端维测相关样例可参考`examples/01_simd_cpp_api`路径下的样例。
- 混合编程通过启动SIMT VF实现SIMT线程级操作，SIMT VF相关的重点特性、维测和性能优化样例可参考`examples/03_simt_api`路径下的样例。

后文将分别介绍混合编程场景下涉及到introduction、utilities、features、trouble_shooting和best_practices样例。

## introduction

[00_introduction](./00_introduction)是SIMD与SIMT混合编程场景的入门参考目录，该目录下包含以下样例。

| 样例名 | 样例描述 |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| simd_simt_gather_and_adds | 基于SIMD与SIMT混合编程实现Gather和Adds融合计算，展示使用SIMT处理离散访存、使用SIMD处理连续向量计算的基本写法，以及核函数和VF函数的定义与调用方式。样例链接：[simd_simt_gather_and_adds](./00_introduction/simd_simt_gather_and_adds)。 |

## utilities

基于SIMD与SIMT混合编程开发自定义算子时，可参考以下utilities样例进行调试调优：

| 样例名 | 样例描述 | 备注说明 |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| profiling | 展示如何使用Profiling采集算子执行数据，并分析混合编程中SIMD计算、SIMT VF调用和数据搬运的耗时占比。样例链接：[profiling](../01_simd_cpp_api/01_utilities/04_profiling)。 | - |
| sanitizer | 展示如何使用Sanitizer定位内存越界、非法访问等问题，适用于混合编程调试阶段的问题排查。样例链接：[sanitizer](../01_simd_cpp_api/01_utilities/05_sanitizer)。 | - |
| simulator | 展示如何使用Simulator在仿真模式下进行功能调试和基础性能数据采集。样例链接：[simulator](../01_simd_cpp_api/01_utilities/08_simulator)。 | - |
| printf（核函数） | 展示如何在SIMD编程样例的核函数入口中使用`AscendC::printf`打印调试信息。样例链接：[printf（核函数）](../01_simd_cpp_api/01_utilities/00_printf/simple_printf)。 | - |
| printf（SIMD VF函数） | 展示如何在SIMD VF函数中使用`printf`打印不同类型数据、指针信息和计算结果。样例链接：[printf（SIMD VF函数）](../01_simd_cpp_api/01_utilities/00_printf/simd_vf_printf)。 | - |
| printf（SIMT VF函数） | 展示如何在SIMT VF中打印调试信息。样例链接：[printf（SIMT VF函数）](../03_simt_api/01_utilities/00_printf)。 | 该样例为SIMT编程方式实现的参考样例，可转换为混合编程对应的样例实现，方法详见[SIMT参考样例转换说明](#simt参考样例转换说明)。 |
| assert（核函数） | 展示如何在SIMD编程样例的核函数入口中使用`ascendc_assert`进行断言检查。样例链接：[assert（核函数）](../01_simd_cpp_api/01_utilities/01_assert)。 | - |
| assert（SIMT VF函数） | 展示如何在SIMT VF中进行断言检查。样例链接：[assert（SIMT VF函数）](../03_simt_api/01_utilities/01_assert)。 | 该样例为SIMT编程方式实现的参考样例，可转换为混合编程对应的样例实现，方法详见[SIMT参考样例转换说明](#simt参考样例转换说明)。 |
| clock（SIMT VF函数） | 展示如何在SIMT VF中统计局部执行周期，辅助定位混合编程中的热点路径。样例链接：[clock（SIMT VF函数）](../03_simt_api/01_utilities/02_clock)。 | 该样例为SIMT编程方式实现的参考样例，可转换为混合编程对应的样例实现，方法详见[SIMT参考样例转换说明](#simt参考样例转换说明)。 |

## features

基于SIMD与SIMT混合编程开发自定义算子时，可参考以下SIMT重点特性样例。

| 样例名 | 样例描述 | 备注说明 |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| memory_access | 展示Global Memory离散访存相关API的使用方法，适用于混合编程中由SIMT处理复杂索引访问的场景。样例链接：[memory_access](../03_simt_api/02_features/01_api_features/00_memory_access)。 | 该样例为SIMT编程方式实现的参考样例，可转换为混合编程对应的样例实现，方法详见[SIMT参考样例转换说明](#simt参考样例转换说明)。 |
| sync_instruction | 展示同步与内存栅栏接口的使用方法，适用于混合编程中SIMT线程协作和跨阶段数据可见性控制。样例链接：[sync_instruction](../03_simt_api/02_features/01_api_features/01_sync_instruction)。 | 该样例为SIMT编程方式实现的参考样例，可转换为混合编程对应的样例实现，方法详见[SIMT参考样例转换说明](#simt参考样例转换说明)。 |
| atomic_operation | 展示原子操作接口的使用方法，适用于混合编程中多线程累加、计数或冲突写入处理。样例链接：[atomic_operation](../03_simt_api/02_features/01_api_features/02_atomic_operation)。 | 该样例为SIMT编程方式实现的参考样例，可转换为混合编程对应的样例实现，方法详见[SIMT参考样例转换说明](#simt参考样例转换说明)。 |
| warp_instruction | 展示Warp级数据交换能力，可用于了解Warp内线程协作和数据传递方式。样例链接：[warp_instruction](../03_simt_api/02_features/01_api_features/03_warp_instruction)。 | 该样例为SIMT编程方式实现的参考样例，可转换为混合编程对应的样例实现，方法详见[SIMT参考样例转换说明](#simt参考样例转换说明)。 |

## trouble_shooting

[01_trouble_shooting](./01_trouble_shooting)是SIMD与SIMT混合编程场景的问题定位参考目录，该目录下包含以下样例。

| 样例名 | 样例描述 |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| simd_simt_ub_usage_limit | 以UB访问越界导致输出精度异常为例，展示混合编程场景下UB静态内存、动态内存、预留空间、Data Cache布局，以及UB越界问题定位方法。样例链接：[simd_simt_ub_usage_limit](./01_trouble_shooting/simd_simt_ub_usage_limit)。 |

## best_practices

SIMD与SIMT混合编程场景的常用性能优化路径如下：

- 合理配置最大线程数：最大线程数会影响单线程可用寄存器数量，需根据算子计算复杂度配置线程数，避免寄存器溢出影响性能。
- 访存优化：通过访存合并、合理利用UB中转、减少离散访问和规避UB Bank冲突等手段提升访存效率。
    - 访存合并包含GM和UB访问合并，核心思路是让Warp内线程尽量连续访问GM或UB，使底层硬件可以合并内存访问请求，减少请求次数。
    - 通过合理使用UB中转，可减少离散访问。使用UB时需关注UB Bank冲突，可通过优化内存排布降低冲突概率。
    - 连续搬运数据的场景可使用MTE搬运数据。通常情况下，MTE搬运效率高于SIMT LD/ST访存效率，并且MTE与SIMT VF属于不同流水，更容易并行执行以掩盖搬运开销。

- 执行配置优化：根据数据规模和单线程工作量配置gridDim、blockDim以及VF函数调用粒度，减少无效线程和调度开销。
- 计算指令优化：针对固定除数整数除法等场景，使用乘法和移位替代高开销指令。

[best_practices](./02_best_practices)路径下提供了使用SIMD与SIMT混合编程实现的性能优化样例，主要展示如何配合使用SIMD和SIMT提升性能，具体如下：

| 样例名 | 样例描述 |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| simd_simt_matrix_transpose | 以矩阵转置为例介绍UB中转和全局访存合并优化方法。样例链接：[simd_simt_matrix_transpose](./02_best_practices/simd_simt_matrix_transpose)。 |
| simd_simt_high_performance | 以FloorMod计算为例，对比SIMT直接访问GM、SIMD Reg矢量计算、SIMT访问UB以及连续线程映射等实现方式。样例链接：[simd_simt_high_performance](./02_best_practices/simd_simt_high_performance)。 |
| simd_simt_grid_dim_config | 以Gather计算为例介绍线程块数量配置和VF函数调用优化思路。样例链接：[simd_simt_grid_dim_config](./02_best_practices/simd_simt_grid_dim_config)。 |
| simd_simt_integer_fast_div | 以固定除数整数除法为例，展示使用乘法和移位替代普通除法的优化方式。样例链接：[simd_simt_integer_fast_div](./02_best_practices/simd_simt_integer_fast_div)。 |

另外，SIMD与SIMT混合编程场景中的SIMT VF优化手段，可参考如下样例：

| 样例名 | 样例描述 | 备注说明 |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| max_thread_config | 展示最大线程数配置方法，可参考寄存器使用量、最大线程数和执行性能之间的关系。样例链接：[max_thread_config](../03_simt_api/03_best_practices/01_execution_conf_optimizations/max_thread_config)。 | 该样例为SIMT编程方式实现的参考样例，可转换为混合编程对应的样例实现，方法详见[SIMT参考样例转换说明](#simt参考样例转换说明)。 |
| warp_divergence | 以SpMV计算为例展示Warp Divergence对性能的影响，以及通过一个Warp协作处理一行数据减小分支发散的优化思路。样例链接：[warp_divergence](../03_simt_api/03_best_practices/02_control_flow/warp_divergence)。 | 该样例为SIMT编程方式实现的参考样例，可转换为混合编程对应的样例实现，方法详见[SIMT参考样例转换说明](#simt参考样例转换说明)。 |
| cache_hint | 展示数据缓存优化方法，可参考为不同数据指定缓存策略、减少Global Memory访问次数的实现思路。样例链接：[cache_hint](../03_simt_api/03_best_practices/00_memory_optimizations/cache_hint)。 | 该样例为SIMT编程方式实现的参考样例，可转换为混合编程对应的样例实现，方法详见[SIMT参考样例转换说明](#simt参考样例转换说明)。 |
| short_vector_add | 展示短向量优化方法，可参考half2等短向量类型提升访存带宽利用率和计算效率的实现思路。样例链接：[short_vector_add](../03_simt_api/03_best_practices/00_memory_optimizations/short_vector_add)。 | 该样例为SIMT编程方式实现的参考样例，可转换为混合编程对应的样例实现，方法详见[SIMT参考样例转换说明](#simt参考样例转换说明)。 |
| aligned_types | 展示类型对齐优化方法，可参考结构体对齐对Global Memory访存吞吐的影响。样例链接：[aligned_types](../03_simt_api/03_best_practices/00_memory_optimizations/aligned_types)。 | 该样例为SIMT编程方式实现的参考样例，可转换为混合编程对应的样例实现，方法详见[SIMT参考样例转换说明](#simt参考样例转换说明)。 |


## SIMT参考样例转换说明

SIMT编程实现的样例主要展示SIMT Kernel直调写法。混合编程场景可以参考其中的线程映射、访存、同步、原子操作和Warp级协作逻辑，但Kernel入口、Host侧调用配置和CMake编译选项存在差异。将SIMT参考样例用于SIMD与SIMT混合编程时，需重点关注以下差异。

### Kernel定义差异

SIMT编程和SIMD与SIMT混合编程的Kernel定义方式不同，典型差异如下：

SIMT编程：线程级逻辑直接写在__global__ SIMT Kernel中：
```cpp
template <typename type_data, typename type_idx>
__global__ __launch_bounds__(MAX_THREAD_COUNT) void gather_custom(
    type_data* input, type_idx* index, type_data* output, uint32_t in_width, uint32_t index_total_length)
{
    int32_t out_row = blockIdx.x * blockDim.x + threadIdx.x;
    ...
}
```
SIMD与SIMT混合编程：线程级SIMT逻辑实现放在__simt_vf__函数内
```cpp
__simt_vf__ __launch_bounds__(MAX_THREAD_COUNT) inline void simt_gather(
    __gm__ float* input, __gm__ uint32_t* index, __ubuf__ float* gather_output, ...)
{
    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    ...
}

// SIMD与SIMT混合编程：入口Kernel保持__global__ __vector__定义，并通过asc_vf_call调用SIMT VF
__global__ __vector__ void gather_custom(
    __gm__ float* input, __gm__ uint32_t* index, __gm__ float* output, ...)
{
    ...
    asc_vf_call<simt_gather>(dim3(thread_count), input, index, gather_output, ...);
    ...
}
```

具体差异如下：

- SIMT编程样例中，`__global__` Kernel既是Host侧直接启动的入口，也是线程级逻辑的实现位置，线程索引计算、分支判断、访存和线程协作逻辑通常直接写在该Kernel中。
- SIMD与SIMT混合编程样例中，入口Kernel与SIMD编程方式一致，通常定义为`__global__ __vector__`，用于编排数据搬运、UB空间申请、SIMD VF调用以及SIMT VF调用。
- 转换SIMT参考样例时，通常将原SIMT Kernel中的线程级逻辑迁移到`__simt_vf__`函数中，再由`__global__ __vector__`入口Kernel通过`asc_vf_call`调用。

### 调用配置差异

SIMT编程和SIMD与SIMT混合编程的调用配置位置不同，典型差异如下：

SIMT编程：Host侧通过<<<>>>直接配置SIMT Kernel的线程块数量、线程数、动态UB和stream
```cpp
gather_custom<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(...);
```

```cpp
// SIMD与SIMT混合编程：Host侧通过<<<>>>启动Vector Core入口Kernel
gather_and_adds_kernel<<<blocks_per_grid, dyn_ubuf_size, stream>>>(...);

// SIMD与SIMT混合编程：SIMT线程数在入口Kernel内部通过asc_vf_call配置
asc_vf_call<simt_gather>(dim3(threads_per_block), input, index, gather_output, ...);
```

具体差异如下：

- SIMT编程样例中，Host侧`<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>`直接启动SIMT Kernel。第一个参数表示线程块数量，第二个参数表示每个线程块内的线程数，第三个参数表示动态UB大小，第四个参数表示stream。
- SIMD与SIMT混合编程样例中，Host侧`<<<blocks_per_grid, dyn_ubuf_size, stream>>>`启动的是`__global__ __vector__`入口Kernel。第一个参数表示启动的Vector Core数量或逻辑核数量（等同于线程块数量），第二个参数表示动态UB大小，第三个参数表示stream。
- 混合编程中的SIMT线程数不由Host侧`<<<>>>`配置，而是在入口Kernel内部通过`asc_vf_call<simt_func>(dim3(threads_per_block), ...)`配置。


### CMake编译选项差异

SIMT编程样例通过`--enable-simt`显式打开SIMT编译能力，例如：

```cmake
target_compile_options(${TARGET_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES} --enable-simt>
)
```

将SIMT参考样例用于SIMD与SIMT混合编程时，建议以混合编程样例的CMake结构为基础，根据是否需要NPU仿真、CPU调试、Profiling或Sanitizer能力补充对应选项和链接库。
