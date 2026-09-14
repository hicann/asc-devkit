# Memory Access Merging

<!-- md-trans-meta sourceCommit=e2fefeda96ad045b06954f86e5dda7f5097f9311 translatedAt=2026-08-26T14:46:27.425Z -->

[Priority] High

[Description] In SIMT programming mode, adjacent threads within a warp usually issue GM access requests simultaneously. If the GM addresses accessed by these threads are contiguous, the hardware can merge the access requests, thereby improving GM bandwidth utilization. If the GM addresses accessed by adjacent threads span across lines or are scattered, memory access merging is difficult to achieve, and the operator execution time increases significantly. For operators such as transpose, reorder, and gather/scatter, you need to analyze the data layout and thread mapping relationship, and try to make adjacent threads within the same warp access contiguous addresses in the same memory access instruction.

**Figure 1** Memory access merging comparison diagram

<img src="../../../figures/memory_access_merging.png">

Memory access merging optimization applies to two types of scenarios. In the first type, the data itself is contiguous in GM, but the data assigned to adjacent threads is not contiguous, causing adjacent threads within the same warp to access non-contiguous addresses. In this case, prioritize adjusting the data splitting method so that adjacent threads within a warp access contiguous data. In the second type, the operator function includes layout transformations such as transpose and reorder, where simply adjusting the splitting method makes it difficult to ensure that both GM reads and writes are contiguous. In this case, consider introducing UB memory space as an intermediate buffer, and use UB to complete the non-contiguous data reordering process, so as to keep GM-side access contiguous as much as possible.

