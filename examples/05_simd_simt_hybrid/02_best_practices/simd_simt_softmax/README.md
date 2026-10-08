# SIMD与SIMT混合编程Softmax性能调优样例

## 概述

本样例以softmax为例，介绍SIMD与SIMT混合编程场景下行内归约算子的调优方法，依次使用三种优化手段：SIMT VF直接读GM建立基线、MTE搬入UB中转并配合双缓冲使搬运与计算流水重叠、将整行中间值驻留寄存器，消除行内计算的重复读取与重复计算。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```text
├── simd_simt_softmax
│   ├── CMakeLists.txt                  // 编译工程文件
│   ├── data_utils.h                    // Host侧bin文件读写工具
│   ├── figures                        // README中的图片资源
│   ├── scripts
│   │   ├── gen_data.py                 // 输入数据和真值数据生成脚本
│   │   └── verify_result.py            // 真值对比脚本
│   ├── softmax_gm.h                    // Case 0：SIMT直接读GM的基线实现
│   ├── softmax_ub.h                    // Case 1：UB中转与MTE双缓冲实现
│   ├── softmax_bucket.h                // Case 2：中间值驻留寄存器实现
│   ├── softmax_host.asc                // Host侧运行入口
│   ├── README.md                       // 样例说明文档
│   └── README_en.md                    // 英文样例说明文档
```

## 样例描述

- 计算公式：

  $$
  y_{i,j} = \frac{\exp(x_{i,j} - \max_{k}{x_{i,k}})}{\sum_{k}\exp(x_{i,k} - \max_{k}{x_{i,k}})}
  $$

  其中，$x_{i,j}$ 为输入第 $i$ 行第 $j$ 列的元素，$y_{i,j}$ 为对应的输出；$\max_{k}{x_{i,k}}$ 表示取第 $i$ 行所有列元素的最大值（$k$ 遍历该行全部列），$\sum_{k}$ 表示对第 $i$ 行所有列求和。

- 计算规则：

  softmax按行独立计算，行与行之间互不依赖。对每一行：先求行内最大值，再将行内每个元素减去该最大值后做指数运算，并对指数结果按行求和，最后用每个指数值除以该行和，得到归一化输出。

- 样例规格：

  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Softmax</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[8192, 197]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[8192, 197]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">softmax_gm_kernel / softmax_ub_kernel / softmax_bucket_kernel</td></tr>
  </table>

  > **输入约束：** 样例默认shape为[8192, 197]；支持通过运行参数指定自定义shape，每行元素个数（cols）为1~512，且总元素数rows×cols不超过UINT32_MAX（4294967295）。

## 样例实现

### Case实现说明

本样例通过3个Case构成一条优化路径，各Case的核函数、线程块数量、每个线程块的线程数和主要变化如下表所示。

| Case | 核函数                              | 线程块数量         | 每个线程块的线程数                        | 本步引入的变化                                                       |
| ---- | ----------------------------------- | ------------------ | ----------------------------------------- | -------------------------------------------------------------------- |
| 0    | softmax_gm_kernel   | 根据数据量动态计算 | 32 × warps_per_block，随rows自适应，最多2048 | SIMT VF直接读GM，一个Warp处理一行，Warp归约完成行内max和sum          |
| 1    | softmax_ub_kernel | 根据数据量动态计算 | 32 × rows_per_tile，随rows和UB容量自适应，最多2048 | 输入经MTE搬入UB中转，双份tile缓冲区轮换，搬运与计算重叠              |
| 2    | softmax_bucket_kernel | 根据数据量动态计算 | 512/1024/2048（按shape动态选择） | 编译期bucket使中间值驻留寄存器；UB按bucket宽度补齐 |

其中线程块数量和每个Thread Block的线程数在Host侧根据数据量动态计算，具体方式见各Case的核心实现；三个Case均采用“一个Warp处理一行”的线程组织。三个Case通过编译选项 `SCENARIO_NUM` 选择，默认为2。

### 性能指标说明

