# 高阶极限打磨<a name="ZH-CN_TOPIC_000000_MATMUL_LEAKYRELU_ADV"></a>

本节在中级性能优化的基础上，介绍Matmul融合算子的高阶极限打磨方法。

> **说明**：为更好展示VF融合的性能优化手段，本章节以Matmul+Gelu融合算子作为讲解样例。GELU相比LeakyRelu计算逻辑更复杂，能更充分地体现各类优化手段的效果。

## Matmul高阶优化手段

| Case | 优化手段 | 说明 |
|------|---------|------|
| [Case 5](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README.md#case-5-多核mdl--l1cache优化-multi_core_mdl_l1cache) | MDL + L1Cache优化 | 手动调整Tiling参数，充分利用L1缓存 |
| [Case 6](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README.md#case-6-多核mdl--l1cache--l2cache-multi_core_mdl_l1cache_l2cache) | MDL + L1Cache + L2Cache | 分块计算适配L2Cache，提高缓存命中率 |
| [Case 7](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README.md#case-7-多核mdl--l1cache--l2cache--constants-tiling-multi_core_mdl_l1cache_l2cache_constants) | + Constants Tiling | Tiling常量化，减少运行时Scalar计算开销 |
| [Case 8](../../../../../../../examples/01_simd_cpp_api/05_best_practices/01_matrix_compute/matmul_high_performance/README.md#case-8-多核mdl--l1cache--l2cache--constants-tiling--unitflag-multi_core_mdl_l1cache_l2cache_constants_unitflag) | + UnitFlag | 细粒度流水同步，MMAD与FIXPIPE流水并行 |

## Gelu高阶优化手段

| Case | 优化手段 | 说明 |
|------|---------|------|
| [Case 2](../../../../../../../examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance/README.md#case-2-启用regbase-apivf融合能力和循环展开优化) | VF融合 + 循环展开 | `#pragma unroll`展开循环，提高指令级并行度 |

## CV融合：Scenario 2 — CV融合-UB直通

在高阶极限打磨阶段，CV融合层面提供Scenario 2方式（仅Ascend 950PR支持）。AIC侧通过Fixpipe将L0C计算结果直接写入UB，AIV侧无需从GM读取数据，直接在UB上进行GELU计算后将结果写回GM，省去GM中转的读写开销。

> **兼容性说明**：A2/A3芯片不支持Fixpipe直写UB，请使用Scenario 1（CV融合-GM中转）方案。

数据流路径：
```
GM ──(MTE2)──> L1 ──(MTE1)──> L0A/L0B ──(Cube)──> L0C ──(Fixpipe)──> UB ──(Gelu)──> UB ──(MTE3)──> GM
```

**AIC侧实现**：Fixpipe L0C → UB（通过`dualDstCtl=0b01`开启双目标模式，按M维度拆分给2个AIV）

```cpp
AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::ROW_MAJOR> fixpipeParams;
fixpipeParams.mSize = AscendC::DivCeil(curM, 2) * 2;
fixpipeParams.nSize = curN;
fixpipeParams.srcStride = curMAlign;
fixpipeParams.dstStride = curN;
fixpipeParams.dualDstCtl = 0b01;  // 按M维度拆分到2个AIV的UB
AscendC::Fixpipe<float, float, CFG_ROW_MAJOR_UB>(xUB, cLocal, fixpipeParams);
```

**AIV侧实现**：UB → GELU → geluOutUB → GM（无需DataCopy GM→UB）

### 性能数据

以Ascend 950PR芯片数据为例：
- Scenario 2融合后总耗时2573.584μs，相比Scenario 1的2606.91μs进一步减少约33μs
- Scenario 2中aiv_mte2_time仅为0.005μs，相比Scenario 1的57.145μs几乎降为零

详细内容请参见[Matmul+Gelu高阶极限打磨](../../../../../../../examples/01_simd_cpp_api/05_best_practices/03_fusion_compute/matmul_gelu_high_performance/README.md)。
