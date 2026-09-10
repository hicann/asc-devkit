# 线程块数量配置与vf调用优化样例

## 概述

本样例以Gather计算为例，介绍Ascend C SIMD与SIMT混合编程场景下的线程块数量配置和vf函数调用优化思路。样例包含4个Case（16个场景），从空壳核函数调度开销到不同数据量下的线程块数量配置优化。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | > CANN 9.0.0 |

## 目录结构介绍

```text
├── simd_simt_grid_dim_config
│   ├── figures                // README中的图片资源
│   ├── CMakeLists.txt         // 编译工程文件
│   ├── grid_config.asc        // Ascend C算子实现 & 调用样例
│   ├── README.md              // 样例说明文档
│   └── README_en.md           // 英文样例说明文档
```

## 样例描述

- 计算公式：

  $$output[i][j] = input[index[i][j]]$$

  - input为输入张量，数据类型为float，形状为[2200000]
  - index为索引张量，数据类型为uint32，形状因Case不同而异：Case 0/1/2为[1024, 2048]，Case 3为[8, 2048]，索引范围为[0, input_total_length)
  - output为输出张量，数据类型为float，形状与index相同
  - 计算过程：根据index[i][j]索引从input中采集对应位置的数据，写入output[i][j]位置

- 样例规格：

  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Gather</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[2200000]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">index</td><td align="center">[1024,2048]/[8,2048]</td><td align="center">uint32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[1024,2048]/[8,2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">empty_kernel / gather_kernel</td></tr>
  </table>

## 样例实现

### Case实现说明

本样例通过编译宏`SCENARIO_NUM`控制16个测试场景并归为4类Case，从空壳核函数调度开销，再到vf调用次数、大shape和小shape下的配置选择。

| Case   | SCENARIO_NUM           | Shape                      | 核函数                                  | 对比目的                                  |
| ------ | ---------------------- | --------------------------------- | --------------------------------------- | ----------------------------------------- |
| Case 0 | 1-5     | [1024, 2048]           | empty_kernel                            | 线程块数量设置1 / 32 / 64 / 128 / 1024，对比不同线程块数量的调度开销 |
| Case 1 | 6-8              | [1024, 2048]              | empty_kernel                            | 线程块数量设置32 / 64 / 1024，对比1次vf调用和2次vf调用的调度开销        |
| Case 2 | 9-11            | [1024, 2048]      | gather_kernel                      | 大shape下，线程块数量设置1024 / 64 / 32，对比性能差异 |
| Case 3 | 12-16 | [8, 2048] | gather_kernel | 小shape下，线程块数量设置4 / 8 / 16 / 32 / 64，线程数量按每核处理数分配，对比性能差异 |

### 性能指标说明

| 指标                | 说明                                                                                      |
| ------------------- | ----------------------------------------------------------------------------------------- |
| Task Duration(μs)   | Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间                  |

### Case 0: 空核函数调度开销（SCENARIO_NUM=1-5）

**样例目标**：

通过调用空壳核函数，体现不同线程块数量配置下的Task固定启动开销。

**核心实现**：

- 核函数`empty_kernel`仅调用空壳vf函数`simt_empty`，函数体内无实际计算逻辑，用于隔离测试不同线程块数量配置带来的开销
- 通过`SCENARIO_NUM=1~5`控制五种线程块数量配置：1 / 32 / 64 / 128 / 1024，线程数量固定2048，每次调用1次vf

**关键代码**：

```cpp
__simt_vf__ __launch_bounds__(MAX_THREAD_NUM) inline void simt_empty(...)
{}

__global__ __vector__ void empty_kernel(...)
{
    for (uint32_t vf_idx = 0; vf_idx < vf_call_times; vf_idx++) {
        asc_vf_call<simt_empty>(dim3(MAX_THREAD_NUM), ...);
    }
}
```

**性能数据**：

| SCENARIO_NUM | 线程块个数 | Task Duration(μs) |
| :----------: | :-------: | :---------------: |
| 1 | 1 | 1.455 |
| 2 | 32 | 1.589 |
| 3 | 64 | 2.160 |
| 4 | 128 | 3.373 |
| 5 | 1024 | 15.996 |

**分析**：

- 1→32→64→128：Task Duration从1.455μs升到3.373μs，调度开销逐步增长
- 128→1024：Task Duration从3.373μs陡增到15.996μs，调度开销随线程块数量增加而显著增长

**结论**：

