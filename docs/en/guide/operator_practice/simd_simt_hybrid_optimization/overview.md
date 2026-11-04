# SIMD and SIMT Hybrid Operator Performance Optimization Overview

<!-- md-trans-meta sourceCommit=f83e857605e3844db8b9de0442007cc115278a47 translatedAt=2026-10-23T13:02:32.837Z pushedAt=2026-11-03T07:05:13.361Z nl=2 -->

SIMD and SIMT hybrid programming on the AI Core combines the multi-data parallel computing capability of SIMD with the discrete memory access and complex branch handling capability of SIMT. It allows SIMD and SIMT execution logic to be flexibly organized within the same operator, using the Vector Function (VF) as the basic scheduling unit. SIMD programming is suitable for processing continuous and regular vector or cube computations, while SIMT programming is suitable for processing irregular logic such as branch judgment, index mapping, and discrete memory access. By combining the two programming approaches in a program, both high hardware throughput and programming flexibility can be achieved.

This section mainly describes the performance optimization methods for SIMT VF in SIMD and SIMT hybrid programming, helping developers determine the optimization level of SIMT VF segments based on performance data and further select an appropriate optimization direction. During optimization, analyze the theoretical data first, then confirm the bottleneck through actual performance data, and finally optimize in a targeted manner from aspects such as memory access, execution control, and instruction optimization.

## Analyzing Performance Data

The core of performance analysis is to first establish a theoretical baseline, and then use measured data to observe the gap between the current implementation and the theoretical expectation.

1. Theoretical performance evaluation

   Theoretical performance evaluation is used to establish the ideal memory access scale of the operator and the upper limit of the device GM (Global Memory) bandwidth. During analysis, estimate the theoretical read data volume and theoretical write data volume required for the operator to complete one computation based on the operator semantics, data volume, and data type. The calculation is usually the number of accessed elements multiplied by the byte size of the data type. This data volume represents the data scale that the algorithm itself must process, and serves as the baseline for determining whether the actual read and write volumes are reasonable. After obtaining the theoretical read and write data volumes, obtain the theoretical GM bandwidth of the device by referring to the hardware specifications in the product documentation. The theoretical bandwidth represents the ideal upper limit of the memory access path, and can be used to evaluate the bandwidth utilization level and optimization space of the current operator implementation.

