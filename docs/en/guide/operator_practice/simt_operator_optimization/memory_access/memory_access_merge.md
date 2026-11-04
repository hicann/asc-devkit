# Memory Access Merging

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T13:14:56.450Z pushedAt=2026-11-03T07:05:13.375Z nl=2 -->

[Priority] High

[Description] In SIMT programming mode, adjacent threads within a Warp usually issue GM access requests simultaneously. If the GM addresses accessed by these threads are contiguous, the hardware can merge the access requests, thereby improving GM bandwidth utilization. If the GM addresses accessed by adjacent threads span rows or are discretely distributed, memory access merging is difficult to form, and the operator execution time increases significantly. For operators such as transpose, reorder, and gather/scatter, you need to analyze the data layout and thread mapping relationship, and try to make adjacent threads within the same Warp access contiguous addresses in the same memory access instruction.

**Figure 1** Comparison of memory access merging

<img src="../../../figures/mem_merge.png">

Memory access merging optimization applies to two types of scenarios. The first type is that the data itself is contiguous in GM, but the data assigned to adjacent threads is not contiguous, causing adjacent threads within the same Warp to access non-contiguous addresses. In this case, prioritize adjusting the data partitioning method so that adjacent threads within a Warp access contiguous data. The second type is that the operator function includes layout transformations such as transpose and reorder, where simply adjusting the partitioning method makes it difficult to ensure that both GM reads and writes are contiguous. In this case, consider introducing the Unified Buffer (UB) memory space as an intermediate buffer, and use UB to complete the non-contiguous data reordering process, thereby ensuring contiguous access on the GM side as much as possible.

