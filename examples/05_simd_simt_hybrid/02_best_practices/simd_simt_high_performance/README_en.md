# SIMD and SIMT Hybrid Programming High-Performance Optimization Example

## Overview

This example uses the FloorMod computation to demonstrate performance tuning methods in SIMD and SIMT hybrid programming scenarios. The example shows the performance differences among directly accessing GM via SIMT, using SIMD Reg vector computation, accessing UB via SIMT with non-contiguous thread mapping, and adjusting thread mapping so that adjacent threads within a Warp access UB contiguously.

**Optimization Path**:

| Case | SCENARIO_NUM | Implementation | Description |
|:---|:---:|:---|:---|
| Case 0 | 0 | SIMT direct GM access | Both data access and computation are implemented via SIMT. SIMT threads directly access GM, serving as the pure SIMT baseline. |
| Case 1 | 1 | SIMD Reg vector computation | Data transfer between GM and UB is performed through MTE, while FloorMod is implemented with Reg vector computation, serving as the pure SIMD computation baseline. |
| Case 2 | 2 | SIMT non-contiguous UB access | GM data is transferred to UB via data transfer interfaces. Each SIMT thread processes a contiguous segment, but adjacent threads within a Warp access non-contiguous addresses. |
| Case 3 | 3 | SIMT contiguous UB access | GM data is transferred to UB via data transfer interfaces. Thread mapping is adjusted so that adjacent threads within a Warp access contiguous addresses. |

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```
├── simd_simt_high_performance
│   ├── CMakeLists.txt        // Build project file
│   ├── data_utils.h          // Data read/write functions
│   ├── figures               // Illustrations
│   ├── floor_mod.asc         // Ascend C example implementation
│   ├── README.md             // Example documentation
│   └── scripts
│       ├── gen_data.py       // Input data and ground truth generation script
│       └── verify_result.py  // Ground truth comparison script
```

## Example Description

**Example Function**:

This example implements FloorMod computation. FloorMod is commonly used in tensor computation for periodic indexing, group mapping, and other scenarios that require consistent negative modulo semantics. Here, $r$ is the C/C++ remainder result (that is, $r = x - \operatorname{trunc}(x / y) \times y$, with the sign following the dividend $x$), and the FloorMod result $z$ is obtained through sign correction (with the sign following the divisor $y$). The computation formula is as follows:

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

**Example Specifications**:

<table>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">FloorMod</td></tr>
<tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[8192, 8192]</td><td align="center">int32</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[8192, 8192]</td><td align="center">int32</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[8192, 8192]</td><td align="center">int32</td><td align="center">ND</td></tr>
<tr><td rowspan="4" align="center">Kernel Function Name</td><td align="center">SCENARIO_NUM=0</td><td colspan="3" align="center"><code>floor_mod_gm_simt_custom</code></td></tr>
<tr><td align="center">SCENARIO_NUM=1</td><td colspan="3" align="center"><code>floor_mod_simd_custom</code></td></tr>
<tr><td align="center">SCENARIO_NUM=2</td><td colspan="3" align="center"><code>floor_mod_simt_non_contiguous_custom</code></td></tr>
<tr><td align="center">SCENARIO_NUM=3</td><td colspan="3" align="center"><code>floor_mod_simt_contiguous_custom</code></td></tr>
</table>

> **Input constraint:** The divisor $y$ must be non-zero (the SIMT path directly performs modulo, and $y$ being 0 triggers a division-by-zero issue). The data generation script avoids 0 values.

## Example Implementation

### Performance Metric Description

