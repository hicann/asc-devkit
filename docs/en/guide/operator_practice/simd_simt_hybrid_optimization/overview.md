# SIMD and SIMT Hybrid Operator Performance Optimization Overview

<!-- md-trans-meta sourceCommit=195cf801df3a15b430fb9afd7dda8149cb021a63 translatedAt=2026-08-26T14:38:51.735Z -->

Hybrid SIMD and SIMT programming on the AI Core combines the multi-data parallel computing capability of SIMD with the discrete memory access and complex branch handling capability of SIMT. It allows SIMD and SIMT execution logic to be flexibly organized within the same operator using Vector Function (VF) as the basic scheduling unit. SIMD programming is suitable for processing continuous and regular vector or matrix computations, while SIMT programming is suitable for processing irregular logic such as branch determination, index mapping, and discrete memory access. Using both programming approaches together in a program balances high hardware throughput and programming flexibility.

This section mainly describes the performance optimization methods for SIMT VFs in hybrid SIMD and SIMT programming, helping developers determine the optimization level of SIMT VF segments based on performance data and further select an appropriate optimization direction. During optimization, analyze the theoretical data first, then confirm the bottleneck through actual performance data, and finally optimize in a targeted manner from aspects such as memory access, execution control, and instruction optimization.

## Analyzing Performance Data

The core of performance analysis is to first establish a theoretical baseline, and then use measured data to observe the gap between the current implementation and the theoretical expectation.

1. Theoretical performance evaluation

   Theoretical performance evaluation is used to establish the ideal memory access scale of an operator and the upper limit of the device GM (Global Memory) bandwidth. During analysis, first estimate the theoretical read data volume and theoretical write data volume required for the operator to complete one computation based on the operator semantics, data volume, and data type. The calculation is typically the number of accessed elements multiplied by the byte size of the data type. This data volume represents the data scale that the algorithm itself must process, and serves as the baseline for determining whether the actual reads and writes are reasonable. After obtaining the theoretical read and write data volumes, obtain the theoretical GM bandwidth of the device based on the hardware specifications in the product documentation. The theoretical bandwidth represents the ideal upper limit of the memory access path, and can be used to evaluate the bandwidth utilization level and optimization space of the current operator implementation.

