# SIMD and SIMT Hybrid Programming Sample Introduction

## Overview

SIMD and SIMT hybrid programming applies to scenarios where the same kernel needs both vectorized compute capability and thread-level flexible control. In typical cases, SIMD is used for regular vector computation, while SIMT is used for irregular memory access, branch handling, thread mapping, and fine-grained data movement. This keeps the flexibility of thread-level expression while improving regular computation efficiency.

SIMD and SIMT hybrid programming is an advanced programming mode based on SIMD programming and SIMT programming. Related samples are mainly distributed in the following paths:

- The `examples/05_simd_simt_hybrid` path provides introductory hybrid programming samples, troubleshooting samples, and samples that use SIMD and SIMT together for performance optimization.
- Because the kernel entry of hybrid programming is consistent with the SIMD programming mode, end-to-end profiling and debugging samples can refer to the samples under `examples/01_simd_cpp_api`.
- Hybrid programming starts SIMT VF functions to implement SIMT thread-level operations. Key features, profiling/debugging, and performance optimization samples related to SIMT VF can refer to samples under `examples/03_simt_api`.

The following sections introduce the introduction, utilities, features, trouble_shooting, and best_practices samples involved in hybrid programming scenarios.

## introduction

[00_introduction](./00_introduction) is the introductory reference directory for SIMD and SIMT hybrid programming. It contains the following samples.

| Sample Name | Sample Description |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| simd_simt_gather_and_adds | Implements fused Gather and Adds computation based on SIMD and SIMT hybrid programming. It demonstrates the basic method of using SIMT for irregular memory access and SIMD for continuous vector computation, as well as the definition and invocation of kernel functions and VF functions. Sample link: [simd_simt_gather_and_adds](./00_introduction/simd_simt_gather_and_adds). |

## utilities

When developing custom operators based on SIMD and SIMT hybrid programming, refer to the following utilities samples for debugging and tuning:

| Sample Name | Sample Description | Remarks |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| profiling | Demonstrates how to use Profiling to collect operator execution data and analyze the time ratio of SIMD computation, SIMT VF calls, and data movement in hybrid programming. Sample link: [profiling](../01_simd_cpp_api/01_utilities/04_profiling). | - |
| sanitizer | Demonstrates how to use Sanitizer to locate issues such as memory out-of-bounds access and illegal access. It is applicable to issue diagnosis during the hybrid programming debugging phase. Sample link: [sanitizer](../01_simd_cpp_api/01_utilities/05_sanitizer). | - |
| simulator | Demonstrates how to use Simulator for functional debugging and basic performance data collection in simulation mode. Sample link: [simulator](../01_simd_cpp_api/01_utilities/08_simulator). | - |
| printf (Kernel Entry) | Demonstrates how to use `AscendC::printf` in the kernel entry of a SIMD programming sample to print debugging information. Sample link: [printf (Kernel Entry)](../01_simd_cpp_api/01_utilities/00_printf/simple_printf). | - |
| printf (SIMD VF Function) | Demonstrates how to use `printf` in a SIMD VF function to print different data types, pointer information, and computation results. Sample link: [printf (SIMD VF Function)](../01_simd_cpp_api/01_utilities/00_printf/simd_vf_printf). | - |
| printf (SIMT VF Function) | Demonstrates how to print debugging information in a SIMT VF function. Sample link: [printf (SIMT VF Function)](../03_simt_api/01_utilities/00_printf). | This sample is implemented in SIMT programming mode as a reference sample. It can be converted into the corresponding hybrid programming sample implementation. For details, see [SIMT Reference Sample Conversion Guide](#simt-reference-sample-conversion-guide). |
| assert (Kernel Entry) | Demonstrates how to use `ascendc_assert` in the kernel entry of a SIMD programming sample for assertion checks. Sample link: [assert (Kernel Entry)](../01_simd_cpp_api/01_utilities/01_assert). | - |
| assert (SIMT VF Function) | Demonstrates how to perform assertion checks in a SIMT VF function. Sample link: [assert (SIMT VF Function)](../03_simt_api/01_utilities/01_assert). | This sample is implemented in SIMT programming mode as a reference sample. It can be converted into the corresponding hybrid programming sample implementation. For details, see [SIMT Reference Sample Conversion Guide](#simt-reference-sample-conversion-guide). |
| clock (SIMT VF Function) | Demonstrates how to count local execution cycles in a SIMT VF function to help locate hot paths in hybrid programming. Sample link: [clock (SIMT VF Function)](../03_simt_api/01_utilities/02_clock). | This sample is implemented in SIMT programming mode as a reference sample. It can be converted into the corresponding hybrid programming sample implementation. For details, see [SIMT Reference Sample Conversion Guide](#simt-reference-sample-conversion-guide). |

## features

When developing custom operators based on SIMD and SIMT hybrid programming, refer to the following SIMT key feature samples.

| Sample Name | Sample Description | Remarks |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| memory_access | SIMT reference sample. Demonstrates the usage of Global Memory irregular access APIs. It is applicable to scenarios where SIMT handles complex indexed access in hybrid programming. Sample link: [memory_access](../03_simt_api/02_features/01_api_features/00_memory_access). | This sample is implemented in SIMT programming mode as a reference sample. It can be converted into the corresponding hybrid programming sample implementation. For details, see [SIMT Reference Sample Conversion Guide](#simt-reference-sample-conversion-guide). |
| sync_instruction | SIMT reference sample. Demonstrates the usage of synchronization and memory fence APIs. It is applicable to SIMT thread collaboration and cross-stage data visibility control in hybrid programming. Sample link: [sync_instruction](../03_simt_api/02_features/01_api_features/01_sync_instruction). | This sample is implemented in SIMT programming mode as a reference sample. It can be converted into the corresponding hybrid programming sample implementation. For details, see [SIMT Reference Sample Conversion Guide](#simt-reference-sample-conversion-guide). |
| atomic_operation | SIMT reference sample. Demonstrates the usage of atomic operation APIs. It is applicable to multi-thread accumulation, counting, or conflict write handling in hybrid programming. Sample link: [atomic_operation](../03_simt_api/02_features/01_api_features/02_atomic_operation). | This sample is implemented in SIMT programming mode as a reference sample. It can be converted into the corresponding hybrid programming sample implementation. For details, see [SIMT Reference Sample Conversion Guide](#simt-reference-sample-conversion-guide). |
| warp_instruction | SIMT reference sample. Demonstrates Warp-level data exchange capability, which can be used to understand thread collaboration and data transfer within a Warp. Sample link: [warp_instruction](../03_simt_api/02_features/01_api_features/03_warp_instruction). | This sample is implemented in SIMT programming mode as a reference sample. It can be converted into the corresponding hybrid programming sample implementation. For details, see [SIMT Reference Sample Conversion Guide](#simt-reference-sample-conversion-guide). |

## trouble_shooting

[01_trouble_shooting](./01_trouble_shooting) is the troubleshooting reference directory for SIMD and SIMT hybrid programming. It contains the following samples.

| Sample Name | Sample Description |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| simd_simt_ub_usage_limit | Uses a UB out-of-bounds access that causes an output accuracy error as an example to demonstrate UB static memory, dynamic memory, reserved space, Data Cache layout, and out-of-bounds troubleshooting in hybrid programming. Sample link: [simd_simt_ub_usage_limit](./01_trouble_shooting/simd_simt_ub_usage_limit). |

## best_practices

Common performance optimization paths for SIMD and SIMT hybrid programming are as follows:

- Configure the maximum thread count properly: The maximum thread count affects the number of registers available to a single thread. Configure the maximum thread count based on the computation complexity of the operator to avoid register spill and performance degradation.
- Memory access optimization: Improve memory access efficiency through memory coalescing, proper UB staging, reducing irregular access, and avoiding UB bank conflicts.
    - Memory coalescing includes GM and UB access coalescing. The core idea is to make threads within a Warp access GM or UB as continuously as possible so that the underlying hardware can merge memory access requests and reduce the number of requests.
    - Proper UB staging can reduce irregular access. When using UB, pay attention to UB bank conflicts and optimize memory layout to reduce the conflict probability.
    - MTE can be used to move continuous data. In general, MTE data movement is more efficient than SIMT LD/ST memory access. MTE and SIMT VF are on different pipelines, which makes it easier to overlap data movement overhead.
- Execution configuration optimization: Configure gridDim, blockDim, and VF function invocation granularity based on data size and per-thread workload to reduce invalid threads and scheduling overhead.
- Compute instruction optimization: For scenarios such as integer division by a fixed divisor, use multiplication and shift operations to replace high-overhead division instructions.

The [`02_best_practices`](./02_best_practices) path provides performance optimization samples implemented using SIMD and SIMT hybrid programming. These samples mainly demonstrate how to use SIMD and SIMT together to improve performance.

| Sample Name | Sample Description |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| simd_simt_matrix_transpose | Uses matrix transpose as an example to introduce UB staging and Global Memory access coalescing optimization. Sample link: [simd_simt_matrix_transpose](./02_best_practices/simd_simt_matrix_transpose). |
| simd_simt_high_performance | Uses FloorMod computation as an example to compare implementations such as direct GM access by SIMT, SIMD Reg vector computation, SIMT access to UB, and continuous thread mapping. Sample link: [simd_simt_high_performance](./02_best_practices/simd_simt_high_performance). |
| simd_simt_grid_dim_config | Uses Gather computation as an example to introduce thread block count configuration and VF function invocation optimization. Sample link: [simd_simt_grid_dim_config](./02_best_practices/simd_simt_grid_dim_config). |
| simd_simt_integer_fast_div | Uses integer division by a fixed divisor as an example to demonstrate the instruction optimization method of replacing normal division with multiplication and shift operations. Sample link: [simd_simt_integer_fast_div](./02_best_practices/simd_simt_integer_fast_div). |

In addition, SIMT VF optimization methods in SIMD and SIMT hybrid programming scenarios can refer to the SIMT samples under `03_simt_api/03_best_practices`, as follows:

| Sample Name | Sample Description | Remarks |
| ------------------------------------------------------------ | ---------------------------------------------------- | --- |
| max_thread_config | SIMT reference sample. Demonstrates the maximum thread count configuration method and the relationship among register usage, maximum thread count, and execution performance. Sample link: [max_thread_config](../03_simt_api/03_best_practices/01_execution_conf_optimizations/max_thread_config). | This sample is implemented in SIMT programming mode as a reference sample. It can be converted into the corresponding hybrid programming sample implementation. For details, see [SIMT Reference Sample Conversion Guide](#simt-reference-sample-conversion-guide). |
| warp_divergence | SIMT reference sample. Uses SpMV computation as an example to demonstrate the impact of Warp Divergence on performance and the optimization idea of reducing branch divergence by letting one Warp collaboratively process one row. Sample link: [warp_divergence](../03_simt_api/03_best_practices/02_control_flow/warp_divergence). | This sample is implemented in SIMT programming mode as a reference sample. It can be converted into the corresponding hybrid programming sample implementation. For details, see [SIMT Reference Sample Conversion Guide](#simt-reference-sample-conversion-guide). |
| cache_hint | SIMT reference sample. Demonstrates data cache optimization methods and can be used as a reference for specifying cache policies for different data and reducing Global Memory access. Sample link: [cache_hint](../03_simt_api/03_best_practices/00_memory_optimizations/cache_hint). | This sample is implemented in SIMT programming mode as a reference sample. It can be converted into the corresponding hybrid programming sample implementation. For details, see [SIMT Reference Sample Conversion Guide](#simt-reference-sample-conversion-guide). |
| short_vector_add | SIMT reference sample. Demonstrates short vector optimization methods and can be used as a reference for improving memory bandwidth utilization and computation efficiency with short vector types such as half2. Sample link: [short_vector_add](../03_simt_api/03_best_practices/00_memory_optimizations/short_vector_add). | This sample is implemented in SIMT programming mode as a reference sample. It can be converted into the corresponding hybrid programming sample implementation. For details, see [SIMT Reference Sample Conversion Guide](#simt-reference-sample-conversion-guide). |
| aligned_types | SIMT reference sample. Demonstrates type alignment optimization methods and can be used as a reference for understanding the impact of structure alignment on Global Memory access throughput. Sample link: [aligned_types](../03_simt_api/03_best_practices/00_memory_optimizations/aligned_types). | This sample is implemented in SIMT programming mode as a reference sample. It can be converted into the corresponding hybrid programming sample implementation. For details, see [SIMT Reference Sample Conversion Guide](#simt-reference-sample-conversion-guide). |

## SIMT Reference Sample Conversion Guide

Samples implemented in SIMT programming mode mainly demonstrate direct SIMT Kernel invocation. Hybrid programming scenarios can refer to their thread mapping, memory access, synchronization, atomic operation, and Warp-level collaboration logic. However, the kernel entry, Host-side invocation configuration, and CMake compilation options are different. When converting SIMT reference samples to SIMD and SIMT hybrid programming samples, pay attention to the following differences.

### Kernel Definition Difference

SIMT programming and SIMD and SIMT hybrid programming define Kernels in different ways. A typical comparison is as follows:

SIMT programming: thread-level logic is written directly in the `__global__` SIMT Kernel:

```cpp
template <typename type_data, typename type_idx>
__global__ __launch_bounds__(MAX_THREAD_COUNT) void gather_custom(
    type_data* input, type_idx* index, type_data* output, uint32_t in_width, uint32_t index_total_length)
{
    int32_t out_row = blockIdx.x * blockDim.x + threadIdx.x;
    ...
}
```

SIMD and SIMT hybrid programming: thread-level SIMT logic is split into a `__simt_vf__` function.

```cpp
__simt_vf__ __launch_bounds__(MAX_THREAD_COUNT) inline void simt_gather(
    __gm__ float* input, __gm__ uint32_t* index, __ubuf__ float* gather_output, ...)
{
    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    ...
}

// SIMD and SIMT hybrid programming: the entry Kernel keeps the __global__ __vector__
// definition and invokes the SIMT VF function through asc_vf_call.
__global__ __vector__ void gather_custom(
    __gm__ float* input, __gm__ uint32_t* index, __gm__ float* output, ...)
{
    ...
    asc_vf_call<simt_gather>(dim3(thread_count), input, index, gather_output, ...);
    ...
}
```

The specific differences are as follows:

- In SIMT programming samples, the `__global__` Kernel is both the entry directly launched from the Host side and the implementation location of thread-level logic. Thread index calculation, branch handling, memory access, and thread collaboration logic are usually written directly in this Kernel.
- In SIMD and SIMT hybrid programming samples, the entry Kernel is consistent with the SIMD programming mode and is usually defined as `__global__ __vector__`. It orchestrates data movement, UB space allocation, SIMD VF calls, and SIMT VF calls.
- When converting SIMT reference samples, thread-level logic in the original SIMT Kernel is usually migrated to a `__simt_vf__` function, which is then invoked by the `__global__ __vector__` entry Kernel through `asc_vf_call`.

### Invocation Configuration Difference

SIMT programming and SIMD and SIMT hybrid programming configure invocation parameters in different places. A typical comparison is as follows:

SIMT programming: the Host side uses `<<<>>>` to directly configure the number of thread blocks, number of threads, dynamic UB size, and stream for the SIMT Kernel.

```cpp
gather_custom<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(...);
```

```cpp
// SIMD and SIMT hybrid programming: the Host side uses <<<>>> to launch the Vector Core entry Kernel.
gather_and_adds_kernel<<<blocks_per_grid, dyn_ubuf_size, stream>>>(...);

// SIMD and SIMT hybrid programming: the SIMT thread count is configured
// inside the entry Kernel through asc_vf_call.
asc_vf_call<simt_gather>(dim3(threads_per_block), input, index, gather_output, ...);
```

The specific differences are as follows:

- In SIMT programming samples, the Host-side `<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>` directly starts the SIMT Kernel. The first parameter indicates the thread block count, the second parameter indicates the thread count in each thread block, the third parameter indicates the dynamic UB size, and the fourth parameter indicates the stream.
- In SIMD and SIMT hybrid programming samples, the Host-side `<<<blocks_per_grid, dyn_ubuf_size, stream>>>` starts the `__global__ __vector__` entry Kernel. The first parameter indicates the number of Vector Cores or logical cores to start (equivalent to the number of thread blocks), the second parameter indicates the dynamic UB size, and the third parameter indicates the stream.
- The SIMT thread count in hybrid programming is not configured by Host-side `<<<>>>`. It is configured inside the entry Kernel through `asc_vf_call<simt_func>(dim3(threads_per_block), ...)`.

### CMake Compilation Option Difference

SIMT programming samples explicitly enable SIMT compilation capability through `--enable-simt`, for example:

```cmake
target_compile_options(${TARGET_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES} --enable-simt>
)
```

When converting SIMT reference samples to SIMD and SIMT hybrid programming, it is recommended to use the CMake structure of hybrid programming samples as the basis and add corresponding options and linked libraries according to whether NPU simulation, CPU debugging, Profiling, or Sanitizer capability is required.
