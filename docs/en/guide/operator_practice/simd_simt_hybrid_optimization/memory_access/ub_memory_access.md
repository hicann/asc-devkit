# Using Unified Buffer to Improve Memory Access Efficiency<a name="ZH-CN_TOPIC_0000002511242528"></a>

<!-- md-trans-meta sourceCommit=155d12bd3f10bfd637ddd47c9f145f13f914b4c3 translatedAt=2026-10-23T13:05:11.501Z pushedAt=2026-11-03T07:05:13.366Z nl=2 -->

> [!NOTE] Note 
> This performance optimization suggestion applies to the following models:
     <!-- npu="950" id1 -->
>-   Ascend 950PR&950DT products
     <!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/en/guide/operator_practice/simd_simt_hybrid_optimization/memory_access/ub_memory_access_res.md#id1 -->

[Priority] High

[Description] When SIMT threads directly access Global Memory, memory access requests go through the GM access path, and data transfer and thread computation are coupled in the same SIMT logic, which tends to reduce overall memory access efficiency. When the amount of data processed at a time can fit into the maximum available Unified Buffer (UB) space (256 KB - 8 KB reserved by the system - 32 KB minimum Dcache), the transfer APIs can be used to continuously transfer data from Global Memory to UB, so that SIMT programming directly accesses the data in UB, thereby improving memory access efficiency and the overall performance of the operator.

[Sample introduction] Take the floor\_mod operator implemented in SIMD and SIMT hybrid programming as an example. The shapes of the operator inputs x and y are both \[8192, 8192\], the data type is int32, and the shape of the output z is \[8192, 8192\]. For the complete example, see [SIMT and SIMD Hybrid Programming High-Performance Optimization Example](../../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance).

**Table 1**  Sample specifications

<table>
<thead>
<tr>
<th>Name</th>
<th>Name</th>
<th>Shape</th>
<th>Data Type</th>
<th>Format</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="2">Operator input</td>
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
<td>Operator output</td>
<td>z</td>
<td>[8192, 8192]</td>
<td>int32</td>
<td>ND</td>
</tr>
</tbody>
</table>

The SIMT thread hierarchy is as follows:

-   Core count started by the kernel function: 64
-   Number of threads per SIMT VF call: 1024

[Negative example]

SIMT threads directly access the x, y, and z data in Global Memory, corresponding to scenario 0 in the sample (SCENARIO\_NUM=0). In this scenario, data is not transferred between GM and UB through MTE. SIMT threads directly read inputs from GM and write results back to GM. The code is as follows.

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

[Positive example]

The transfer APIs are used to continuously transfer x and y from Global Memory to UB. Based on the SIMT programming model, data is directly read from UB and written to UB, and the results are then continuously written back to Global Memory, corresponding to scenario 3 in the sample (SCENARIO\_NUM=3). This implementation adopts the ping-pong double buffering strategy, which alternately processes data through two groups of UB buffers, allowing the MTE2 input pipeline, the Vector computation pipeline, and the MTE3 output pipeline to execute in parallel. The code is as follows.

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
        event_t event_id = ((tile_idx & 1) == 0 ? EVENT_ID0 : EVENT_ID1);
        uint32_t tile_offset = tile_idx * TILE_LENGTH;
        constexpr uint32_t tile_bytes = TILE_LENGTH * sizeof(int32_t);
        __ubuf__ int32_t* x_local = ((tile_idx & 1) == 0 ? x_ping : x_pong);
        __ubuf__ int32_t* y_local = ((tile_idx & 1) == 0 ? y_ping : y_pong);
        __ubuf__ int32_t* z_local = ((tile_idx & 1) == 0 ? z_ping : z_pong);

        asc_sync_wait(PIPE_V, PIPE_MTE2, event_id);
        asc_copy_gm2ub_align(x_local, x_gm + tile_offset, 1, tile_bytes, 0, 0, false, asc_load_l2_cache_mode::NOTALLOC_DROP, 0, 0);
        asc_copy_gm2ub_align(y_local, y_gm + tile_offset, 1, tile_bytes, 0, 0, false, asc_load_l2_cache_mode::NOTALLOC_DROP, 0, 0);
        asc_sync_notify(PIPE_MTE2, PIPE_V, event_id);

        asc_sync_wait(PIPE_MTE2, PIPE_V, event_id);
        asc_sync_wait(PIPE_MTE3, PIPE_V, event_id);
        asc_vf_call<floor_mod_simt_contiguous>(dim3(THREAD_COUNT), x_local, y_local, z_local, TILE_LENGTH);
        asc_sync_data_barrier(mem_dsb_t::DSB_UB);
        asc_sync_notify(PIPE_V, PIPE_MTE2, event_id);

        asc_sync_notify(PIPE_V, PIPE_MTE3, event_id);
        asc_sync_wait(PIPE_V, PIPE_MTE3, event_id);
        asc_copy_ub2gm_align(z_gm + tile_offset, z_local, 1, tile_bytes, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
        asc_sync_notify(PIPE_MTE3, PIPE_V, event_id);
    }

    asc_sync_wait(PIPE_V, PIPE_MTE2, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE2, EVENT_ID1);
    asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE3, PIPE_V, EVENT_ID1);
}
```

[Performance comparison]

The performance comparison between scenario 0 and scenario 3 is as follows.

Except for Task Duration, all other metrics are the average values across all Thread Blocks.

| Scenario | Implementation | Core Count | Task Duration \(μs\) | aiv\_vec\_time \(μs\) | aiv\_mte2\_time \(μs\) | aiv\_mte3\_time \(μs\) |
|:---|:---|:---:|---:|---:|---:|---:|
| Scenario 0 | SIMT direct access to GM | 64 | 812.063 | 785.715 | 0.004 | 0.001 |
| Scenario 3 | SIMT continuous access to UB | 64 | 463.179 | 301.474 | 437.055 | 110.788 |

In scenario 0, **aiv\_mte2\_time** and **aiv\_mte3\_time** are close to 0, indicating that data reads and writes do not go through the MTE transfer paths from GM to UB and from UB to GM. The time is mainly consumed by SIMT direct access to GM and the computation process. In scenario 3, inputs and outputs are continuously transferred between GM and UB through the transfer APIs, and SIMT threads directly access UB to complete the computation. Task Duration decreases from 812.063 μs to 463.179 μs, reducing the end-to-end time by approximately 43.0%.
