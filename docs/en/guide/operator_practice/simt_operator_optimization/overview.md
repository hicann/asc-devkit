# Overview

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T13:08:30.810Z pushedAt=2026-11-03T07:05:13.369Z nl=2 -->

## Overall Performance Optimization Strategy

The SIMT programming model uses the thread as the basic programming unit. Developers write the execution logic of a single thread, and then organize a large number of threads into thread blocks and a grid through the `<<<gridDim, blockDim>>>` configuration, scheduling them to the Vector Core (physical core, that is, the AIV core) of the hardware for parallel execution in units of warps.

The overall goal of performance optimization is to make the operator run as efficiently as possible on the hardware, with the core being to keep all kinds of hardware resources in a busy and effective state. SIMT operator optimization mainly revolves around the following three basic strategies:

- **Maximize utilization**: Make as many physical cores and threads work in parallel as possible, so that the parallel computing capability of the hardware is fully leveraged, avoiding idle physical cores or threads waiting in queues.
- **Maximize storage throughput**: Minimize low-bandwidth data movement, so that each Global Memory (GM) access moves as much effective data as possible, fully utilizing the GM bandwidth, cache, and Unified Buffer (UB).
- **Maximize instruction throughput**: Reduce high-overhead instructions, redundant computation, and branch divergence within threads, and increase the proportion of effective execution of compute instructions.

The degree to which these three strategies affect operator performance varies by operator. You should determine the main bottleneck based on measured performance data before applying targeted optimization. The general optimization process is as follows: first establish a theoretical performance baseline (theoretical read/write data volume and theoretical GM bandwidth), then use tools such as msOpProf to collect actual performance data (Task Duration, GM read/write bandwidth, DCache access count, aiv_vec_ratio, etc.), compare the theoretical and measured values to locate the bottleneck, and finally select optimization methods from the corresponding level and verify the optimization effect through performance data. The optimization methods for the three levels are described separately below.

## Maximizing Utilization

The goal of maximizing utilization is to fully leverage the hardware parallelism. Utilization optimization for SIMT operators can be understood at three levels: application level, device level, and vector core level. The application level focuses on whether the kernel launch configuration allows enough vector cores to work in parallel. The device level and vector core level focus on whether the threads and warps within a single vector core remain busy, avoiding parallelism degradation caused by resource constraints (such as registers). In Ascend C SIMT, parallel optimization of vector core resources is mainly achieved through the kernel execution configuration (`gridDim`, `blockDim`, `__launch_bounds__`).

- **Properly configure the thread block count to balance the number of parallel cores and scheduling overhead**

  Thread blocks are ultimately scheduled to vector cores for execution. A vector core can host and execute only one thread block at a time. Too few thread blocks lead to idle vector cores, insufficient parallelism, and an excessive workload on a single core. When the number of thread blocks far exceeds the number of vector cores, the excess blocks must wait in a queue for preceding blocks to finish, and the fixed overhead of launching and scheduling multiple thread blocks accumulates noticeably. Therefore, the thread block count should be set based on the number of vector cores and the data scale.

- **Properly configure the maximum thread count to avoid register spilling that reduces effective utilization**

  The maximum thread count specified by `__launch_bounds__` in the kernel determines the number of registers available to each thread: the larger the maximum thread count, the fewer registers each thread can be allocated. For compute-intensive operators, if the thread count is configured too high and causes insufficient registers per thread, intermediate data spills to the stack space in Global Memory, introducing additional GM access and actually reducing effective utilization. You should first check register usage with the `--cce-res-usage` compilation option (a `Stack size` greater than 0 indicates spilling), and then select a maximum thread count that satisfies the per-thread register requirement based on the relationship between registers and the maximum thread count.

There is a trade-off in utilization optimization: increasing the thread block size and configuring a higher maximum thread count can improve parallelism, but too many thread blocks accumulate scheduling overhead, and an excessively high maximum thread count compresses the per-thread register count and causes spilling. Therefore, you should find the optimal configuration through actual measurement based on the computational complexity and data scale of the operator.

## Maximizing Storage Throughput