| 指标                 | 说明                                                                        |
| -------------------- | --------------------------------------------------------------------------- |
| Task Duration(μs)   | Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间     |
| aiv_time(μs)        | Task在AI Vector Core上的理论执行时间，单位为μs                              |
| aiv_total_cycles    | Task被分配到每个AI Vector Core计算单元上后的执行cycle总数                   |
| aiv_vec_time(μs)    | vec类型指令（向量类运算指令）耗时，SIMT VF调用中的线程计算也体现在该类统计中 |
| aiv_vec_ratio       | vec类型指令的cycle数在total cycle数中的占用比                               |
| aiv_scalar_time(μs) | scalar类型指令（标量类运算指令）耗时，单位为μs                              |
| aiv_scalar_ratio    | scalar类型指令的cycle数在total cycle数中的占用比                            |
| aiv_mte2_time(μs)   | MTE2搬入指令耗时，主要对应GM到UB的搬运                                      |
| aiv_mte3_time(μs)   | MTE3搬出指令耗时，主要对应UB到GM的搬运                                      |

除Task Duration外，其余指标均为所有Thread Block上的平均值。

### Case 0：SIMT直接读GM基线

**优化目标**：以最直接的实现建立功能正确性和性能基线：Warp直接从GM读取输入，完成行内max、exp/sum和归一化，不引入任何中间缓冲。

**核心实现**：行内softmax分三轮完成，依次求行最大值、指数和与归一化输出，三轮均直接读GM。线程组织采用“一个Warp处理一行”：Warp内32个线程分别负责同一行中不同列的数据，行内最大值与求和通过Warp归约原语完成；一个Thread Block内的多个Warp各自处理不同的行，行方向按grid-stride循环覆盖全部行；每个Thread Block启动的Warp数随rows自适应，rows不足AIV核数时每个Thread Block只处理一行。以默认规格[8192, 197]为例，一行197列由32个线程分多轮处理：每个线程从与自身线程号相同的列开始，以32列为步进依次处理后续列；197 = 6×32 + 5，前6轮32个线程全部参与计算，第7轮仅前5个线程存在待处理数据。一个Warp处理一行，行与行之间并行，如下图所示。

<img src="./figures/warp_per_row.png">

以行内求max为例，Warp内32个线程先各自求得部分最大值，再通过Warp归约原语得到行最大值，如下图所示。

<img src="./figures/case0.png">

```cpp
for (uint32_t row = blockIdx.x * warps_per_block + warp_in_block;  // 一个Warp处理一行
     row < rows;
     row += row_stride) {  // grid-stride循环覆盖全部行
    const uint32_t row_offset = row * cols;

    // 第一轮：从GM读取整行求最大值，asc_reduce_max在Warp内32个线程间归约。
    float max_value = -ASCRT_INF_F;
    for (uint32_t col = thread_in_warp; col < cols; col += warpSize) {
        const float value = static_cast<float>(input[row_offset + col]);
        max_value = max_value > value ? max_value : value;
    }
    max_value = asc_reduce_max(max_value);

    // 第二轮：重新读取GM，计算expf(x - max)并用asc_reduce_add在Warp内归约求和。
    float sum_value = 0.0F;
    for (uint32_t col = thread_in_warp; col < cols; col += warpSize) {
        const float value = static_cast<float>(input[row_offset + col]);
        sum_value += expf(value - max_value);
    }
    sum_value = asc_reduce_add(sum_value);

    // 第三轮：再次读取GM完成归一化写回。三轮循环读取整行三次、计算exp两次。
    for (uint32_t col = thread_in_warp; col < cols; col += warpSize) {
        const float value = static_cast<float>(input[row_offset + col]);
        output[row_offset + col] = static_cast<half>(expf(value - max_value) / sum_value);
    }
}
```

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 12.975 | 11.553 | 19063.1 | 10.917 | 0.946 | 0.633 | 0.054 | 0.005 | 0.000 | 0.002 | 0.000 |

**分析**：

Case 0的 `aiv_vec_ratio` 达到0.946，耗时集中在SIMT线程的GM读写和行内计算上。从实现看，同一行输入在三轮循环中被重复读取三次，`exp` 被计算两次，且GM访问与计算在同一段SIMT线程内串行执行，无法与搬运流水重叠。下一步将数据搬运与计算拆开，由MTE把输入搬入UB中转。

