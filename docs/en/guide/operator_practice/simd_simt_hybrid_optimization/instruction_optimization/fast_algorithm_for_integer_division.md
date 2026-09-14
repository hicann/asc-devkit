# Fast Algorithm for Integer Division<a name="ZH-CN_TOPIC_INTEGER_FAST_DIV"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:41:43.360Z -->

> [!NOTE] Note
> This performance optimization suggestion applies to the following models:
> -   Ascend 950PR/Ascend 950DT

[Priority] Low

[Description] On AI processors, integer division instructions incur significantly higher execution overhead than multiplication and shift operations. When the divisor is fixed, you can use a fast division algorithm based on multiplication and shift. In SIMD, precompute the multiplication magic number `magic` and the shift amount `shift` based on the divisor. In SIMT parallel computation, use multiplication and shift operations to obtain results consistent with ordinary integer division, thereby reducing the execution overhead of each division.

[Example Introduction] Take the uint32 integer division operator as an example. The operator divides each element of the input `input` by a fixed divisor `divisor` and writes the result to the output `output`. The example compares the performance difference between directly using ordinary division in SIMT and using multiplication and shift instead of ordinary division after precomputing `magic` and `shift` for the fixed divisor. For the complete operator implementation code, see the [SIMD and SIMT hybrid programming fast division operator example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/05_simd_simt_hybrid/02_best_practices/simd_simt_integer_fast_div).

**Table 1** Example specifications

| Name | Name | Shape | Data Type | Format |
|:---|:---|:---|:---|:---|
| Operator input | input | [8388608] | uint32 | ND |
| Operator input | divisor | scalar | uint32 | - |
| Operator output | output | [8388608] | uint32 | ND |

The SIMT thread hierarchy is as follows:

-   Number of thread blocks launched by the kernel: 4096
-   Number of threads per SIMT VF call: 2048

[Negative Example]

SIMT-based ordinary integer division implementation: corresponds to scenario 0 (SCENARIO\_NUM=0) in the sample. In this scenario, each thread in SIMT processes one element and directly uses ordinary division, which incurs high execution overhead, to complete the computation. The code is as follows.

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

Scenario 0 and scenario 1 use the same data movement and write-back process: each thread block moves the corresponding data it is responsible for from GM to UB, completes the computation in the SIMT VF and writes the result to UB, and then writes the result from UB back to GM in the kernel function. The performance difference between the two scenarios mainly comes from the division implementation in the SIMT computation process.

[Positive Example]

Fast division implementation based on mixed SIMD and SIMT programming: corresponds to scenario 1 (SCENARIO\_NUM=1) in the sample. When the divisor is fixed, the required magic and shift can be precomputed in SIMD programming, and then the ordinary division can be replaced by a combination of multiplication and shift operations in SIMT programming.

For uint32 integer division, the following transformation can be performed to convert the division into multiplication and shift operations:

$$
\left\lfloor \frac{n}{d} \right\rfloor =
\left\lfloor \frac{n}{d} \times \frac{2^s}{2^s} \right\rfloor =
\left\lfloor \frac{2^s}{d} \times \frac{n}{2^s} \right\rfloor =
\left\lfloor {m} \times \frac{n}{2^s} \right\rfloor
$$

Here, $n$ is the dividend, $d$ is the divisor, and $m = \frac{2^s}{d}$. Dividing $n$ by $2^s$ can be implemented by shifting right by $s$ bits. To avoid overflow in the $m \times n$ multiplication, $m$ is split as follows:

$$
m = (m - 2^{32}) + 2^{32}
$$

Let $magic = m - 2^{32}$. Substituting this into the above equation yields:

$$
\left\lfloor \frac{n}{d} \right\rfloor =
\left\lfloor \frac{n \times {magic} + n \times 2^{32}}{2^s} \right\rfloor =
\left\lfloor \left(\frac{n \times magic}{2^{32}} + n\right) >> (s-32) \right\rfloor =
\left\lfloor \left(\frac{n \times magic}{2^{32}} + n\right) >> shift \right\rfloor
$$

Here, magic and shift are the multiplication magic number and shift amount required for fast division. The computation of magic and shift depends only on the fixed divisor, so they can be precomputed in SIMD and reused during the SIMT computation process. The relevant code is as follows.

```cpp
__aicore__ inline void calc_magic_shift(uint32_t& magic_out, uint32_t& shift_out, uint32_t divisor)
{
    if (divisor == 0) {
        magic_out = 0;
        shift_out = 0;
        return;
    }

    int64_t pos = BIT_64_LEN - CountLeadingZero(divisor);
    int64_t cnt1 = GetBitCount1(divisor);
    uint32_t shift = (cnt1 == 1) ? (pos - 1) : pos;
    uint32_t magic = (1l << BIT_32_LEN) * ((1l << shift) - divisor) / divisor + 1;

    magic_out = magic;
    shift_out = shift;
}
```

During the SIMT computation process, each thread reads the dividend value to be processed, calls __umulhi(value, magic) to obtain the high 32 bits q of the product of value and magic, and then shifts value + q right by shift bits to obtain a result equivalent to value / divisor. The key code is as follows.

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
    uint32_t q = __umulhi(value, magic);
    uint32_t result = (value + q) >> shift;
    output[local_idx] = result;
}
```

[Performance Comparison]

With the same number of thread blocks, the same input and output specifications, and the SIMD copy interface used for data transfer between GM and UB, the performance data of scenario 0 and scenario 1 are compared as follows.

| Scenario | Implementation | Data Size | Task Duration\(μs\) | aiv\_vec\_time\(μs\) | aiv\_scalar\_time\(μs\) |
|:---|:---|---:|---:|---:|---:|
| Scenario 0 | Ordinary division | 8388608 | 110.167 | 40.685 | 10.701 |
| Scenario 1 | Fast division | 8388608 | 98.235 | 22.497 | 12.707 |

Compared with scenario 0, scenario 1 replaces ordinary integer division with multiplication and shift. The task duration decreases from 110.167 μs to 98.235 μs, and the end-to-end time drops by about 10.8%. aiv\_vec\_time decreases from 40.685 μs to 22.497 μs, a drop of about 44.7%, indicating that after replacing ordinary division with multiplication and shift, the time consumed by computation instructions is significantly reduced. aiv\_scalar\_time increases from 10.701 μs to 12.707 μs, mainly because fast division requires additionally computing magic and shift in the Scalar computation unit. However, the task duration still decreases, indicating that the computation benefit brought by fast division can offset the Scalar computation overhead.

[Summary] For SIMT integer division scenarios with a fixed divisor, the multiplication magic number magic and the shift amount shift can be precomputed in SIMD and reused during the SIMT computation process. By replacing ordinary integer division with multiplication and shift operations, the division computation overhead can be effectively reduced.
