# Add性能调优样例

## 概述

本样例以加法为例，介绍基于静态Tensor方式编程的性能调优方法。整个调优过程分为七个步骤（case 0-6），逐步展示从标量运算到向量运算、从单核到多核、从基础实现到深度优化的完整调优路径。

**优化路径**：
- Case 0: 单核标量版本（基准）
- Case 1: 单核向量版本
- Case 2: 多核均匀切分 + 小块搬运
- Case 3: 多核均匀切分 + 大块搬运
- Case 4: 多核均匀切分 + 双缓冲优化
- Case 5: 多核均匀切分 + 双缓冲 + L2Cache bypass
- Case 6: 多核均匀切分 + 双缓冲 + L2Cache bypass + 避免Bank Conflict

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```
├── add_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── figures                 // 图示
│   ├── add.asc                 // Ascend C样例实现（包含7个优化case）
│   └── README.md               // 样例说明文档
```

## 样例描述

**样例功能**：

  样例实现的是固定shape为8192×8192的两个矩阵相加。

  Add的计算公式为：

$$
  z = x + y
$$

  - x：输入，形状为[8192, 8192]，数据类型为half；
  - y：输入，形状为[8192, 8192]，数据类型为half；
  - z：输出，形状为[8192, 8192]，数据类型为half；

## 样例实现

### 性能指标说明

**表1 AI Core 性能指标字段说明表**
| 字段名 | 字段含义 |
|:---:|:---|
| Task Duration(μs)|Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间。|
| aiv_time|Task在AI Vector Core上的执行时间，单位为μs。|
| aiv_vec_time(μs) | vec类型指令（向量类运算指令）耗时，单位μs。 |
| aiv_vec_ratio | vec类型指令（向量类运算指令）的cycle数在total cycle数中的占用比。 |
| aiv_scalar_time(μs) | scalar类型指令（标量类运算指令）耗时，单位μs。 |
| aiv_scalar_ratio | scalar类型指令（标量类运算指令）的cycle数在total cycle数中的占用比。 |
| aiv_mte2_time(μs) | mte2类型指令（GM->UB搬运类指令）耗时，单位μs。 |
| aiv_mte2_ratio | mte2类型指令（GM->UB搬运类指令）的cycle数在total cycle数中的占用比。 |
| aiv_mte3_time(μs) | mte3类型指令（UB->GM搬运类指令）耗时，单位μs。 |
| aiv_mte3_ratio | mte3类型指令（UB->GM搬运类指令）的cycle数在total cycle数中的占用比。 |

本章节性能数据在Atlas A2训练系列产品上运行得到。

### Case 0: 单核标量版本（基准程序）

**实现方式**：参考 `KernelAdd::ProcessScalar()` 函数实现

基准程序实现了`half`类型的两组输入数据的加法，采用`for`循环`scalar`运算的方式进行计算。

**关键代码**：
```cpp
for (uint32_t i = 0; i < curLen; i++) {
      float xVal = (float)xLocal.GetValue(i);
      float yVal = (float)yLocal.GetValue(i);
      zLocal.SetValue(i, (half)(xVal + yVal));
    }
```

**样例配置**：
- 单核标量运算
- `dataCopyLen = 4096` 为每次搬运的数据量元素个数
- 单次搬运的数据量为 4096 * 2B = 8192 Byte，单次scalar处理的数据量为 4 Byte

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 1239689.1 | 1239688.63 | 0.015 | 0 | 1233742.494 | 0.995 | 5916.341 | 0.005 | 2485.465 | 0.002 |

**优化效果分析**：
- 端到端耗时：**1239689.1μs**（约1.24秒）
- 标量指令耗时：1233742.494μs，占比 **99.5%**
- 向量指令耗时：0.015μs，占比接近0%
- 性能瓶颈：标量运算串行执行，无法利用硬件并行能力，该场景仅作为Add运算性能对比样例，在实际业务场景中不建议用户使用Scalar运算

**原理说明**：
- 标量运算每次只能处理1个数据元素，需要逐元素循环
- AI Core的硬件优势在于向量/矩阵并行计算，标量运算无法发挥硬件能力

**性能优化建议**：
> ⚠️ **避免标量循环，使用向量指令**
> 
> 在Ascend C编程中，应避免使用`for`循环配合`GetValue/SetValue`的标量运算。使用`AscendC::Add`等向量指令可带来数量级的性能提升。

