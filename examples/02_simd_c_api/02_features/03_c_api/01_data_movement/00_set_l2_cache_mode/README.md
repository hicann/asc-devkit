# L2 Cache Mode 最佳实践样例

## 概述

MTE2将数据从Global Memory（GM）搬运到Unified Buffer（UB）时，可通过`asc_copy_gm2ub_align`接口的`l2_cache_mode`参数（本样例使用[基于指针的C语言编程](../../../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simd_programming/c_pointer_programming)）显式配置数据在L2 Cache中的管理策略。本样例说明**复用数据**和**流式数据**两大类场景下，如何选择合适的L2 Cache模式来优化MTE2搬运性能，以及在启用L2 Cache的前提下如何通过分片策略提升L2 Cache命中率。

- **复用数据场景（数据需多次读取）**
  Case1: 整块重复搬4次，`l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM` → 整块数据远超L2容量，命中率极低，展示未分片时的性能瓶颈。
  Case2: N方向切4份，每份连续搬4次，`l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM` → 分片后单份工作集降至L2容量以内，命中率提升。

- **流式数据场景（数据只读一次）**
  Case3: Add + 双缓冲，`l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`（基准）。
  Case4: Add + 双缓冲，`l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP`跳过L2 Cache → 与Case3对比。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构介绍

```text
├── set_l2_cache_mode
│   ├── scripts
│   │   └── gen_data.py              // 输入数据和真值数据生成脚本文件
│   ├── CMakeLists.txt               // 编译工程文件
│   ├── set_l2_cache_mode.asc        // 样例入口（kernel 调用 + main 函数）
│   ├── set_l2_cache_mode.h          // Kernel 实现（DataCopyRepeat + Add）
│   ├── data_utils.h                 // 数据读入写出函数
│   ├── README.md                    // 样例说明文档
│   └── README_en.md                 // 样例说明文档（英文）
```

## 样例描述

C-API的GM→UB搬运接口[asc_copy_gm2ub_align](../../../../../../docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md)通过入参`l2_cache_mode`来控制本次搬运数据在L2 Cache中的管理策略，取值及含义详见枚举类[asc_load_l2_cache_mode](../../../../../../docs/zh/api/SIMD-API/c_api/)的说明。

围绕上述两类场景，本样例设计了4个Case进行对比验证：

- **数据复用场景（Case1-2）**
本组场景输入为half类型二维矩阵，shape为[12288, 12288]。使用`asc_copy_gm2ub_align`将数据从GM搬运到UB，通过对比不同搬运策略下的L2 Cache命中率，观察整块重复vs分片重复的性能差异。本组场景不包含计算逻辑，仅执行数据搬运操作。

- **流式数据场景（Case3-4）**
本组场景实现两个shape为[8192, 8192]的half类型矩阵相加（z = x + y），采用双缓冲（Ping-Pong）技术实现数据搬运与向量计算的流水线并行。通过对比`l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM` vs `l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP`，展示bypass对流式数据的优化效果。

## 样例实现

### 性能指标说明

本章节性能数据在Ascend 950系列产品上采集，将Ascend 950PR/Ascend 950DT简称为Ascend 950系列。

采用不同的性能采集指令，分别获取不同的性能指标：

- `msopprof ./demo` — 采集AI Core指令级耗时及占比（MTE2/MTE3/Vector），对应**表1**。
- `msopprof --ai-core=on --aic-metrics=L2Cache ./demo` — 采集L2 Cache读写命中/缺失计数，对应**表2**。

**表1** AI Core性能指标字段说明表

| 字段名 | 字段含义 |
|:---:|:---|
| Task Duration(μs) | 整个任务执行的总时间，算子执行时间以该参数为准。|
| aiv_total_cycles | Task 执行总 cycle 数。|
| aiv_mte2_time(μs) | MTE2 类型指令（GM → AI Core 搬运类指令）耗时，单位为 μs。|
| aiv_mte2_ratio | MTE2 类型指令的 cycle 数在 total cycle 数中的占比。|
| aiv_mte3_time(μs) | MTE3 类型指令（UB → GM 搬运类指令）耗时，单位为 μs。|
| aiv_mte3_ratio | MTE3 类型指令的 cycle 数在 total cycle 数中的占比。|
| aiv_vec_time(μs) | vec 类型指令（向量类运算指令）耗时，单位 μs。|
| aiv_vec_ratio | vec 类型指令的 cycle 数在 total cycle 数中的占用比。|

