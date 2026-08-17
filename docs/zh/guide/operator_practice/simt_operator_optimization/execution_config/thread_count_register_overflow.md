# 合理配置线程数避免寄存器溢出

【优先级】中

【描述】在SIMT编程的程序中应合理配置线程数避免寄存器溢出。SIMT编程模式下，核函数通过`__launch_bounds__`指定的**最大线程数**决定每个线程可用的寄存器数量。最大线程数越大，每个线程可分配的寄存器越少，二者的对应关系如下：

| 最大线程数 | 每个线程可用寄存器个数 |
|------------|------------------------|
| 1025~2048 | 16 |
| 513~1024 | 32 |
| 257~512 | 64 |
| 1~256 | 127 |

`__launch_bounds__(N)`是核函数上的可选限定符，在核函数定义时配置，用于在编译期向编译器声明执行该核函数的**最大线程数**为N，编译器据此确定每个线程可分配的寄存器数量。当核函数未配置`__launch_bounds__`时，最大线程数默认为1024，此时每个线程可使用32个寄存器。对于计算密集型算子，单个线程占用的寄存器通常较多，在默认配置下容易超出寄存器上限，超出部分的中间数据会溢出到栈空间（位于Global Memory），引入额外的Global Memory访问，导致性能下降。

避免寄存器溢出的思路是：先通过`--cce-res-usage`编译选项查看核函数的寄存器使用情况（`Stack size`大于0即表明存在溢出），再根据上表中寄存器与最大线程数的对应关系，选择一档能满足算子单线程寄存器需求的最大线程数，并通过`__launch_bounds__(N)`将其配置给编译器。编译器据此放宽每线程的寄存器配额，从而避免寄存器溢出，将中间数据保留在寄存器中，减少Global Memory访问，提升性能。一般建议计算密集型算子：若需多于32个寄存器（如sincos），建议配置512线程；若不超过32个，可配置1024线程。关于`__launch_bounds__`的详细说明，请参考[SIMT BuiltIn关键字](../../../programming_guide/language_extension/simt_builtin_keywords.md#核函数配置)。

【样例介绍】以[SinCosCompute算子](../../../../../../examples/03_simt_api/03_best_practices/01_execution_conf_optimizations/max_thread_config/sincos_compute)为例，使用`sincosf`接口同时计算sin和cos结果，计算公式为 $output\_sin[i] = sin(input[i])$、$output\_cos[i] = cos(input[i])$。输入数据为float类型，数据规模为393216个元素，配置48个线程块、每个线程块512个线程，每个线程循环计算16个输入值。基线版本与优化版本的核函数计算逻辑完全相同，仅在是否配置`__launch_bounds__`上存在差异。

  【反例】不配置`__launch_bounds__`，最大线程数取默认值1024，编译器据此分配寄存器，导致寄存器溢出。

  ```cpp
  __global__ void sincos_thread_1024(float* input, float* output_sin,
                                     float* output_cos, uint64_t total_length)
  {
      int32_t blk_start_idx = blockIdx.x * THREADS_PER_BLOCK * PER_THREAD_LOOP;
      // 每个核计算PER_THREAD_LOOP * THREADS_PER_BLOCK的运算量
      for (int i = 0; i < PER_THREAD_LOOP; i++) {
          int idx = blk_start_idx + i * THREADS_PER_BLOCK + threadIdx.x;
          sincosf(input[idx], output_sin + idx, output_cos + idx);
      }
  }
  ```

  在上述实现中，由于未配置`__launch_bounds__`，最大线程数取默认值1024，编译器据此为每个线程仅分配32个寄存器，而`sincosf`计算所需的寄存器超出该上限，超出部分溢出到栈空间。使用`--cce-res-usage`编译选项可查看寄存器使用情况：

  ```
  [BISHENG] Function properties for _Z18sincos_thread_1024PfS_S_m_simt_entry: Stack size: 32 bytes, Used register number: 32
  ```

  其中`Stack size: 32 bytes`表明存在寄存器溢出（栈位于Global Memory），`Used register number: 32`已达到1024线程下的寄存器上限。

  在Ascend 950PR产品上，该实现的性能数据如下：

  | Task Duration(us) | DCache Read GM | DCache Read Vector | DCache Write Vector |
  |:-----------------:|:--------------:|:------------------:|:-------------------:|
  |      102.47       |      256       |        640         |         768         |

  寄存器溢出导致中间数据反复访问栈空间（Global Memory），体现为较高的DCache Read Vector（640次）和DCache Write Vector（768次）。

  【正例】配置`__launch_bounds__(512)`，提示编译器真实的最大线程数，充分利用寄存器，避免溢出。

  ```cpp
  __global__ __launch_bounds__(512) void sincos_thread_512(float* input,
                                     float* output_sin, float* output_cos,
                                     uint64_t total_length)
  {
      int32_t blk_start_idx = blockIdx.x * THREADS_PER_BLOCK * PER_THREAD_LOOP;
      // 每个核计算PER_THREAD_LOOP * THREADS_PER_BLOCK的运算量
      for (int i = 0; i < PER_THREAD_LOOP; i++) {
          int idx = blk_start_idx + i * THREADS_PER_BLOCK + threadIdx.x;
          sincosf(input[idx], output_sin + idx, output_cos + idx);
      }
  }
  ```

  配置`__launch_bounds__(512)`后，编译器为每个线程分配64个寄存器，`sincosf`计算所需的寄存器在限制范围内，无溢出。编译信息如下：

  ```
  [BISHENG] Function properties for _Z17sincos_thread_512PfS_S_m_simt_entry: Stack size: 0 bytes, Used register number: 48
  ```

  其中`Stack size: 0 bytes`表明无寄存器溢出，`Used register number: 48`在64个寄存器限制内，所有中间数据保存在寄存器中。

  在Ascend 950PR产品上，使用`__launch_bounds__(512)`后的性能数据如下：

  | Task Duration(us) | DCache Read GM | DCache Read Vector | DCache Write Vector |
  |:-----------------:|:--------------:|:------------------:|:-------------------:|
  |       96.22       |      256       |        512         |         256         |

  **优化效果分析**：

  - **端到端耗时**：从102.47us降低到96.22us，下降约6.1%。
  - **DCache Read GM**：保持256次不变，说明优化未引入额外的Global Memory读取开销。
  - **DCache Read/Write Vector**：Read Vector从640降至512，Write Vector从768降至256。栈空间物理位于Global Memory，寄存器溢出时对栈的访问会体现在Data Cache的读写次数上；消除溢出后，这两项访问次数明显减少。

【总结】对于计算密集型算子，应先通过`--cce-res-usage`编译选项查看寄存器使用情况，再根据寄存器与最大线程数的对应关系选择一档能满足单线程寄存器需求的最大线程数，并通过`__launch_bounds__`配置给编译器，使其放宽寄存器配额、避免寄存器溢出到Global Memory，最后对比优化前后的`Stack size`、`Task Duration`与DCache访问次数验证优化效果。