调度开销随线程块数量增加而增长，线程块数量越多，Task的固定启动开销越大

---

### Case 1: vf调用次数对调度头开销的影响（SCENARIO_NUM=6-8）

**样例目标**：

在Case 0的基础上，对比1次和2次vf调用对Task Duration的影响，展示vf调用次数和调度头开销的关系。

**核心实现**：

- 与Case 0使用相同的空壳核函数`empty_kernel`，唯一变化是将vf调用次数从1次增加到2次，通过配置vf_call_times来控制vf调用次数
- 通过`SCENARIO_NUM=6-8`分别启动32、64、1024个线程块，每次调用2次vf

**性能数据**：

| SCENARIO_NUM | 线程块个数 | vf调用次数 | Task Duration(μs) |
| :----------: | :-------: | :--------: | :---------------: |
| 6 | 32 | 2 | 1.780 |
| 7 | 64 | 2 | 2.327 |
| 8 | 1024 | 2 | 16.598 |

**与Case 0对比**：

| 对比 | 线程块个数 | vf次数变化 | Task Duration变化 | 增幅 |
| ---- | :-------: | :--------: | :---------------: | :--: |
| SCENARIO_NUM=2→6 | 32 | 1→2 | 1.589 → 1.780 | +12.0% |
| SCENARIO_NUM=3→7 | 64 | 1→2 | 2.160 → 2.327 | +7.7% |
| SCENARIO_NUM=5→8 | 1024 | 1→2 | 15.996 → 16.598 | +3.8% |

性能数据如下图所示：

<img src="figures/fig_case01_scheduling.png" alt="Case 0/1 调度开销：线程块数量与vf调用次数" width="600" />

**分析**：

- 32、64和1024核下，1次→2次vf调用的Task Duration均增加，说明vf调用存在额外调度开销
- 1024核下绝对耗时增加0.602μs，高于32核和64核下的0.191μs和0.167μs

**结论**：

vf调用也存在调度开销，应尽量调用1次vf处理完所有数据

---

### Case 2: 大shape Gather配置不同线程块数量性能对比（SCENARIO_NUM=9-11）

**样例目标**：

对比同一大shape下执行Gather计算，配置不同线程块数量的性能差异。

**核心实现**：

- 三个场景均使用核函数`gather_kernel`，差异在于线程块个数和每个线程处理的数据量
- input形状为[2200000]，index和output形状为[1024, 2048]
- Gather算子计算逻辑较简单，主要受访存带宽影响，属于Memory Bound类型。本样例整体计算量较小，占用寄存器较少，因此每个线程块的线程数可配置为2048
- vf函数`simt_gather`采用grid-stride loop：以`blockIdx.x * blockDim.x + threadIdx.x`为起始索引，以`gridDim.x * blockDim.x`为步长遍历index数组，每个线程以for循环方式处理本block负责的全部元素
- 线程数量通过`resolve_thread_num`按每核实际处理元素数分配：当每核处理元素数≥2048时取2048，否则取实际处理元素数

**关键代码**：

```cpp
// simt_gather: SIMT vf函数 —— grid-stride loop，以总线程数为步长遍历整个index数组
__simt_vf__ __launch_bounds__(MAX_THREAD_NUM) inline void simt_gather(..., uint32_t index_total_length)
{
    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    uint32_t stride = gridDim.x * blockDim.x;
    for (uint32_t i = idx; i < index_total_length; i += stride) {
        uint32_t gather_idx = index[i];
        if (gather_idx >= input_total_length) { gather_idx = 0; }
        output[i] = input[gather_idx];
    }
}

// gather_kernel: 核函数调用simt_gather，线程数量由resolve_thread_num自适应分配
__global__ __vector__ void gather_kernel(..., uint32_t thread_num)
{
    asc_vf_call<simt_gather>(dim3(thread_num), ..., index_total_length);
}
```

#### 逻辑核与物理核

在SIMT核函数启动时，内核调用符<<<>>>中第一个入参配置决定本次任务要启动多少个线程块。对于本样例可以把线程块数量理解为要启动的**逻辑核数量**。

这些逻辑核最终要落到硬件实际存在的Vector Core上执行，因此需要引入两个概念：

