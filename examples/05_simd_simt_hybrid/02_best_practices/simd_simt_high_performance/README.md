# SIMD与SIMT混合编程高性能优化样例


## 概述

本样例以FloorMod计算为例，介绍SIMD与SIMT混合编程场景下的性能调优方法。样例展示直接使用SIMT访问GM、使用SIMD Reg矢量计算、使用SIMT访问UB但线程映射不连续，以及调整线程映射使Warp内相邻线程连续访问UB后的性能差异。

**优化路径**：

| Case | SCENARIO_NUM | 实现方式 | 说明 |
|:---|:---:|:---|:---|
| Case 0 | 0 | SIMT直接访问GM | 数据访问和计算均通过SIMT实现，SIMT线程直接访问GM，作为纯SIMT实现的对照组。 |
| Case 1 | 1 | SIMD Reg矢量计算 | GM与UB之间的数据搬运通过MTE完成，使用Reg矢量计算实现FloorMod，作为纯SIMD计算的对照组。 |
| Case 2 | 2 | SIMT非连续访问UB | 基于搬运接口将GM连续搬运到UB，SIMT每个线程处理一段连续元素，但Warp内相邻线程访问不连续。 |
| Case 3 | 3 | SIMT连续访问UB | 基于搬运接口将GM连续搬运到UB，调整SIMT线程映射，使Warp内相邻线程访问连续地址。 |

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构介绍

```
├── simd_simt_high_performance
│   ├── CMakeLists.txt        // 编译工程文件
│   ├── data_utils.h          // 数据读入写出函数
│   ├── figures               // 图示
│   ├── floor_mod.asc         // Ascend C样例实现
│   ├── README.md             // 样例说明文档
│   └── scripts
│       ├── gen_data.py       // 输入数据和真值数据生成脚本
│       └── verify_result.py  // 真值对比脚本
```

## 样例描述

**样例功能**：

样例实现FloorMod计算，FloorMod常用于张量计算中的周期性索引、分组映射等需要保持负数取模语义一致的场景。其中 $r$ 为 C/C++ 取余结果（即 $r = x - \operatorname{trunc}(x / y) \times y$，符号跟随被除数 $x$），再经符号修正得到 FloorMod 结果 $z$（符号跟随除数 $y$）。计算公式如下：

$$
r = x - \operatorname{trunc}(x / y) \times y
$$

$$
z =
\begin{cases}
r + y, & r \ne 0 \land ((r < 0) \ne (y < 0)) \\
r, & \text{otherwise}
\end{cases}
$$

**样例规格**：

<table>
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">FloorMod</td></tr>
<tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[8192, 8192]</td><td align="center">int32</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[8192, 8192]</td><td align="center">int32</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[8192, 8192]</td><td align="center">int32</td><td align="center">ND</td></tr>
<tr><td rowspan="4" align="center">核函数名</td><td align="center">SCENARIO_NUM=0</td><td colspan="3" align="center"><code>floor_mod_gm_simt_custom</code></td></tr>
<tr><td align="center">SCENARIO_NUM=1</td><td colspan="3" align="center"><code>floor_mod_simd_custom</code></td></tr>
<tr><td align="center">SCENARIO_NUM=2</td><td colspan="3" align="center"><code>floor_mod_simt_non_contiguous_custom</code></td></tr>
<tr><td align="center">SCENARIO_NUM=3</td><td colspan="3" align="center"><code>floor_mod_simt_contiguous_custom</code></td></tr>
</table>

> **输入约束：** 除数 $y$ 取值必须为非零（SIMT 路径直接执行取模，$y$ 为 0 会触发除零问题）。数据生成脚本会避开 0 值。

## 样例实现

### 性能指标说明

| 字段名 | 字段含义 |
|:---|:---|
| Task Duration(μs) | Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间。 |
| aiv_time(μs) | Task 在 AI Vector Core 上的执行时间。 |
| aiv_vec_time(μs) | vec类型指令耗时。SIMT VF调用中的线程计算也体现在该类统计中。 |
| aiv_vec_ratio | vec类型指令cycle数在total cycle数中的占用比。 |
| aiv_scalar_time(μs) | scalar类型指令耗时。 |
| aiv_scalar_ratio | scalar类型指令cycle数在total cycle数中的占用比。 |
| aiv_mte2_time(μs) | mte2类型指令耗时，主要对应GM到UB的搬运。 |
| aiv_mte2_ratio | mte2类型指令cycle数在total cycle数中的占用比。 |
| aiv_mte3_time(μs) | mte3类型指令耗时，主要对应UB到GM的搬运。 |
| aiv_mte3_ratio | mte3类型指令cycle数在total cycle数中的占用比。 |