The core of maximizing storage throughput is to reduce low-bandwidth data movement so that each memory access moves as much effective data as possible. The highest priority is to reduce data transfer with low-bandwidth storage (Global Memory) and improve the effective bandwidth utilization of each GM access; the next is to make full use of the on-chip Data Cache (DCache) and UB, replacing low-bandwidth GM access with high-bandwidth on-chip access. In Ascend C SIMT, storage throughput optimization is usually the highest-priority optimization direction, and it mainly includes the following methods.

- **Memory coalescing on GM**

  This is the primary analysis point for SIMT memory access optimization. Read and write memory operations on GM are performed at the granularity of a 128-byte Cache Line. You should try to make each Cache Line access operation process more effective data. Adjacent threads within a Warp usually initiate GM access simultaneously on the same memory access instruction. If the GM addresses accessed by these threads fall within the same Cache Line, the hardware can merge the requests into fewer memory access transactions, significantly improving bandwidth utilization; if the addresses span lines or are scattered, they are difficult to merge, and the latency increases noticeably. There are two optimization methods: one is to adjust the data partitioning method, assigning contiguous data regions to thread blocks and Warps so that adjacent threads within a Warp access contiguous addresses in each iteration; the other is, for layout transformation operators such as transpose and rearrangement, to introduce UB as an intermediate buffer for rearrangement, thereby ensuring contiguous access on both sides of GM.

- **Using short vector types to improve bandwidth utilization**

  The AI processor accesses L2 Cache in 128-byte Cache Lines. When processing 2-byte data types (such as half), the effective data width of coalesced access by a single Warp (32 threads) is only 64 bytes, so the Cache Line is not fully occupied and the bandwidth utilization is only 50%. By using short vector types (such as half2) for movement and computation, a single access by a single Warp can cover an entire Cache Line, achieving 100% bandwidth utilization while also reducing the number of instruction issues.

- **Improving movement efficiency through type alignment**

  In the SIMT programming model, memory access instructions of four bit widths are supported: 1B, 2B, 4B, and 8B. The alignment attribute (`alignof`) of a struct determines the memory access instruction bit width that the compiler can select. When the total amount of data remains unchanged, aligning the struct to a larger bit width using `alignas` allows the compiler to complete the movement with wider memory access instructions, reducing the movement time.

- **DCache access optimization to improve hot data cache hits**

  SIMT accesses GM data through DCache. When an operator contains both input data that is traversed only once and hot data that needs to be accessed repeatedly (such as a lookup table), if they are not distinguished, the hot data will be evicted from DCache by other data, and subsequent accesses will need to be reloaded from GM. You can specify cache policies for different data through memory access functions: `asc_ldcg` is used for data traversed only once to reduce its Cache occupancy, `asc_ldca` is used for hot data to keep it resident in DCache, and `asc_stcg` is used for output data that does not need to remain resident after being written.

- **Memory access coalescing and bank conflicts on UB**

  As an on-chip cache, UB offers much higher memory access efficiency than GM and is typically used as a data relay. When an operator uses UB as a relay for transposition or reordering, even if GM reads and writes are already contiguous, you still need to pay attention to the access pattern inside UB. You must consider both UB memory access coalescing and minimizing the probability of UB bank conflicts. UB is divided into multiple banks organized into bank groups. If concurrent threads within the same Warp concentrate their accesses on a small number of banks or bank groups, bank conflicts (read-write, write-write, and read-read conflicts) occur, causing access requests to be queued and throughput to degrade. A common approach is to add padding to UB two-dimensional arrays to scatter the bank distribution of column-direction accesses and reduce the scale of conflicts.

The general analysis sequence for storage throughput optimization is as follows: first, consider whether memory access coalescing can be used to ensure contiguous GM access at the Warp level; in some scenarios, UB can be introduced as a relay to reduce the number of GM accesses or optimize the GM access order. Then, analyze whether short vector types and type alignment can be used to increase the amount of effective data per access. In scenarios where UB is introduced as a relay, UB memory access coalescing and UB bank conflicts must be considered. In the few scenarios with hot data, you can consider optimizing the DCache access pattern to improve the on-chip cache hit rate.

## Maximizing Instruction Throughput

The goal of maximizing instruction throughput is to reduce the overhead of ineffective instructions during thread execution and improve the execution efficiency of compute instructions. This mainly involves reducing high-overhead instructions, redundant computation, and branch divergence.

In Ascend C SIMT, common approaches to instruction throughput optimization include:

