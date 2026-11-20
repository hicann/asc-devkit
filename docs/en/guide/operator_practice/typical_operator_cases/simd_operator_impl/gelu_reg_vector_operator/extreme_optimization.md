# Advanced Extreme Optimization<a name="ZH-CN_TOPIC_000000_GELU_ADV"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-11-09T10:43:14.712Z pushedAt=2026-11-19T06:32:19.838Z nl=2 -->

This section describes the advanced extreme optimization methods for the GELU operator, building on the intermediate performance optimization.

## Residual Bottleneck Analysis

After intermediate optimization, the IPC is 1.20, which is still far from the theoretical VF fusion limit of IPC = 2.0. The residual bottlenecks are mainly as follows:

- **Bottleneck 1: Overly long instruction dependency chain in the VF function loop**: The GELU computation contains eight vector instructions that form a long dependency chain. The compiler schedules too few loop iterations in the limited execution queue, resulting in an insufficient number of instructions that can be dual-issued per cycle, so the IPC is only 1.20.
- **Bottleneck 2: Scalar overhead of loop control**: The variable update and condition check of the for loop introduce scalar instructions, whose relative proportion increases after the vector instructions are significantly shortened.

## Ultimate Refinement: Loop Unrolling Optimization

**Principle**: Use `#pragma unroll N` to instruct the compiler to unroll the loop N times, eliminating loop control overhead. This also allows the VF instructions of multiple iterations to be interleaved, improving instruction-level parallelism (ILP) so that more instructions can be dual-issued consecutively.

```
Unrolled:                          After unrolling:
Iter0: Load→Mul→...→Store       Iter0: Load→Mul→...→Store
  ↓ (loop control overhead)               Iter1: Load→Mul→...→Store
Iter1: Load→Mul→...→Store       Iter2: Load→Mul→...→Store
  ↓ (loop control overhead)               Iter3: Load→Mul→...→Store
Iter2: Load→Mul→...→Store       ... (no loop control overhead, instructions can be interleaved)
```

**Optimization**: Add `#pragma unroll 6` before the for loop in the VF function. The unroll factor 6 is an empirical value and should be tuned according to the actual scenario.

**Suggestions for choosing the unroll factor**:
- Too much unrolling: increases register pressure and may cause spilling, degrading performance instead.
- Too little unrolling: the optimization effect is not obvious.
- Suggestion: Try values such as 2, 4, 6, and 8 in sequence to find the optimal unrolling factor.

## Ultimate Performance Achieved

| Metric | Entry-level (Baseline) | Intermediate Optimization (VF Fusion) | Advanced Optimization (VF Fusion + Loop Unrolling) |
|:---|:---:|:---:|:---:|
| Task Duration (μs) | 351.525 | 348.868 | **344.436** |
| aiv_vec_time (μs) | 147.895 | 66.277 | **63.655** |
| IPC | - | 1.20 | **1.25** |

**Key verification metrics**:
- The vector instruction time drops from 147.895 μs in the entry-level version to **63.655 μs**, a reduction of **56.9%**
- The IPC increases from 1.20 in the intermediate optimization version to **1.25**, an improvement of **4.2%**
- The current IPC is 1.25, still below the theoretical limit of 2.0, mainly limited by the dependency chain length of the eight GELU instructions

**Directions for further IPC improvement**: Split an overly long loop into multiple short loops at the completion points of long-latency instructions (such as Exp and Div), so that each dependency chain is shorter and dual-issue efficiency is higher. The loop in this sample is relatively short and does not apply to this optimization.

For details, see the [GELU performance tuning sample case 2](../../../../../../../examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance/README_en.md#case-2-enable-regbase-api-vf-fusion-and-loop-unrolling-optimization).
