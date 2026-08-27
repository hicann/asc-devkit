# DataCacheHint性能调优样例

## 概述

本样例以查表法计算sin值为例，介绍Ascend C SIMT编程方式下的数据缓存优化思路。样例包含1个基线版本以及1个优化版本，基线版本中所有数据使用默认方式加载，输入、输出和sin表数据共同竞争DCache空间；优化版本为不同类型的数据指定不同的缓存策略，确保频繁访问的sin查找表常驻DCache，减少Global Memory访问次数，展示SIMT编程方式下数据缓存优化的调优路径。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.1.0

## 目录结构介绍

```text
├── cache_hint
│   ├── CMakeLists.txt              // cmake编译文件
│   ├── data_cache_hint.asc         // SIMT数据缓存样例实现
│   ├── figures                     // README中的图片资源
│   └── README.md
```

## 样例描述

- 样例功能

  使用查表法计算sin值，通过线性插值提高精度。

- 样例规格：

  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">SinTableLookup</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[65536]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">sin_table</td><td align="center">[8192]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[65536]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">sin_table_lookup_baseline / sin_table_lookup_optimized</td></tr>
  </table>

## 样例实现

### 查表法计算说明

本样例使用查表法计算sin值，核心思路是预先将[0, π]区间的sin值存入查找表，通过线性插值获取任意角度的sin近似值。计算流程如下：

1. 将输入角度归约到[0, π]区间，处理负数和周期性
2. 计算查找表索引：`index_float = x / π * table_length`
3. 获取`index_float`的整数部分`n`和小数部分`frac`
4. 从查找表读取`table[n]`和`table[n+1]`
5. 线性插值得到结果：`y = table[n] + frac * (table[n+1] - table[n])`

### 样例实现说明

本样例通过2个独立的kernel实现不同的数据缓存策略，每个kernel对应特定的Case版本。

| Case   | 实现特点                                                                 | 使用的核函数                     | 优化特性                |
|--------|----------------------------------------------------------------------|----------------------------|---------------------|
| Case 0 | 所有数据使用默认方式加载，输入、输出和sin表数据共同竞争DCache空间                                | sin_table_lookup_baseline  | 基线版本，无缓存优化          |
| Case 1 | 输入数据`x`直接从GM加载，查找表`sin_table`为热点数据，优先从DCache加载，减少对GM的访问，输出数据直接写出到GM | sin_table_lookup_optimized | 数据缓存优化，sin表常驻DCache |

#### 1. 数据加载特点

在样例中，为了体现性能差异，构造DCache空间大小为32KB的场景。默认的数据加载会优先从DCache读取，若DCache中数据不存在，则从Global Memory（GM）加载并同时缓存到DCache中。本样例中三类数据的访问特征如下：

| 数据        | 大小    | 访问特征              |
|-----------|-------|-------------------|
| input     | 256KB | 每个元素仅读取一次         |
| sin_table | 32KB  | 数据被反复访问，属于热点数据    |
| output    | 256KB | 每个元素仅写入一次，写入后不再读取 |

默认情况下，加载的数据会在DCache中保存。在计算过程中，部分热点数据会被多个线程多次使用，但另一部分数据在计算的过程中仅使用一次，这类数据不需要进行缓存，不应让这类数据与热点数据竞争DCache空间

- **`asc_ldcg()`**：加载数据时从Global Memory加载，适用于仅遍历一次的输入数据，减少其对DCache空间的占用
- **`asc_ldca()`**：加载数据时优先从DCache加载，适用于需要频繁访问的热点数据（如查找表），确保热点数据常驻DCache，减少从Global Memory重新加载的次数
- **`asc_stcg()`**：存储数据时直接写入Global Memory空间，适用于数据写到GM后不会再被使用，不需要使用Cache缓存，避免输出数据占用DCache空间影响热点数据的缓存
- input数据通过`asc_ldcg()`直接从GM加载，不缓存到DCache
- sin_table数据通过`asc_ldca()`优先从DCache加载，确保常驻DCache
- output数据通过`asc_stcg()`直接写入GM，不经过DCache缓存