---

### Case 1：UB中转与MTE双缓冲

**优化目标**：使用MTE完成GM与UB之间的数据搬运，SIMT VF使用UB上的数据完成行内计算，并通过双缓冲机制使计算与数据搬运流水并行。

**核心实现**：

- 每个Thread Block按tile处理行数据。tile内的行数决定一次VF调用启动的Warp数量（一个Warp处理一行），行数取以下三者的最小值：本Thread Block分到的行数、UB容量内四份tile缓冲区可容纳的行数、一个Thread Block最多启动2048个线程（64个Warp）对应的64行。
- kernel将动态UB划分为双份输入tile缓冲区与双份输出tile缓冲区，两组缓冲区随tile交替使用，各自的生命周期由独立的事件管理。
- MTE2按真实cols把GM每行搬入UB；MTE3按同样方式把结果搬回GM。
- SIMT VF仍按一个Warp一行的方式完成max、exp/sum和归一化三轮循环，行内处理逻辑与Case 0相同，输入来自UB。

```cpp
uint32_t tile_idx = 0;
for (uint32_t row_base = first_row_base; row_base < rows; row_base += tile_stride) {
    const uint32_t slot = tile_idx & 1;
    const event_t event_id = slot == 0 ? EVENT_ID0 : EVENT_ID1;
    const uint32_t rows_remaining = rows - row_base;
    const uint32_t rows_in_tile = rows_remaining < rows_per_tile ? rows_remaining : rows_per_tile;
    __ubuf__ half* input_tile_ub = slot == 0 ? input_tile_ub0 : input_tile_ub1;
    __ubuf__ half* output_tile_ub = slot == 0 ? output_tile_ub0 : output_tile_ub1;

    asc_sync_wait(PIPE_MTE2, PIPE_V, event_id);
    const uint32_t next_row_base = row_base + tile_stride;
    if (next_row_base < rows) {
        // 当前tile计算前，先把下一个tile搬入另一组输入缓冲区。
        const uint32_t next_slot = slot ^ 1;
        const event_t next_event_id = next_slot == 0 ? EVENT_ID0 : EVENT_ID1;
        const uint32_t next_rows_remaining = rows - next_row_base;
        const uint32_t next_rows_in_tile =
            next_rows_remaining < rows_per_tile ? next_rows_remaining : rows_per_tile;
        __ubuf__ half* next_input_tile_ub = next_slot == 0 ? input_tile_ub0 : input_tile_ub1;

        asc_sync_wait(PIPE_V, PIPE_MTE2, next_event_id);
        asc_copy_gm2ub_align(
            next_input_tile_ub, mutable_gm_ptr(input + next_row_base * cols),
            static_cast<uint16_t>(next_rows_in_tile), row_bytes, 0, 0, false,
            asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, row_bytes, padded_row_bytes);
        asc_sync_notify(PIPE_MTE2, PIPE_V, next_event_id);
    }

    asc_sync_wait(PIPE_MTE3, PIPE_V, event_id);
    asc_vf_call<softmax_ub_vf>(
        dim3(warpSize, rows_in_tile), output_tile_ub, input_tile_ub, rows_in_tile, cols, ub_stride);
    asc_sync_notify(PIPE_V, PIPE_MTE2, event_id);

    asc_sync_notify(PIPE_V, PIPE_MTE3, event_id);
    asc_sync_wait(PIPE_V, PIPE_MTE3, event_id);
    asc_copy_ub2gm_align(
        output + row_base * cols, output_tile_ub, static_cast<uint16_t>(rows_in_tile), row_bytes,
        asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, row_bytes, padded_row_bytes);
    asc_sync_notify(PIPE_MTE3, PIPE_V, event_id);
    ++tile_idx;
}
```

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 11.923 | 10.362 | 17096.9 | 7.833 | 0.758 | 0.721 | 0.069 | 2.495 | 0.241 | 0.568 | 0.055 |