---

## 中级性能优化

### Case 1: 单核向量版本

**实现方式**：参考 `KernelAdd::ProcessSingle()` 函数实现

将标量运算转换为向量运算，使用`AscendC::Add`向量指令替代标量循环，大幅提升计算效率。

**关键代码**：
```cpp
AscendC::Add(zLocal, xLocal, yLocal, curLen);
```

**样例配置**：
- 单核运算
- `dataCopyLen = 4096` 为每次搬运的数据量元素个数
- 单次搬运操作`DataCopy`的数据量为 8192 Byte
- 单次`Add`处理两个输入`Tensor`，处理的总数据量为 16384 Byte


**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 6909.6 | 6909.14 | 761.65 | 0.11 | 231.166 | 0.033 | 6208.762 | 0.873 | 2613.205 | 0.378 |

**优化效果分析**：
- 端到端性能：6909.6μs，相比Case 0提升 **99.4%**
- 标量指令耗时：从1233742.494μs降至231.166μs，大幅减少了标量指令
- 向量指令耗时：761.65μs，占比11%
- 数据搬运耗时：6208.762μs，占比87.3%，搬运流水串行提升

**原理说明**：
- 向量指令单次可处理多个数据元素（本例中单次处理 4096*2 个half元素）
- 向量单元的并行计算能力远超标量单元
- 但数据搬运成为瓶颈，说明计算速度已经快于数据供给，单核时数据搬运的请求量不足，导致带宽未用满

**性能优化建议**：
> 💡 **使用向量指令替代标量循环**
> 
> 使用`AscendC::Add`、`AscendC::Mul`等向量API替代逐元素的标量循环，可充分利用AI Core的向量计算单元，获得百倍以上的性能提升。

> 💡 **不建议用户仅使用单核**

**下一步优化方向**：
- 数据搬运（MTE2）占比87.3%，成为主要瓶颈
- 需要通过多核并行和增大搬运粒度来提升带宽利用率

---

### Case 2: 多核均匀切分 + 小块搬运

**实现方式**：参考 `KernelAdd::Process()` 函数实现

开启多核并行计算，将8192×8192的矩阵切分到多个AIV Core上并行处理，采用均匀切分策略。

**样例配置**：
- 行方向切分48份，将数据均匀切分至48个核运算
- `dataCopyLen = 4096` 为每次搬运的数据量元素个数
- 单次搬运操作`DataCopy`的数据量为 4096 * 2B = 8192 Byte
- 单次`Add`处理两个输入`Tensor`，处理的总数据量为 16384 Byte


**关键代码**：
```cpp
// 均匀切分计算每个核处理的行数（M方向）
uint32_t baseCoreM = totalM / splitM;
uint32_t remainderM = totalM % splitM;
if (blockIdxM < remainderM) {
    actualCoreM = baseCoreM + 1;  // 均匀分配余数
    startM = blockIdxM * actualCoreM;
} else {
    actualCoreM = baseCoreM;
    startM = remainderM * (baseCoreM + 1) + (blockIdxM - remainderM) * baseCoreM;
}
```

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 306.58 | 297.14 | 15.885 | 0.053 | 8.219 | 0.028 | 215.485 | 0.725 | 54.33 | 0.183 |

**优化效果分析**：
- 端到端任务耗时为306.58μs，相比Case 1耗时减少 **95.5%**
- 数据搬运MTE2耗时为215.485μs，占比72.5%

**原理说明**：
- 48个AIV Core并行处理，理论上可获得48倍加速
- 实际加速低于理论值的原因：
  - 数据搬运仍是瓶颈（mte2占比72.5%）
- 均匀切分确保各核负载均衡，避免出现部分核空闲、部分核繁忙的负载不均问题

- 本样例选择 `M` 方向切48份（`splitM=48, splitN=1`），而不是 `M` 切8、`N` 切6，核心目的是保持每个核处理的数据在GM上连续，从而在分块循环中用单条 `DataCopy` 搬运 `dataCopyLen`

  <img src="figures/SplitCoreM.png" width="50%">

**性能优化建议**：
> 💡 **充分利用多核并行，采用均匀切分策略**
> 
> 1. 将数据均匀切分到多个AIV Core，实现并行计算
> 2. 使用均匀切分策略（余数分配到前几个核），确保负载均衡
> 3. 切分粒度需考虑：核数、数据量、UB空间大小