2. Actual data measurement

   Actual data measurement is used to observe the GM access behavior during operator execution, as well as the proportion of SIMT VF in the AIV core execution time. You can use the [msOpProf tool](https://gitcode.com/Ascend/msopprof/blob/26.1.0/docs/en/user_guide/msopprof_user_guide.md) to collect performance data:
   - View the overall GM read/write bandwidth and read/write data volume of the AIV core through `Memory.csv`. The related performance data fields are: `aiv_main_mem_read_bw`, `aiv_main_mem_write_bw`, `read_main_memory_datas`, and `write_main_memory_datas`.
   - View the execution time of the operator on the AIV core and the cycle ratio of SIMT VF type instructions through `ArithmeticUtilization.csv`. The related performance data fields are: `aiv_time` and `aiv_vec_simt_vf_ratio`.

   In SIMD and SIMT hybrid programming, `aiv_time` covers the entire AIV core execution interval. When analyzing the performance of SIMT VF itself, use `aiv_vec_simt_vf_ratio` to convert the SIMT VF execution time:

   ```text
   SIMT VF execution time = aiv_time * aiv_vec_simt_vf_ratio
   ```

   Here, `aiv_vec_simt_vf_ratio` indicates the ratio of the cycle count of SIMT VF type instructions to the total cycle count executed on the AIV core.

   `read_main_memory_datas` and `write_main_memory_datas` are the GM read and write data volumes when the operator is executed, and they are not necessarily all read/write operations triggered by SIMT VF. When calculating the actual bandwidth of SIMT VF, use the GM read/write data volume related to SIMT VF for the calculation:

   ```text
   SIMT VF actual bandwidth = SIMT VF-related GM read-write data size / SIMT VF execution time
   ```

   If the same kernel also contains GM reads/writes from SIMD VF, MTE transfers, or other stages, and it is impossible to determine whether the GM read/write data is triggered by SIMT VF, do not directly divide the overall GM read/write volume of the AIV kernel by the SIMT VF execution time. In this case, use `read_main_memory_datas`, `write_main_memory_datas`, `aiv_main_mem_read_bw`, and `aiv_main_mem_write_bw` as a reference for the overall GM path pressure of the AIV kernel, and then determine the bottleneck source by combining the code structure, `aiv_vec_simt_vf_ratio`, and the MTE ratio. If the GM data read within SIMT VF passes through the Data Cache and hits, the measured GM read data volume may be smaller than the theoretical read data volume, which usually indicates that some read accesses are reused by the cache.

## Designing an Optimization Solution

After completing the theoretical performance evaluation and actual data measurement, prioritize analyzing the GM read/write bandwidth and the actual read/write data volume before deciding on the optimization direction. For SIMD and SIMT hybrid operators, optimization can generally be carried out from three aspects: memory access, execution control, and instruction optimization.

- Memory access optimization

  Memory access optimization is the highest-priority optimization direction. In hybrid programming, SIMT VFs are often used to handle irregular logic such as scattered memory access and index mapping. The purpose of memory access optimization is to use memory access coalescing in SIMT VFs to reduce invalid GM access and improve effective bandwidth and data reuse.

  In SIMT programming, GM access requests from threads within the same warp can be coalesced into a single memory request if they fall within the same sector. On Ascend 950, the sector size is 128B. Scattered access causes access requests to be distributed across multiple sectors, resulting in low memory access efficiency. During optimization, prioritize adjusting the mapping between threads and data so that threads within the same warp access GM addresses that are as close or contiguous as possible, thereby improving effective bandwidth utilization. For scattered data scenarios in SIMT VFs where contiguous GM access is difficult, you can first use the MTE copy interface to move the relevant data from GM to UB cache, and then access the UB data in the SIMT VF to reduce the overhead of scattered GM access and improve bandwidth utilization. During analysis, also pay attention to the change in read data volume caused by Data Cache hits, and whether UB access introduces new conflicts.

- Execution control optimization

  The goal of execution control optimization is to match the thread count, thread block count, and SIMT VF task volume with the actual workload. Developers should reasonably set the thread count, `__launch_bounds__`, and data partitioning method of the SIMT VF based on the input/output scale and thread mapping, so that threads cover the actual output scale and inter-core load balancing is ensured as much as possible. In addition, in SIMD and SIMT hybrid operators, attention should also be paid to the number of VF calls. If the VF is called too many times, unnecessary scheduling overhead is introduced, affecting overall execution efficiency.

- Instruction optimization

  Instruction optimization is usually considered when there is a compute bottleneck, or when memory access is already sufficient but performance still does not meet expectations. Its purpose is to reduce high-overhead instructions, branch divergence, and redundant computation within SIMT VFs, thereby improving the execution efficiency of thread-level compute logic.

  For logic such as complex branches and scattered indexes, SIMT can be used to express thread-level control flow. However, branch divergence must still be controlled within SIMT. Optimize the compute logic to reduce the use of high-overhead instructions such as division and modulo, and avoid repeatedly executing logic that can be computed in advance or reused within a thread.

## Optimization Suggestion Overview Table

Table 1 Computation optimization suggestion overview table

| Optimization Suggestion | Example |
| --- | --- |
| [Implementing Branch Determination via SIMT](./compute_optimization/implement_branch_judgement_via_simt.md) | [simd_simt_high_performance](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance) |

Table 2 Memory access optimization suggestion overview table

| Optimization Suggestion | Example |
| --- | --- |
| [Using Unified Buffer to Improve Memory Access Efficiency](./memory_access/use_unified_buffer_to_improve_memory_access_efficiency.md) | [simd_simt_high_performance](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance) |
| [Memory Access Coalescing](../simt_operator_optimization/memory_access/memory_access_merging.md) | [simd_simt_matrix_transpose](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/05_simd_simt_hybrid/02_best_practices/simd_simt_matrix_transpose) |
| [Avoiding UB Bank Conflicts](../simt_operator_optimization/memory_access/avoid_ub_bank_conflict.md) | [matrix_transpose_practice](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice) |
| [DCache Access Optimization](../simt_operator_optimization/memory_access/dcache_access_optimization.md) | [cache_hint](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/03_best_practices/00_memory_optimizations/cache_hint) |
| [Using Short Vector Types to Improve Efficiency](../simt_operator_optimization/memory_access/short_vector_types_for_efficiency_optimization.md) | [short_vector_add](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/03_best_practices/00_memory_optimizations/short_vector_add) |
| [Improving Transfer Efficiency via Type Alignment](../simt_operator_optimization/memory_access/improve_movement_efficiency_via_type_alignment.md) | [aligned_types](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/03_best_practices/00_memory_optimizations/aligned_types) |

Table 3 Execution control optimization suggestion overview table

| Optimization Suggestion | Example |
| --- | --- |
| [Thread Block Count Configuration Optimization](./execution_control/thread_block_count_configuration_optimization.md) | [simd_simt_grid_dim_config](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/05_simd_simt_hybrid/02_best_practices/simd_simt_grid_dim_config) |

Table 4 Instruction optimization suggestion overview table

| Optimization Suggestion | Example |
| --- | --- |
| [Fast Algorithm for Integer Division](./instruction_optimization/fast_algorithm_for_integer_division.md) | [simd_simt_integer_fast_div](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/05_simd_simt_hybrid/02_best_practices/simd_simt_integer_fast_div) |
