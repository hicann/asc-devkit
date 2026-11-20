# Overview

<!-- md-trans-meta sourceCommit=74b0b309d83ca550795885a8c5110b383506a67b translatedAt=2026-11-09T10:42:54.942Z pushedAt=2026-11-19T06:32:19.836Z nl=2 -->

## Overall Performance Optimization Strategies

The SIMT programming model uses the thread as the basic programming unit. Developers write the execution logic of a single thread, and then organize a large number of threads into thread blocks and grids through the `<<<gridDim, blockDim>>>` configuration, scheduling them to the Vector Cores (physical cores, that is, AIV cores) of the hardware for parallel execution in units of warps.

The overall goal of performance optimization is to make operators run as efficiently as possible on the hardware, with the core being to keep all types of hardware resources busy and effective. SIMT operator optimization mainly revolves around the following three basic strategies:

- **Maximizing utilization**: Keep as many physical cores and threads working in parallel as possible, so that the parallel computing capability of the hardware is fully leveraged, avoiding idle physical cores or threads waiting in queues.
- **Maximizing memory throughput**: Minimize low-bandwidth data movement, so that each Global Memory (GM) access moves as much valid data as possible, fully utilizing GM bandwidth, cache, and Unified Buffer (UB).
- **Maximizing instruction throughput**: Reduce high-overhead instructions, redundant computation, and branch divergence within threads, improving the effective execution ratio of compute instructions.

The impact of these three strategies on operator performance varies by operator. You should determine the main bottleneck based on measured performance data before applying targeted optimization. The general optimization process is as follows: first establish a theoretical performance baseline (theoretical read/write data volume and theoretical GM bandwidth), then use tools such as msOpProf to collect actual performance data (task duration, GM read/write bandwidth, DCache access count, **aiv_vec_ratio**, and so on), compare the gap between theory and measurement to locate the bottleneck, and finally select optimization methods from the corresponding level and verify the optimization effect through performance data. The optimization methods for the three levels are described separately below.

## Maximizing Utilization

The goal of maximizing utilization is to fully leverage the hardware parallelism. Utilization optimization for SIMT operators can be understood at three levels: application level, device level, and physical core level. The application level focuses on whether the kernel launch configuration keeps enough physical cores working in parallel. The device level and physical core level focus on whether the threads and warps within a single physical core remain busy, avoiding parallelism degradation caused by resource constraints (such as registers). In Ascend C SIMT, parallel optimization of physical core resources is mainly achieved through the kernel execution configuration (`gridDim`, `blockDim`, `__launch_bounds__`).

- **Reasonable configuration of the number of thread blocks to balance parallel cores and schedulingoverhead**

  Thread blocks are ultimately scheduled to physical cores for execution. A physical core can host and execute only one thread block at a time. Too few thread blocks lead to idle physical cores, insufficient parallelism, and excessive workload on a single core. When the number of thread blocks far exceeds the number of physical cores, the excess blocks must wait in queue for preceding blocks to finish, and the fixed overhead of launching and scheduling multiple thread blocks accumulates noticeably. Therefore, the number of thread blocks should be set based on the number of physical cores and the data scale.

- **Reasonable configuration of the maximum number of threads to avoid register spilling that reduces effective utilization**

  The maximum number of threads specified by the kernel through `__launch_bounds__` determines the number of registers available to each thread: the larger the maximum number of threads, the fewer registers each thread can be allocated. For compute-intensive operators, if the thread count is configured too high and causes insufficient registers per thread, intermediate data spills to the stack space in Global Memory, introducing additional GM access and actually reducing effective utilization. You should first use the `--cce-res-usage` compilation option to check register usage (`Stack size` greater than 0 indicates spilling), and then select a maximum thread count that meets the per-thread register requirement based on the correspondence between registers and the maximum number of threads.

There is a trade-off in utilization optimization: increasing thread blocks and configuring a higher maximum number of threads can improve parallelism, but too many thread blocks accumulate scheduling overhead, and an excessively high maximum number of threads compresses the per-thread register count and causes spilling. Therefore, you should find the optimal configuration through actual measurement based on the computational complexity and data scale of the operator.

## Maximizing Storage Throughput

The core of maximizing storage throughput is to reduce low-bandwidth data movement so that every memory access moves as much valid data as possible. The highest priority is to reduce data transfer with low-bandwidth storage (Global Memory) and improve the effective bandwidth utilization of each GM access. The next priority is to make full use of the on-chip Data Cache (DCache) and UB, replacing low-bandwidth GM access with high-bandwidth on-chip access. In Ascend C SIMT, storage throughput optimization is usually the highest-priority optimization direction and mainly includes the following methods.