**下一步优化方向**：
- MTE2占比72.5%，MTE3占比18.3%，搬运仍是瓶颈
- 计算仅占5.3%，说明"计算快、搬运慢"
- 可通过增大单次搬运数据量提升带宽利用率
---

### Case 3: 多核均匀切分 + 大块数据搬运

**实现方式**：参考 `KernelAdd::Process()` 函数实现

为了充分利用带宽资源，增大搬运指令的数据量。

**样例配置**：
- 行方向切分48份，将数据均匀切分至48个核运算
- `dataCopyLen = 16384` 为每次切分的数据量元素个数（4倍于Case2）
- 单次搬运操作`DataCopy`的数据量为 32768 Byte
- 单次`Add`处理两个输入`Tensor`，处理的总数据量为 65536 Byte


**性能数据**：

| Task Duration(μs)  | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 268.5 |261.5 | 12.845 | 0.049 | 2.833 | 0.011 | 184.369 | 0.705 | 57.354 | 0.219 |

**优化效果分析**：
- 端到端性能：268.5μs，相比Case 2耗时减少 **12.5%**
- 通过增大单次搬运的数据量，MTE2耗时从215.485μs降至184.369μs，耗时减少 **14.4%**

**原理说明**：
- 增大单次搬运数据量可减少搬运次数
- 更大的连续数据块可更好地利用内存带宽
- 但数据量受限于UB空间大小（本例UB需容纳x、y、z三份数据）

**性能优化建议**：
> 💡 **增大单次数据搬运量，减少搬运次数**
> 
> 1. 在UB空间允许的范围内，尽量增大`dataCopyLen`
> 2. 使用连续的大块数据搬运，避免频繁的小数据块搬运
> 3. 需平衡UB空间使用和搬运效率


> ⚠️ **注意：dataCopyLen并非越大越好**
> 
> 在Case 3的基础上，如果进一步增大dataCopyLen（如从16384增至16512），端到端性能基本持平（268.5μs vs 267.76μs）。建议结合数据总量、UB空间、对齐要求综合考虑，确定最优的dataCopyLen值。

---

### Case 4: 双缓冲优化

**实现方式**：参考 `KernelAdd::ProcessDoubleBuffer()` 函数实现

采用双缓冲（Double Buffer）技术，实现数据搬运与计算的流水线并行，隐藏内存访问延迟。

**关键代码**：
```cpp
// Ping-Pong双缓冲地址
static constexpr uint32_t xAddrPing = 0;
static constexpr uint32_t yAddrPing = MAX_DATA_COPY_LEN * sizeof(half);
static constexpr uint32_t zAddrPing = yAddrPing + MAX_DATA_COPY_LEN * sizeof(half);
static constexpr uint32_t xAddrPong = zAddrPing + MAX_DATA_COPY_LEN * sizeof(half);
static constexpr uint32_t yAddrPong = xAddrPong + MAX_DATA_COPY_LEN * sizeof(half);
static constexpr uint32_t zAddrPong = yAddrPong + MAX_DATA_COPY_LEN * sizeof(half);

// 双缓冲流水线：交替使用两个事件ID和两组缓冲区
for (uint32_t loopIdx = 0; loopIdx < totalBlocks; loopIdx++) {
    int32_t eventID = (loopIdx % 2 == 0 ? EVENT_ID0 : EVENT_ID1);
    AscendC::LocalTensor<half> &xLocal = (loopIdx % 2 == 0 ? xPing : xPong);
    // ... 数据搬运和计算，使用对应的eventID同步
    AscendC::Add(zLocal, xLocal, yLocal, curLen);
}
```

**样例配置**：
- 行方向切分48份，将数据均匀切分至48个核运算
- `dataCopyLen = 16384` 为每次切分的数据量元素个数
- 单次搬运操作`DataCopy`的数据量为 32768 Byte
- 单次`Add`处理两个输入`Tensor`，处理的总数据量为 65536 Byte
- 将待处理的数据一分为二，由此数据的进出搬运和Vector计算实现并行执行

**内存布局**：