2. Actual data measurement

   Actual data measurement is used to observe the GM access behavior during operator execution, as well as the proportion of SIMT VF in the AIV core execution time. You can use the msOpProf tool in [Operator Development Tool](https://gitcode.com/Ascend/msopprof/blob/26.2.0/docs/en/user_guide/msopprof_user_guide.md) to collect performance data:
   - View the overall GM read/write bandwidth and read/write data volume of the AIV core through `Memory.csv`. The related performance data fields are: `aiv_main_mem_read_bw`, `aiv_main_mem_write_bw`, `read_main_memory_datas`, `write_main_memory_datas`.
   - View the execution time of the operator on the AIV core and the cycle proportion of SIMT VF type instructions through `ArithmeticUtilization.csv`. The related performance data fields are: `aiv_time`, `aiv_vec_simt_vf_ratio`.

   In SIMD and SIMT hybrid programming, `aiv_time` covers the entire AIV core execution interval. When analyzing the performance of SIMT VF itself, use `aiv_vec_simt_vf_ratio` to convert the SIMT VF execution time:

   ```text
   SIMT VF execution time = aiv_time * aiv_vec_simt_vf_ratio
   ```

   Here, `aiv_vec_simt_vf_ratio` indicates the proportion of SIMT VF instruction cycles to the total cycles executed on the AIV core.

   `read_main_memory_datas` and `write_main_memory_datas` are the GM read and write data volumes during operator execution, and not all of them are necessarily read/write operations triggered by SIMT VF. When calculating the actual SIMT VF bandwidth, use the SIMT VF-related GM read/write data volume:

   ```text
   SIMT VF actual bandwidth = SIMT VF-related GM read/write data volume / SIMT VF execution time
   ```

   If the same kernel function also contains SIMD VF, MTE transfers, or GM read/write operations in other stages, and it cannot be determined whether the GM read/write data is triggered by SIMT VF, do not directly divide the AIV core overall GM read/write volume by the SIMT VF execution time. In this case, use `read_main_memory_datas`, `write_main_memory_datas`, `aiv_main_mem_read_bw`, and `aiv_main_mem_write_bw` as references for the AIV core overall GM path pressure, and then determine the source of the bottleneck by combining the code structure, `aiv_vec_simt_vf_ratio`, and the MTE proportion. If the GM data read within SIMT VF passes through the Data Cache and hits, the measured GM read data volume may be smaller than the theoretical read data volume, which usually indicates that some read accesses are reused by the cache.

## Designing an Optimization Solution

After completing the theoretical performance evaluation and actual data measurement, you should first analyze the GM read/write bandwidth and the actual amount of data read and written, and then decide the optimization direction. For SIMD and SIMT hybrid operators, optimization can generally be carried out from four aspects: memory access, execution control, control flow, and instruction optimization.

- Memory access optimization

  Memory access optimization is the highest-priority optimization direction. In hybrid programming, the SIMT VF is often used to handle irregular logic such as scattered memory access and index mapping. The purpose of memory access optimization is to use memory access coalescing in the SIMT VF to reduce invalid GM access and improve effective bandwidth and data reuse.

  In SIMT programming, if the GM access requests from threads in the same Warp fall within the same region (sector), they can be coalesced into a single memory request. Scattered access causes access requests to be distributed across multiple sectors, resulting in low memory access efficiency. During optimization, you should first adjust the mapping between threads and data so that threads in the same Warp access GM addresses that are as close or contiguous as possible, thereby improving the effective bandwidth utilization. For scattered data scenarios in the SIMT VF where contiguous GM access is difficult, you can first use the MTE transfer interface to move the relevant data from GM to the Unified Buffer (UB) cache, and then access the UB data in the SIMT VF to reduce the overhead of scattered GM access and improve bandwidth utilization. During analysis, you should also pay attention to the change in the amount of read data caused by Data Cache hits, and whether UB access introduces new conflicts.

- Execution control optimization

  The goal of execution control optimization is to match the number of threads, the number of thread blocks, and the SIMT VF task volume with the actual workload. Developers should reasonably set the number of SIMT VF threads, `__launch_bounds__`, and the data partitioning method based on the input/output scale and the thread mapping relationship, so that the threads cover the actual output scale and the load is balanced across cores as much as possible. In addition, in SIMD and SIMT hybrid operators, you should also pay attention to the number of VF calls. If the VF is called too many times, unnecessary scheduling overhead is introduced, which affects the overall execution efficiency.

- Control flow optimization

  The goal of control flow optimization is to reduce branch divergence within the SIMT VF and improve the execution efficiency of threads within a Warp. Flow control instructions (if, switch, do, for, while) may cause threads in the same Warp to enter different execution paths, resulting in branch divergence. Within a SIMT VF function, you should try to make threads in the same Warp follow the same execution path, so as to reduce the serialized execution overhead caused by branch divergence.

- Instruction optimization

  Instruction optimization is typically prioritized when the computation is a bottleneck, or when memory access is already sufficient but performance still falls short of expectations. Its purpose is to reduce high-cost instructions and redundant computation within the SIMT VF, thereby improving the execution efficiency of thread-level computation logic. By optimizing the computation logic, the use of high-cost instructions such as division and modulo is reduced, and logic that can be computed in advance or reused is prevented from being repeatedly executed within a thread.

## Optimization Suggestion Overview Table

Table 1 Compute optimization suggestion overview table

| Optimization Suggestion | Example |
| --- | --- |
| [Implementing Branch Judgment Through SIMT](./compute_optimization/simt_branch_judgment.md) | [simd_simt_high_performance](../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance) |

Table 2 Memory access optimization suggestion overview table

| Optimization Suggestion | Example |
| --- | --- |
| [Using Unified Buffer to Improve Memory Access Efficiency](./memory_access/ub_memory_access.md) | [simd_simt_high_performance](../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance) |
| [Memory Access Coalescing](../simt_operator_optimization/memory_access/memory_access_merge.md) | [simd_simt_matrix_transpose](../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_matrix_transpose) |
| [Avoiding UB Bank Conflicts](../simt_operator_optimization/memory_access/avoid_ub_bank_conflict_simt.md) | [matrix_transpose_practice](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice) |
| [DCache Access Optimization](../simt_operator_optimization/memory_access/dcache_access.md) | [cache_hint](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/cache_hint) |
| [Using Short Vector Types to Improve Efficiency](../simt_operator_optimization/memory_access/short_vector_efficiency.md) | [short_vector_add](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/short_vector_add) |
| [Improving Transfer Efficiency Through Type Alignment](../simt_operator_optimization/memory_access/type_alignment_transfer.md) | [aligned_types](../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/aligned_types) |

Table 3 Execution control optimization suggestion overview table

| Optimization Suggestion | Example |
| --- | --- |
| [Thread Block Count Configuration Optimization](./execution_control/thread_block_config.md) | [simd_simt_grid_dim_config](../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_grid_dim_config) |

Table 4 Control flow optimization suggestion overview table

| Optimization Suggestion | Example |
| --- | --- |
| [Avoiding Branch Jumps Within the Same Warp](../simt_operator_optimization/control_flow/avoid_warp_branch.md) | [warp_divergence](../../../../../examples/03_simt_api/03_best_practices/02_control_flow/warp_divergence) |

Table 5 Instruction optimization suggestion overview table

| Optimization Suggestion | Example |
| --- | --- |
| [Fast algorithm for integer division](./instruction_optimization/fast_integer_division.md) | [simd_simt_integer_fast_div](../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_integer_fast_div) |