| Field Name | Field Description |
|:---|:---|
| Task Duration(μs) | Total task latency, including scheduling time to the accelerator, execution time on the accelerator, and response completion time. |
| aiv_time(μs) | Task execution time on the AI Vector Core. |
| aiv_vec_time(μs) | vec-type instruction latency. Thread computation in SIMT VF calls is also reflected in this metric. |
| aiv_vec_ratio | Ratio of vec-type instruction cycles to total cycles. |
| aiv_scalar_time(μs) | scalar-type instruction latency. |
| aiv_scalar_ratio | Ratio of scalar-type instruction cycles to total cycles. |
| aiv_mte2_time(μs) | mte2-type instruction latency, primarily corresponding to GM-to-UB transfers. |
| aiv_mte2_ratio | Ratio of mte2-type instruction cycles to total cycles. |
| aiv_mte3_time(μs) | mte3-type instruction latency, primarily corresponding to UB-to-GM transfers. |
| aiv_mte3_ratio | Ratio of mte3-type instruction cycles to total cycles. |

Except for Task Duration, all other metrics are averages across all Thread Blocks.

### Case 0: SIMT Direct GM Access

**Implementation**: Refer to the `floor_mod_simt_gm_contiguous()` function.

This implementation does not use MTE2/MTE3 for data transfer between GM and UB. Instead, SIMT threads directly read `x` and `y` from GM and write results back to GM after computation.

**Key Code**:

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

**Performance Data**:

| Case | Implementation | Cores | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 0 | SIMT direct GM access | 64 | 812.063 | 786.138 | 785.715 | 0.999 | 0.411 | 0.001 | 0.004 | 0.000 | 0.001 | 0.000 |

**Performance Data Analysis**:

- In this case, `aiv_mte2_time` and `aiv_mte3_time` are close to 0, indicating that data reads and writes do not go through the MTE2/MTE3 GM->UB/UB->GM transfer path.
- `Task Duration` is **812.063μs**, of which `aiv_vec_time` is **785.715μs** and `aiv_vec_ratio` reaches **0.999**. The latency is primarily concentrated on SIMT internal GM reads/writes and FloorMod computation.

**Principle Description**:

When SIMT threads directly access GM, each thread must read `x` and `y` from GM and write results back to GM during computation. This results in low access efficiency, and data transfer is coupled with computation in the same SIMT thread, preventing parallel transfer and computation pipelining.

The input and output in this example are contiguously arranged 2D ND data on GM, which is suitable for contiguous block transfer. Subsequent implementations use `asc_copy_gm2ub_align` and `asc_copy_ub2gm_align` for GM->UB and UB->GM transfers. These transfers are executed by MTE, which can transfer larger contiguous data blocks from GM to UB and then contiguously write results from UB back to GM. Compared to SIMT threads accessing GM element by element, this approach provides higher memory access efficiency and enables parallel data transfer and computation pipelining.

**Next Optimization Direction**:

This case serves as the baseline for pure SIMT direct GM access, showing the performance data of a pure SIMT implementation. The next step introduces a SIMD implementation, using data transfer interfaces for GM->UB and UB->GM transfers and Reg vector computation for FloorMod, to show the performance data of this implementation.

### Case 1: SIMD Reg Vector Computation

**Implementation**: Refer to the `floor_mod_simd()` function.

This implementation uses `asc_copy_gm2ub_align` and `asc_copy_ub2gm_align` for data transfer between GM and UB. In the SIMD VF function, UB data is loaded into vector registers via `asc_loadalign`, vector instructions such as `asc_div`, `asc_mul`, `asc_sub`, `asc_ne`, and `asc_select` are executed, and results are finally written back to UB via `asc_storealign`. Computation is implemented through SIMD, while data transfer between GM and UB is performed through MTE.

**Key Code**:

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

**Performance Data**:

| Case | Implementation | Cores | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 | SIMD Reg vector computation | 64 | 525.736 | 515.723 | 509.987 | 0.989 | 2.419 | 0.005 | 217.341 | 0.421 | 62.274 | 0.121 |

**Performance Data Analysis**:

- Compared with Case 0, Case 1 introduces contiguous MTE transfers for GM->UB/UB->GM transfer. `Task Duration` decreases from **812.063μs** to **525.736μs**, an end-to-end latency reduction of **35.3%**.
- `Task Duration` is **525.736μs**, of which `aiv_vec_time` is **509.987μs** and `aiv_vec_ratio` reaches **98.9%**, indicating that the latency is primarily concentrated on the Reg vector computation path.
- `aiv_mte2_ratio` is **42.1%** and `aiv_mte3_ratio` is **12.1%**. Transfer latency can partially overlap with computation. The current bottleneck is not the GM transfer process but the FloorMod computation itself in vector instruction execution.

**Principle Description**:

FloorMod computation involves not only remainder calculation but also conditional correction based on the sign relationship between the remainder and divisor. When implemented with SIMD Reg vector computation, branch decisions cannot be written as simple `if/else` statements. Instead, they must be decomposed into multiple vector instructions such as `Compare`, `And`, `MaskAnd`, and `Select` to express conditional logic. Meanwhile, `asc_div`, `asc_mul`, `asc_sub`, sign determination, and final selection have long data dependency chains, and subsequent instructions must wait for preceding results, resulting in high `aiv_vec_ratio`, manifesting as Vec bound.

Compared with SIMD, SIMT is better suited for handling this type of element-wise computation with branch decisions. It can directly use `%` and conditional statements within threads to complete FloorMod correction, reducing the extra computation chain caused by assembling branch logic with vector mask instructions.

**Next Optimization Direction**:

This case serves as the baseline for SIMD computation, showing the performance data of a pure SIMD implementation. The next step optimizes the Vector computation part using SIMT while retaining the contiguous GM<->UB transfer approach.

### Case 2: SIMT Non-Contiguous UB Access

**Implementation**: Refer to the `floor_mod_simt_non_contiguous()` function.

This implementation first transfers `x` and `y` from GM to UB contiguously via `asc_copy_gm2ub_align`, then uses SIMT programming for the computation process, completing branch decisions through if-else statements. Each thread processes a contiguous segment of 8 elements, but adjacent threads within the same Warp access non-contiguous addresses:

**Key Code**:

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

**Performance Data**:

| Case | Implementation | Cores | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 2 | SIMT non-contiguous UB access | 64 | 538.098 | 532.659 | 501.605 | 0.942 | 2.338 | 0.004 | 527.987 | 0.991 | 40.091 | 0.075 |

**Performance Data Analysis**:

- Compared with Case 0, Case 2 uses data transfer interfaces for GM->UB/UB->GM transfer. `Task Duration` decreases from **812.063μs** to **538.098μs**, an end-to-end latency reduction of **33.7%**, indicating that the overhead from direct GM access is significantly mitigated after introducing UB as an intermediary.
- Compared with Case 1, Case 2's `aiv_vec_time` decreases from **509.987μs** to **501.605μs**, a **1.6%** reduction, indicating some benefit on the computation side after using SIMT to express FloorMod element-wise computation.
- However, Case 2's `Task Duration` is **2.4%** higher than Case 1, with no end-to-end optimization benefit. The primary reason is that `aiv_mte2_time` increases from **217.341μs** in Case 1 to **527.987μs**, a **142.9%** increase; `aiv_mte2_ratio` also reaches **0.991**.

**Principle Description**:

The indexing pattern in Case 2 is `index = tid * elems_per_thread + i`, where `elems_per_thread = 8`. From a single thread's perspective, it processes 8 contiguous elements.

The UB on Ascend 950PR/950DT is divided into 16 banks organized into 8 bank groups, with each bank row being 32B (every contiguous 32B falls into the next bank). The vector computation unit can read/write one row (32B) from each bank group per cycle. The 32 threads within a Warp execute the same instruction simultaneously, each with independent addressing.

In Case 2, adjacent threads `tid` and `tid+1` access elements that differ by `elems_per_thread = 8` in the same iteration `i`, meaning the byte address difference is `8 * sizeof(int32) = 32B`, exactly one bank row span. This means adjacent threads fall on **different bank rows**, and the hardware cannot serve multiple threads with a single row read (32B). The same amount of data requires more cycles to fetch, and UB access is prolonged.