除Task Duration外，其余指标均为所有Thread Block上的平均值。

### Case 0：SIMT直接访问GM

**实现方式**：参考 `floor_mod_simt_gm_contiguous()` 函数实现。

该实现不通过MTE2/MTE3做GM与UB之间的数据搬运，而是由SIMT线程直接从GM读取 `x` 和 `y`，计算后直接写回GM。

**关键代码**：

```cpp
__simt_vf__ inline void floor_mod_simt_gm_contiguous(
    __gm__ int32_t* x, __gm__ int32_t* y, __gm__ int32_t* z, uint32_t input_total_length)
{
    for (uint32_t index = blockIdx.x * blockDim.x + threadIdx.x; index < input_total_length;
         index += gridDim.x * blockDim.x) {
        int32_t y_value = y[index];
        const int32_t rem = x[index] % y_value;
        bool signs_differ = ((rem < 0) != (y_value < 0));
        if (signs_differ && (rem != 0)) {
            z[index] = rem + y_value;
        } else {
            z[index] = rem;
        }
    }
}
```

**性能数据**：

| Case | 实现方式 | 核数 | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 0 | SIMT直接访问GM | 64 | 812.063 | 786.138 | 785.715 | 0.999 | 0.411 | 0.001 | 0.004 | 0.000 | 0.001 | 0.000 |

**性能数据分析**：

- 该Case的 `aiv_mte2_time` 和 `aiv_mte3_time` 接近0，说明数据读写没有走MTE2/MTE3的GM->UB/UB->GM搬运路径。
- `Task Duration` 为 **812.063μs**，其中 `aiv_vec_time` 为 **785.715μs**，`aiv_vec_ratio` 达到 **0.999**，耗时主要集中在SIMT内部的GM读写和FloorMod计算上。

**原理说明**：

SIMT线程直接访问GM时，每个线程在计算过程中都需要从GM读取 `x`、`y`，并将结果写回GM，访问效率低，且数据搬运与计算耦合在同一段SIMT线程中，无法并行搬运和计算流水。

本样例的输入、输出在GM上是连续排布的二维ND数据，适合使用连续块搬运。后续实现中通过 `asc_copy_gm2ub_align` 和 `asc_copy_ub2gm_align` 完成GM->UB和UB->GM搬运，搬运由MTE执行，可以按较大的连续数据块把GM数据搬入UB，再将UB中结果连续写回GM。相比SIMT线程逐元素直接访问GM，这种方式内存访问效率更高，也能并行化数据搬运和计算流水。

**下一步优化方向**：

当前Case作为纯SIMT直接访问GM的基准Case，用于展示纯SIMT实现样例的性能数据；下一步将引入SIMD实现，使用搬运接口完成GM->UB和UB->GM搬运，并使用Reg矢量计算实现FloorMod，从而展示该实现的性能数据。

### Case 1：SIMD Reg矢量计算

**实现方式**：参考 `floor_mod_simd()` 函数实现。

该实现使用 `asc_copy_gm2ub_align` 和 `asc_copy_ub2gm_align` 完成GM与UB之间的数据搬运，并在SIMD VF函数中通过 `asc_loadalign` 将UB数据加载到向量寄存器，执行 `asc_div`、`asc_mul`、`asc_sub`、`asc_ne`、`asc_select` 等向量指令，最后通过 `asc_storealign` 写回UB。计算过程通过SIMD实现，GM与UB之间的数据搬运通过MTE完成。

**关键代码**：