**表2** L2 Cache性能指标字段说明表

| 字段名 | 字段含义 |
|:---:|:---|
| Task Duration(μs) | 整个任务执行的总时间。|
| aiv_total_cycles | Task 执行总 cycle 数。|
| aiv_write_cache_hit | 写 Cache 命中的次数。|
| aiv_write_cache_miss_allocate | 写 Cache 缺失后重新分配缓存的次数。|
| aiv_read_local_l2_hit | 读 Cache 命中的次数。|
| aiv_read_local_l2_miss | 读 Cache 缺失次数。|
| aiv_read_local_l2_victim | 读 Cache 未命中并触发 Cache 中数据被换出的次数。|

L2 Cache命中率 = `aiv_read_local_l2_hit / (aiv_read_local_l2_hit + aiv_read_local_l2_miss + aiv_read_local_l2_victim)`

### 数据复用模式图解

Case1和Case2中数据在GM与UB之间的搬运模式如下：

**Case1：整块矩阵连续重复搬运4次**

```text
GM 矩阵: [M, N]
┌─────────────────────────────── N ───────────────────────────────┐
│                          全部列一次搬完                          │
└─────────────────────────────────────────────────────────────────┘

启动全部 core，按相同路径搬运完整矩阵:
第 1 轮: 全部 core 从 GM 读取完整矩阵 -> UB
第 2 轮: 全部 core 再次读取完整矩阵 -> UB
第 3 轮: 全部 core 再次读取完整矩阵 -> UB
第 4 轮: 全部 core 再次读取完整矩阵 -> UB
说明: 每轮工作集都是整块矩阵，L2 Cache 难以完整保留上一轮数据
```

**Case2：N方向切4份后，每份连续重复搬运4次**

```text
GM 矩阵: [M, N]
┌──────── N/4 ────────┬──────── N/4 ────────┬──────── N/4 ────────┬──────── N/4 ────────┐
│       分片0         │       分片1         │       分片2         │       分片3          │
└─────────────────────┴─────────────────────┴─────────────────────┴─────────────────────┘

启动全部 core，先把分片0的数据连续搬运 4 轮，再处理下一个分片:
分片0: 第 1 轮从 GM 读取，第 2~4 轮优先从 L2 Cache 读取
分片1: 第 1 轮从 GM 读取，第 2~4 轮优先从 L2 Cache 读取
分片2: 第 1 轮从 GM 读取，第 2~4 轮优先从 L2 Cache 读取
分片3: 第 1 轮从 GM 读取，第 2~4 轮优先从 L2 Cache 读取
说明: 单个分片工作集更小，连续重复访问时更容易保留在 L2 Cache 中
```

### Case1: 整块连续重复搬运4次 + `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`

**设计意图**：整块矩阵（301.99MB）大于L2 Cache容量（约128MB），每次完整矩阵搬运都会导致旧数据被换出，L2命中率较低。与Case2（分片重复）对比，说明分片策略控制工作集大小的重要性——即使启用L2 Cache，如果单次工作集超出L2容量，也必须通过分片策略来控制工作集大小。

**样例配置**：

- 整块矩阵沿相同路径连续重复搬运4次。
- 总搬运数据量：301.99MB × 4 = 1207.96MB。
- Tile：[64, 1024]，单次`asc_copy_gm2ub_align`搬运131,072B。

**L2策略**：`l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`

**性能数据**：

| 架构 | Scenario | 配置 | Task Duration(μs) | aiv_total_cycles | aiv_time(μs) | aiv_read_local_l2_hit | aiv_read_local_l2_miss | aiv_read_local_l2_victim | 说明 | L2Cache命中率 |
|:---|:---|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---|:---:|
| Ascend 950系列 | Case1 | Tensor=[12288,12288]<br>Tile=[64,1024]<br>Block Num=64 | 690.38 | 72348218 | 689.48 | 731534 | 523177 | 7672752 | 整块矩阵按相同路径连续搬4次 | 8.2% |

**优化效果分析**：
- L2 Cache命中率仅为**8.2%**，绝大多数重复访问仍需从GM读取。
- 整块矩阵（302MB）远超L2 Cache容量（约128MB），启用L2 Cache收益极低。
- victim计数高达7,672,752，说明上一轮缓存数据几乎全部被换出。