- Optimizing data partitioning to implement memory access merging

  [Sample introduction] Take the [Gather operator](../../../../../../examples/03_simt_api/00_introduction/01_gather/general_gather) as an example. The operator reads data from the input by row, and the calculation formula can be expressed as `output[row, col] = input[index[row], col]`. The column-direction data within each row of the output cube is contiguous in GM. When optimizing the partitioning, adjacent threads within the same Warp should access adjacent columns within the same row as much as possible, rather than only ensuring contiguity within a single thread.

  [Negative example] Partition data by thread, where each thread processes a contiguous row of data.

  ```cpp
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

  The above implementation assigns output rows to different threads, and each thread processes an entire row of data continuously within a `for` loop. From the perspective of a single thread, both `input_idx` and `output_idx` are consecutively increasing. However, the key to SIMT memory access merging is that adjacent threads within the same Warp access contiguous addresses in the same memory access instruction. In this implementation, adjacent threads within the same Warp process different `out_row` values. Although access within a single thread is contiguous, access within the Warp is not contiguous, making GM memory access difficult to merge.

  Taking an input shape of 20001×1023 and an index length equal to the number of cores multiplied by 2048 as an example, the total number of thread blocks equals the number of physical cores, and each thread block launches 2048 threads. The performance data of the negative example operator executing this case is as follows:

  | Task Duration (us) | aiv_time (us) | aiv_total_cycles | aiv_vec_time (us) | aiv_vec_ratio | aiv_scalar_time (us) | aiv_scalar_ratio |
  | :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
  |     4886.076     |   4535.644   |   7483813.083   |     4535.178     |     0.999     |        0.454        |      0.001      |

  [Positive example] Partition the output data by thread block so that adjacent threads within a warp access contiguous addresses.


  ```cpp
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

  The implementation above organizes the output cube by one-dimensional linear addresses and lets each thread block cover a contiguous segment of output elements. For the same loop iteration, the `idx` of threads within a thread block increases consecutively, and adjacent threads within the same warp access contiguous `gather_output[idx]`. When these `idx` values fall within the same output row, `out_row` is the same and `out_col` increases consecutively. Since `in_row = index[out_row]`, `in_row` is also the same, so the input access address `input[in_row * in_width + out_col]` is also contiguous. Both input and output can therefore achieve memory access merging.

  In the positive example code, each thread does not process a contiguous segment of data throughout the entire process. Instead, it processes multiple elements with a stride of `blockDim.x * gridDim.x`. In other words, the data accessed by a single thread across multiple iterations is non-contiguous. However, in each iteration, adjacent threads within a warp access contiguous addresses. The benefit of SIMT memory access merging comes from contiguous access at the warp level, so this partitioning method generally exploits GM bandwidth better than "each thread processing a contiguous segment of data".

  The corresponding thread block partitioning is also adjusted from partitioning by `index_total_length` to partitioning by `output_total_size`, so that the number of threads covers the actual output element scale:

  ```cpp
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

  The positive example operator implements memory access merging. The performance data of executing this case is as follows:

  | Task Duration (us) | aiv_time (us) | aiv_total_cycles | aiv_vec_time (us) | aiv_vec_ratio | aiv_scalar_time (us) | aiv_scalar_ratio |
  | :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
  |      866.103      |   864.080   |   1425731.861   |     863.590     |     0.999     |        0.478        |      0.001      |

  In terms of Task Duration, the execution time after memory access merging is 866.103us, a decrease of about 82.3% compared with the 4886.076us before optimization. This indicates that the main bottleneck before optimization came from the non-contiguous GM addresses accessed by adjacent threads within a warp. After adjusting the data partitioning, adjacent threads access adjacent elements in the same memory access instruction, and GM access efficiency is significantly improved.
- Introducing UB relay to implement memory access merging

  [Sample introduction] Take [cube transpose](../../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice) as an example. The input cube has a shape of 1024×1024 and the data type is `float`. The direct global memory transpose version traverses elements by one-dimensional linear addresses and writes each element to the transposed GM position. The number of Thread Blocks is fixed to the number of hardware vector cores, and each Thread Block launches 2048 threads. The UB transfer version divides the cube into 32×32 tiles. The number of Thread Blocks is set to `ceil(total_tiles / 2)`, and each Thread Block launches 2048 threads to process two tiles simultaneously. The 32 threads in the `threadIdx.x` direction correspond to the 32 consecutive elements in the same row within a tile. The tile is first read into UB contiguously, and then read in the transpose direction and written back to GM contiguously.

  [Negative example] Write back to GM directly according to the transpose formula.

  ```cpp
  uint32_t row = i / width;
  uint32_t col = i - row * width;
  // input(row,col) -> output(col,row)
  output[col * height + row] = input[i];
  ```

  The implementation above traverses the input cube by one-dimensional linear addresses. Each thread processes multiple `input[i]` elements in a loop and writes them to the transposed position `output[col * height + row]`. When reading the input, the `i` of adjacent threads within the same Warp increases consecutively, so the `input[i]` addresses are contiguous and the GM read access can be merged. When writing back the output, the increasing `i` of adjacent threads within the same Warp corresponds to an increasing `col` (within the same row). The transposed write address `col * height + row` is distributed across rows with a stride of `height`, so adjacent threads write to different rows of the output cube, making it difficult to form merged memory access for the GM write access.

  In the 1024×1024 cube transpose sample, this implementation corresponds to Case 2, and the performance data is as follows:

  | Task Duration (μs) | aiv_time (μs) | aiv_total_cycles | aiv_vec_time (μs) | aiv_vec_ratio | aiv_scalar_time (μs) | aiv_scalar_ratio |
  | :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
  |      36.337      |    34.022    |     56136.2      |     33.568      |     0.987     |        0.442        |      0.013      |

  [Positive example] Use UB transfer to convert non-contiguous GM writes into transposed reads within UB.


  ```cpp
  __ubuf__ float tile[TILES_PER_BLOCK][TILE_DIM][TILE_DIM];
  ...
  uint32_t local_tile = threadIdx.y >> 5;       // 0 or 1, distinguishing the two tiles within the Thread Block
  uint32_t ty = threadIdx.y & (TILE_DIM - 1);
  uint32_t tx = threadIdx.x;
  ...
  tile[local_tile][ty][tx] = input[x_index + y_index * width];
  asc_syncthreads();
  output[x_index + y_index * height] = tile[local_tile][tx][ty];
  ```

  The implementation above first reads the input cube into UB contiguously by 32×32 tiles, and then swaps the output tile coordinates so that the same Warp writes back to GM contiguously along the row direction of the output cube. After this processing, the GM read phase accesses contiguously along the row direction of the input cube, and the GM write-back phase accesses contiguously along the row direction of the output cube. The non-contiguous access caused by the transpose is transferred to transposed-direction reads within UB. Each Thread Block launches 2048 threads. The two tiles within the Block are distinguished by `local_tile = threadIdx.y >> 5`, where `threadIdx.y=0..31` processes the first tile and `threadIdx.y=32..63` processes the second tile.

  In the 1024×1024 cube transpose sample, this implementation corresponds to Case 3, and the performance data is as follows:

  | Task Duration (μs) | aiv_time (μs) | aiv_total_cycles | aiv_vec_time (μs) | aiv_vec_ratio | aiv_scalar_time (μs) | aiv_scalar_ratio |
  | :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
  |      28.190      |    2.896    |      4778.1      |      2.712      |     0.938     |        0.172        |      0.058      |

  In terms of Task Duration, the execution time after using UB relay is 28.190 μs, a decrease of about 22.4% compared with 36.337 μs for direct global memory transpose. This indicates that the main benefit in this transpose scenario comes from converting the originally non-contiguous GM write access into a transpose read within UB, so that both GM read and write sides maintain contiguous access as much as possible. Although introducing UB brings one relay and thread synchronization overhead, in this use case the benefit of GM write access merging outweighs these additional overheads.

[Summary] When optimizing memory access for SIMT operators, you should first analyze whether the GM addresses accessed by adjacent threads within the same Warp under the same memory access instruction are contiguous. Common optimization methods include: first, adjust the data partitioning method to assign contiguous data regions to Thread Blocks and Warps, so that adjacent threads within a Warp access contiguous addresses in each iteration, and a single thread can process the remaining data through stride to cover the complete output; second, introduce UB for data relay and layout rearrangement, converting the originally non-contiguous GM access into rearranged access within UB, so as to keep both GM read and write along the contiguous direction as much as possible.
