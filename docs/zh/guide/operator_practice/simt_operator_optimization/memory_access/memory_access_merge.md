# 访存合并

【优先级】高

【描述】SIMT编程模式下，一个Warp内的相邻线程（thread）通常同时发起GM访问请求。若这些线程访问的GM地址连续，硬件可以将访问请求合并，从而提高GM带宽利用率；若相邻线程访问的GM地址跨行或离散分布，则难以形成访存合并，算子执行耗时会明显增加。对于转置、重排、gather/scatter等算子，需要结合数据排布和线程映射关系进行分析，尽量让同一个Warp内相邻线程在同一条访存指令上访问连续地址。

**图1**  访存合并对比示意图

<img src="../../../figures/mem_merge.png">

访存合并优化适用于两类场景：第一类是数据本身在GM上连续，只是相邻thread被分配处理的数据不连续，导致同一个Warp内相邻线程访问了不连续地址，此时优先调整数据切分方式，使Warp内相邻thread访问连续数据；第二类是算子功能包含转置、重排等布局变换，单纯调整切分方式很难同时保证GM读写都连续，此时考虑引入Unified Buffer（UB）内存空间作为中转，利用UB完成不连续的数据重排过程，从而尽量保证GM侧访问连续。

- 优化数据切分实现访存合并

  【样例介绍】以[Gather算子](../../../../../../examples/03_simt_api/00_introduction/01_gather/general_gather)为例，算子按行从输入中取数，计算公式可以表示为`output[row, col] = input[index[row], col]`。输出矩阵每一行内部的列方向数据在GM上连续，优化切分时应让同一个Warp内相邻线程尽量访问同一行内的相邻列，而不是只保证单个线程内部连续。

  【反例】按thread切分数据，每个thread处理连续的一行数据。

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

  上述实现把输出行分配给不同thread，每个thread在`for`循环内连续处理一整行数据。从单个thread视角看，`input_idx`和`output_idx`都是连续递增的；但SIMT访存合并的关键是同一个Warp内相邻thread在同一条访存指令上访问的地址连续。在该实现中，同一个Warp内的相邻thread分别处理不同的`out_row`，虽然单个thread内部访问连续，但Warp内访问不连续，GM访存难以合并。

  以输入shape为20001×1023，index长度为核数乘以2048为例，线程块总数与物理核数相等，每个线程块启动2048个线程，反例算子执行该用例的性能数据如下：

  | Task Duration(us) | aiv_time(us) | aiv_total_cycles | aiv_vec_time(us) | aiv_vec_ratio | aiv_scalar_time(us) | aiv_scalar_ratio |
  | :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
  |     4886.076     |   4535.644   |   7483813.083   |     4535.178     |     0.999     |        0.454        |      0.001      |

  【正例】按Thread Block切分输出数据，Warp内相邻thread访问连续地址。


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

  上述实现将输出矩阵按一维线性地址组织，并让每个Thread Block覆盖一段连续的输出元素。对于同一次循环迭代，Thread Block内thread的`idx`连续递增，同一个Warp内相邻thread会访问连续的`gather_output[idx]`。当这些`idx`位于同一个输出行内时，`out_row`相同，`out_col`连续递增，`in_row = index[out_row]`，因此`in_row`也相同，输入访问`input[in_row * in_width + out_col]`的地址也是连续的，输入和输出都可以实现访存合并。

  在正例的代码中，每个thread在整个过程中并非处理一段连续数据，而是以`blockDim.x * gridDim.x`为步长处理多个元素。也就是说，单个thread多次迭代访问的数据是跳跃的；但在每一次迭代中，Warp内相邻thread访问的是连续地址。SIMT访存合并的收益来自Warp维度的连续访问，因此这种切分方式通常比“每个thread处理一整段连续数据”更能发挥GM带宽优势。

  配套的Thread Block切分也从按`index_total_length`切分调整为按`output_total_size`切分，使线程数量覆盖真实输出元素规模：

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

  正例算子实现访存合并，执行该用例的性能数据如下：

  | Task Duration(us) | aiv_time(us) | aiv_total_cycles | aiv_vec_time(us) | aiv_vec_ratio | aiv_scalar_time(us) | aiv_scalar_ratio |
  | :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
  |      866.103      |   864.080   |   1425731.861   |     863.590     |     0.999     |        0.478        |      0.001      |

  从Task Duration看，访存合并后的执行耗时为866.103us，相比优化前的4886.076us，下降约82.3%。这说明优化前的主要瓶颈来自Warp内相邻thread访问的GM地址不连续，调整数据切分后，相邻thread在同一次访存指令上访问相邻元素，GM访存效率得到明显改善。
