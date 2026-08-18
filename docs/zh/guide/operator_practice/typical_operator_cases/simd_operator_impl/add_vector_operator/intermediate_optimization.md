# 中级性能优化<a name="ZH-CN_TOPIC_000000_ADD_MID"></a>

本节在入门功能落地的基础上，介绍Add算子的中级性能优化方法。

## 性能瓶颈分析

基于入门级代码的Profiling数据（以[8192×8192 half](../../../../../../../examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance/README.md#case-0-单核标量版本基准程序)为基准），核心性能瓶颈如下：

> **说明**：入门功能落地版使用float数据类型是为了简化理解；从中级性能优化版起切换至half数据类型，以获得更高的向量计算性能和搬运效率。以下Profiling数据及优化效果均基于half类型测试。

| 指标 | Case 0（标量基准） | Case 1（单核向量） | Case 2（多核切分） |
|:---|:---:|:---:|:---:|
| Task Duration(μs) | 1239689.1 | 6909.6 | 306.58 |
| aiv_vec_ratio | 0 | 0.11 | 0.053 |
| aiv_scalar_ratio | 0.995 | 0.033 | 0.028 |
| aiv_mte2_ratio | 0.005 | 0.873 | 0.725 |
| aiv_mte3_ratio | 0.002 | 0.378 | 0.183 |

**核心瓶颈解读**：

- **痛点1：标量运算瓶颈**（Case 0）：标量指令耗时占比99.5%，逐元素串行执行，无法利用硬件并行能力。
- **痛点2：数据搬运瓶颈**（Case 1）：单核向量版本中MTE2占比87.3%，数据搬运成为主要瓶颈，单核搬运请求量不足导致带宽未用满。
- **痛点3：搬运粒度小**（Case 2）：多核切分后MTE2占比72.5%，小块搬运（8KB）未充分利用带宽。

## 优化手段解析与实施

### 优化策略1：标量→向量指令

**原理解析**：标量运算每次只能处理1个数据元素，需要逐元素循环；向量指令（如[AscendC::Add](../../../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Add.md)）单次可处理多个数据元素，利用向量单元的并行计算能力，可获得百倍以上性能提升。

**代码改造点**：将`for`循环配合`GetValue/SetValue`的标量运算替换为向量API：

```cpp
// 标量方式（Case 0）
for (uint32_t i = 0; i < curLen; i++) {
    float xVal = (float)xLocal.GetValue(i);
    float yVal = (float)yLocal.GetValue(i);
    zLocal.SetValue(i, (half)(xVal + yVal));
}

// 向量方式（Case 1）
AscendC::Add(zLocal, xLocal, yLocal, curLen);
```

性能提升：端到端耗时从1239689.1μs降至6909.6μs，提升**99.4%**。

### 优化策略2：多核均匀切分 + 大块搬运

**原理解析**：
- **多核并行**：将数据均匀切分到多个AIV Core，实现并行计算。使用均匀切分策略（余数分配到前几个核），确保负载均衡。
- **大块搬运**：在Unified Buffer（UB）空间允许范围内增大`dataCopyLen`，减少搬运次数，摊薄启动开销，更好地利用内存带宽。

**代码改造点**：

```cpp
// 均匀切分：计算每个核处理的行数
uint32_t baseCoreM = totalM / splitM;
uint32_t remainderM = totalM % splitM;
if (blockIdxM < remainderM) {
    actualCoreM = baseCoreM + 1;  // 均匀分配余数
}

// 增大搬运粒度：dataCopyLen从4096提升至16384（Case 2→Case 3）
```

性能提升：Case 2→Case 3，端到端耗时从306.58μs降至268.5μs，减少**12.5%**。

### 优化策略3：双缓冲流水线并行

**原理解析**：采用Ping-Pong双缓冲技术，Ping缓冲区进行计算时，Pong缓冲区进行数据搬运，交替执行实现计算与搬运的流水线并行。

<img src="../../../../../../../examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance/figures/DoubleBuffer.png" width="50%">

**代码改造点**：

```cpp
// Ping-Pong双缓冲地址分配
static constexpr uint32_t xAddrPing = 0;
static constexpr uint32_t yAddrPing = MAX_DATA_COPY_LEN * sizeof(half);
static constexpr uint32_t zAddrPing = yAddrPing + MAX_DATA_COPY_LEN * sizeof(half);
static constexpr uint32_t xAddrPong = zAddrPing + MAX_DATA_COPY_LEN * sizeof(half);
// ...

// 双缓冲流水线：交替使用两组缓冲区
for (uint32_t loopIdx = 0; loopIdx < totalBlocks; loopIdx++) {
    int32_t eventID = (loopIdx % 2 == 0 ? EVENT_ID0 : EVENT_ID1);
    // ... 数据搬运和计算，使用对应的eventID同步
}
```

性能提升：Case 3→Case 4，端到端耗时从268.5μs降至264.02μs，减少**1.7%**。双缓冲的收益有限，说明瓶颈在搬运速度本身。

## 优化效果评估

| 指标 | Case 0（入门基准） | Case 4（中级优化版） | 提升幅度 |
|:---|:---:|:---:|:---:|
| Task Duration(μs) | 1239689.1 | 264.02 | **4695倍** |
| aiv_vec_time(μs) | 0.015 | 12.846 | - |
| aiv_scalar_time(μs) | 1233742.494 | 2.796 | **99.8%** |
| aiv_mte2_ratio | 0.005 | 0.973 | MTE2利用率大幅提升 |

**本阶段结论**：经过标量→向量、多核切分、大块搬运、双缓冲四级优化，端到端性能从1.24秒降至264μs，提升4695倍。当前瓶颈在于搬运速度本身，MTE2利用率已达97.3%，进一步优化需要从L2 Cache和UB内存布局入手。

详细内容请参见[Add性能调优样例 Case 1-4](../../../../../../../examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance/README.md#中级性能优化)。