- **Control branch divergence**: If threads within the same Warp take different control flow branches, they execute different branches serially due to divergence, reducing instruction throughput. Minimize branch differences among threads within a Warp, and when necessary, restructure the algorithm to convert data-dependent branches into unified computation logic.
- **Reduce the use of high-overhead instructions**: Division, modulo, synchronization, and atomic operations are all high-overhead instructions, so their usage frequency should be minimized. For integer division scenarios where the divisor is fixed or can be precomputed, use the fast algorithm of multiplication plus shift to replace division/modulo. For scenarios where atomic operations are required for aggregation, prioritize hierarchical aggregation: first perform local aggregation in registers or UB, then perform a small amount of summarization in GM, because the performance of `asc_atomic_add` in GM is far lower than that in UB.
- **Select high-performance instructions**: When instructions are semantically equivalent and satisfy the data range requirements, prioritize instruction forms with lower execution overhead. Taking `asc_atomic_add` as an example, if the 32-bit data type can cover the target value range, prioritize the 32-bit atomic operation to avoid the extra overhead of the 64-bit atomic operation. If the caller does not depend on the return value of the atomic operation, avoid using the return value to allow the underlying layer to generate a more efficient instruction sequence.
- **Avoid the extra instruction overhead introduced by spilling through execution configuration**: As described earlier, register spilling converts intermediate data access into access to the GM stack space, increasing both storage overhead and the number of instructions. Properly configuring `__launch_bounds__` to avoid spilling is also beneficial to instruction throughput.

> **Note:** Instruction throughput optimization is usually considered when storage throughput is already sufficient and the bottleneck shifts to computation. The SIMT programming model itself is suitable for expressing thread-level complex branches and discrete logic, and in such scenarios, special attention should be paid to optimizing branch divergence.

## SIMT Performance Optimization Cases

The following table summarizes the current SIMT operator performance optimization cases, categorized by optimization level. Each case is accompanied by a runnable best practice example.

Table 1 Maximizing utilization (execution configuration)

| Optimization Recommendations | Priority | Example |
| --- | :---: | --- |
| [Thread Block Count Configuration Optimization](./execution_config/thread_block_config.md) | High | [grid_dim_config](../../../../../examples/03_simt_api/03_best_practices/01_execution_conf_optimizations/grid_dim_config) |
| [Proper Configuration of Maximum Thread Count to Avoid Register Overflow](./execution_config/thread_count_register_overflow.md) | Medium | [max_thread_config](../../../../../examples/03_simt_api/03_best_practices/01_execution_conf_optimizations/max_thread_config) |

Table 2 Maximizing storage throughput (memory access)

| Optimization Recommendations | Priority | Example |
| --- | :---: | --- |
| [Memory Access Merge](./memory_access/memory_access_merge.md) | High | [matrix_transpose_practice](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice) |
| [Avoid UB Bank Conflicts](./memory_access/avoid_ub_bank_conflict_simt.md) | High | [matrix_transpose_practice](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice) |
| [Use Short Vector Types to Improve Efficiency](./memory_access/short_vector_efficiency.md) | Medium | [short_vector_add](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/short_vector_add) |
| [Improve Transfer Efficiency Through Type Alignment](./memory_access/type_alignment_transfer.md) | Medium | [aligned_types](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/aligned_types) |
| [DCache Access Optimization](./memory_access/dcache_access.md) | Medium | [cache_hint](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/cache_hint) |

Table 3 Maximizing instruction throughput (control flow)

| Optimization Recommendations | Priority | Example |
| --- | :---: | --- |
| [Avoiding Branch Jumps Within the Same Warp](./control_flow/avoid_warp_branch.md) | High | [warp_divergence](../../../../../examples/03_simt_api/03_best_practices/02_control_flow/warp_divergence) |

Table 4 Maximizing instruction throughput (instruction optimization)

| Optimization Recommendations | Priority | Example |
| --- | :---: | --- |
| [Atomic Operation Instruction Optimization](./instruction_optimization/atomic_instruction_optimization.md) | High | [histogram](../../../../../examples/03_simt_api/03_best_practices/03_instruction_optimizations/atomic_histogram)<br>[atomic_add_perf](../../../../../examples/03_simt_api/02_features/01_api_features/02_atomic_operation/atomic_add_perf) |