**原理说明**：
- 每轮搬运完整矩阵时，上一轮缓存在L2中的数据几乎全部被换出（victim计数高达7,672,752），下一轮重复访问时仍需从GM重新读取。
- 每次miss都会触发Cache Line分配和旧数据换出操作，这些无效的缓存管理额外消耗了L2控制器管理带宽，降低了MTE2的搬运效率。

**性能优化建议**：
- 当单次工作集远超L2容量时，仅启用L2 Cache无法获得复用收益，应通过分片策略将单次工作集控制在L2容量以内。

### Case2: N方向切4份 + 每份连续搬4次 + `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`

**设计意图**：对于需要多次重复读取的数据，启用L2 Cache将首次读取的数据缓存起来，后续重复访问直接从L2读取，显著降低MTE2搬运耗时。输入矩阵301.99MB > L2容量（约128MB），整块无法被完整缓存，因此N方向切4份后每份约75.50MB，每个分片可在一定范围内获得L2命中率提升。

**样例配置**：

- N方向切4份，每个分片内连续重复搬运4次。
- 总搬运数据量：301.99MB × 4 = 1207.96MB。
- Tile：[64, 1024]，单次`asc_copy_gm2ub_align`搬运131,072B。

**L2策略**：`l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`

**性能数据**：

| 架构 | Scenario | 配置 | Task Duration(μs) | aiv_total_cycles | aiv_time(μs) | aiv_read_local_l2_hit | aiv_read_local_l2_miss | aiv_read_local_l2_victim | 说明 | L2Cache命中率 |
|:---|:---|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---|:---:|
| Ascend 950系列 | Case2 | Tensor=[12288,12288]<br>Tile=[64,1024]<br>Block Num=64 | 350.95 | 36233475 | 350.03 | 6668774 | 528676 | 1720984 | N方向切4份，每份连续搬4次 | 74.8% |

**优化效果分析**：
- L2 Cache命中率从**8.2%**（Case1）提升至**74.8%**，接近理论极限75%（4轮搬运中，第1轮必miss，第2~4轮理想情况下全部命中）。
- Task Duration从690.38μs降低至350.95μs，减少**49.2%**。
- victim计数从7,672,752降至1,720,984，缓存颠簸显著减轻。

**原理说明**：
- 整块矩阵（302MB）远超L2容量（128MB），无法被完整缓存。N方向切4份后，单分片约75.5MB，可在L2容量范围内被有效缓存。
- 每个分片内连续重复访问时，第2~4轮的数据直接从L2命中，大幅减少GM读取次数。

**性能优化建议**：
- 对于超L2容量的重复访问数据，应先通过分片将单次工作集控制在L2容量以内，再启用L2 Cache获得复用收益。

### Case3: Add双缓冲 + `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`

**设计意图**：Add算子的输入x、y均为流式数据（每个元素只读一次），采用双缓冲流水线并行。`l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`时，MTE2将数据搬运到UB的同时会写入L2 Cache，但数据不会被再次访问，写入L2的操作是纯浪费的，额外消耗了L2控制器管理带宽。

**样例配置**：

- 矩阵：[8192, 8192]，half类型（128MB每矩阵）。
- 均匀切分到64个AIV Core。

