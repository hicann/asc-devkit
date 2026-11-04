# Implementing Branch Judgment Through SIMT<a name="ZH-CN_TOPIC_0000002552896537"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T13:02:55.753Z pushedAt=2026-11-03T07:05:13.362Z nl=2 -->

> [!NOTE] 
> This performance optimization suggestion applies to the following models:
     <!-- npu="950" id1 -->
>-   Ascend 950PR&950DT products
     <!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/en/guide/operator_practice/simd_simt_hybrid_optimization/compute_optimization/simt_branch_judgment_res.md#id1 -->

Priority: High

Description: Batch data computation implemented based on the SIMD programming model delivers high performance. However, when branch judgment is involved in the operator implementation logic, SIMD-based computation operations become relatively complex, leading to performance degradation. In this case, consider using the SIMT approach, because SIMT programming is more flexible and better suited to branch judgment scenarios.

Example introduction: Take the floor\_mod operator as an example. The operator divides each element of input x by the corresponding element of input y and obtains the remainder. The remainder should have the same sign as the divisor y, and its absolute value should be smaller than the absolute value of y. During computation, it is necessary to determine the sign of each element in y and the relationship between the remainder and the absolute value of that element. For the complete operator implementation code, see [SIMT and SIMD Hybrid Programming High-Performance Optimization Example](../../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance).

**Table 1**  Example specifications

<table>
<thead>
<tr>
<th>Name</th>
<th>Name</th>
<th>Dhape</th>
<th>Data Type</th>
<th>Format</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="2">Operator Input</td>
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
<td>Operator Output</td>
<td>z</td>
<td>[8192, 8192]</td>
<td>int32</td>
<td>ND</td>
</tr>
</tbody>
</table>

The SIMT thread hierarchy is as follows:

-   Number of cores launched by the kernel function: 64
-   Number of threads per SIMT VF call: 1024

[Negative example]

Implementation of the floor\_mod operator based on SIMD Reg vector computation: corresponds to scenario 1 (SCENARIO\_NUM=1) in the sample. This scenario uses `asc_copy_gm2ub_align` and `asc_copy_ub2gm_align` to move data between GM and the Unified Buffer (UB), and uses SIMD VF functions on the UB to perform the computation. Because SIMD cannot directly express element-wise branch judgment through ordinary if-else statements, multiple vector computation APIs are required to complete sign comparison, condition combination, and result selection. The relevant code is as follows.

```cpp
__simd_vf__ inline void floor_mod_simd(
    __ubuf__ int32_t* z_addr, __ubuf__ int32_t* x_addr, __ubuf__ int32_t* y_addr, const uint32_t count)
{
    constexpr uint32_t one_repeat_size = asc_get_vf_len() / sizeof(int32_t);
    uint16_t loop_times = ceil_div(count, one_repeat_size);
    vector_int32_t x_value;
    vector_int32_t y_value;
    vector_int32_t mod_value;
    vector_int32_t temp_value;
    vector_int32_t default_value;
    vector_int32_t sign_value;

    vector_bool mask;
    vector_bool select_mask;
    vector_bool adjust_mask;
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
```

[Positive example]

Implementation of the floor\_mod operator based on SIMT: corresponds to scenario 3 (SCENARIO\_NUM=3) in the sample. This scenario also uses the data movement APIs to move data between GM and UB, adopts the SIMT programming approach on the UB to implement the computation, and completes branch judgment through if-else statements. The code is as follows.

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
```

[Performance comparison]

With the same core count, the same input and output specifications, and the use of data movement APIs to move data between GM and UB, the performance data of scenario 1 and scenario 3 are compared as follows.

Except for Task Duration, all other metrics are the average values across all Thread Blocks.

| Scenario | Implementation | Core Count | Task Duration\(μs\) | aiv\_vec\_time\(μs\) | aiv\_vec\_ratio | aiv\_mte2\_time\(μs\) | aiv\_mte2\_ratio |
|:---|:---|:---:|---:|---:|---:|---:|---:|
| Scenario 1 | SIMD Reg vector computation | 64 | 525.736 | 509.987 | 0.989 | 217.341 | 0.421 |
| Scenario 3 | SIMT continuous UB access | 64 | 463.179 | 301.474 | 0.668 | 437.055 | 0.968 |

Compared with scenario 1, scenario 3 uses SIMT to directly express the branch judgment logic in floor\_mod, and aiv\_vec\_time decreases from 509.987 μs to 301.474 μs, a reduction of about 40.9%, indicating that the time consumed on the Vector computation side is significantly reduced. The end-to-end Task Duration decreases from 525.736 μs to 463.179 μs, a reduction of about 11.9%. The aiv\_mte2\_ratio of scenario 3 reaches 0.968, and the main bottleneck has shifted to MTE2 Bound, meaning that the greatest impact on operator performance has shifted to the efficiency of moving data from GM to UB.