```
UB内存分配（双缓冲）：
┌──────────────┐
│  xPing       │  0x00000
│  16384*2B    │
├──────────────┤
│  yPing       │  0x08000 (32768)
│  16384*2B    │
├──────────────┤
│  zPing       │  0x10000 (65536)
│  16384*2B    │
├──────────────┤
│  xPong       │  0x18000 (98304)
│  16384*2B    │
├──────────────┤
│  yPong       │  0x20000 (131072)
│  16384*2B    │
├──────────────┤
│  zPong       │  0x28000 (163840)
│  16384*2B    │
└──────────────┘
```

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 264.02 | 257.56 | 12.846 | 0.05 | 2.796 | 0.011 | 250.528 | 0.973 | 84.988 | 0.33 |

**优化效果分析**：
- 端到端性能：264.02μs，相比Case 3耗时减少 **1.7%**
- MTE2耗时从184.369μs增至250.528μs（+35.9%），MTE3耗时从57.354μs增至84.988μs（+48.2%），此时从串行的纯读带宽变成混合读写带宽，因此耗时增加，用户需更多关注端到端耗时的减少
- 由于开启了双缓冲，在流水中搬运和计算并行执行，隐藏了数据搬运时间并降低Vector指令的等待时间

**原理说明**：
- **Ping-Pong机制**：
  - Ping缓冲区进行计算时，Pong缓冲区进行数据搬运
  - 交替执行，实现计算与搬运的流水线并行，如下图所示  
    <img src="figures/DoubleBuffer.png" width="50%">

**性能优化建议**：
> 💡 **使用双缓冲实现搬运与计算并行**
> 
> 1. 当计算与搬运时间相近时，双缓冲收益最大
> 2. 需要足够的UB空间（约2倍单缓冲空间）
> 3. 使用独立的Event ID管理两组缓冲区的同步

**下一步优化方向**：
- 双缓冲的收益有限，说明瓶颈在搬运速度本身
- 可尝试L2 Cache优化来提升搬运效率
---

## 高阶性能打磨

### Case 5: 双缓冲 + L2 Cache bypass

**实现方式**：参考 `KernelAdd::ProcessDoubleBufferL2Bypass()` 函数实现（在该函数内部先设置 `SetL2CacheHint(CACHE_MODE_DISABLE)`，再调用 `ProcessDoubleBuffer()`）

在双缓冲基础上，对于只需要载入一次的数据量可以设置L2 Cache bypass，直接从GM载入UB。

**关键代码**：
```cpp
  xGm.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);
  yGm.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);
```

**样例配置**：
- 行方向切分48份，将数据均匀切分至48个核运算
- `dataCopyLen = 16384` 为每次切分的数据量元素个数
- 单次搬运操作`DataCopy`的数据量为 32768 Byte
- 单次`Add`处理两个输入`Tensor`，处理的总数据量为 65536 Byte
- 将待处理的数据一分为二，由此数据的进出搬运和Vector计算实现并行执行

**L2 Cache策略**：
- xGm：禁用L2 Cache（一次性读取）
- yGm：禁用L2 Cache（一次性读取）

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 187.68 | 185.15 | 12.846 | 0.069 | 4.479 | 0.024 | 175.997 | 0.951 | 160.291 | 0.866 |

**优化效果分析**：
- 端到端性能：187.68μs，相比Case 4耗时减少 **28.9%**
- MTE2耗时：从250.528μs降至175.997μs，减少 **29.8%**
- 向量指令耗时：12.846μs，保持不变

**原理说明**：
- **L2 Cache的作用**：
  - L2 Cache是AI Core和HBM之间的缓存层
  - 重复访问的数据可从L2 Cache读取，速度更快
- **流式访问特点**：
  - Add的输入数据只读取一次，不存在数据复用

**性能优化建议**：
> 💡 **合理采用L2 Cache bypass**
> 
> 1. 对于只读取一次的输入数据（如本例的x、y），设置`SetL2CacheHint(CACHE_MODE_DISABLE)`
> 2. 对于需要重复访问的数据（如卷积的权重），保留L2 Cache
> 3. 建议用户按照实测数据进行配置优化，在实际的模型和训练场景中，需要结合上下游进行合理配置

**下一步优化方向**：
- 搬运效率已提升，但向量指令效率仍有优化空间
- 可尝试优化UB内存布局避免Bank Conflict
---

### Case 6: 双缓冲 + L2 Cache bypass + 避免Bank Conflict

**实现方式**：参考 `KernelAdd::ProcessDoubleBufferBankConflict()` 函数实现

