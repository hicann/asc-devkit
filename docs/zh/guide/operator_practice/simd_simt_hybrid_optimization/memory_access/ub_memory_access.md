# 使用Unified Buffer提升内存访问效率<a name="ZH-CN_TOPIC_0000002511242528"></a>

<!-- npu="950" id1 -->
>[!NOTE]说明 
>该性能优化建议适用于如下型号：
>-   Ascend 950PR/Ascend 950DT
<!-- end id1 -->

【优先级】高

【描述】SIMT线程直接访问Global Memory时，访存请求会经过GM访问路径，数据搬运和线程计算耦合在同一段SIMT逻辑中，容易降低整体访存效率。当单次处理的数据量可放入最大可用Unified Buffer空间（256KB - 系统预留8KB - 最小Dcache 32KB）时，可以使用搬运接口将数据从Global Memory连续搬运到Unified Buffer，使SIMT编程直接访问Unified Buffer中的数据，从而提高内存访问效率，提升算子的整体性能。

【样例介绍】以SIMD与SIMT混合编程方式实现的floor\_mod算子为例。该算子输入x和y的shape均为\[8192, 8192\]，数据类型为int32，输出z的shape为\[8192, 8192\]。完整样例请参考[SIMT与SIMD混合编程高性能优化样例](../../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance)。

**表1**  样例规格

<table>
<thead>
<tr>
<th>名称</th>
<th>name</th>
<th>shape</th>
<th>data type</th>
<th>format</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="2">算子输入</td>
<td>x</td>
<td>[8192, 8192]</td>
<td>int32</td>
<td>ND</td>
</tr>
<tr>
<td>y</td>
<td>[8192, 8192]</td>
<td>int32</td>
<td>ND</td>
</tr>
<tr>
<td>算子输出</td>
<td>z</td>
<td>[8192, 8192]</td>
<td>int32</td>
<td>ND</td>
</tr>
</tbody>
</table>

SIMT线程层次结构为：

-   Kernel启动核数：64
-   单次SIMT VF调用线程数：1024

【反例】

SIMT直接访问Global Memory上的x、y和z数据，对应样例中的场景0（SCENARIO\_NUM=0）。该场景未通过MTE完成GM与UB之间的数据搬运，SIMT线程直接从GM读取输入并将结果写回GM，代码如下。

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