由于sin_table大小为32KB，可以缓存到DCache后不会被挤出，热点数据持续驻留DCache，可减少sin_table数据从GM→DCache的反复加载。

---

### 性能指标说明

| 指标                  | 说明                          |
|---------------------|-----------------------------|
| Task Duration(μs)   | 整个任务执行的总时间，算子执行时间以该参数为准     |
| DCache Read GM      | DCache从Global Memory读取数据的次数 |
| DCache Read Vector  | Vector Core从DCache读取数据的次数   |
| DCache Write Vector | Vector Core向DCache写入数据的次数   |

---

### Case 0: 基线版本

**样例目标**：建立查表法计算sin值的耗时基线，观察输入、输出和sin表数据共同竞争DCache空间对性能的影响

**核心实现**：
- 直接读取`input[idx]`和`sin_table[n]` / `sin_table[n+1]`的数据
- 使用线性插值进行计算得到数据结果
- 线程直接写回`output[idx]`
- 所有数据访问使用默认方式加载，`input`、`sin_table`和`output`数据共同竞争DCache空间

当访问大量输入和输出数据时，sin查找表的数据会被换出DCache，导致后续查表时需要重新从GM加载，增加访存延迟。

**关键代码**：

```cpp
float x = input[idx];
float index_float = x * static_cast<float>(table_length) / PI;
uint32_t n = static_cast<uint32_t>(floorf(index_float));
float frac = index_float - static_cast<float>(n);
float low_val = sin_table[n];
float high_val = 0.0f;
if (n + 1 >= table_length) {
    high_val = sin_table[0];
} else {
    high_val = sin_table[n + 1];
}
output[idx] = sign * (low_val + frac * (high_val - low_val));
```

**性能数据**：

| Task Duration(μs) | DCache Read GM | DCache Read Vector | DCache Write Vector |
|:-----------------:|:-----------------:|:---------------------:|:----------------------:|
|       56.82       |       5064        |         2048          |          6144          |

**分析**：

Case 0的Task Duration为56.82μs，DCache Read GM为5064次，作为基线版本。

Case 0的场景中，加载input，output时会把数据缓存在DCache上，替换DCache中某个缓存数据，假设sin_table中索引2对应的数据被替换掉了，当后续某个线程计算时需要sin_table中索引2对应数据时，就会出现cache miss，需要从GM重新加载。此外，被缓存到DCache上的input和output数据实际上不会再被使用，不需要缓存，不应该占用DCache空间。

<img src="./figures/DCache默认加载.png" width="80%">

优化方向：加载input，output时不需要缓存在DCache上，保证table数据常驻DCache，后续在使用sin_table中索引2对应数据时，可以直接从DCache加载，不需要从GM重新加载数据。

---

### Case 1: 数据缓存优化版本

**优化目标**：通过访存函数区分不同数据的缓存策略，确保sin查找表常驻DCache，减少GM访问次数，降低端到端耗时

**核心优化**：
- 使用`asc_ldcg()`读取数据：每个输入数据仅访问一次，不需要使用DCache
- 使用`asc_ldca()`读取sin表数据：sin表是热点数据，需要频繁访问，确保数据常驻DCache
- 使用`asc_stcg()`写出数据：输出数据写到GM后不会再被使用，不需要使用DCache

**关键代码**：

```cpp
float x = asc_ldcg(&input[idx]);
float index_float = x * static_cast<float>(table_length) / PI;
uint32_t n = static_cast<uint32_t>(floorf(index_float));
float frac = index_float - static_cast<float>(n);
float low_val = asc_ldca(&sin_table[n]);
float high_val = 0.0f;
if (n + 1 >= table_length) {
    high_val = asc_ldca(&sin_table[0]);
} else {
    high_val = asc_ldca(&sin_table[n + 1]);
}
float y = sign * (low_val + frac * (high_val - low_val));
asc_stcg(&output[idx], y);
```