在双缓冲+L2 Cache bypass基础上，优化内存地址布局，避免UB（Unified Buffer）的Bank Conflict，实现最优性能。

**关键代码**：
```cpp
// 设置L2 Cache bypass
xGm.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);
yGm.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE);

// 优化后的地址布局（避免Bank Conflict）
static constexpr uint32_t xAddrPingBC = 0;
static constexpr uint32_t yAddrPingBC = BANK_CONFLICT_DATA_COPY_LEN * sizeof(half);  // 错开256B；注意：sizeof(half)=2B，float场景需调整为sizeof(float)
static constexpr uint32_t xAddrPongBC = MAX_DATA_COPY_LEN * sizeof(half) * 2;
static constexpr uint32_t yAddrPongBC = xAddrPongBC + BANK_CONFLICT_DATA_COPY_LEN * sizeof(half);
static constexpr uint32_t zAddrPingBC = MAX_DATA_COPY_LEN * sizeof(half) * 4;
static constexpr uint32_t zAddrPongBC = zAddrPingBC + BANK_CONFLICT_DATA_COPY_LEN * sizeof(half);
```

**样例配置**：
- 行方向切分48份，将数据均匀切分至48个核运算
- `dataCopyLen = 16256` 为每次切分的数据量元素个数
- 单次搬运操作`DataCopy`的数据量为 32512 Byte
- 单次`Add`处理两个输入`Tensor`，处理的总数据量为 65024 Byte
- 将待处理的数据一分为二，由此数据的进出搬运和Vector计算实现并行执行

**内存布局优化**：

针对Atlas A2/A3 系列产品，UB的大小为192KB，包含16个Bank Group，每个Bank Group包含3个Bank，每个Bank大小为4KB，由128行组成，每行长度为32B。

未优化前的UB Bank内存布局（即case5）
<img src="figures/UBBankConflict.png" width="100%">

可以看到这样同时存在一个bank内的读写冲突，一个bankgroup内的读读冲突以及写写冲突。

优化后的UB Bank内存布局（case6）
<img src="figures/UBBankConflictResolution.png" width="100%">

由于vec指令一拍读取256B的数据（即同时读取8个block的数据），如上图xping、yping的起始地址正好错开了256B，有效消解了ub bank冲突。

**Bank Conflict详解**：
- UB分为多个Bank Group，同时读写同一Bank Group会导致冲突
- 通过调整dataCopyLen（16384→16256）使数据起始地址偏移
- 确保vec指令一拍访存的数据分布在不同的Bank Group

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 184.52 | 178.49 | 6.954 | 0.039 | 3.424 | 0.019 | 171.611 | 0.961 | 121.442 | 0.68 |

**优化效果分析**：
- 端到端性能：184.52μs，相比Case 5耗时减少 **1.7%**
- 向量指令耗时：从12.846μs降至6.954μs，减少 **45.9%**
- MTE3耗时：从160.291μs降至121.442μs，减少 **24.2%**

**原理说明**：
- **Bank Conflict问题**：
  - UB（Unified Buffer）分为多个Bank Group
  - 向量指令一次读写的数据如果落在同一个Bank，会产生读写冲突
  - 向量指令一次读/写的数据如果落在同一个Bank Group，会产生读读冲突或写写冲突
  - Bank Conflict会导致访存串行化，降低向量指令效率
- **解决方法**：
  - 减小dataCopyLen（16384→16256），使数据起始地址产生偏移
  - 重新设计内存布局，确保同一时刻访问的数据分布在不同Bank

**性能优化建议**：
> 💡 **优化UB内存布局，避免Bank Conflict**
> 
> 1. 当`aiv_vec_time`异常高时，可能存在Bank Conflict
> 2. 通过调整dataCopyLen或内存布局偏移，使数据分布在不同Bank Group 
> 3. Bank Conflict优化对vector-bound场景收益明显

**最终性能总结**：
- 相比基准Case 0：性能提升 **6718.5倍**（1239689.1μs → 184.52μs）
- 相比单核向量Case 1：性能提升 **37.4倍**（6909.6μs → 184.52μs）


---

## 性能对比总结

### Atlas A2训练系列性能对比
下表为本样例在Atlas A2训练系列产品上运行的性能数据对比：