```cpp
__simd_vf__ inline void floor_mod_simd(
    __ubuf__ int32_t* z_addr, __ubuf__ int32_t* x_addr, __ubuf__ int32_t* y_addr, const uint32_t count)
{
    constexpr uint32_t one_repeat_size = asc_get_vf_len() / sizeof(int32_t);
    uint16_t loop_times = ceil_div(count, one_repeat_size);
    vector_int32_t x_value, y_value, mod_value, temp_value, default_value, sign_value;
    vector_bool mask, select_mask, adjust_mask;
    uint32_t mask_count = count;

    asc_duplicate_scalar(default_value, int32_t(-1));
    asc_duplicate_scalar(sign_value, static_cast<int32_t>(FMOD_B32_SIGN));

    for (uint16_t i = 0; i < loop_times; i++) {
        mask = asc_update_mask_b32(mask_count);
        asc_loadalign(x_value, x_addr + i * one_repeat_size);
        asc_loadalign(y_value, y_addr + i * one_repeat_size);
        asc_div(temp_value, x_value, y_value, mask);
        asc_mul(temp_value, y_value, temp_value, mask);
        asc_sub(mod_value, x_value, temp_value, mask);
        asc_ne_scalar(select_mask, y_value, int32_t(0), mask);
        asc_select(temp_value, mod_value, default_value, select_mask);
        asc_add(mod_value, temp_value, y_value, mask);
        asc_ne_scalar(adjust_mask, temp_value, int32_t(0), mask);
        asc_and(x_value, temp_value, sign_value, mask);
        asc_and(y_value, y_value, sign_value, mask);
        asc_ne(select_mask, x_value, y_value, mask);
        asc_and(adjust_mask, select_mask, adjust_mask, mask);
        asc_select(mod_value, mod_value, temp_value, adjust_mask);
        asc_storealign(z_addr + i * one_repeat_size, mod_value, mask);
    }
}

__aicore__ inline void process_tiles(
    __gm__ int32_t* x, __gm__ int32_t* y, __gm__ int32_t* z, uint32_t compute_mode)
{
    ...
    for (uint32_t tile_idx = 0; tile_idx < TILE_NUM_PER_CORE; ++tile_idx) {
        uint32_t tile_offset = tile_idx * TILE_LENGTH;
        ...

        asc_copy_gm2ub_align(x_local, x_gm + tile_offset, 1, tile_bytes, 0, 0, false, asc_load_l2_cache_mode::NOTALLOC_KEEP, 0, 0);
        asc_copy_gm2ub_align(y_local, y_gm + tile_offset, 1, tile_bytes, 0, 0, false, asc_load_l2_cache_mode::NOTALLOC_KEEP, 0, 0);
        ...
        asc_vf_call<floor_mod_simd>(z_local, x_local, y_local, TILE_LENGTH);
        ...
        asc_copy_ub2gm_align(z_gm + tile_offset, z_local, 1, tile_bytes, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
    }
    ...
}
```

**性能数据**：

| Case | 实现方式 | 核数 | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 | SIMD Reg矢量计算 | 64 | 525.736 | 515.723 | 509.987 | 0.989 | 2.419 | 0.005 | 217.341 | 0.421 | 62.274 | 0.121 |

**性能数据分析**：

- 相比Case 0，Case 1引入MTE搬运完成GM->UB/UB->GM搬运，`Task Duration` 从 **812.063μs** 降到 **525.736μs**，端到端耗时下降 **35.3%**。
- `Task Duration` 为 **525.736μs**，其中 `aiv_vec_time` 为 **509.987μs**，`aiv_vec_ratio` 达到 **98.9%**，说明耗时主要集中在Reg矢量计算路径上。
- `aiv_mte2_ratio` 为 **42.1%**，`aiv_mte3_ratio` 为 **12.1%**，搬运耗时可以与计算过程形成一定重叠；当前主要瓶颈不是GM搬运过程，而是 FloorMod 计算本身的向量指令执行。

**原理说明**：

FloorMod 计算不仅包含取余，还需要根据余数和除数的符号关系做条件修正。使用Reg矢量计算实现时，分支判断不能直接写成普通 `if/else`，需要拆成 `Compare`、`And`、`MaskAnd`、`Select` 等多条向量指令来表达条件逻辑。同时，`asc_div`、`asc_mul`、`asc_sub`、符号判断和最终选择之间存在较长的数据依赖链，且后续指令需要等待前序结果，因此导致 `aiv_vec_ratio` 较高，表现为Vec bound。

相比SIMD，SIMT更适合处理这类带分支判断的逐元素计算，可以在线程内部直接使用 `%` 和条件语句完成 FloorMod 修正，减少用向量掩码指令拼接分支逻辑带来的额外计算链路。

