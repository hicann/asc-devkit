# Properly Configuring the Thread Count to Avoid Register Spilling

<!-- md-trans-meta sourceCommit=198cbfdf29f72f78c8de735d88a5244dc258f2e7 translatedAt=2026-08-26T14:49:27.611Z -->

[Priority] Medium

[Description] In SIMT programming, the thread count should be properly configured to avoid register spilling. In SIMT programming mode, the **maximum number of threads** specified by `__launch_bounds__` for a kernel function determines the number of registers available to each thread. The larger the maximum number of threads, the fewer registers can be allocated to each thread. The correspondence between the two is as follows:

| Maximum Thread Count | Number of Registers Available to Each Thread |
|------------|------------------------|
| 1025~2048 | 16 |
| 513~1024 | 32 |
| 257~512 | 64 |
| 1~256 | 127 |

`__launch_bounds__(N)` is an optional qualifier on a kernel function. It is configured when the kernel function is defined and is used to declare to the compiler at compile time that the **maximum number of threads** for executing the kernel function is N. Based on this, the compiler determines the number of registers that can be allocated to each thread. When `__launch_bounds__` is not configured for a kernel function, the maximum number of threads defaults to 1024, in which case each thread can use 32 registers. For compute-intensive operators, a single thread usually occupies many registers. Under the default configuration, the register limit is easily exceeded, and the excess intermediate data spills to the stack space (located in Global Memory), introducing additional Global Memory access and degrading performance.

The approach to avoiding register spilling is as follows: first, use the `--cce-res-usage` compiler option to check the register usage of the kernel function (a `Stack size` greater than 0 indicates spilling). Then, based on the correspondence between registers and the maximum number of threads in the table above, select a maximum number of threads that can meet the per-thread register requirement of the operator, and configure it for the compiler through `__launch_bounds__(N)`. The compiler then relaxes the per-thread register quota accordingly, thereby avoiding register spilling, keeping intermediate data in registers, reducing Global Memory access, and improving performance. For compute-intensive operators, the following is generally recommended: if more than 32 registers are required (for example, sincos), configure 512 threads; if no more than 32 registers are required, configure 1024 threads. For details about `__launch_bounds__`, see [SIMT BuiltIn Keywords](../../../programming_guide/language_extension/SIMT-BuiltIn_keyword.md#kernel-function-configuration).

[Example Introduction] Take the [SinCosCompute operator](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/03_best_practices/01_execution_conf_optimizations/max_thread_config/sincos_compute) as an example. It uses the `sincosf` API to compute the sine and cosine results simultaneously, with the calculation formulas $output\_sin[i] = sin(input[i])$ and $output\_cos[i] = cos(input[i])$. The input data is of the float type with a data size of 393216 elements. It is configured with 48 thread blocks, 512 threads per thread block, and each thread computes 16 input values in a loop. The kernel functions of the baseline version and the optimized version have exactly the same computation logic, differing only in whether `__launch_bounds__` is configured.

  [Negative Example] `__launch_bounds__` is not configured, so the maximum number of threads takes the default value 1024. The compiler allocates registers accordingly, causing register spilling.

  ```cpp
  __global__ void sincos_thread_1024(float* input, float* output_sin,
                                     float* output_cos, uint64_t total_length)
  {
      int32_t blk_start_idx = blockIdx.x * THREADS_PER_BLOCK * PER_THREAD_LOOP;
      // Each core computes PER_THREAD_LOOP * THREADS_PER_BLOCK operations
      for (int i = 0; i < PER_THREAD_LOOP; i++) {
          int idx = blk_start_idx + i * THREADS_PER_BLOCK + threadIdx.x;
          sincosf(input[idx], output_sin + idx, output_cos + idx);
      }
  }
  ```

  In the preceding implementation, because `__launch_bounds__` is not configured, the maximum number of threads takes the default value 1024. The compiler therefore allocates only 32 registers to each thread, while the registers required by the `sincosf` computation exceed this limit, and the excess spills to the stack space. Use the `--cce-res-usage` compiler option to check the register usage:

  ```
  [BISHENG] Function properties for _Z18sincos_thread_1024PfS_S_m_simt_entry: Stack size: 32 bytes, Used register number: 32
  ```

  Here, `Stack size: 32 bytes` indicates register spilling (the stack resides in Global Memory), and `Used register number: 32` has reached the register limit for 1024 threads.

  On the Ascend 950PR product, the performance data of this implementation is as follows:

  | Task Duration(us) | DCache Read GM | DCache Read Vector | DCache Write Vector |
  |:-----------------:|:--------------:|:------------------:|:-------------------:|
  |      102.47       |      256       |        640         |         768         |

  Register spilling causes intermediate data to repeatedly access the stack space (Global Memory), which is reflected in the high DCache Read Vector (640 times) and DCache Write Vector (768 times).

  [Positive Example] Configure `__launch_bounds__(512)` to inform the compiler of the actual maximum number of threads, fully utilize registers, and avoid spilling.

  ```cpp
  __global__ __launch_bounds__(512) void sincos_thread_512(float* input,
                                     float* output_sin, float* output_cos,
                                     uint64_t total_length)
  {
      int32_t blk_start_idx = blockIdx.x * THREADS_PER_BLOCK * PER_THREAD_LOOP;
      // Each core computes PER_THREAD_LOOP * THREADS_PER_BLOCK operations.
      for (int i = 0; i < PER_THREAD_LOOP; i++) {
          int idx = blk_start_idx + i * THREADS_PER_BLOCK + threadIdx.x;
          sincosf(input[idx], output_sin + idx, output_cos + idx);
      }
  }
  ```

  After configuring `__launch_bounds__(512)`, the compiler allocates 64 registers per thread. The registers required by the `sincosf` computation are within the limit, so no spilling occurs. The compilation information is as follows:

  ```
  [BISHENG] Function properties for _Z17sincos_thread_512PfS_S_m_simt_entry: Stack size: 0 bytes, Used register number: 48
  ```

  Here, `Stack size: 0 bytes` indicates no register spilling, and `Used register number: 48` is within the 64-register limit, so all intermediate data is kept in registers.

  On the Ascend 950PR product, the performance data after using `__launch_bounds__(512)` is as follows:

  | Task Duration(us) | DCache Read GM | DCache Read Vector | DCache Write Vector |
  |:-----------------:|:--------------:|:------------------:|:-------------------:|
  |       96.22       |      256       |        512         |         256         |

  **Optimization effect analysis**:

  - **End-to-end duration**: Reduced from 102.47us to 96.22us, a decrease of about 6.1%.
  - **DCache Read GM**: Remains unchanged at 256, indicating that the optimization introduces no additional Global Memory read overhead.
  - **DCache Read/Write Vector**: Read Vector decreases from 640 to 512, and Write Vector decreases from 768 to 256. The stack space is physically located in Global Memory, so accesses to the stack during register spilling are reflected in the Data Cache read/write counts. After eliminating the spilling, these two access counts are significantly reduced.

[Summary] For compute-intensive operators, first use the `--cce-res-usage` compiler option to check register usage. Then, based on the correspondence between registers and the maximum number of threads, select a maximum number of threads that satisfies the per-thread register requirement, and configure it to the compiler through `__launch_bounds__` so that the compiler relaxes the register quota and avoids register spilling to Global Memory. Finally, compare the `Stack size`, `Task Duration`, and DCache access counts before and after optimization to verify the optimization effect.