**分析**：

Case 1把GM访问交给MTE流水并引入双缓冲，SIMT VF不再直接读GM：`aiv_mte2_time` 和 `aiv_mte3_time` 从接近0变为2.495μs和0.568μs，说明GM访问已经转移到MTE搬运路径；`aiv_vec_time` 从10.917μs降至7.833μs，下降约28.2%；`Task Duration` 从12.975μs降至11.923μs，下降约8.1%。此时 `aiv_vec_ratio` 达到0.758，是各分项中占比最高的开销，瓶颈已从GM访存转移到行内计算，下一步需要优化计算耗时。

Case 1的计算耗时主要集中在行内计算的组织方式上：行内计算按max、exp/sum、归一化三轮循环组织，每轮都重新读取一遍UB输入行，`exp` 被计算两次。下一步的优化方向是把每个线程分到的元素缓存到局部数组中复用 `exp` 结果，并使数组驻留寄存器。

---

### Case 2：中间值驻留寄存器

**优化目标**：将计算的中间结果，即每个元素的exp值，保存在寄存器中，避免重复的数据搬运和计算。第一步引入静态数组，把整行元素从UB读入，一次读入完成max、exp/sum和归一化；第二步使数组驻留寄存器。为此将实现收敛为编译期bucket：按cols上界64/128/256/384/512分成五档，每档生成特化kernel；Host侧按shape动态选择线程数。

**核心实现**：

- 编译期bucket：本样例cols不超过512，一个线程分到的元素最多为512/32 = 16个，适合全部驻留在寄存器中；这要求编译期可以确定所有对数组的取值地址，即所有数组下标的计算能在编译期确定。但遍历数组的for循环以cols为判断条件，cols是运行时参数，循环次数和每次迭代访问的数组下标在编译期均无法确定，编译器不能展开循环，数组只能分配在栈空间，访问效率低于寄存器。为此将cols按上界划分为64/128/256/384/512五档bucket，按档位实例化特化kernel：bucket容量为编译期常量，for循环的判断条件为常量，循环可完全展开，`elements[kWarpIterations]` 得以驻留寄存器；尾部无效列用 `-ASCRT_INF_F` 屏蔽，不参与归约与归一化。
- UB布局：tile缓冲区按bucket容量 `kBucketCols` 申请，MTE按运行时的真实cols搬运每行数据，真实元素之后的余量空置；计算结果写到UB上，MTE3按真实cols将结果搬回GM。以行内求max为例，Case 2补齐尾部列后的规约过程如下图所示。

  <img src="./figures/case2.png">

- 动态线程选择：UB缓冲区为静态数组，维度由线程数与bucket容量共同决定，必须编译期确定，因此线程数只能取512/1024/2048固定档位，实际启动数与档位一致。Host按每核行数选择档位，512/1024/2048线程对应每个tile最多处理16/32/64行；`cols > 256` 时考虑寄存器压力，最多启动1024个线程；每个Thread Block按固定行数处理数据，rows较小时部分Thread Block无行可处理、直接退出。

SIMT VF将线程分到的元素一次读入数组，后续计算复用已读入的数据，减少数据拷贝：