**L2策略**：`l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 240.01 | 239.13 | 83.14 | 0.348 | 1.815 | 0.008 | 233.869 | 0.978 | 121.44 | 0.507 |

**优化效果分析**：
- MTE2搬运数据的同时写入L2 Cache，浪费了L2控制器管理带宽（aiv_mte2_time达到**233.869μs**，占比**97.8%**）。

**原理说明**：
- Add算子的输入x、y均为流式数据，每个元素仅被访问一次后不再复用。
- `l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`会使MTE2在GM→UB搬运时额外执行L2 Cache写入操作，这些写入对性能无帮助，反而占用L2控制器管理带宽并污染L2 Cache空间。

**性能优化建议**：

- 对于流式访问的数据，应配置`l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP`跳过L2 Cache写入，省去不必要的cache管理开销。

### Case4: Add双缓冲 + `l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP`

**设计意图**：与Case3的区别仅在于`asc_copy_gm2ub_align`调用时传入`l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP`代替`l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM`。对于只读一次的流式数据，禁用L2 Cache后MTE2直接将数据从GM搬运到UB，避免了不必要的cache写开销，并可避免流式数据占据L2空间。

**关键代码**：

```cpp
// Case 3（l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM）和 Case 4（l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP）
// 唯一区别在于 l2_cache_mode 参数值
__aicore__ inline void ProcessDoubleBufferImpl(asc_load_l2_cache_mode l2CacheMode)
{
    // ...
    asc_copy_gm2ub_align(xLocal, xGm + startElement,
        1, (uint32_t)(curLen * sizeof(half)),
        0, 0, 0, l2CacheMode, 0, 0);
    asc_copy_gm2ub_align(yLocal, yGm + startElement,
        1, (uint32_t)(curLen * sizeof(half)),
        0, 0, 0, l2CacheMode, 0, 0);
    // ...
}
```

**L2策略**：`l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP`

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 172.996 | 171.81 | 82.462 | 0.48 | 1.801 | 0.01 | 166.752 | 0.971 | 18.723 | 0.109 |

**优化效果分析**：
- Task Duration从240.01μs降低至172.996μs，减少**27.9%**。
- MTE2耗时从233.869μs降低至166.752μs，减少**28.7%**。
- MTE3耗时从121.44μs降低至18.723μs，减少**84.6%**。
- aiv_vec_time基本不变（83.14μs → 82.462μs，变化**0.8%**），说明L2 Cache bypass不影响向量计算部分的耗时。

**原理说明**：

- `l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP`跳过L2 Cache写入，MTE2直接将数据从GM搬运到UB，省去了不必要的cache写操作和Cache Line分配开销。

**性能优化建议**：

- 对于数据量大且只读取一次的流式数据（如Add、Mul等逐元素算子的输入），建议在`asc_copy_gm2ub_align`中配置`l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP`，跳过不必要的cache写入，提升MTE2搬运效率。

## 优化要点总结

| 优化手段 | 核心原理 | 适用场景 |
|:---|:---|:---|
| L2 Cache bypass | 避免Cache污染，减少开销 | 流式访问（只读一次） |
| 启用L2 Cache | 利用L2 Cache将首次读取的数据缓存起来，后续重复访问直接从L2读取（访问L2 Cache带宽相较直接访问GM更大）| 重复读取的数据 |
| 分片后重复访问 | 将重复访问限制在更小的数据范围内，提高L2Cache命中机会 | 复用的数据大小超过L2Cache容量 |

**L2 Cache Mode决策树**：

```text
该数据会被多次读取吗？
  ├── 是 → 使用 l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM
  │         单次工作集 > L2 容量？
  │         └── 是 → 先分片，再在每个分片内使用 l2_cache_mode = asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM
  └── 否 → 使用 l2_cache_mode = asc_load_l2_cache_mode::NOTALLOC_KEEP（流式数据 bypass）
```

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为 CANN 包安装目录。

- 样例执行

  ```bash
  SCENARIO_NUM=1 ASC_ARCH=dav-3510
  mkdir -p build && cd build
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=$ASC_ARCH ..
  make -j
  python3 ../scripts/gen_data.py -scenarioNum $SCENARIO_NUM
  ./demo
  ```

- 编译选项说明

  | 参数 | 说明 | 可选值 | 默认值 |
  |------|------|--------|--------|
  | `SCENARIO_NUM` | 场景编号 | 1-4 | 1 |
  | `CMAKE_ASC_RUN_MODE` | 运行模式 | `npu`、`sim` | `npu` |
  | `CMAKE_ASC_ARCHITECTURES` | NPU 硬件架构 | `dav-3510`（仅该架构支持） | `dav-3510` |

- 性能采集

  使用`msopprof`工具获取详细性能数据：

  ```bash
  msopprof ./demo                                              # 采集 MTE2/MTE3/Vector 性能数据，字段含义见表1。
  msopprof --ai-core=on --aic-metrics=L2Cache ./demo           # 采集 L2 Cache 性能数据，字段含义见表2。
  ```

  采集后会在默认目录下生成以“OPPROF_{timestamp}_XXX”命名的文件夹,性能数据文件夹结构示例如下。

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
  # 查看 Task Duration 以及各项数据
  cat ./PROF_*/PipeUtilization.csv
  ```
