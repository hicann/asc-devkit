# 概述

## 整体性能优化策略

SIMT编程模式以线程（Thread）为基本编程单位，由开发者编写单个线程的执行逻辑，再通过`<<<gridDim, blockDim>>>`配置将大量线程组织为线程块（线程块）与线程网格（Grid），调度到硬件的Vector Core（物理核，即AIV核）上以Warp为单位并行执行。

性能优化的整体目标是让算子在硬件上尽可能高效地运行，核心在于保持各类硬件资源处于忙碌且有效的状态。SIMT算子的优化主要围绕以下三个基本策略展开：

- **最大化利用率（Maximize Utilization）**：让尽可能多的物理核、线程并行工作，使硬件的并行计算能力被充分发挥，避免物理核闲置或线程排队等待。
- **最大化存储吞吐量（Maximize Memory Throughput）**：尽量减少低带宽的数据搬运，让每一次Global Memory（GM）访问都搬运尽可能多的有效数据，充分利用GM带宽、Cache和Unified Buffer（UB）。
- **最大化指令吞吐量（Maximize Instruction Throughput）**：减少线程内高开销指令、冗余计算和分支发散，提升计算指令的有效执行占比。

这三种策略对算子性能的影响程度因算子而异，应结合实测性能数据确定主要瓶颈后再针对性优化。优化的一般流程是：先建立理论性能基准（理论读写数据量、GM理论带宽），再使用msOpProf等工具采集实际性能数据（Task Duration、GM读写带宽、DCache访问次数、aiv_vec_ratio等），对比理论与实测的差距定位瓶颈，最后从对应层次选择优化手段并通过性能数据验证优化效果。下面分别介绍三个层次的优化方法。

## 最大化利用率

最大化利用率的目标是充分发挥硬件并行能力。SIMT算子的利用率优化可以从应用级、设备级和物理核级三个层面理解：应用级关注核函数（Kernel）启动配置是否让足够多的物理核并行工作；设备级与物理核级关注单个物理核内的线程、Warp是否能保持忙碌，避免因资源限制（如寄存器）导致并行度下降。在Ascend C SIMT中，主要通过核函数（Kernel）的执行配置（`gridDim`、`blockDim`、`__launch_bounds__`）实现物理核资源的并行优化。

- **合理配置线程块数量，平衡并行核数与调度开销**

  线程块最终被调度到物理核上执行，一个物理核同一时刻只能驻留并执行一个线程块。线程块数量过少会导致物理核闲置、并行度不足、单核工作量过重；线程块数量远超物理核数时，超出部分需排队等待前序线程块执行完成，多线程块的启动和调度固定开销会明显累积。因此线程块数量应结合物理核数与数据规模设置。

- **合理配置最大线程数，避免寄存器溢出降低有效利用率**

  核函数（Kernel）通过`__launch_bounds__`指定的最大线程数决定每个线程可用的寄存器数量：最大线程数越大，每个线程可分配的寄存器越少。对于计算密集型算子，若线程数配置过高导致每个线程的寄存器数量不足，中间数据会溢出到位于Global Memory的栈空间，引入额外的GM访问，反而降低有效利用率。应先通过`--cce-res-usage`编译选项查看寄存器使用情况（`Stack size`大于0即表明溢出），再根据寄存器与最大线程数的对应关系选择一档能满足单线程寄存器需求的最大线程数。

利用率优化中存在权衡：增大线程块、配置较高的最大线程数可以提高并行度，但过多的线程块会累积调度开销，过高的最大线程数会压缩每线程寄存器个数引发溢出。因此应结合实际算子计算复杂度、数据规模，通过实测找到最优配置。

## 最大化存储吞吐量

最大化存储吞吐量的核心是减少低带宽的数据搬运，让每一次访存都尽可能搬运有效数据。最高优先级是减少与低带宽存储（Global Memory）之间的数据传输，并提高每次GM访问的有效带宽利用率；其次是充分利用片上的Data Cache（DCache）和UB，用高带宽的片上访问替代低带宽的GM访问。在Ascend C SIMT中，存储吞吐量优化通常是最高优先级的优化方向，主要包括以下手段。

- **GM上的访存合并（Memory Coalescing）**

  这是SIMT访存优化的首要分析点。GM上的读写内存操作是以128字节的Cache Line的粒度进行的，应尽量让每次Cache Line的访存操作能够处理更多的有效数据，而一个Warp内的相邻线程通常在同一条访存指令上同时发起GM访问，若这些线程访问的GM地能落在同一个Cache Line里，硬件可将请求合并为更少的访存事务，显著提高带宽利用率；若地址跨行或离散，则难以合并，耗时明显增加。优化方法有两类：一是调整数据切分方式，将连续数据区域分配给线程块和Warp，使Warp内相邻线程在每次迭代访问连续地址；二是对转置、重排等布局变换算子，引入UB作中转重排，从而保证GM两侧访问连续。

- **使用短向量类型提升带宽利用率**

  AI处理器以128字节的Cache Line访问L2 Cache。处理2字节数据类型（如half）时，单个Warp（32线程）合并访存的有效数据宽度仅为64字节，Cache Line占用不满，带宽利用率仅50%。通过使用短向量类型（如half2）进行搬运和计算，单个Warp单次访存可覆盖整行Cache Line，带宽利用率达到100%，同时精简指令发射数量。

- **通过类型对齐提升搬运效率**

  SIMT编程模式下，支持1B、2B、4B、8B四种位宽的访存指令。结构体的对齐属性（`alignof`）决定了编译器能选用的访存指令位宽。在数据总量不变时，通过`alignas`将结构体对齐到更大的位宽，编译器可以用更宽位宽的访存指令完成搬运，降低搬运耗时。