| Case | 优化策略 | 核数 | dataCopyLen | Task Duration(μs) | aiv_vec_time(μs) | 理论vector耗时(μs) | 读带宽(TB/S) | 读写混合带宽(TB/S) | 端到端耗时相对Case 0 |
|:---:|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 0 | 单核标量（基准） | 1 | 4096 | 1239689.1 | NA | 283.405 | 0.0454 | NA | 1x |
| 1 | 单核向量 | 1 | 4096 | 6909.6 | 761.65 | 283.405 | 0.0432 | NA | 179.4x |
| 2 | 多核均匀切分 | 48 | 4096 | 306.58 | 15.885 | 5.904 | 1.2457 | NA | 4043.6x |
| 3 | 增大搬运粒度 | 48 | 16384 | 268.5 | 12.845 | 5.904 | 1.4560 | NA | 4617.1x |
| 4 | 双缓冲 | 48 | 16384 | 264.02 | 12.846 | 5.904 | NA | 1.6072 | 4695.4x |
| 5 | L2 Cache bypass | 48 | 16384 | 187.68 | 12.846 | 5.904 | NA | 2.2755 | 6605.3x |
| 6 | Bank Conflict优化 | 48 | 16256 | 184.52 | 6.954 | 5.904 | NA | 2.3456 | 6718.5x |

表中的“理论vector耗时”表示在当前核数配置下，仅考虑Vector计算本身时的理论执行时间。本样例的性能数据在Atlas A2训练系列产品上运行，该处理器每cycle处理128个half数据，主频为1.85GHz。理论vector耗时的计算公式为
$$
T_{\text{theory}} = \frac{M \times N}{128 \times 1.85 \times 10^9 \times \text{核数}}
$$

例如，48 核场景下：
$$
T_{\text{theory}} = \frac{8192 \times 8192}{128 \times 1.85 \times 10^9 \times 48} = \frac{67108864}{1.13664 \times 10^{13}} \approx 5.904 \times 10^{-6} \text{ s} = 5.904 \text{ μs}
$$

可以看到 Case 6 的 aiv_vec_time 为 6.954 μs，已经很接近 Atlas A2训练系列48核场景下的理论耗时。

Case 0-3 未开启双缓冲，数据搬运串行执行，使用读带宽衡量性能。从 Case 4 开始开启双缓冲，此时 mte2 利用率较高，读写行为并行发生，因此读写混合带宽按总读写数据量除以 $T_{mte2}$ 进行估算。其中，读带宽的计算公式为：
$$
BW_{read} = \frac{D_{read}}{T_{mte2}}
$$

读写混合带宽的计算公式为：
$$
BW_{rw} = \frac{D_{read} + D_{write}}{T_{mte2}}
$$

式中：
- $D_{read} = M \times N \times sizeof(half) \times 2$ 为总读数据量（x和y两个输入矩阵）
- $D_{write} = M \times N \times sizeof(half)$ 为总写数据量（z输出矩阵）
- $T_{mte2}$ 为 aiv_mte2_time（GM→UB搬运耗时，μs）
- 开启双缓冲后，mte2 与 mte3 流水并行；从本样例数据看，Case 4-6 的 mte2 利用率分别为 97.3%、95.1%、96.1%，因此这里采用 $T_{mte2}$ 作为主路径时间进行估算

以 Case 3 为例（$M=N=8192$，$T_{mte2}=184.369\mu s$）：
$$
BW_{read} = \frac{8192 \times 8192 \times 2 \times 2}{184.369 \times 10^{-6}} = \frac{268435456}{184.369 \times 10^{-6}} \approx 1.4560 \times 10^{12} \text{ B/s} \approx 1.4560 \text{ TB/s}
$$

以 Case 6 为例（$T_{mte2}=171.611\mu s$）：
$$
BW_{rw} = \frac{8192 \times 8192 \times (2+1) \times 2}{171.611 \times 10^{-6}} = \frac{402653184}{171.611 \times 10^{-6}} \approx 2.3456 \times 10^{12} \text{ B/s} \approx 2.3456 \text{ TB/s}
$$

之所以读写混合带宽会高于 1.8 TB/s，是因为这里统计的不是纯读带宽，而是“读 + 写”的混合带宽。开启双缓冲后，读写流水是并行的；同时，z 写数据时会命中 L2 Cache，而 L2 的带宽很高，所以写的带宽较高。因此，在以 mte2 作为读写混合时间来估算时，分子统计了读写总数据量，最终得到的混合带宽会高于 1.8 TB/s。

