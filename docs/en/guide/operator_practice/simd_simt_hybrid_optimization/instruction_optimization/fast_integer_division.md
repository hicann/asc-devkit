# Fast Algorithm for Integer Division<a name="ZH-CN_TOPIC_INTEGER_FAST_DIV"></a>

<!-- md-trans-meta sourceCommit=4f3a01a62259e37de28314829eb11eb21e7f9acd translatedAt=2026-11-09T10:40:18.700Z pushedAt=2026-11-19T06:32:19.818Z nl=2 -->

>[!NOTE] Note 
>This performance optimization suggestion applies to the following models:
     <!-- npu="950" id1 -->
>-   Ascend 950PR&950DT products
     <!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/zh/guide/operator_practice/simd_simt_hybrid_optimization/instruction_optimization/fast_integer_division_res.md#id1 -->

[Priority] Low

[Description] Integer division instructions incur significantly higher execution overhead than multiplication and shift on AI processors. When the divisor is fixed, you can use a fast division algorithm based on multiplication and shift. In the SIMD scalar computation logic, call the [asc_get_uintdiv_magic_and_shift](../../../../../../docs/en/api/SIMT-API/math_functions/integer_math_functions/asc_get_uintdiv_magic_and_shift.md) API to precompute the multiplication magic number `magic` and the shift amount `shift` based on the divisor. In SIMT parallel computation, call the [asc_uintdiv](../../../../../../docs/en/api/SIMT-API/math_functions/integer_math_functions/asc_uintdiv.md) API to perform multiplication and shift operations, obtaining a result consistent with ordinary integer division, thereby reducing the execution overhead of each division.

[Sample introduction] Take the uint32 integer division operator as an example. The operator divides each element of the operator input `input` by a fixed divisor `divisor` and writes the result to the output `output`. The example compares the performance difference between directly using ordinary division in SIMT and precomputing the magic number and shift amount for the fixed divisor and then using multiplication and shift instead of ordinary division. For the complete operator implementation code, see the [example for implementing the fast division operator through SIMD and SIMT hybrid programming](../../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_integer_fast_div).

**Table 1**  Sample specifications

| Name | Name | Shape | Data Type | Format |
|:---|:---|:---|:---|:---|
| Operator input | input | [8388608] | uint32 | ND |
| Operator input | divisor | scalar | uint32 | - |
| Operator output | output | [8388608] | uint32 | ND |

The SIMT thread hierarchy is as follows:

-   Number of thread blocks launched by the kernel: 4096
-   Number of threads per SIMT VF call: 2048

[Negative example]

SIMT-based ordinary integer division implementation: corresponds to scenario 0 (SCENARIO\_NUM=0) in the sample. In this scenario, each thread in SIMT processes one element and directly uses ordinary division, which has a high execution overhead, to complete the computation. The code is as follows.

```cpp
__simt_vf__ __launch_bounds__(THREAD_LIMIT) inline void simt_normal_div(
    __ubuf__ uint32_t* input,
    __ubuf__ uint32_t* output,
    uint32_t divisor,
    uint32_t total_length)
{
    uint32_t local_idx = threadIdx.x;
    uint32_t idx = blockIdx.x * blockDim.x + local_idx;

    if (idx >= total_length) {
        return;
    }

    uint32_t value = input[local_idx];
    uint32_t result = value / divisor;
    output[local_idx] = result;
}
```

Scenario 0 and scenario 1 use the same data movement and write-back process: each thread block moves the corresponding data it is responsible for from GM to the Unified Buffer (UB), completes the computation in the SIMT VF and writes the result to the UB, and then writes the result from the UB back to GM in the kernel. The performance difference between the two scenarios mainly comes from the division implementation in the SIMT computation process.

[Positive example]

Fast division implementation based on SIMD and SIMT hybrid programming: corresponds to scenario 1 in the sample (SCENARIO\_NUM=1). When the divisor is fixed, you can call the fast division precomputation API [asc_get_uintdiv_magic_and_shift](../../../../../../docs/en/api/SIMT-API/math_functions/integer_math_functions/asc_get_uintdiv_magic_and_shift.md) in the SIMD scalar computation logic to compute the required `magic` and `shift`, and then call the fast division API [asc_uintdiv](../../../../../../docs/en/api/SIMT-API/math_functions/integer_math_functions/asc_uintdiv.md) in SIMT computation to replace ordinary division with a combined multiplication and shift operation.

For uint32 integer division, the following transformation can be performed to convert the division computation into multiplication and shift operations:

$$
\left\lfloor \frac{n}{d} \right\rfloor =
\left\lfloor \frac{n}{d} \times \frac{2^s}{2^s} \right\rfloor =
\left\lfloor \frac{2^s}{d} \times \frac{n}{2^s} \right\rfloor =
\left\lfloor {m} \times \frac{n}{2^s} \right\rfloor
$$

Where $n$ is the dividend, $d$ is the divisor, and $m = \frac{2^s}{d}$. Dividing $n$ by $2^s$ can be implemented by shifting right by $s$ bits. To avoid overflow in the multiplication $m \times n$, split $m$ as follows:

