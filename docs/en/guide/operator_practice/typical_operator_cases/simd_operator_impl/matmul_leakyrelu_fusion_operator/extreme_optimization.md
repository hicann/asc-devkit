# Advanced Extreme Optimization<a name="ZH-CN_TOPIC_000000_MATMUL_LEAKYRELU_ADV"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T13:24:08.238Z pushedAt=2026-11-03T07:05:13.383Z nl=2 -->

This section describes advanced optimization techniques for the Matmul fusion operator, building on the intermediate-level performance optimization.

> **Note:** To better demonstrate the performance optimization techniques of VF fusion, this chapter uses the Matmul+Gelu fusion operator as an example. Compared with LeakyRelu, GELU involves more complex computation logic and can more fully reflect the effects of various optimization techniques.

## Advanced Matmul Optimization Techniques

| Case | Optimization Technique | Description |
|------|---------|------|
| [Case 5](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README_en.md#case-5-multi-core-mdl--l1cache-optimization-multi_core_mdl_l1cache) | MDL + L1Cache optimization | Manually adjust the tiling parameters to make full use of the L1 cache. |
| [Case 6](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README_en.md#case-6-multi-core-mdl--l1cache--l2cache-multi_core_mdl_l1cache_l2cache) | MDL + L1Cache + L2Cache | Compute in blocks to fit the L2 cache and improve the cache hit rate. |
| [Case 7](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README_en.md#case-7-multi-core-mdl--l1cache--l2cache--constants-tiling-multi_core_mdl_l1cache_l2cache_constants) | + Constants Tiling | Make tiling constant to reduce the runtime scalar computation overhead. |
| [Case 8](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README_en.md#case-8-multi-core-mdl--l1cache--l2cache--constants-tiling--unitflag-multi_core_mdl_l1cache_l2cache_constants_unitflag) | + UnitFlag | Fine-grained pipeline synchronization to run the MMAD and FIXPIPE pipelines in parallel. |

## Gelu Advanced Optimization Techniques

| Case | Optimization Technique | Description |
|------|---------|------|
| [Case 2](../../../../../../../examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance/README_en.md#case-2-enable-regbase-api-vf-fusion-and-loop-unrolling-optimization) | VF fusion + loop unrolling | `#pragma unroll` unrolls the loop to improve instruction-level parallelism |

## CV Fusion: Scenario 2 — CV Fusion-UB Direct Pass-Through

In the advanced optimization stage, the CV fusion layer provides the Scenario 2 method (supported only on Ascend 950PR). On the AIC side, the L0C computation results are written directly to the Unified Buffer (UB) through Fixpipe. On the AIV side, there is no need to read data from GM; GELU computation is performed directly on the UB and the results are then written back to GM, eliminating the read/write overhead of GM transfer.

> **Compatibility note:** A2/A3 chips do not support Fixpipe direct write to UB. Use the Scenario 1 (CV fusion-GM transfer) solution instead.

Data flow path:
```
GM ──(MTE2)──> L1 ──(MTE1)──> L0A/L0B ──(Cube)──> L0C ──(Fixpipe)──> UB ──(Gelu)──> UB ──(MTE3)──> GM
```

**AIC-side implementation**: Fixpipe L0C → UB (enable dual-destination mode via `dualDstCtl=0b01`, splitting by the M dimension to 2 AIVs)

```cpp
AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::ROW_MAJOR> fixpipeParams;
fixpipeParams.mSize = AscendC::DivCeil(curM, 2) * 2;
fixpipeParams.nSize = curN;
fixpipeParams.srcStride = curMAlign;
fixpipeParams.dstStride = curN;
fixpipeParams.dualDstCtl = 0b01;  // Split by the M dimension to the UBs of 2 AIVs
AscendC::Fixpipe<float, float, CFG_ROW_MAJOR_UB>(xUB, cLocal, fixpipeParams);
```

**AIV-side implementation**: UB → GELU → geluOutUB → GM (no DataCopy GM→UB required)

### Performance Data

The following uses data from the Ascend 950PR chip as an example:
- After Scenario 2 fusion, the total time is 2573.584 μs, which is about 33 μs less than the 2606.91 μs of Scenario 1.
- In Scenario 2, aiv_mte2_time is only 0.005 μs, which is almost reduced to zero compared with the 57.145 μs of Scenario 1.

For details, see [Matmul+Gelu Advanced Extreme Optimization](../../../../../../../examples/01_simd_cpp_api/05_best_practices/03_fusion_compute/matmul_gelu_high_performance/README_en.md).
