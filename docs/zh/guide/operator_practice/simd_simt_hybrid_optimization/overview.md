# SIMD与SIMT混合算子性能优化概述

AI Core上SIMD与SIMT混合编程结合了SIMD多数据并行计算能力与SIMT离散访存、复杂分支处理能力，可以在同一算子中以Vector Function（VF）为基本调度单位灵活组织SIMD和SIMT执行逻辑。SIMD编程适合处理连续、规整的向量或矩阵计算，SIMT编程适合处理分支判断、索引映射、离散访存等不规则逻辑，在程序中结合使用两种编程方式，可以兼顾硬件高吞吐和编程灵活性。

本节主要介绍SIMD与SIMT混合编程中SIMT VF的性能优化方法，帮助开发者基于性能数据判断SIMT VF片段的优化程度，并进一步选择合适的优化方向。优化时应先分析理论数据，再通过实际性能数据确认瓶颈，最后针对性地从内存访问、执行控制、指令优化等方面进行优化。

## 分析性能数据

性能分析的核心是先建立理论基准，再用实测数据观察当前实现与理论预期之间的差距。

1. 理论性能评估

   理论性能评估用于建立算子的理想访存规模和设备GM（Global Memory）带宽上限。分析时应先根据算子语义、数据量和数据类型，估算算子完成一次计算所需的理论读数据量和理论写数据量，计算方式通常为访问元素个数乘以数据类型字节数。该数据量表示算法本身必须处理的数据规模，是后续判断实际读写是否合理的基准。在得到理论读写数据量后，还需要结合产品文档中的硬件规格获取设备GM理论带宽。理论带宽代表访存通路的理想上限，可用于评估当前算子实现的带宽利用水平和优化空间。

