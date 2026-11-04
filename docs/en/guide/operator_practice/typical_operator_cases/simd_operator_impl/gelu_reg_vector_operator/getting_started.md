# Entry-Level Feature Implementation<a name="ZH-CN_TOPIC_000000_GELU_ENTRY"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T13:21:52.113Z pushedAt=2026-11-03T07:05:13.381Z nl=2 -->

This section uses the GELU (Gaussian Error Linear Unit) activation function as an example to introduce the basic method of implementing a vector operator based on the Ascend C RegBase programming model, helping developers quickly get started with Reg vector operator development.

## Overview

The approximate calculation formula of GELU is as follows:

$$
GELU(x) \approx \frac{x}{1 + e^{-1.595769 \cdot x - 0.071405 \cdot x^3}}
$$

- x: input, with a shape of [256, 32] and the data type float;
- y: output, with a shape of [256, 32] and the data type float;

## Operator Implementation

The GELU operator is implemented using the RegBase programming model: it calls VF functions through [asc_vf_call](../../../../../api/SIMD-API/basic_api/reg_vector_compute/vf_call/asc_vf_call.md) to perform step-by-step computation in registers. Compared with the traditional MemBase approach (based on [LocalTensor](../../../../../api/SIMD-API/basic_api/data_structures/LocalTensor/LocalTensor_intro.md) + Compute API), RegBase allows intermediate computation results to be temporarily stored in registers instead of the Unified Buffer (UB), reducing the number of UB read/write operations and making it suitable for multi-step fused computation scenarios.

The complete data path is: GM → UB → registers (step-by-step computation) → UB → GM.

The core computation logic is encapsulated in the VF function:

```cpp
// VF function: perform step-by-step GELU computation in registers
__simd_vf__ inline static void GeluVfMethod2(
    __ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t count, uint32_t loopNum)
{
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);  // GetVecLen() returns the byte length of one repeat (VL=256B). Divide it by the byte size of the data type to obtain the number of elements processed per repeat (64 elements for float).
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg;
    AscendC::Reg::RegTensor<float> yReg;
    AscendC::Reg::RegTensor<float> tmpReg;
    for (uint32_t i = 0; i < loopNum; ++i) {
        mask = AscendC::Reg::UpdateMask<float>(count);
        AscendC::Reg::LoadAlign(xReg, xAddr + i * oneRepeatSize);   // UB → register
        AscendC::Reg::Mul(yReg, xReg, xReg, mask);                  // x²
        AscendC::Reg::Mul(yReg, yReg, xReg, mask);                  // x³
        AscendC::Reg::Muls(yReg, yReg, COEFF_CUBIC, mask);          // -0.071405 * x³
        AscendC::Reg::Muls(tmpReg, xReg, COEFF_LINEAR, mask);       // -1.595769 * x
        AscendC::Reg::Add(yReg, tmpReg, yReg, mask);                // -1.595769x - 0.071405x³
        AscendC::Reg::Exp(yReg, yReg, mask);                        // exp(...)
        AscendC::Reg::Adds(yReg, yReg, 1.0f, mask);                 // 1 + exp(...)
        AscendC::Reg::Div(yReg, xReg, yReg, mask);                  // x / (1 + exp(...))
        AscendC::Reg::StoreAlign(yAddr + i * oneRepeatSize, yReg, mask); // register → UB
    }
}
```

For the complete sample code, see [GELU Introduction Sample](../../../../../../../examples/01_simd_cpp_api/00_introduction/04_reg_compute/gelu).

## Optimization Direction Analysis

| No. | Optimization Direction | Issue in Current Implementation | Expected Optimization Benefit |
|------|-----------|--------------|------------|
| 1 | VF fusion dual-issue optimization | The GELU computation inside the VF function has a long dependency path, the dual-issue feature of VF fusion is not fully utilized, and the IPC is low | By leveraging the VF fusion dual-issue feature, the parallelism of regular computation instructions can reach 512 bytes/cycle, and the vectorized instruction time can be reduced by more than **55%** |
| 2 | Loop unrolling optimization | The loop inside the VF function is a simple for loop, so the compiler cannot fully schedule instruction-level parallelism | Use `#pragma unroll N` to unroll the loop, improving instruction issue parallelism and IPC. On top of VF fusion, the vector instruction time can be further reduced by about **4.6%** |
| 3 | Serial pipeline execution | The three stages of copy-in, computation, and copy-out are executed strictly serially, so the hardware units (MTE2/V/MTE3) cannot work simultaneously | Adopt a multi-buffering mechanism so that copy-in, computation, and copy-out can run in parallel, improving hardware utilization |
| 4 | Fixed number of cores | Two cores are used fixedly, without dynamic allocation based on the actual number of available cores | Dynamically obtain the number of available cores to fully leverage multi-core parallel capability |