**下一步优化方向**：

当前Case作为SIMD计算的基准Case，用于展示纯SIMD实现时样例的性能数据；下一步将在保留连续GM<->UB搬运方式的基础上，使用SIMT优化Vector计算部分。

### Case 2：SIMT非连续访问UB

**实现方式**：参考 `floor_mod_simt_non_contiguous()` 函数实现。

该实现先通过 `asc_copy_gm2ub_align` 将GM中的 `x` 和 `y` 连续搬运到UB，然后采用SIMT编程方式实现计算过程，通过if else语句完成分支判断。每个线程处理一段连续的8个元素，同一个Warp内相邻线程访问的地址并不连续：

**关键代码**：

```cpp
__simt_vf__ inline void floor_mod_simt_non_contiguous(
    __ubuf__ int32_t* x, __ubuf__ int32_t* y, __ubuf__ int32_t* z, uint32_t input_total_length)
{
    constexpr uint32_t elems_per_thread = TILE_LENGTH / THREAD_COUNT;
    uint32_t tid = threadIdx.x;
    for (uint32_t i = 0; i < elems_per_thread; i++) {
        uint32_t index = tid * elems_per_thread + i;
        if (index >= input_total_length) {
            break;
        }
        int32_t y_value = y[index];
        int32_t rem = x[index] % y_value;
        bool signs_differ = ((rem < 0) != (y_value < 0));
        if (signs_differ && (rem != 0)) {
            z[index] = rem + y_value;
        } else {
            z[index] = rem;
        }
    }
}

__aicore__ inline void process_tiles(
    __gm__ int32_t* x, __gm__ int32_t* y, __gm__ int32_t* z, uint32_t compute_mode)
{
    ...
    for (uint32_t tile_idx = 0; tile_idx < TILE_NUM_PER_CORE; ++tile_idx) {
        uint32_t tile_offset = tile_idx * TILE_LENGTH;
        ...
        asc_copy_gm2ub_align(x_local, x_gm + tile_offset, 1, tile_bytes, 0, 0, false, asc_load_l2_cache_mode::NOTALLOC_KEEP, 0, 0);
        asc_copy_gm2ub_align(y_local, y_gm + tile_offset, 1, tile_bytes, 0, 0, false, asc_load_l2_cache_mode::NOTALLOC_KEEP, 0, 0);
        ...

        asc_vf_call<floor_mod_simt_non_contiguous>(dim3(THREAD_COUNT), x_local, y_local, z_local, TILE_LENGTH);
        asc_sync_data_barrier(mem_dsb_t::DSB_UB);
        ...
        asc_copy_ub2gm_align(z_gm + tile_offset, z_local, 1, tile_bytes, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
    }
    ...
}
```

**性能数据**：

| Case | 实现方式 | 核数 | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 2 | SIMT非连续访问UB | 64 | 538.098 | 532.659 | 501.605 | 0.942 | 2.338 | 0.004 | 527.987 | 0.991 | 40.091 | 0.075 |

**性能数据分析**：

- 相比Case 0，Case 2使用搬运接口完成GM->UB/UB->GM搬运，`Task Duration` 从 **812.063μs** 降到 **538.098μs**，端到端耗时下降 **33.7%**，说明引入UB中转后，直接GM访问带来的开销被明显缓解。
- 相比Case 1，Case 2的 `aiv_vec_time` 从 **509.987μs** 降到 **501.605μs**，下降 **1.6%**，说明使用SIMT表达FloorMod的逐元素计算后，计算侧有一定收益。
- 但Case 2的 `Task Duration` 比Case 1增加 **2.4%**，端到端没有取得优化收益。主要原因是 `aiv_mte2_time` 从Case 1的 **217.341μs** 增加到 **527.987μs**，增加 **142.9%**；`aiv_mte2_ratio` 也达到 **0.991**。

**原理说明**：

Case 2的索引方式为 `index = tid * elems_per_thread + i`，其中 `elems_per_thread = 8`。从单个线程看，它会连续处理8个元素；

Ascend 950PR/950DT的UB划分为16个bank、组织为8个bank group，每个bank每行为32B（连续每32B落到下一个bank），向量计算单元每拍可从每个bank group读/写一行（32B）。Warp内的32个线程同拍执行同一条指令、各自寻址。

