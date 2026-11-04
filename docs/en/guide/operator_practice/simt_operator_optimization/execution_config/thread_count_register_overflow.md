# Configuring the Thread Count Properly to Avoid Register Overflow

<!-- md-trans-meta sourceCommit=58025861f63fbbf547b32a1e64731b761bd8937e translatedAt=2026-10-23T13:11:45.840Z pushedAt=2026-11-03T07:05:13.370Z nl=2 -->

[Priority] Medium

[Description] In SIMT programming, configure the thread count properly to avoid register overflow. In SIMT programming mode, the **maximum thread count** specified by `__launch_bounds__` for a kernel determines the number of registers available to each thread. The larger the maximum thread count, the fewer registers can be allocated to each thread. The correspondence between the two is as follows:

| Maximum Thread Count | Registers Available per Thread |
|------------|------------------------|
| 1025~2048 | 16 |
| 513~1024 | 32 |
| 257~512 | 64 |
| 1~256 | 127 |

`__launch_bounds__(N)` is an optional qualifier on a kernel. It is configured when the kernel is defined and is used to declare to the compiler at compile time that the **maximum thread count** for executing the kernel is N. Based on this, the compiler determines the number of registers that can be allocated to each thread. When `__launch_bounds__` is not configured for a kernel, the maximum thread count defaults to 1024, in which case each thread can use 32 registers. For compute-intensive operators, a single thread usually occupies many registers. Under the default configuration, the register limit is easily exceeded, and the excess intermediate data overflows to the stack space (located in Global Memory), introducing additional Global Memory access and degrading performance.

The approach to avoiding register overflow is as follows: first, use the `--cce-res-usage` compilation option to check the register usage of the kernel (a `Stack size` greater than 0 indicates overflow). Then, based on the correspondence between registers and the maximum thread count in the table above, select a maximum thread count that meets the per-thread register requirement of the operator, and configure it for the compiler through `__launch_bounds__(N)`. Accordingly, the compiler relaxes the per-thread register quota, thereby avoiding register overflow, keeping intermediate data in registers, reducing Global Memory access, and improving performance. For compute-intensive operators, the following is generally recommended: if more than 32 registers are required (for example, sincos), configure 512 threads; if no more than 32 are required, configure 1024 threads. For details about `__launch_bounds__`, see [SIMT Built-In Keywords](../../../programming_guide/language_extension/simt_builtin_keywords.md#kernel-function-configuration).

[Example] Take the [SinCosCompute operator](../../../../../../examples/03_simt_api/03_best_practices/01_execution_conf_optimizations/max_thread_config/sincos_compute) as an example. It uses the `sincosf` API to compute the sine and cosine results simultaneously. The calculation formulas are $output\_sin[i] = sin(input[i])$ and $output\_cos[i] = cos(input[i])$. The input data is of the float type, with a data size of 393216 elements. 48 thread blocks are configured, each with 512 threads, and each thread computes 16 input values in a loop. The baseline version and the optimized version have exactly the same kernel computation logic, differing only in whether `__launch_bounds__` is configured.

  [Negative example] `__launch_bounds__` is not configured, so the maximum thread count takes the default value of 1024. The compiler allocates registers accordingly, causing register overflow.

  ```cpp
  __global__ void sincos_thread_1024(float* input, float* output_sin,
                                     float* output_cos, uint64_t total_length)
  {
      int32_t blk_start_idx = blockIdx.x * THREADS_PER_BLOCK * PER_THREAD_LOOP;
      // Compute PER_THREAD_LOOP * THREADS_PER_BLOCK operations per core.
      for (int i = 0; i < PER_THREAD_LOOP; i++) {
          int idx = blk_start_idx + i * THREADS_PER_BLOCK + threadIdx.x;
          sincosf(input[idx], output_sin + idx, output_cos + idx);
      }
  }
  ```

  In the preceding implementation, because `__launch_bounds__` is not configured, the maximum thread count takes the default value of 1024. Accordingly, the compiler allocates only 32 registers to each thread, while the registers required by the `sincosf` computation exceed this limit, and the excess overflows to the stack space. Use the `--cce-res-usage` compilation option to check the register usage:

  ```
  [BISHENG] Function properties for _Z18sincos_thread_1024PfS_S_m_simt_entry: Stack size: 32 bytes, Used register number: 32
  ```

  Here, `Stack size: 32 bytes` indicates register overflow (the stack resides in Global Memory), and `Used register number: 32` has reached the register limit for 1024 threads.

  On Ascend 950PR products, the performance data of this implementation is as follows:

  | Task Duration (us) | DCache Read GM | DCache Read Vector | DCache Write Vector |
  |:-----------------:|:--------------:|:------------------:|:-------------------:|
  |      102.47       |      256       |        640         |         768         |

  Register overflow causes intermediate data to be repeatedly accessed from the stack space (Global Memory), which is reflected in the high DCache Read Vector (640 times) and DCache Write Vector (768 times).

  [Positive example] Configure `__launch_bounds__(512)` to inform the compiler of the actual maximum thread count, fully utilize registers, and avoid overflow.

  ```cpp
  __global__ __launch_bounds__(512) void sincos_thread_512(float* input,
                                     float* output_sin, float* output_cos,
                                     uint64_t total_length)
  {
      int32_t blk_start_idx = blockIdx.x * THREADS_PER_BLOCK * PER_THREAD_LOOP;
      // Each core computes PER_THREAD_LOOP * THREADS_PER_BLOCK operations
      for (int i = 0; i < PER_THREAD_LOOP; i++) {
          int idx = blk_start_idx + i * THREADS_PER_BLOCK + threadIdx.x;
          sincosf(input[idx], output_sin + idx, output_cos + idx);
      }
  }
  ```

  After configuring `__launch_bounds__(512)`, the compiler allocates 64 registers per thread. The registers required by the `sincosf` computation are within the limit, so no overflow occurs. The compilation information is as follows:

  ```
  [BISHENG] Function properties for _Z17sincos_thread_512PfS_S_m_simt_entry: Stack size: 0 bytes, Used register number: 48
  ```

  Here, `Stack size: 0 bytes` indicates no register overflow, and `Used register number: 48` is within the 64-register limit, so all intermediate data is stored in registers.

  On Ascend 950PR products, the performance data after using `__launch_bounds__(512)` is as follows:

  | Task Duration (us) | DCache Read GM | DCache Read Vector | DCache Write Vector |
  |:-----------------:|:--------------:|:------------------:|:-------------------:|
  |       96.22       |      256       |        512         |         256         |

  **Optimization effect analysis**:

  - **End-to-end duration**: reduced from 102.47us to 96.22us, a decrease of about 6.1%.
  - **DCache Read GM**: remains unchanged at 256 times, indicating that the optimization does not introduce additional Global Memory read overhead.
  - **DCache Read/Write Vector**: Read Vector decreases from 640 to 512, and Write Vector decreases from 768 to 256. The stack space is physically located in Global Memory, so when register overflow occurs, accesses to the stack are reflected in the Data Cache read/write counts. After eliminating the overflow, these two access counts are significantly reduced.

[Summary] For compute-intensive operators, first use the `--cce-res-usage` compilation option to check register usage, then select a maximum thread count that satisfies the per-thread register requirement based on the relationship between registers and the maximum thread count, and configure it to the compiler through `__launch_bounds__` to relax the register quota and avoid register overflow to Global Memory. Finally, compare the `Stack size`, `Task Duration`, and DCache access counts before and after optimization to verify the optimization effect.