- **DCache访问优化，提高热点数据缓存命中**

  SIMT经由DCache访问GM数据。当算子中同时存在仅遍历一次的输入数据与需反复访问的热点数据（如查找表）时，若不加区分，热点数据会被其他数据挤出DCache，后续访问需重新从GM加载。可通过访存函数为不同数据指定缓存策略：`asc_ldcg`用于仅遍历一次的数据以降低其Cache占用，`asc_ldca`用于热点数据使其常驻DCache，`asc_stcg`用于写后无需驻留的输出数据。

- **UB上的访存合并与Bank冲突**

  UB作为片上缓存，访存效率远高于GM，通常会被用作数据中转。当算子使用UB中转做转置、重排时，即使GM读写已经连续，仍需关注UB内部的访问模式，既要考虑UB的访存合并，也要尽量减少UB Bank冲突概率。UB划分为多个bank并组织为bank group，若同一个Warp内的并发线程集中访问少数bank或bank group，会产生bank冲突（读写、写写、读读冲突），导致访问请求排队、吞吐下降。常用手段是为UB二维数组增加padding，打散列方向访问的bank分布，降低冲突规模。

存储吞吐量优化的一般分析顺序是：先考虑是否能用访存合并保证Warp维度的GM访问连续，部分场景可引入UB中转达到减少GM访存次数或优化GM访存顺序；再分析能否通过短向量类型和类型对齐提升单次访存的有效数据量。引入UB中转的场景下需要考虑UB的访存合并和UB的bank冲突。少数有热点数据场景，可考虑优化DCache访存方式提升片上缓存的命中率。

## 最大化指令吞吐量

最大化指令吞吐量的目标是降低线程执行过程中的无效指令开销，提高计算指令的执行效率，主要包括减少高开销指令、冗余计算和分支发散。

在Ascend C SIMT中，指令吞吐量优化的常用方向包括：

- **控制分支发散**：同一个Warp内的线程若走不同的控制流分支，会因发散而串行执行不同分支，降低指令吞吐。应尽量减少Warp内线程间的分支差异，必要时通过算法重构将数据相关的分支转换为统一的计算逻辑。
- **减少高开销指令的使用**：除法、取模、同步、原子操作等都属于高开销指令，应尽量减少其使用频次。对于除数固定或可预计算的整数除法场景，可用乘法加移位的快速算法替代除法/取模；对于必须使用原子操作做聚合的场景，应优先做分层聚合——先在寄存器或UB上完成局部聚合，再在GM上做少量汇总，因为GM上的`asc_atomic_add`性能远低于UB。
- **选择高性能指令**：在语义等价且满足数据范围要求的前提下，优先选择执行开销较低的指令形式。以`asc_atomic_add`为例，若32位数据类型能够覆盖目标取值范围，应优先使用32位原子操作，避免引入64位原子操作的额外开销；若调用方不依赖原子操作返回值，应避免使用该返回值，便于底层生成更高效的指令序列。
- **配合执行配置避免溢出引入的额外指令开销**：如前文所述，寄存器溢出会把中间数据存取转为对GM栈空间的访问，既增加存储开销也增加指令数量；合理配置`__launch_bounds__`避免溢出，同样有利于指令吞吐。

> 说明：通常在存储吞吐量已较充分、瓶颈转移到计算时重点考虑指令吞吐量优化。SIMT编程模式本身适合表达线程级的复杂分支与离散逻辑，该场景下需要重点关注分支发散的优化。

## SIMT性能优化案例列表

下表汇总当前SIMT算子性能优化的案例，并按优化层次归类，每个案例均配套可运行的最佳实践样例。

表1 最大化利用率（执行配置）

| 优化建议 | 优先级 | 样例 |
| --- | :---: | --- |
| [线程块数量配置优化](./execution_config/thread_block_config.md) | 高 | [grid_dim_config](../../../../../examples/03_simt_api/03_best_practices/01_execution_conf_optimizations/grid_dim_config) |
| [合理配置最大线程数避免寄存器溢出](./execution_config/thread_count_register_overflow.md) | 中 | [max_thread_config](../../../../../examples/03_simt_api/03_best_practices/01_execution_conf_optimizations/max_thread_config) |

表2 最大化存储吞吐量（内存访问）

| 优化建议 | 优先级 | 样例 |
| --- | :---: | --- |
| [访存合并](./memory_access/memory_access_merge.md) | 高 | [matrix_transpose_practice](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice) |
| [避免UB的Bank冲突](./memory_access/avoid_ub_bank_conflict_simt.md) | 高 | [matrix_transpose_practice](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice) |
| [使用短向量类型提升效率](./memory_access/short_vector_efficiency.md) | 中 | [short_vector_add](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/short_vector_add) |
| [通过类型对齐提升搬运效率](./memory_access/type_alignment_transfer.md) | 中 | [aligned_types](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/aligned_types) |
| [DCache访问优化](./memory_access/dcache_access.md) | 中 | [cache_hint](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/cache_hint) |

表3 最大化指令吞吐量（控制流）

| 优化建议 | 优先级 | 样例 |
| --- | :---: | --- |
| [同一Warp内避免分支跳转](./control_flow/avoid_warp_branch.md) | 高 | [warp_divergence](../../../../../examples/03_simt_api/03_best_practices/02_control_flow/warp_divergence) |

表4 最大化指令吞吐量（指令优化）

| 优化建议 | 优先级 | 样例 |
| --- | :---: | --- |
| [原子操作指令优化](./instruction_optimization/atomic_instruction_optimization.md) | 高 | [histogram](../../../../../examples/03_simt_api/03_best_practices/03_instruction_optimizations/atomic_histogram)<br>[atomic_add_perf](../../../../../examples/03_simt_api/02_features/01_api_features/02_atomic_operation/atomic_add_perf) |