__global__ __vector__ void floor_mod_gm_simt_custom(__gm__ int32_t* x, __gm__ int32_t* y, __gm__ int32_t* z)
{
    asc_init();
    asc_vf_call<floor_mod_simt_gm_contiguous>(dim3(THREAD_COUNT), x, y, z, INPUT_TOTAL_LENGTH);
}
```

【正例】

使用搬运接口将x和y从Global Memory连续搬运到Unified Buffer，基于SIMT编程方式直接从Unified Buffer读取数据并写入Unified Buffer，再将结果连续写回Global Memory，对应样例中的场景3（SCENARIO\_NUM=3）。该实现采用ping-pong双缓冲策略，通过两组UB缓冲区交替处理数据，使MTE2搬入流水、Vector计算流水和MTE3搬出流水并行执行。代码如下。

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

__aicore__ inline void process_tiles(__gm__ int32_t* x, __gm__ int32_t* y, __gm__ int32_t* z, uint32_t compute_mode)
{
    uint32_t core_offset = block_idx * DATA_LEN_PER_CORE;
    __gm__ int32_t* x_gm = x + core_offset;
    __gm__ int32_t* y_gm = y + core_offset;
    __gm__ int32_t* z_gm = z + core_offset;

    extern __ubuf__ int32_t ub_buf[];
    __ubuf__ int32_t* x_ping = ub_buf;
    __ubuf__ int32_t* y_ping = ub_buf + TILE_LENGTH;
    __ubuf__ int32_t* z_ping = ub_buf + TILE_LENGTH * 2;
    __ubuf__ int32_t* x_pong = ub_buf + TILE_LENGTH * 3;
    __ubuf__ int32_t* y_pong = ub_buf + TILE_LENGTH * 4;
    __ubuf__ int32_t* z_pong = ub_buf + TILE_LENGTH * 5;

    asc_sync_notify(PIPE_V, PIPE_MTE2, EVENT_ID0);
    asc_sync_notify(PIPE_V, PIPE_MTE2, EVENT_ID1);
    asc_sync_notify(PIPE_MTE3, PIPE_V, EVENT_ID0);
    asc_sync_notify(PIPE_MTE3, PIPE_V, EVENT_ID1);

    for (uint32_t tile_idx = 0; tile_idx < TILE_NUM_PER_CORE; ++tile_idx) {
        int32_t event_id = ((tile_idx & 1) == 0 ? EVENT_ID0 : EVENT_ID1);
        uint32_t tile_offset = tile_idx * TILE_LENGTH;
        constexpr uint32_t tile_bytes = TILE_LENGTH * sizeof(int32_t);
        __ubuf__ int32_t* x_local = ((tile_idx & 1) == 0 ? x_ping : x_pong);
        __ubuf__ int32_t* y_local = ((tile_idx & 1) == 0 ? y_ping : y_pong);
        __ubuf__ int32_t* z_local = ((tile_idx & 1) == 0 ? z_ping : z_pong);

        asc_sync_wait(PIPE_V, PIPE_MTE2, event_id);
        asc_copy_gm2ub_align(x_local, x_gm + tile_offset, 1, tile_bytes, 0, 0, false, CACHE_MODE_DISABLE, 0, 0);
        asc_copy_gm2ub_align(y_local, y_gm + tile_offset, 1, tile_bytes, 0, 0, false, CACHE_MODE_DISABLE, 0, 0);
        asc_sync_notify(PIPE_MTE2, PIPE_V, event_id);

        asc_sync_wait(PIPE_MTE2, PIPE_V, event_id);
        asc_sync_wait(PIPE_MTE3, PIPE_V, event_id);
        asc_vf_call<floor_mod_simt_contiguous>(dim3(THREAD_COUNT), x_local, y_local, z_local, TILE_LENGTH);
        asc_sync_data_barrier(mem_dsb_t::DSB_UB);
        asc_sync_notify(PIPE_V, PIPE_MTE2, event_id);

        asc_sync_notify(PIPE_V, PIPE_MTE3, event_id);
        asc_sync_wait(PIPE_V, PIPE_MTE3, event_id);
        asc_copy_ub2gm_align(z_gm + tile_offset, z_local, 1, tile_bytes, 0, 0, 0);
        asc_sync_notify(PIPE_MTE3, PIPE_V, event_id);
    }

    asc_sync_wait(PIPE_V, PIPE_MTE2, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE2, EVENT_ID1);
    asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID1);
}
```

【性能对比】

场景0和场景3的性能对比如下。

除Task Duration外，其余指标均为所有Thread Block上的平均值。

| 场景 | 实现方式 | 核数 | Task Duration\(μs\) | aiv\_vec\_time\(μs\) | aiv\_mte2\_time\(μs\) | aiv\_mte3\_time\(μs\) |
|:---|:---|:---:|---:|---:|---:|---:|
| 场景0 | SIMT直接访问GM | 64 | 812.063 | 785.715 | 0.004 | 0.001 |
| 场景3 | SIMT连续访问UB | 64 | 463.179 | 301.474 | 437.055 | 110.788 |

场景0中，aiv\_mte2\_time和aiv\_mte3\_time接近0，说明数据读写没有走GM到UB、UB到GM的MTE搬运路径，耗时主要集中在SIMT直接访问GM和计算过程上。场景3中，输入和输出通过搬运接口在GM与UB之间连续搬运，SIMT线程直接访问UB完成计算，Task Duration从812.063μs降低至463.179μs，端到端耗时下降约43.0%。