$$
m = (m - 2^{32}) + 2^{32}
$$

Let $magic = m - 2^{32}$. Substituting this into the equation above yields:

$$
\left\lfloor \frac{n}{d} \right\rfloor =
\left\lfloor \frac{n \times {magic} + n \times 2^{32}}{2^s} \right\rfloor =
\left\lfloor \left(\frac{n \times magic}{2^{32}} + n\right) >> (s-32) \right\rfloor =
\left\lfloor \left(\frac{n \times magic}{2^{32}} + n\right) >> shift \right\rfloor
$$

Here, `magic` and `shift` are the multiplication magic number and the shift amount required for fast division. The computation of `magic` and `shift` depends only on the fixed divisor. They can be precomputed by calling the [asc_get_uintdiv_magic_and_shift](../../../../../../docs/en/api/SIMT-API/math_functions/integer_math_functions/asc_get_uintdiv_magic_and_shift.md) API in the SIMD scalar computation logic and reused during SIMT computation. The relevant code is as follows.

```cpp
__global__ __vector__ void integer_div_kernel(
    __gm__ uint32_t* input,
    __gm__ uint32_t* output,
    uint32_t divisor,
    uint32_t total_length)
{
    // In the scalar computation logic, precompute magic and shift based on the fixed divisor.
    uint32_t magic = 0;
    uint32_t shift = 0;
    asc_get_uintdiv_magic_and_shift(&magic, &shift, divisor);

    uint32_t block_offset = block_idx * THREAD_COUNT;

    __ubuf__ uint32_t input_buf[THREAD_COUNT];
    __ubuf__ uint32_t output_buf[THREAD_COUNT];
    uint32_t blk_length = THREAD_COUNT * sizeof(uint32_t);
    asc_copy_gm2ub_align(
        input_buf, input + block_offset, 1, blk_length, 0, 0, false, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);

    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

    // Pass magic and shift to the SIMT VF to complete the fast division computation.
    asc_vf_call<simt_fast_div>(dim3(THREAD_COUNT), input_buf, output_buf, magic, shift, total_length);

    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);

    asc_copy_ub2gm_align(
        output + block_offset, output_buf, 1, blk_length, asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
}
```

During the SIMT computation process, each thread reads the dividend value to be processed and calls the [asc_uintdiv](../../../../../../docs/en/api/SIMT-API/math_functions/integer_math_functions/asc_uintdiv.md) API to perform the combined multiplication and shift operation, obtaining a result equivalent to value / divisor. The key code is as follows.

```cpp
__simt_vf__ __launch_bounds__(THREAD_LIMIT) inline void simt_fast_div(
    __ubuf__ uint32_t* input,
    __ubuf__ uint32_t* output,
    uint32_t magic,
    uint32_t shift,
    uint32_t total_length)
{
    uint32_t local_idx = threadIdx.x;
    uint32_t idx = blockIdx.x * blockDim.x + local_idx;

    if (idx >= total_length) {
        return;
    }

    uint32_t value = input[local_idx];
    uint32_t result = asc_uintdiv(value, magic, shift);
    output[local_idx] = result;
}
```

[Performance comparison]

With the same number of thread blocks, the same input and output specifications, and the SIMD copy API used for data transfer between GM and UB in both cases, the performance data of scenario 0 and scenario 1 are compared as follows.

Except for the task duration, all other metrics are the average values across all thread blocks.

| Scenario | Implementation | Data Size | Task Duration \(μs\) | aiv\_vec\_time \(μs\) | aiv\_scalar\_time \(μs\) |
|:---|:---|---:|---:|---:|---:|
| Scenario 0 | Ordinary division | 8388608 | 117.229 | 0.497 | 0.128 |
| Scenario 1 | Fast division | 8388608 | 103.889 | 0.270 | 0.142 |

Compared with scenario 0, scenario 1 uses multiplication and shift to replace ordinary integer division. The task duration decreases from 117.229 μs to 103.889 μs, and the end-to-end time decreases by about 11.4%. **aiv\_vec\_time** decreases from 0.497 μs to 0.270 μs, a decrease of about 45.6%, indicating that after replacing ordinary division with multiplication and shift, the computation instruction time is significantly reduced. **aiv\_scalar\_time** increases from 0.128 μs to 0.142 μs, mainly because fast division requires additional computation of magic and shift in the scalar computation unit. However, the task duration still decreases, indicating that the computation gains brought by fast division can offset the scalar computation overhead.

[Summary] For SIMT integer division scenarios with a fixed divisor, you can call the [asc_get_uintdiv_magic_and_shift](../../../../../../docs/en/api/SIMT-API/math_functions/integer_math_functions/asc_get_uintdiv_magic_and_shift.md) API in the SIMD scalar computation logic to precompute the multiplication magic number and the shift amount, and then call the [asc_uintdiv](../../../../../../docs/en/api/SIMT-API/math_functions/integer_math_functions/asc_uintdiv.md) API during the SIMT computation process to reuse the result for fast division. By replacing ordinary integer division with multiplication and shift operations, the division computation overhead can be effectively reduced.