```cpp
template <uint32_t kThreadCount, uint32_t kBucketCols>
__simt_vf__ __launch_bounds__(kThreadCount) inline void softmax_bucket_vf(
    __ubuf__ half* output_ub, __ubuf__ const half* input_ub, uint32_t row_count, uint32_t cols)
{
    // 每个线程分到的元素个数，为编译期常量。
    constexpr uint32_t kWarpIterations = (kBucketCols + warpSize - 1) / warpSize;
    const uint32_t row_in_tile = threadIdx.x / warpSize;
    const uint32_t thread_in_warp = threadIdx.x % warpSize;
    if (row_in_tile >= row_count) {
        return;
    }

    // tile内每行按kBucketCols的步长存放，真实数据之外的余量空置。
    const uint32_t row_offset = row_in_tile * kBucketCols;
    // 遍历数组的循环边界与访问下标均为编译期常量，数组可驻留寄存器。
    float elements[kWarpIterations];

    // 一轮读入线程分到的元素，尾部无效列用-inf屏蔽，不参与后续计算。
    for (uint32_t iter = 0; iter < kWarpIterations; ++iter) {
        const uint32_t col = thread_in_warp + iter * warpSize;
        elements[iter] = col < cols ? static_cast<float>(input_ub[row_offset + col]) : -ASCRT_INF_F;
    }

    // 对已读入的数组元素求max，再经Warp归约得到行最大值。
    float max_value = elements[0];
    for (uint32_t iter = 1; iter < kWarpIterations; ++iter) {
        max_value = max(max_value, elements[iter]);
    }
    max_value = asc_reduce_max(max_value);

    // exp只计算一次，结果写回数组复用，并经Warp归约得到行指数和。
    float sum_value = 0.0F;
    for (uint32_t iter = 0; iter < kWarpIterations; ++iter) {
        elements[iter] = expf(elements[iter] - max_value);
        sum_value += elements[iter];
    }
    sum_value = asc_reduce_add(sum_value);

    // 归一化写回padded UB，尾部无效列写0；MTE3仅搬回真实cols内的数据。
    for (uint32_t iter = 0; iter < kWarpIterations; ++iter) {
        const uint32_t col = thread_in_warp + iter * warpSize;
        output_ub[row_offset + col] = static_cast<half>(elements[iter] / sum_value);
    }
}
```

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 8.588 | 7.067 | 11661.3 | 4.593 | 0.653 | 0.583 | 0.081 | 2.557 | 0.362 | 0.570 | 0.081 |

**分析**：

Case 2相对Case 1的主要变化是整行中间值驻留寄存器：“运行时cols”变为“编译期bucket上界”后，遍历数组的循环边界与下标在编译期确定，输入行与 `exp` 均只处理一次。`aiv_vec_time` 从7.833μs进一步降至4.593μs，下降约41.4%；`Task Duration` 从11.923μs降至8.588μs，下降约28.0%。bucket只影响UB布局的步长，MTE仍按真实cols搬运，`aiv_mte2_time` 与Case 1基本持平；总耗时下降使 `aiv_vec_ratio` 从0.758降至0.653、`aiv_mte2_ratio` 从0.241升至0.362，行内计算与MTE2搬运的耗时占比已接近。相对Case 0，`Task Duration` 从12.975μs降至8.588μs，整体下降约33.8%。

## 性能对比总结

下面给出各Case在Ascend 950PR上、默认shape[8192, 197]下的性能数据。本样例中的性能数据基于64个AIV核的测试环境采集得到，线程块数量和线程数按该核数计算。

| Case | 优化手段                   | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_mte2_time(μs) | aiv_mte3_time(μs) |
| ---- | -------------------------- | :---: | :---: | :---: | :---: | :---: | :---: |
| 0    | SIMT直接读GM基线           | 12.975 | 11.553 | 10.917 | 0.946 | 0.005 | 0.002 |
| 1    | UB中转与MTE双缓冲          | 11.923 | 10.362 | 7.833  | 0.758 | 2.495 | 0.568 |
| 2    | 中间值驻留寄存器 | 8.588  | 7.067  | 4.593  | 0.653 | 2.557 | 0.570 |

调优主路径Case 0 -> Case 1 -> Case 2，`Task Duration` 由12.975μs依次降至11.923μs、8.588μs，Case 2相对Case 0整体下降约33.8%。

从指标变化可以观察各阶段瓶颈的转移：Case 0的耗时集中在SIMT线程直接读写GM（`aiv_vec_ratio` 0.946，MTE接近0）；Case 1把GM访问转移到MTE2后，`aiv_vec_time` 下降28.2%，但三轮循环仍重复读取UB输入行并重复计算 `exp`；Case 2通过编译期bucket让整行中间值驻留寄存器后，`aiv_vec_time` 再下降41.4%，`aiv_vec_ratio` 降至0.653，同时 `aiv_mte2_ratio` 升至0.362，行内计算与MTE2搬运的耗时占比已接近。