在Case 2中，相邻线程 `tid` 与 `tid+1` 在同一轮迭代 `i` 下访问的元素相差 `elems_per_thread = 8`，即字节地址相差 `8 * sizeof(int32) = 32B`，恰好为一条bank行的跨度。这意味着相邻线程分别落在**不同的bank行**上，硬件无法用一次行读取（32B）同时服务多个线程，相同数量的数据需要更多拍才能取完，UB访问被拉长。

<div align="center">
  <img src="./figures/ubBank.png" alt="Ascend 950PR ubBanks示意图" width="600">
</div>

**下一步优化方向**：

Case 2虽然用SIMT改善了FloorMod的计算表达，但非连续UB访问把MTE2搬运时间显著拉长，导致端到端性能反而比纯SIMD实现更差。下一步将在保留连续GM<->UB搬运和SIMT计算表达的基础上，调整线程到数据的映射关系，把“每个线程处理一整段连续数据”改成“同一轮迭代中Warp内相邻线程访问连续元素”，使相邻线程落入同一条32B bank行，由一次行读取同时服务多个线程，提升UB访问效率。

### Case 3：SIMT连续访问UB

**实现方式**：参考 `floor_mod_simt_contiguous()` 函数实现。

该实现同样通过 `asc_copy_gm2ub_align` 将GM中的 `x` 和 `y` 连续搬运到UB，但改变SIMT线程映射方式，让同一个Warp内相邻线程访问相邻元素：

**关键代码**：

```cpp
__simt_vf__ inline void floor_mod_simt_contiguous(
    __ubuf__ int32_t* x, __ubuf__ int32_t* y, __ubuf__ int32_t* z, uint32_t input_total_length)
{
    for (uint32_t index = threadIdx.x; index < input_total_length; index += blockDim.x) {
        int32_t y_value = y[index];
        const int32_t rem = x[index] % y_value;
        bool signs_differ = ((rem < 0) != (y_value < 0));
        if (signs_differ && (rem != 0)) {
            z[index] = rem + y_value;
        } else {
            z[index] = rem;
        }
    }
}

__aicore__ inline void process_tiles(
    __gm__ int32_t* x, __gm__ int32_t* y, __gm__ int32_t* z, uint32_t compute_mode)
{
    ...
    for (uint32_t tile_idx = 0; tile_idx < TILE_NUM_PER_CORE; ++tile_idx) {
        uint32_t tile_offset = tile_idx * TILE_LENGTH;
        ...

        asc_copy_gm2ub_align(x_local, x_gm + tile_offset, 1, tile_bytes, 0, 0, false, asc_load_l2_cache_mode::NOTALLOC_KEEP, 0, 0);
        asc_copy_gm2ub_align(y_local, y_gm + tile_offset, 1, tile_bytes, 0, 0, false, asc_load_l2_cache_mode::NOTALLOC_KEEP, 0, 0);
        ...
        asc_vf_call<floor_mod_simt_contiguous>(dim3(THREAD_COUNT), x_local, y_local, z_local, TILE_LENGTH);
        asc_sync_data_barrier(mem_dsb_t::DSB_UB);
        ...
        asc_copy_ub2gm_align(z_gm + tile_offset, z_local, 1, tile_bytes, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
    }
    ...
}
```

**性能数据**：

| Case | 实现方式 | 核数 | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 3 | SIMT连续访问UB | 64 | **463.179** | 451.519 | **301.474** | 0.668 | 2.349 | 0.005 | 437.055 | 0.968 | 110.788 | 0.245 |

**性能数据分析**：

- 相比Case 2，Case 3仍然使用搬运接口完成GM->UB/UB->GM搬运，但把线程映射调整为Warp内相邻线程访问相邻元素，`Task Duration` 从 **538.098μs** 降到 **463.179μs**，端到端耗时下降 **13.9%**。
- 相比Case 1，Case 3的 `Task Duration` 下降 **11.9%**。
- `aiv_vec_time` 从Case 2的 **501.605μs** 降到 **301.474μs**，下降 **39.9%**。Case 3让Warp内相邻线程访问相邻元素，32个相邻线程落入连续的几条32B bank行，每条行一次读取即可同时服务该行内的多个线程，UB访问被合并、所需拍数减少；
- `aiv_mte2_time` 从Case 2的 **527.987μs** 降到 **437.055μs**，下降 **17.2%**，说明连续访问已经缓解了SIMT计算阶段对MTE2的UB资源干扰；但 `aiv_mte2_ratio` 仍达到 **0.968**，说明当前主要瓶颈已经转移到MTE2搬运通路。