- **逻辑核**：启动的线程块数量，是软件侧希望启动的并行任务数
- **物理核**：硬件实际拥有的Vector Core数量，需要通过[aclrtGetDeviceInfo](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta1/API/runtimeapi/aclcppdevg_03_1867.html)在运行期查询获取（本样例环境为64），代码示例如下：
  ```cpp
  // 运行期查询物理 Vector Core(AIV)数量；查询失败时返回 AIV_CORE_NUM
  uint32_t query_aiv_core_num(int32_t device_id)
  {
      int64_t core_num = 0;
      aclError ret = aclrtGetDeviceInfo(
          static_cast<uint32_t>(device_id), ACL_DEV_ATTR_VECTOR_CORE_NUM, &core_num);
      return (ret == ACL_SUCCESS && core_num > 0) ? static_cast<uint32_t>(core_num) : AIV_CORE_NUM;
  }
  ```

两者的关系会影响调度开销：当逻辑核数 ≤ 物理核数时，所有逻辑核可以一次占满物理核并行执行；当逻辑核数 > 物理核数时，超出的逻辑核必须等待前面的逻辑核执行完、让出物理核后才能被调度，等待开销会随超出的逻辑核数量增加而上升。

**性能数据**：

| SCENARIO_NUM | 线程块个数 | 每核处理元素数 | 每线程处理元素数 | Task Duration(μs) |
| :----------: | :-------: | :-------: | :-------: | :---------------: |
| 9 | 1024 | 2048 | 1 | 78.921 |
| **10** | **64** | **32768** | **16** | **59.109** |
| 11 | 32 | 65536 | 32 | 110.540 |

性能数据如下图所示：

<img src="figures/fig_case2_large.png" alt="Case 2 Task Duration 随线程块数量变化" width="600" />

**分析**：

- **线程块数量为64相比线程块数量为1024，Task Duration从78.921μs降低到59.109μs，耗时下降约25.1%，性能提升约34%**。线程块数量从1024降到64，启动和调度的线程块数量显著减少，调度开销大幅降低
- **线程块数量为64相比线程块数量为32，Task Duration从110.540μs降低到59.109μs，耗时下降约46.5%，性能提升约87%**。同样的grid-stride loop写法下，线程块数量为64时具备更高的并行度；虽然线程块数量增加会带来更多启动和调度开销，但这部分开销小于并行度提升带来的收益，因此整体耗时更低
- 受硬件资源限制，实际可并行执行的物理核数量存在上限。由于一个物理核同一时刻只能驻留并执行一个线程块，当线程块数量为1024时，超出物理核数量的线程块需要等待前序线程块执行完成后再调度，额外的启动和调度固定开销会明显增加；当线程块数量为32时，只使用了一半物理核，空闲物理核较多，并行度不足，单线程块工作量翻倍

**结论**：

大shape下应优先让线程块数量匹配实际物理核数。本样例中线程块数量为64时，既避免了线程块数量为1024时带来的大量线程块调度开销，又避免了线程块数量为32时并行度不足，因此是Case 2中的最优配置

---

### Case 3: 小shape Gather配置不同线程块数量与线程数量性能对比（SCENARIO_NUM=12-16）

**样例目标**：

对比同一小shape下执行Gather计算，配置不同线程块数量与线程数量的性能差异。

**核心实现**：

- 五个场景均使用核函数`gather_kernel`
- input形状为[2200000]，index和output形状为[8, 2048]
- 线程数量通过`resolve_thread_num`按每核实际处理元素数分配：每核处理元素数小于2048时降低线程数量，消除空转线程

**性能数据**：

| SCENARIO_NUM | 线程块个数 | 线程数量 | 每核处理元素数 | 每线程处理元素数 | Task Duration(μs) |
| :----------: | :-------: | :-------: | :-------: | :-------: | :---------------: |
| 12 | 4 | 2048 | 4096 | 2 | 10.760 |
| 13 | 8 | 2048 | 2048 | 1 | 6.741 |
| 14 | 16 | 1024 | 1024 | 1 | 4.514 |
| **15** | **32** | **512** | **512** | **1** | **3.866** |
| 16 | 64 | 256 | 256 | 1 | 4.115 |

性能数据如下图所示：

<img src="figures/fig_case3_small_griddim.png" alt="Case 3 Task Duration 随线程块数量变化" width="600" />

**分析**：

- 折线呈"先降后升"的谷形，**最优线程块数量为32（物理核数的1/2）**，并非打满64核
- 线程块数量从4依次增加到8、16、32时，Task Duration持续下降（10.760 → 6.741 → 4.514 → 3.866），并行收益持续提高
- 32→64（物理核）：线程块数量翻倍，Task Duration从3.866μs升到4.115μs，**耗时增加约6.4%**。小shape下每核任务量较小（256/512元素），增加线程块数量带来的调度开销反超并行收益
- 与Case 2对照：同样的kernel，shape小到一定程度后最优线程块数量从64降到32，说明"最优线程块数量"随shape缩小而变化，必须实测