- Optimizing data splitting to implement memory access merging

  [Example Introduction] Take the [Gather operator](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/00_introduction/01_gather/general_gather) as an example. The operator fetches data from the input by row, and the calculation formula can be expressed as `output[row, col] = input[index[row], col]`. The column-direction data within each row of the output matrix is contiguous in GM. When optimizing the splitting, adjacent threads within the same warp should access adjacent columns within the same row as much as possible, rather than only ensuring continuity within a single thread.

  [Negative Example] Split data by thread, with each thread processing a contiguous row of data.

  ```
  template <typename type_data, typename type_idx>
  __global__ __launch_bounds__(MAX_THREAD_COUNT) void gather_custom(
      type_data* input,
      type_idx* index,
      type_data* gather_output,
      uint32_t in_width,
      uint32_t index_total_length)
  {
      int32_t out_row = blockIdx.x * blockDim.x + threadIdx.x;

      if (out_row >= index_total_length) {
          return;
      }

      uint32_t in_row = index[out_row];
      int input_idx = in_row * in_width;
      int output_idx = out_row * in_width;
      for (int32_t col = 0; col < in_width; col++) {
          gather_output[output_idx] = input[input_idx];
          input_idx += 1;
          output_idx += 1;
      }
  }
  ```

  In the implementation above, output rows are assigned to different threads, and each thread processes an entire row of data contiguously within a `for` loop. From the perspective of a single thread, both `input_idx` and `output_idx` increase contiguously. However, the key to SIMT memory access merging is that adjacent threads within the same warp access contiguous addresses in the same memory access instruction. In this implementation, adjacent threads within the same warp process different `out_row` values. Although access within a single thread is contiguous, access within the warp is not contiguous, making GM memory access merging difficult.

  Taking an input shape of 20001×1023 and an index length equal to the number of cores multiplied by 2048 as an example, the total number of thread blocks equals the number of physical cores, and each thread block launches 2048 threads. The performance data of the counterexample operator running this case is as follows:

  | Task Duration(us) | aiv_time(us) | aiv_total_cycles | aiv_vec_time(us) | aiv_vec_ratio | aiv_scalar_time(us) | aiv_scalar_ratio |
  | :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
  |     4886.076     |   4535.644   |   7483813.083   |     4535.178     |     0.999     |        0.454        |      0.001      |

  [Positive Example] Split the output data by ThreadBlock so that adjacent threads within a warp access contiguous addresses.


  ```
  template <typename type_data, typename type_idx>
  __global__ __launch_bounds__(MAX_THREAD_COUNT) void gather_custom(
      type_data* input,
      type_idx* index,
      type_data* gather_output,
      uint32_t in_width,
      uint32_t output_total_size)
  {
      uint32_t start = blockIdx.x * blockDim.x + threadIdx.x;
      uint32_t stride = blockDim.x * gridDim.x;

      for (uint32_t idx = start; idx < output_total_size; idx += stride) {
          uint32_t out_row = idx / in_width;
          uint32_t out_col = idx - out_row * in_width;
          uint32_t in_row = index[out_row];
          uint32_t input_i = in_row * in_width + out_col;
          gather_output[idx] = input[input_i];
      }
  }
  ```

  The implementation above organizes the output matrix by one-dimensional linear addresses and lets each ThreadBlock cover a contiguous segment of output elements. For the same loop iteration, the `idx` of threads within a ThreadBlock increases consecutively, and adjacent threads within the same warp access contiguous `gather_output[idx]`. When these `idx` values fall within the same output row, `out_row` is the same and `out_col` increases consecutively. Since `in_row = index[out_row]`, `in_row` is also the same, so the input access address `input[in_row * in_width + out_col]` is also contiguous. Both input and output can therefore achieve memory access merging.

  In the positive example code, each thread does not process a contiguous segment of data throughout the entire process. Instead, it processes multiple elements with a stride of `blockDim.x * gridDim.x`. In other words, the data accessed by a single thread across multiple iterations is non-contiguous; however, in each iteration, adjacent threads within a warp access contiguous addresses. The benefit of SIMT memory access merging comes from contiguous access at the warp dimension, so this splitting method generally leverages GM bandwidth better than "each thread processing a contiguous segment of data".

  The accompanying ThreadBlock splitting is also adjusted from splitting by `index_total_length` to splitting by `output_total_size`, so that the number of threads covers the actual output element scale:

  ```
  bool block_split(uint32_t output_total_size, uint32_t &blocks_per_grid, uint32_t &threads_per_block)
  {
      uint32_t real_core_num = 0;
      const auto& platformInfoMgr = platform_ascendc::PlatformAscendCManager::GetInstance();
      if (platformInfoMgr == nullptr) {
          std::cout << "[ERROR] Get platform info failed, please check device status." << std::endl;
          return false;
      }
      real_core_num = platformInfoMgr->GetCoreNumAiv();
      threads_per_block = std::min(output_total_size, MAX_THREAD_COUNT);
      blocks_per_grid = (output_total_size + threads_per_block - 1) / threads_per_block;
      if (blocks_per_grid > real_core_num) {
          blocks_per_grid = real_core_num;
          threads_per_block = MAX_THREAD_COUNT;
      }
      return true;
  }
  ```

  The positive example operator implements memory access merging. The performance data of running this case is as follows:

  | Task Duration(us) | aiv_time(us) | aiv_total_cycles | aiv_vec_time(us) | aiv_vec_ratio | aiv_scalar_time(us) | aiv_scalar_ratio |
  | :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
  |      866.103      |   864.080   |   1425731.861   |     863.590     |     0.999     |        0.478        |      0.001      |

  In terms of task duration, the execution time after memory access merging is 866.103us, a decrease of about 82.3% compared with 4886.076us before optimization. This indicates that the main bottleneck before optimization came from the non-contiguous GM addresses accessed by adjacent threads within a warp. After adjusting the data splitting, adjacent threads access adjacent elements in the same memory access instruction, and the GM memory access efficiency is significantly improved.