## 性能对比总结

### Ascend 950PR性能对比

下表为本样例在Ascend 950系列产品上运行的性能数据对比：

| Case | 实现方式 | 核数 | Task Duration(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio | 主要瓶颈 |
|:---|:---|:---:|---:|---:|---:|---:|---:|---:|---:|:---|
| 0 | SIMT直接访问GM | 64 | 812.063 | 785.715 | 0.999 | 0.004 | 0.000 | 0.001 | 0.000 | SIMT直接访问GM |
| 1 | SIMD Reg矢量计算 | 64 | 525.736 | 509.987 | 0.989 | 217.341 | 0.421 | 62.274 | 0.121 | Vec bound |
| 2 | SIMT非连续访问UB | 64 | 538.098 | 501.605 | 0.942 | 527.987 | 0.991 | 40.091 | 0.075 | 跨bank行访问UB |
| 3 | SIMT连续访问UB | 64 | **463.179** | **301.474** | 0.668 | 437.055 | 0.968 | 110.788 | 0.245 | MTE2 bound |

### 优化要点总结

| 优化手段 | 核心原理 | 样例体现 |
|:---|:---|:---|
| 使用SIMD处理连续数据搬运 | SIMT直接访问GM时，访问粒度和缓存路径可能导致带宽利用率低；先通过搬运接口搬入UB，再由SIMT访问UB，可提升访问效率 | Case 3相比Case 0端到端耗时下降43.0% |
| 使用SIMT处理分支判断 | 对带条件修正的逐元素计算，SIMD Reg矢量计算需要用多条Compare/Select/Mask指令表达分支，依赖链较长；SIMT可在线程内直接表达 `%` 和条件判断，减少向量掩码拼接分支逻辑的开销 | Case 2相比Case 1的 `aiv_vec_time` 下降1.6%；在连续访问后，Case 3相比Case 2的 `aiv_vec_time` 下降39.9% |
| 调整线程映射使Warp内连续访问UB | UB按16 bank/8 bank group组织、每bank行32B，硬件每拍每个bank group读/写一行；让同一Warp内相邻线程访问相邻元素、落入同一条32B bank行，可由一次行读取同时服务多个线程，比“单个线程内部连续、相邻线程跨32B行”更高效 | Case 3将 `tid*8+i` 改为 `tid+i*1024` 形态，相比Case 2端到端耗时下降13.9% |

---

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 切换Case

  在cmake编译时通过 `-DSCENARIO_NUM=N` 指定要编译的Case，各Case说明：
  - `0`: SIMT直接访问GM
  - `1`: SIMD Reg矢量计算
  - `2`: SIMT非连续访问
  - `3`: SIMT连续访问

  示例：
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=npu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=3
  ```

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  SCENARIO_NUM=1                                                       # 选择执行场景
  mkdir -p build && cd build;                                          # 创建并进入 build 目录
  cmake -DCMAKE_ASC_RUN_MODE=npu -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;  # 编译工程
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用NPU仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |:---|:---|:---|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构，对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0`、`1`、`2`、`3` | Case编号：0=SIMT直接访问GM，1=SIMD Reg矢量计算，2=SIMT非连续访问，3=SIMT连续访问 |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```

### 性能分析

### msOpProf工具介绍
msOpProf工具是单算子性能分析工具。包含msopprof和msopprof simulator两种使用方式。该工具协助用户定位算子内存、算子代码以及算子指令的异常，实现全方位的算子调优。当前支持基于不同运行模式（上板或仿真）和不同文件形式（可执行文件或算子二进制.o文件）进行性能数据的采集和自动解析。

- 上板性能采集

    通过上板性能采集，可以直接测定算子昇腾AI处理器上的运行时间。该方式适合在板环境中快速定位算子性能问题。

    基于可执行文件demo通过msopprof执行算子调优：
    ```
    msopprof ./demo
    ```

    - 性能数据说明  
      命令完成后，会在默认目录下生成以“OPPROF_{timestamp}_XXX”命名的文件夹,性能数据文件夹结构示例如下：

      ```bash
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