**结论**：

小shape无法直接沿用"使用全部物理核"的经验，必须参考本Case的方式设置多档线程块数量实测

---

## 性能对比总结

**调度开销随线程块数量变化（Case 0）**：

| SCENARIO_NUM | 线程块个数 | Task Duration(μs) |
| ------------ | --------- | ----------------- |
| 1 | 1 | 1.455 |
| 2 | 32 | 1.589 |
| 3 | 64 | 2.160 |
| 4 | 128 | 3.373 |
| 5 | 1024 | 15.996 |

- 调度开销随线程块数量增加而增长

**vf调用次数对头开销的影响（Case 1）**：

| 对比 | 线程块个数 | vf次数 | Task Duration(μs) | 增幅 |
| ---- | --------- | ------ | ----------------- | ---- |
| SCENARIO_NUM=2→6 | 32 | 1→2 | 1.589 → 1.780 | +12.0% |
| SCENARIO_NUM=3→7 | 64 | 1→2 | 2.160 → 2.327 | +7.7% |
| SCENARIO_NUM=5→8 | 1024 | 1→2 | 15.996 → 16.598 | +3.8% |

- vf调用也存在调度开销

**大shape的最优线程块数量（Case 2）**：

| SCENARIO_NUM | 线程块个数 | 每核处理元素数 | Task Duration(μs) |
| :----------: | :-------: | :-------: | :---------------: |
| 9 | 1024 | 2048 | 78.921 |
| **10** | **64** | **32768** | **59.109** |
| 11 | 32 | 65536 | 110.540 |

- 最优线程块数量为 **64**（等于物理核数）；线程块数量为64时，既避免了线程块数量为1024时带来的大量线程块调度开销，又避免了线程块数量为32时并行度不足

**小shape的最优线程块数量（Case 3）**：

| SCENARIO_NUM | 线程块个数 | 线程数量 | 每核处理元素数 | Task Duration(μs) |
| :----------: | :-------: | :-------: | :-------: | :---------------: |
| 12 | 4 | 2048 | 4096 | 10.760 |
| 13 | 8 | 2048 | 2048 | 6.741 |
| 14 | 16 | 1024 | 1024 | 4.514 |
| **15** | **32** | **512** | **512** | **3.866** |
| 16 | 64 | 256 | 256 | 4.115 |

- 最优线程块数量为 **32**（物理核数的1/2），不是打满的64核；线程块数量从32增加到64后耗时回升

## 调优建议

1. **大shape需优先匹配物理核数**：受硬件资源限制，实际可并行执行的物理核数量存在上限。线程块数量应尽量等于物理核数，避免线程块数量过多（排队等待、调度开销累积）或过少（并行度不足、单线程块工作量重）。
2. **小shape需实测线程块数量性能**：小shape下每核工作量较小，启动更多线程块带来的调度头开销会反超收益，最优线程块数量不一定是最大物理核数。可参考Case 3通过分档测试寻找最优线程块数量。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  SCENARIO_NUM=1                       # 选择执行场景，可选1-16
  mkdir -p build && cd build;          # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # 编译工程
  ./grid_config                        # 执行样例
  ```

- 编译选项说明

  | 选项             | 可选值      | 说明              |
  | ---------------- | ----------- | ----------------- |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |
  | `SCENARIO_NUM` | `1`-`16` | 样例类型，默认为1 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```text
  [Success] Case accuracy is verification passed.
  ```

## 性能调试

### msOpProf工具介绍

msOpProf工具是单算子性能分析工具。包含msopprof和msopprof simulator两种使用方式。该工具协助用户定位算子内存、算子代码以及算子指令的异常，实现全方位的算子调优。当前支持基于不同运行模式（上板或仿真）和不同文件形式（可执行文件或算子二进制.o文件）进行性能数据的采集和自动解析。

通过上板性能采集，可以直接测定算子在昇腾AI处理器上的运行时间。该方式适合在板环境中快速定位算子性能问题。

使用`msOpProf`工具获取详细性能数据：

```bash
msopprof ./grid_config
```

命令完成后，会在默认目录下生成以“OPPROF_{timestamp}_XXX”命名的性能数据文件夹，文件夹结构示例如下：

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