**性能数据**：

| Task Duration(μs) | DCache Read GM | DCache Read Vector | DCache Write Vector |
|:-----------------:|:-----------------:|:---------------------:|:----------------------:|
|      50.895       |       3531        |         2048          |          6144          |

**分析**：
- 相比Case 0基线版本，Task Duration从56.82μs降低到50.895μs，耗时下降约10.4%
- DCache Read GM从5064次降至3531次，减少约30.2%（减少1533次Global Memory访问），这是性能提升的关键因素
- DCache Read Vector和DCache Write Vector保持不变，说明优化没有增加额外开销
- sin表数据常驻DCache后，避免被输入/输出数据挤出，减少了从Global Memory重新加载sin表的次数
---

## 性能对比总结

### Ascend 950PR性能数据

**综合优化效果**：
- 从Case 0基线版本到Case 1优化版本，Task Duration从56.82μs降低到50.895μs，耗时下降约10.4%
- DCache Read GM从5064次降至3531次，减少约30.2%，减少1533次Global Memory访问

| Case version | Task Duration(μs) | Task Duration相对Case 0 | 优化点                   |
|--------------|-------------------|-----------------------|-----------------------|
| Case 0       | 56.82             | **1x**                | 基线版本，所有数据共同竞争DCache空间 |
| Case 1       | 50.895            | **0.90x耗时**           | 数据缓存优化，sin表常驻DCache空间 |

## 调优建议

1. **识别热点数据**：在查表类算子中，查找表是频繁访问的热点数据，应优先保证其常驻DCache。
2. **区分数据访问模式**：对于仅访问一次的数据（如输入数据），使用`asc_ldcg()`避免其对DCache空间的占用；对于需要频繁访问的数据（如查找表），使用`asc_ldca()`优先分配Cache空间。
3. **避免写出数据占用Cache**：对于写入后不再读取的输出数据，使用`asc_stcg()`避免其占用DCache空间。
4. **关注DCache Read GM指标**：DCache Read GM次数的减少直接反映了Cache命中率提升和Global Memory访问延迟降低的效果。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  SCENARIO_NUM=0                       # 选择执行场景，可选0-1
  mkdir -p build && cd build;          # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # 编译工程
  ./data_cache_hint                    # 执行样例
  ```

- 编译选项说明

  | 选项                        | 可选值        | 说明                                                |
  |---------------------------|------------|---------------------------------------------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |
  | `SCENARIO_NUM`            | `0`-`1`    | 样例类型，默认为0，0:基线版本，1：数据缓存优化版本                 |

  执行结果如下，说明精度对比成功。

  ```text
  [Success] Case accuracy is verification passed.
  ```

## 性能分析

使用 `msOpProf` 工具获取详细性能数据：

```bash
msopprof ./data_cache_hint   # 分析case的性能
```

命令完成后，会在默认目录下生成以"OPPROF_{timestamp}_XXX"命名的文件夹,性能数据文件夹结构示例如下：

```text
├──dump                       # 原始的性能数据，用户无需关注
├──ArithmeticUtilization.csv  # cube/vector指令cycle占比
├──L2Cache.csv                # L2 Cache命中率
├──Memory.csv                 # UB，L1和主存储器读写带宽速率
├──MemoryL0.csv               # L0A，L0B，和L0C读写带宽速率
├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
├──OpBasicInfo.csv            # 算子基础信息
├──PipeUtilization.csv        # 采集计算单元和搬运单元耗时和占比
├──ResourceConflictRatio.csv  # UB上的 bank group、bank conflict和资源冲突率在所有指令中的占比
└──visualize_data.bin         # MindStudio Insight呈现文件
```