固定rows为8192、逐渐增大一行数据量（cols从64到512），Case 1与Case 2的 `Task Duration` 变化如下图所示，图中竖直虚线为bucket档位边界。

<img src="./figures/perf_trend.png">

Case 1的 `Task Duration` 随cols增大持续上升：行内计算三轮串行遍历，每个线程处理的元素数与cols成正比。Case 2的增长呈阶梯状：同一bucket档位内基本持平——每线程的计算量由 `kWarpIterations` 决定，保持不变，只有MTE按真实cols搬运的数据量随cols小幅增长；跨过bucket边界时计算量跳升一档，`Task Duration` 随之跳升。全部cols区间Case 2均优于Case 1，且cols越大优势越明显，cols=512时Case 2的 `Task Duration` 约为Case 1的48%；bucket上界超出真实cols较多的档位中部（如cols=96、197），Case 2需要多处理尾部无效列，相对优势小于档位边界处（如cols=128、384、512）。

## 调优建议

1. 归约类计算按归约维度组织线程：归约维度内的数据交给一个Warp，用Warp归约原语完成归约；维度之间天然并行，不需要跨Warp同步。
2. 要让数组驻留寄存器，需使数组的访问下标在编译期确定；当循环边界或下标依赖运行时参数、且循环次数较少时，可将参数按少量上界分档并固化为编译期常量，使循环完全展开，避免重复读取输入和重复计算。

## 编译运行

在本样例根目录下执行如下步骤，编译并运行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令，默认运行shape为[8192, 197]的样例。

  ```bash
  SCENARIO_NUM=2                                                                # 选择执行场景，可选0-2
  mkdir -p build && cd build;                                                   # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # 编译工程
  python3 ../scripts/gen_data.py                                                # 生成输入数据和真值数据
  ./softmax                                                                     # 执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin       # 校验真值数据
  ```

  也支持自定义shape：先执行 `python3 ../scripts/gen_data.py <rows> <cols>`，再执行 `./softmax <rows> <cols>`，其中每行元素个数为1~512。

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..; make -j;   # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |:---|:---|:---|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构：本样例仅支持dav-3510（Ascend 950PR&950DT系列产品） |
  | `SCENARIO_NUM` | `0`、`1`、`2` | 样例类型，默认为2：0=SIMT直接读GM，1=UB中转与MTE双缓冲，2=中间值驻留寄存器 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```text
  [Success] Case accuracy verification passed.
  ```

## 性能调试

### msOpProf工具介绍

msOpProf工具是单算子性能分析工具。包含msopprof和msopprof simulator两种使用方式。该工具协助用户定位算子内存、算子代码以及算子指令的异常，实现全方位的算子调优。当前支持基于不同运行模式（上板或仿真）和不同文件形式（可执行文件或算子二进制.o文件）进行性能数据的采集和自动解析。

通过上板性能采集，可以直接测定算子在昇腾AI处理器上的运行时间。该方式适合在板环境中快速定位算子性能问题。

基于可执行文件softmax通过msopprof执行算子调优：

```bash
msopprof ./softmax
```

命令完成后，会在默认目录下生成以“OPPROF_{timestamp}_XXX”命名的性能数据文件夹，文件夹结构示例如下：

```text
├──dump                       # 原始的性能数据，用户无需关注
├──ArithmeticUtilization.csv  # cube/vector指令cycle占比
├──L2Cache.csv                # L2 Cache命中率，影响MTE2，建议合理规划数据搬运逻辑，增加命中率
├──Memory.csv                 # UB，L1和主存储器读写带宽速率
├──MemoryL0.csv               # L0A，L0B，和L0C读写带宽速率
├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
├──OpBasicInfo.csv            # 算子基础信息
├──PipeUtilization.csv        # 采集计算单元和搬运单元耗时和占比
├──ResourceConflictRatio.csv  # UB上的bank group、bank conflict和资源冲突率在所有指令中的占比
└──visualize_data.bin         # MindStudio Insight呈现文件
```

查看具体的性能分析结果：

```bash
# 查看Task Duration 以及各项数据
cat ./OPPROF_*/PipeUtilization.csv
```
