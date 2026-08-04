# 高阶性能打磨<a name="ZH-CN_TOPIC_000000_ADD_ADV"></a>

本节在中级性能优化的基础上，介绍Add算子的高阶极限打磨方法。

## 残余瓶颈分析

中级优化后（Case 4），端到端耗时264.02μs，MTE2利用率97.3%，搬运与计算已实现流水并行。残余瓶颈主要体现在：

- **瓶颈1：L2 Cache污染**：Add输入数据只读取一次（流式访问），但默认经过L2 Cache，增加了不必要的Cache开销和搬运延时。
- **瓶颈2：UB Bank Conflict**：向量指令同时读写同一Bank Group内的数据，产生读写冲突和读读/写写冲突，导致访存串行化，向量指令效率降低。

## 极致打磨手段

### 极限调优A：L2 Cache bypass

**原理**：L2 Cache是AI Core和HBM之间的缓存层，适合需要重复访问的数据。对于Add算子这种流式访问场景（输入数据只读取一次），数据经过L2 Cache反而会占用Cache空间（Cache污染），增加搬运延时。通过[SetL2CacheHint](../../../../../api/SIMD-API/basic_api/data_structures/GlobalTensor/SetL2CacheHint.md)设置`CACHE_MODE_DISABLE`，让数据绕过L2 Cache直接从GM载入UB。

**优化方案**：

```cpp
// 设置L2 Cache bypass：对流式访问数据禁用L2 Cache
xGm.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);
yGm.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);
```

性能提升：Case 4→Case 5，端到端耗时从264.02μs降至187.68μs，减少**28.9%**；MTE2耗时从250.528μs降至175.997μs，减少**29.8%**。

### 极限调优B：消除UB Bank Conflict

**原理**：针对Atlas A2/A3系列产品，UB的大小为192KB，包含16个Bank Group，每个Bank Group包含3个Bank。向量指令一拍读取256B数据（即同时读取8个block），当xPing和yPing的起始地址落在同一Bank Group时，会产生读写冲突和读读/写写冲突，导致访存串行化。

**优化方案**：通过调整`dataCopyLen`（16384→16256）使xPing和yPing的起始地址错开256B，确保vec指令一拍访存的数据分布在不同Bank Group，有效消解Bank Conflict。

```cpp
// 优化后的地址布局（避免Bank Conflict）
static constexpr uint32_t xAddrPingBC = 0;
static constexpr uint32_t yAddrPingBC = BANK_CONFLICT_DATA_COPY_LEN * sizeof(half);  // 错开256B；注意：sizeof(half)=2B，float场景需调整为sizeof(float)
static constexpr uint32_t xAddrPongBC = MAX_DATA_COPY_LEN * sizeof(half) * 2;
// ...
```

性能提升：Case 5→Case 6，向量指令耗时从12.846μs降至6.954μs，减少**45.9%**；端到端耗时从187.68μs降至184.52μs，减少**1.7%**。

## 终极性能达成

| 指标 | Case 0（入门基准） | Case 4（中级优化） | Case 6（高阶打磨） |
|:---|:---:|:---:|:---:|
| Task Duration(μs) | 1239689.1 | 264.02 | **184.52** |
| aiv_vec_time(μs) | 0.015 | 12.846 | **6.954** |
| aiv_mte2_ratio | 0.005 | 0.973 | **0.961** |
| 端到端提升 | 1x | 4695x | **6711x** |

**关键验证指标**：
- 端到端耗时从入门版的1.24秒降至**184.52μs**，提升**6711倍**
- 向量指令耗时从12.846μs降至**6.954μs**，接近Atlas A2训练系列48核理论vector耗时5.904μs
- Case 6的读写混合带宽达到**2.3456 TB/s**，接近硬件带宽上限

**最终结论**：Add算子经过入门→中级→高阶三级优化，端到端性能提升6711倍。当前瓶颈主要在搬运带宽本身，向量计算已接近理论极限。对于Ascend 950系列产品，Bank排列方式不同，无需Bank Conflict优化，Case 5即为最优版本。

详细内容请参见[Add性能调优样例 Case 5-6](../../../../../../../examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance/README.md#高阶性能打磨)。