- Introducing UB relay to implement memory access merging

  [Example Introduction] Take [matrix transpose](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice) as an example. The input matrix has a shape of 1024×1024 and the data type is `float`. Matrix transpose first divides the matrix into multiple fixed-size sub-matrix blocks for processing. These sub-matrix blocks are called tiles. This example uses a 32×32 tile, and each ThreadBlock is responsible for processing one tile. The Grid is configured as `(matrix_width / 32, matrix_height / 32, 1)`, and the ThreadBlock is configured as `(32, 32, 1)`, with each thread processing one element in the tile. Under this splitting method, the 32 threads in the `threadIdx.x` direction correspond to 32 contiguous elements in the same row within the tile, which is suitable for forming contiguous GM read access.

  [Negative Example] Write back to GM directly according to the transpose formula.

  ```
  constexpr int TILE_DIM = 32;

  __global__ void transpose_naive_kernel(float *output, const float *input, int width, int height)
  {
      int x_index = blockIdx.x * TILE_DIM + threadIdx.x;
      int y_index = blockIdx.y * TILE_DIM + threadIdx.y;

      int index_in = x_index + width * y_index;
      int index_out = y_index + height * x_index;

      output[index_out] = input[index_in];
  }
  ```

  In the implementation above, threads within the same warp read data from one row of the input matrix, and `index_in` is contiguous, so the GM read access can be merged. However, when writing back to GM, threads with adjacent `threadIdx.x` correspond to different `x_index` values, and `index_out = y_index + height * x_index` causes adjacent threads to write to different rows of the output matrix, with an address interval of `height` elements. In this case, the addresses written back to GM are not contiguous, making it difficult to form efficient merged memory access.

  In the 1024×1024 matrix transpose example, the performance data of the operator using direct index transpose is as follows:

  | Task Duration(us) | aiv_time(us) | aiv_total_cycles | aiv_vec_time(us) | aiv_vec_ratio | aiv_scalar_time(us) | aiv_scalar_ratio |
  | :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
  |      60.477      |    3.516    |     5801.925     |      3.357      |     0.955     |        0.147        |      0.041      |

  [Positive Example] Use UB as an intermediate buffer to convert non-contiguous GM writes into transposed reads within UB.


  ```
  constexpr int TILE_DIM = 32;

  __global__ void transpose_coalesced_kernel(float *output, const float *input, int width, int height)
  {
      __ubuf__ float tile[TILE_DIM][TILE_DIM];

      int x_index = blockIdx.x * TILE_DIM + threadIdx.x;
      int y_index = blockIdx.y * TILE_DIM + threadIdx.y;
      int index_in = x_index + y_index * width;

      tile[threadIdx.y][threadIdx.x] = input[index_in];
      asc_syncthreads();

      x_index = blockIdx.y * TILE_DIM + threadIdx.x;
      y_index = blockIdx.x * TILE_DIM + threadIdx.y;
      int index_out = x_index + y_index * height;

      output[index_out] = tile[threadIdx.x][threadIdx.y];
  }
  ```

  In the implementation above, each thread first reads data from GM according to the original layout and writes it to `tile[threadIdx.y][threadIdx.x]` in UB. After synchronization, the ThreadBlock coordinates are swapped to locate the transposed output tile, and then `tile[threadIdx.x][threadIdx.y]` is read from UB and written back to GM. After this processing, the same warp reads one row of the input on the GM side and also writes one row of the output, keeping both GM reads and writes contiguous; the non-contiguous access caused by the transpose is transferred to be completed inside UB. Among them, \_\_ubuf\_\_ is a memory space modifier in the SIMT programming model used to declare UB (shared memory) variables. Arrays declared through \_\_ubuf\_\_ are located in the Unified Buffer of the AIV core and are shared by all threads within the thread block.

  In the 1024×1024 matrix transpose example, the performance data of the operator after using UB as an intermediate buffer is as follows:

  | Task Duration(us) | aiv_time(us) | aiv_total_cycles | aiv_vec_time(us) | aiv_vec_ratio | aiv_scalar_time(us) | aiv_scalar_ratio |
  | :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
  |      35.945      |    1.814    |     2993.315     |      1.646      |     0.910     |        0.156        |      0.083      |

  From the perspective of task duration, the execution time after using UB as an intermediate buffer is 35.945us, which is about 40.6% lower than the 60.477us of direct index transpose. This indicates that the main benefit in this transpose scenario comes from converting the originally non-contiguous GM write access into transposed reads within UB, keeping both GM read and write sides as contiguous as possible. Although introducing UB brings an additional intermediate transfer and thread synchronization overhead, in this use case the benefit brought by GM write access merging outweighs these additional overheads.

[Summary] When performing memory access optimization for SIMT operators, first analyze whether the GM addresses accessed by adjacent threads within the same warp under the same memory access instruction are contiguous. Common optimization methods include: first, adjust the data splitting method to assign contiguous data regions to ThreadBlocks and warps, so that adjacent threads within a warp access contiguous addresses in each iteration, and a single thread can process subsequent data through strides to cover the complete output; second, introduce UB for data relay and layout rearrangement, converting originally non-contiguous GM accesses into rearranged accesses within UB, thereby ensuring that both GM reads and writes proceed along contiguous directions as much as possible.