2. 实际数据度量

   实际数据度量用于观察算子运行时的GM访问情况，以及SIMT VF在AIV核执行时间中的占比。可使用[算子开发工具](https://gitcode.com/Ascend/msopprof/blob/26.1.0/docs/zh/user_guide/msopprof_user_guide.md)中的msOpProf工具采集性能数据：
   - 通过`Memory.csv`查看AIV核整体GM读写带宽和读写数据量，相关性能数据字段为：`aiv_main_mem_read_bw`、`aiv_main_mem_write_bw`、`read_main_memory_datas`、`write_main_memory_datas`。
   - 通过`ArithmeticUtilization.csv`查看算子在AIV核上的执行时间以及SIMT VF类型指令的cycle占比，相关性能数据字段为：`aiv_time`、`aiv_vec_simt_vf_ratio`。

   在SIMD与SIMT混合编程中，`aiv_time`覆盖整个AIV核执行区间。分析SIMT VF自身性能时，应使用`aiv_vec_simt_vf_ratio`折算SIMT VF执行时间：

   ```text
   SIMT VF执行时间 = aiv_time * aiv_vec_simt_vf_ratio
   ```

   其中，`aiv_vec_simt_vf_ratio`表示SIMT VF类型指令cycle数占AIV核上执行的cycle总数的比例。

   `read_main_memory_datas`和`write_main_memory_datas`是执行算子时的GM读、写数据量，未必全部是由SIMT VF触发的读写操作。计算SIMT VF实际带宽时，应使用SIMT VF相关GM读写数据量进行计算：

   ```text
   SIMT VF实际带宽 = SIMT VF相关GM读写数据量 / SIMT VF执行时间
   ```

   如果同一个核函数（Kernel）中还存在SIMD VF、MTE搬运或其他阶段的GM读写，且无法判断GM读写数据是否由SIMT VF触发，则不应将AIV核整体GM读写量直接除以SIMT VF执行时间。此时应将`read_main_memory_datas`、`write_main_memory_datas`以及`aiv_main_mem_read_bw`、`aiv_main_mem_write_bw`作为AIV核整体GM通路压力的参考，再结合代码结构、`aiv_vec_simt_vf_ratio`和MTE占比判断瓶颈来源。若SIMT VF内读取GM数据经过Data Cache并命中，实测GM读数据量可能小于理论读数据量，这通常表示部分读访问被缓存复用。

## 设计优化方案

完成理论性能评估和实际数据度量后，应优先分析GM读写带宽和实际读写数据量，再决定优化方向。对于SIMD与SIMT混合算子，优化通常可从内存访问、执行控制、控制流和指令优化四个方面展开。

- 内存访问优化

  内存访问优化是最高优先级的优化方向。混合编程中的SIMT VF常用于处理离散访存、索引映射等不规则逻辑，内存访问优化的作用是在SIMT VF中利用访存合并减少无效GM访问，提高有效带宽和数据复用率。

  SIMT编程中同一个Warp内的线程对GM的访问请求，如果落在同一个区域（sector）内，可以合并为同一个内存请求；在Ascend 950上，sector大小为128B。离散访问会使访问请求分散到多个sector范围，访存效率较低。优化时应优先调整线程与数据的映射关系，让同一个Warp内的线程尽量访问相近或连续的GM地址，提高带宽的有效利用率。对于SIMT VF中难以连续访问GM的离散数据场景，可以先通过MTE搬运接口将相关数据从GM搬运到Unified Buffer（UB）缓存，再在SIMT VF中访问UB数据，降低离散GM访问开销，从而提高带宽利用率。分析过程中还应关注Data Cache命中带来的读数据量变化，以及UB访问是否引入新的冲突。

- 执行控制优化

  执行控制优化的目标是让线程数量、线程块数量和SIMT VF任务量与真实工作量匹配。开发者应根据输入输出规模和线程映射关系，合理设置SIMT VF的线程数、`__launch_bounds__`和数据切分方式，使线程覆盖真实输出规模，并尽量保证核间负载均衡。此外，在SIMD与SIMT混合算子中还需要关注VF调用次数，如果VF调用次数过多，会引入不必要的调度开销，影响整体执行效率。

- 控制流优化

  控制流优化的目标是减少SIMT VF内部的分支发散，提升Warp内线程的执行效率。流控制指令（if、switch、do、for、while）可能使同一Warp内的线程进入不同的执行路径，导致分支发散。SIMT VF函数内应尽量使同一Warp内的线程走相同执行路径，减少因分支分歧导致的串行化执行开销。

- 指令优化

  指令优化通常在计算瓶颈，或内存访问已经较充分但性能仍未达到预期时重点考虑。它的作用是减少SIMT VF内部的高开销指令和冗余计算，提升线程级计算逻辑的执行效率。通过优化计算逻辑减少除法、取模等高开销指令的使用，并避免在线程内重复执行可提前计算或可复用的逻辑。

## 优化建议总览表

表1 计算优化建议总览表

| 优化建议 | 样例 |
| --- | --- |
| [通过SIMT实现分支判断](./compute_optimization/simt_branch_judgment.md) | [simd_simt_high_performance](../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance) |

表2 内存访问优化建议总览表

| 优化建议 | 样例 |
| --- | --- |
| [使用Unified Buffer提升内存访问效率](./memory_access/ub_memory_access.md) | [simd_simt_high_performance](../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance) |
| [访存合并](../simt_operator_optimization/memory_access/memory_access_merge.md) | [simd_simt_matrix_transpose](../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_matrix_transpose) |
| [避免UB的Bank冲突](../simt_operator_optimization/memory_access/avoid_ub_bank_conflict_simt.md) | [matrix_transpose_practice](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice) |
| [DCache访问优化](../simt_operator_optimization/memory_access/dcache_access.md) | [cache_hint](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/cache_hint) |
| [使用短向量类型提升效率](../simt_operator_optimization/memory_access/short_vector_efficiency.md) | [short_vector_add](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/short_vector_add) |
| [通过类型对齐提升搬运效率](../simt_operator_optimization/memory_access/type_alignment_transfer.md) | [aligned_types](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/aligned_types) |

表3 执行控制优化建议总览表

| 优化建议 | 样例 |
| --- | --- |
| [线程块数量配置优化](./execution_control/thread_block_config.md) | [simd_simt_grid_dim_config](../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_grid_dim_config) |

表4 控制流优化建议总览表

| 优化建议 | 样例 |
| --- | --- |
| [同一Warp内避免分支跳转](../simt_operator_optimization/control_flow/avoid_warp_branch.md) | [warp_divergence](../../../../../examples/03_simt_api/03_best_practices/02_control_flow/warp_divergence) |

表5 指令优化建议总览表

| 优化建议 | 样例 |
| --- | --- |
| [整数除法的快速算法](./instruction_optimization/fast_integer_division.md) | [simd_simt_integer_fast_div](../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_integer_fast_div) |