### Ascend 950系列性能对比

下表为本样例在Ascend 950系列产品上运行的性能数据对比：

| Case | 优化策略 | 核数 | dataCopyLen | Task Duration(μs) | aiv_vec_time(μs) | 理论vector耗时(μs) | 读带宽(TB/S) | 读写混合带宽(TB/S) | 端到端耗时相对Case 0 |
|:---:|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 0 | 单核标量（基准） | 1 | 4096 | 1228475.435 | NA | 317.750 | 0.034 | NA | 1x |
| 1 | 单核向量 | 1 | 4096 | 8924.021 | 943.362 | 317.750 | 0.035 | NA | 137.7x |
| 2 | 多核均匀切分 | 64 | 4096 | 310.489 | 15.465 | 4.965 | 1.184 | NA | 3956.6x |
| 3 | 增大搬运粒度 | 64 | 21760 | 251.684 | 11.121 | 4.965 | 1.454 | NA | 4881.0x |
| 4 | 双缓冲 | 64 | 21760 | 247.665 | 10.976 | 4.965 | NA | 1.712 | 4960.2x |
| 5 | L2 Cache bypass | 64 | 21760 | 182.04 | 10.997 | 4.965 | NA | 2.256 | 6748.4x |

Ascend 950系列上 `dataCopyLen` 增大的主要原因是UB容量增大。本样例在双缓冲场景下需要同时放置 `x/y/z` 的 Ping-Pong 共6块缓冲区，因此单块可用空间可近似表示为 `UBSIZE/6`：
- Atlas A2/A3 系列：`192KB / 6 = 32KB`，对应 `32KB / 2B = 16384` 个`half`
- Ascend 950系列：`256KB / 6 ≈ 42.67KB`，对应约 `21840` 个`half`

因此在Ascend 950系列上，按容量上限估算，单块 `dataCopyLen` 可提升至约 `21840`个`half`数据。
在实际实现中，推荐用户将搬运粒度和计算粒度都对齐到512B，以获得更稳定的性能表现，因此case3-5选用 `dataCopyLen=21760`，其对应字节数为`21760 * 2B = 43520B = 85 * 512B`。

在Ascend 950系列产品上BANK的排列不同，本样例暂时不需要考虑Bank冲突，因此只列出了case0～case5的性能数据。该处理器每cycle处理128个half数据，主频为1.65GHz，AIV核数为64，因此理论vector耗时为
$$
T_{\text{theory}} = \frac{8192 \times 8192}{128 \times 1.65 \times 10^9 \times 64} = \frac{67108864}{1.35168 \times 10^{13}} \approx 4.965 \times 10^{-6} \text{ s} = 4.965 \text{ μs}
$$

Ascend 950系列未达到理论vector耗时峰值的原因如下：

- 该理论值仅统计“纯Vector计算”时间，不包含数据搬运、事件同步、流水线调度等开销；而实测Case 5中 `aiv_vec_time=10.997μs`、Task Duration为 `182.04μs`，`aiv_mte2_ratio`高达98.3%，受限于带宽限制，说明端到端瓶颈不在纯计算本身。
- Ascend 950系列 RegBase的主要收益来自“减少冗余Load/Store、寄存器复用、提升无依赖并发指令比例”。当前Add样例核心计算基本是单条Add，计算链较短、可融合/可双发空间有限，难以充分释放RegBase的优势。

### 优化要点总结

| 优化手段 | 核心原理 | 适用场景 |
|:---|:---:|:---|
| 标量→向量 | 向量指令并行处理多个元素 | 所有计算密集型算子 |
| 单核→多核 | 多核并行，负载均衡 | 大数据量场景 |
| 增大搬运粒度 | 减少搬运次数，摊薄启动开销 | 搬运密集型场景 |
| 双缓冲 | 搬运与计算流水线并行 | 计算与搬运时间相近 |
| L2 Cache bypass | 避免Cache污染，减少开销 | 流式访问（只读一次） |
| Bank Conflict优化 | 优化内存布局，避免访存冲突 | Vector-bound场景 |

---

## 编译运行