<div align="center">
  <img src="./figures/ubBank.png" alt="Ascend 950PR UB bank diagram" width="600">
</div>

**Next Optimization Direction**:

Although Case 2 improves FloorMod computation expression with SIMT, non-contiguous UB access significantly prolongs MTE2 transfer time, resulting in worse end-to-end performance than the pure SIMD implementation. The next step adjusts the thread-to-data mapping while retaining contiguous GM<->UB transfer and SIMT computation expression, changing from "each thread processes a contiguous block of data" to "adjacent threads within a Warp access contiguous elements in the same iteration," so that adjacent threads fall into the same 32B bank row, enabling a single row read to serve multiple threads simultaneously and improving UB access efficiency.

### Case 3: SIMT Contiguous UB Access

**Implementation**: Refer to the `floor_mod_simt_contiguous()` function.

This implementation also transfers `x` and `y` from GM to UB contiguously via `asc_copy_gm2ub_align`, but changes the SIMT thread mapping so that adjacent threads within the same Warp access adjacent elements:

**Key Code**:

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

**Performance Data**:

| Case | Implementation | Cores | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 3 | SIMT contiguous UB access | 64 | **463.179** | 451.519 | **301.474** | 0.668 | 2.349 | 0.005 | 437.055 | 0.968 | 110.788 | 0.245 |

**Performance Data Analysis**:

- Compared with Case 2, Case 3 still uses data transfer interfaces for GM->UB/UB->GM transfer, but adjusts thread mapping so that adjacent threads within a Warp access adjacent elements. `Task Duration` decreases from **538.098μs** to **463.179μs**, an end-to-end latency reduction of **13.9%**.
- Compared with Case 1, Case 3's `Task Duration` decreases by **11.9%**.
- `aiv_vec_time` decreases from **501.605μs** in Case 2 to **301.474μs**, a **39.9%** reduction. Case 3 ensures that adjacent threads within a Warp access adjacent elements, so 32 adjacent threads fall into contiguous 32B bank rows. Each row read can simultaneously serve multiple threads in that row, UB access is coalesced, and the required number of cycles decreases.
- `aiv_mte2_time` decreases from **527.987μs** in Case 2 to **437.055μs**, a **17.2%** reduction, indicating that contiguous access has alleviated UB resource contention from the SIMT computation phase on MTE2. However, `aiv_mte2_ratio` still reaches **0.968**, indicating that the current bottleneck has shifted to the MTE2 transfer path.

## Performance Comparison Summary

### Ascend 950PR Performance Comparison

The following table shows the performance data comparison for this example running on Ascend 950 series products:

| Case | Implementation | Cores | Task Duration(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio | Primary Bottleneck |
|:---|:---|:---:|---:|---:|---:|---:|---:|---:|---:|:---|
| 0 | SIMT direct GM access | 64 | 812.063 | 785.715 | 0.999 | 0.004 | 0.000 | 0.001 | 0.000 | SIMT direct GM access |
| 1 | SIMD Reg vector computation | 64 | 525.736 | 509.987 | 0.989 | 217.341 | 0.421 | 62.274 | 0.121 | Vec bound |
| 2 | SIMT non-contiguous UB access | 64 | 538.098 | 501.605 | 0.942 | 527.987 | 0.991 | 40.091 | 0.075 | Cross-bank-row UB access |
| 3 | SIMT contiguous UB access | 64 | **463.179** | **301.474** | 0.668 | 437.055 | 0.968 | 110.788 | 0.245 | MTE2 bound |

### Optimization Key Points Summary

| Optimization Method | Core Principle | Example Demonstration |
|:---|:---|:---|
| Use SIMD for contiguous data transfer | When SIMT directly accesses GM, access granularity and cache paths may result in low bandwidth utilization. First transfer data into UB using data transfer interfaces, then have SIMT access UB, which improves access efficiency | Case 3 reduces end-to-end latency by 43.0% compared to Case 0 |
| Use SIMT for branch decisions | For element-wise computation with conditional correction, SIMD Reg vector computation requires multiple Compare/Select/Mask instructions to express branches, resulting in long dependency chains. SIMT can directly express `%` and conditional decisions within threads, reducing the overhead of assembling branch logic with vector mask instructions | Case 2 reduces `aiv_vec_time` by 1.6% compared to Case 1; after contiguous access, Case 3 reduces `aiv_vec_time` by 39.9% compared to Case 2 |
| Adjust thread mapping for contiguous UB access within Warp | UB is organized as 16 banks / 8 bank groups with 32B per bank row. The hardware reads/writes one row per bank group per cycle. Having adjacent threads within the same Warp access adjacent elements that fall into the same 32B bank row enables a single row read to serve multiple threads simultaneously, which is more efficient than "contiguous within a single thread but adjacent threads spanning 32B rows" | Case 3 changes `tid*8+i` to `tid+i*1024` pattern, reducing end-to-end latency by 13.9% compared to Case 2 |

---

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Switch Cases

  Specify the Case to build via `-DSCENARIO_NUM=N` during cmake compilation. Case descriptions:
  - `0`: SIMT direct GM access
  - `1`: SIMD Reg vector computation
  - `2`: SIMT non-contiguous access
  - `3`: SIMT contiguous access

  Example:
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=npu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=3
  ```

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=1                                                       # Select the execution scenario
  mkdir -p build && cd build;                                          # Create and enter the build directory
  cmake -DCMAKE_ASC_RUN_MODE=npu -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;  # Build the project
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output is correct and confirm algorithm logic correctness
  ```

  To use the NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

- Build option description

  | Option | Values | Description |
  |:---|:---|:---|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture, corresponding to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0`, `1`, `2`, `3` | Case number: 0=SIMT direct GM access, 1=SIMD Reg vector computation, 2=SIMT non-contiguous access, 3=SIMT contiguous access |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.
  ```bash
  test pass!
  ```

### Performance Analysis

### Introduction to the msOpProf Tool

`msOpProf` is a single-operator performance analysis tool. It offers two usage methods: `msopprof` and `msopprof simulator`. The tool helps users identify anomalies in operator memory, operator code, and operator instructions, enabling comprehensive operator tuning. It currently supports performance data collection and automatic parsing for different run modes (on-device or simulation) and different file types (executables or operator binary `.o` files).

- On-device performance collection

    On-device performance collection directly measures the execution time of an operator on an Ascend AI Processor. This method is suitable for quickly locating operator performance issues in an on-device environment.

    Run operator tuning on the executable demo with `msopprof`:

    ```
    msopprof ./demo
    ```

    - Performance data description  
      After the command completes, a folder named "OPPROF_{timestamp}_XXX" will be generated in the default directory. The performance data folder structure is as follows:

      ```bash
      ├──dump                       # Raw performance data; users do not need to inspect it
      ├──ArithmeticUtilization.csv  # Cube/Vector instruction cycle proportions
      ├──L2Cache.csv                # L2 Cache hit rate; affects MTE2. Plan data transfer logic properly to increase the hit rate
      ├──Memory.csv                 # Read/write bandwidth rates of UB, L1, and main memory
      ├──MemoryL0.csv               # Read/write bandwidth rates of L0A, L0B, and L0C
      ├──MemoryUB.csv               # Read/write bandwidth rates from Vector and Scalar to UB
      ├──OpBasicInfo.csv            # Basic operator information
      ├──PipeUtilization.csv        # Durations and proportions of computation and data transfer units
      ├──ResourceConflictRatio.csv  # Proportions of UB bank groups, bank conflicts, and resource conflicts among all instructions
      └──visualize_data.bin         # MindStudio Insight presentation file
      ```

View the specific performance analysis results:

```bash
# View Task Duration and various metrics
cat ./OPPROF_*/PipeUtilization.csv
```