- 引入UB中转实现访存合并

  【样例介绍】以[矩阵转置](../../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/matrix_transpose_practice)为例，输入矩阵shape为1024×1024，数据类型为`float`。直接全局内存转置版本按一维线性地址遍历元素，逐元素写入转置后的GM位置，Thread Block数量固定为硬件vector core数，每个Thread Block启动2048个线程；UB中转版本将矩阵划分为32×32的tile，Thread Block数量按`ceil(total_tiles / 2)`设置，每个Thread Block启动2048个线程同时处理两个tile，`threadIdx.x`方向的32个thread对应tile内同一行的32个连续元素，先将tile连续读入UB，再按转置方向读取并连续写回GM。

  【反例】直接按照转置公式写回GM。

  ```cpp
  uint32_t row = i / width;
  uint32_t col = i - row * width;
  // input(row,col) -> output(col,row)
  output[col * height + row] = input[i];
  ```

  上述实现按一维线性地址遍历输入矩阵，每个线程在循环中处理多个`input[i]`，并写入转置位置`output[col * height + row]`。读取输入时，同一个Warp内相邻thread的`i`连续递增，`input[i]`地址连续，GM读访问可以合并；写回输出时，同一Warp内相邻thread的`i`递增对应`col`递增（同一行内），转置后的写地址`col * height + row`按`height`步长跨行分布，相邻thread会写到输出矩阵的不同行，GM写访问难以形成合并访存。

  在1024×1024矩阵转置样例中，该实现对应Case 2，性能数据如下：

  | Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
  | :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
  |      36.337      |    34.022    |     56136.2      |     33.568      |     0.987     |        0.442        |      0.013      |

  【正例】使用UB中转，将非连续GM写转换为UB内转置读。


  ```cpp
  __ubuf__ float tile[TILES_PER_BLOCK][TILE_DIM][TILE_DIM];
  ...
  uint32_t local_tile = threadIdx.y >> 5;       // 0或1，区分Thread Block内两个tile
  uint32_t ty = threadIdx.y & (TILE_DIM - 1);
  uint32_t tx = threadIdx.x;
  ...
  tile[local_tile][ty][tx] = input[x_index + y_index * width];
  asc_syncthreads();
  output[x_index + y_index * height] = tile[local_tile][tx][ty];
  ```

  上述实现先将输入矩阵按32×32的tile连续读入UB，再交换输出tile坐标，使同一个Warp按输出矩阵的行方向连续写回GM。这样处理后，GM读取阶段按输入矩阵行方向连续访问，GM写回阶段按输出矩阵行方向连续访问；转置造成的非连续访问被转移为UB内部的转置方向读取。每个Thread Block启动2048个线程，通过`local_tile = threadIdx.y >> 5`区分Block内的两个tile，`threadIdx.y=0..31`处理第一个tile，`threadIdx.y=32..63`处理第二个tile。

  在1024×1024矩阵转置样例中，该实现对应Case 3，性能数据如下：

  | Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio |
  | :---------------: | :----------: | :--------------: | :--------------: | :-----------: | :-----------------: | :--------------: |
  |      28.190      |    2.896    |      4778.1      |      2.712      |     0.938     |        0.172        |      0.058      |

  从Task Duration看，使用UB中转后的执行耗时为28.190μs，相比直接全局内存转置的36.337μs，下降约22.4%。这说明该转置场景的主要收益来自将原本不连续的GM写访问转换为UB内转置读，使GM读写两侧都尽量保持连续访问；虽然引入UB会带来一次中转和线程同步开销，但在该用例中，GM写访问合并带来的收益超过了这些额外开销。

【总结】SIMT算子进行访存优化时，应优先分析同一个Warp内相邻thread在同一条访存指令下访问的GM地址是否连续。常用优化方法包括：一是调整数据切分方式，将连续的数据区域分配给Thread Block和Warp，使每次迭代中Warp内相邻thread访问连续地址，单个thread可通过stride处理剩余数据以覆盖完整输出；二是引入UB进行数据中转和布局重排，将原本不连续的GM访问转换为UB内的重排访问，从而尽量保证GM读写都沿连续方向进行。