- 切换Case

  在 cmake 编译时通过 `-DSCENARIO_NUM=N` 指定要编译的 case：

  ```bash
  cmake -DSCENARIO_NUM=6 -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..   # 编译 case 6（可替换为0-6）
  ```

  各 case 说明：
  - `0`: 单核标量版本
  - `1`: 单核向量版本
  - `2`: 多核均匀切分 (dataCopyLen=4096)
  - `3`: 多核均匀切分 (数据大块搬运)
  - `4`: 双缓冲优化
  - `5`: 双缓冲 + L2Cache bypass
  - `6`: 双缓冲 + L2Cache bypass + 避免Bank Conflict

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。
    
- 样例执行

  在本样例目录下执行如下命令。
  ```bash 
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake -DSCENARIO_NUM=6 -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行（使用编译时指定的case）
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU 调试 或 NPU 仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DSCENARIO_NUM=6 -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU调试模式
  cmake -DSCENARIO_NUM=6 -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。
  

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0`（默认）、`1`、`2`、`3`、`4`、`5`、`6` | 性能优化case编号 |
  
  执行结果如下，说明精度对比成功。
  ```bash
  error ratio: 0.0000, tolerance:0.0001
  test pass!
  ```

## 功能调试

### printf

该接口提供CPU域/NPU域调试场景下的格式化输出功能。

在算子kernel侧实现代码中需要输出日志信息的地方调用[printf](../../../../../docs/zh/api/Utils-API/tuning_interface/printf.md)接口打印相关内容。

示例如下：

```cpp
AscendC::printf("add blockIdx=%d\n", AscendC::GetBlockIdx());
```

> **注意：** printf（PRINTF）接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需通过设置`ASCENDC_DUMP=0`的方式关闭打印功能。

### DumpTensor

基于算子工程开发的算子，可以使用该接口Dump指定[LocalTensor](../../../../../docs/zh/api/SIMD-API/basic_api/data_structures/LocalTensor/LocalTensor_intro.md)的内容。同时支持打印自定义的附加信息（仅支持uint32\_t数据类型的信息），比如打印当前行号等。

在算子kernel侧实现代码中需要打印Tensor数据的地方调用[DumpTensor](../../../../../docs/zh/api/SIMD-API/basic_api/debug_interface/onboard_print/DumpTensor.md)接口打印相关内容。样例如下：

```cpp
// 向量计算: z = x + y
AscendC::Add(zLocal, xLocal, yLocal, blockLength);
AscendC::DumpTensor(zLocal, 1, 32);
```

> **注意：** DumpTensor接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需通过设置`ASCENDC_DUMP=0`来关闭打印功能。

## 性能调试

### msOpProf工具介绍
msOpProf工具是单算子性能分析工具。包含msopprof和msopprof simulator两种使用方式。该工具协助用户定位算子内存、算子代码以及算子指令的异常，实现全方位的算子调优。当前支持基于不同运行模式（上板或仿真）和不同文件形式（可执行文件或算子二进制.o文件）进行性能数据的采集和自动解析。

- 上板性能采集

    通过上板性能采集，可以直接测定算子昇腾AI处理器上的运行时间。该方式适合在板环境中快速定位算子性能问题。

    基于可执行文件demo通过msopprof执行算子调优：
    ```
    msopprof ./demo
    ```

    - 性能数据说明  
      命令完成后，会在默认目录下生成以“OPPROF_{timestamp}_XXX”命名的文件夹,性能数据文件夹结构示例如下：

      ```bash
      ├──dump                       # 原始的性能数据，用户无需关注
      ├──ArithmeticUtilization.csv  # cube/vector指令cycle占比
      ├──L2Cache.csv                # L2 Cache命中率，影响MTE2，建议合理规划数据搬运逻辑，增加命中率
      ├──Memory.csv                 # UB，L1和主存储器读写带宽速率
      ├──MemoryL0.csv               # L0A，L0B，和L0C读写带宽速率
      ├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
      ├──OpBasicInfo.csv            # 算子基础信息
      ├──PipeUtilization.csv        # 采集计算单元和搬运单元耗时和占比
      ├──ResourceConflictRatio.csv  # UB上的bank group、bank conflict和资源冲突率在所有指令中的占比
      └──visualize_data.bin         # MindStudio Insight呈现文件
      ```

查看具体的性能分析结果：

```bash
# 查看Task Duration 以及各项数据
cat ./OPPROF_*/PipeUtilization.csv
```