- **Memory access merging on GM**

  This is the primary analysis point for SIMT memory access optimization. Read and write memory operations on GM are performed at the granularity of a 128-byte cache line. You should try to make each cache line access operation process more valid data. Adjacent threads within a warp usually initiate GM access simultaneously on the same memory access instruction. If the GM addresses accessed by these threads fall within the same cache line, the hardware can merge the requests into fewer memory access transactions, significantly improving bandwidth utilization. If the addresses span lines or are scattered, they are difficult to merge and the latency increases noticeably. There are two types of optimization methods. First, adjust the data partitioning method to allocate contiguous data regions to thread blocks and warps, so that adjacent threads within a warp access contiguous addresses in each iteration. Second, for layout transformation operators such as transpose and reorder, introduce UB as an intermediate buffer for reordering, thereby ensuring contiguous access on both sides of GM.

- **Using short vector types to improve bandwidth utilization**

  The AI processor accesses L2 Cache in 128-byte cache lines. When processing 2-byte data types (such as half), the valid data width of coalesced memory access by a single warp (32 threads) is only 64 bytes, so the cache line is not fully occupied and the bandwidth utilization is only 50%. By using short vector types (such as half2) for movement and computation, a single memory access by a single warp can cover an entire cache line, achieving 100% bandwidth utilization while also reducing the number of issued instructions.

- **Improving transfer efficiency through type alignment**

  In the SIMT programming model, memory access instructions of five bit widths are supported: 1B, 2B, 4B, 8B, and 16B. The alignment attribute (`alignof`) of a structure determines the memory access instruction bit width that the compiler can select. When the total amount of data remains unchanged, using `alignas` to align the structure to a larger bit width allows the compiler to complete the movement with wider memory access instructions, reducing the movement latency.

- **Optimizing DCache access to improve hot data cache hits**

  SIMT accesses GM data through DCache. When an operator contains both input data that is traversed only once and hot data that is accessed repeatedly (such as a lookup table), if they are not distinguished, the hot data will be evicted from DCache by other data, and subsequent accesses will need to be reloaded from GM. You can specify cache policies for different data through memory access functions: `asc_ldcg` is used for data traversed only once to reduce its cache occupancy, `asc_ldca` is used for hot data to keep it resident in DCache, and `asc_stcg` is used for output data that does not need to remain resident after writing.

- **Memory access merging and bank conflicts on UB**

  As an on-chip cache, UB offers much higher memory access efficiency than GM and is typically used as a data relay. When an operator uses UB as a relay for transposition or reordering, even if GM reads and writes are already contiguous, you still need to pay attention to the access pattern within UB, considering both UB memory access merging and minimizing the probability of UB bank conflicts. UB is divided into multiple banks and organized into bank groups. If concurrent threads within the same warp concentrate their accesses on a small number of banks or bank groups, bank conflicts (read-write, write-write, and read-read conflicts) occur, causing access requests to be queued and throughput to drop. A common approach is to add padding to UB two-dimensional arrays to disperse the bank distribution of column-direction accesses and reduce the scale of conflicts.

The general analysis order for storage throughput optimization is as follows: first consider whether memory access merging can be used to ensure contiguous GM access at the warp dimension; in some scenarios, UB relay can be introduced to reduce the number of GM accesses or optimize the GM access order. Then analyze whether short vector types and type alignment can improve the amount of valid data per access. In scenarios where UB relay is introduced, UB memory access merging and UB bank conflicts need to be considered. In a few scenarios with hot data, consider optimizing the DCache access method to improve the on-chip cache hit rate.

## Maximizing Instruction Throughput

The goal of maximizing instruction throughput is to reduce the overhead of ineffective instructions during thread execution and improve the execution efficiency of compute instructions. This mainly involves reducing high-overhead instructions, redundant computation, and branch divergence.

In Ascend C SIMT, common directions for instruction throughput optimization include:

- **Controlling branch divergence**: If threads within the same warp take different control flow branches, they execute different branches serially due to divergence, reducing instruction throughput. Minimize branch differences among threads within a warp, and when necessary, restructure the algorithm to convert data-dependent branches into unified computation logic.
- **Reducing the use of high-overhead instructions**: Division, modulo, synchronization, and atomic operations are all high-overhead instructions, and their frequency of use should be minimized. For integer division scenarios where the divisor is fixed or can be precomputed, use the fast algorithm of multiplication plus shift to replace division/modulo. For scenarios where atomic operations must be used for aggregation, prioritize hierarchical aggregation: first perform local aggregation in registers or UB, then perform a small amount of summarization in GM, because the performance of `asc_atomic_add` in GM is far lower than that in UB.
- **Selecting high-performance instructions**: On the premise of semantic equivalence and satisfying the data range requirements, prioritize instruction forms with lower execution overhead. Taking `asc_atomic_add` as an example, if a 32-bit data type can cover the target value range, prioritize 32-bit atomic operations to avoid the extra overhead of 64-bit atomic operations. If the caller does not depend on the return value of the atomic operation, avoid using the return value so that the underlying layer can generate a more efficient instruction sequence.
- **Coordinating with execution configuration to avoid extra instruction overhead introduced by spilling**: As mentioned earlier, register spilling converts intermediate data access into access to the GM stack space, increasing both storage overhead and the number of instructions. Reasonably configuring `__launch_bounds__` to avoid spilling also benefits instruction throughput.

> **Note:** Instruction throughput optimization is usually prioritized when storage throughput is already sufficient and the bottleneck shifts to computation. The SIMT programming model itself is suitable for expressing thread-level complex branches and discrete logic, and in such scenarios, special attention should be paid to branch divergence optimization.

## SIMT Performance Optimization Case List

The following table summarizes the current SIMT operator performance optimization cases and categorizes them by optimization level. Each case is accompanied by a runnable best-practice example.

Table 1 Maximizing utilization (execution configuration)

| Optimization Recommendation | Priority | Example |
| --- | :---: | --- |
| [Thread Block Count Configuration Optimization](./execution_config/thread_block_config.md) | High | [grid_dim_config](../../../../../examples/03_simt_api/03_best_practices/01_execution_conf_optimizations/grid_dim_config) |
| [Configuring the Thread Count Properly to Avoid Register Overflow](./execution_config/thread_count_register_overflow.md) | Medium | [max_thread_config](../../../../../examples/03_simt_api/03_best_practices/01_execution_conf_optimizations/max_thread_config) |

Table 2 Maximizing storage throughput (memory access)

| Optimization Recommendation | Priority | Example |
| --- | :---: | --- |
| [Memory Access Merging](./memory_access/memory_access_merge.md) | High | [matrix_transpose_practice](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice) |
| [Avoiding UB Bank Conflicts](./memory_access/avoid_ub_bank_conflict_simt.md) | High | [matrix_transpose_practice](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice) |
| [Improving Efficiency with Short Vector Types](./memory_access/short_vector_efficiency.md) | Medium | [short_vector_add](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/short_vector_add) |
| [Improving Transfer Efficiency Through Type Alignment](./memory_access/type_alignment_transfer.md) | Medium | [aligned_types](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/aligned_types) |
| [DCache Access Optimization](./memory_access/dcache_access.md) | Medium | [cache_hint](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/cache_hint) |
| [Improving Hash Table Lookup Efficiency with SoA Layout](./memory_access/soa_layout_efficiency.md) | Medium | [hashtable_lookup](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/hashtable_lookup) |

Table 3 Maximizing instruction throughput (control flow)

| Optimization Recommendation | Priority | Example |
| --- | :---: | --- |
| [Avoiding Branch Divergence Within the Same Warp](./control_flow/avoid_warp_branch.md) | High | [warp_divergence](../../../../../examples/03_simt_api/03_best_practices/02_control_flow/warp_divergence) |
| [Branch Handling](./control_flow/branch_handing.md) | Medium | [predicate_optimization](../../../../../examples/03_simt_api/03_best_practices/02_control_flow/predicate_optimization) |

Table 4 Maximizing instruction throughput (instruction optimization)

| Optimization Recommendation | Priority | Example |
| --- | :---: | --- |
| [Atomic Operation Instruction Optimization](./instruction_optimization/atomic_instruction_optimization.md) | High | [histogram](../../../../../examples/03_simt_api/03_best_practices/03_instruction_optimizations/atomic_histogram)<br>[atomic_add_perf](../../../../../examples/03_simt_api/02_features/01_api_features/02_atomic_operation/atomic_add_perf) |
| [In-place Add Instruction Optimization](./instruction_optimization/inplace_add_instruction_optimization.md) | Medium | [inplace_add_atomic](../../../../../examples/03_simt_api/03_best_practices/03_instruction_optimizations/inplace_add_atomic) |
