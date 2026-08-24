# 融合向量计算Double Buffer性能样例

## 概述

本样例使用Ascend C C API实现half融合向量计算，并对比单Buffer和Ping/Pong Double Buffer两种实现。通过编译参数调整寄存器内的融合计算量，在不增加GM搬运量的情况下观察Vector时间变化，以及不同计算强度下Double Buffer的性能收益。

## 支持的产品及CANN软件版本

**表1**  支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构

```text
03_add_double_buffer
├── figures
│   └── double_buffer.png    # Ping/Pong Buffer流水示意图
├── scripts
│   ├── gen_data.py          # 生成输入数据和真值数据
│   └── verify_result.py     # 校验输出结果
├── CMakeLists.txt           # 编译工程文件
├── add_double_buffer.asc    # Kernel及Host侧实现
├── data_utils.h             # 二进制数据读写工具
├── README.md
└── README_en.md
```

## 样例描述

样例对两个形状为`[8192, 8192]`的half矩阵执行逐元素融合计算，计算公式为：

$$
z = x^{R+1} + y \sum_{i=0}^{R} x^i, \quad R = \text{FUSED\_ITERATIONS}
$$

公式中的乘法、幂和求和均为逐元素运算。中间结果保存在Vector寄存器中，每轮始终只从GM读取一次x和y，并写回一次z。

`FUSED_ITERATIONS`可设置为任意正整数，默认值为`8`。每个VF迭代执行`1 + 2 * FUSED_ITERATIONS`条Vector算术指令；`FUSED_ITERATIONS=0`仅对应历史纯Add配置，不属于当前可配置取值。

输入、输出和真值数据见表2，均以ND格式连续存放在GM中。

**表2**  输入输出数据规格

| 数据 | 输入/输出 | 数据文件 | 数据类型 | Shape | 数据说明 |
| --- | --- | --- | --- | --- | --- |
| x | 输入 | `input/input_x.bin` | half | `[8192, 8192]` | 在区间`[-0.1, 0.1)`内随机生成。 |
| y | 输入 | `input/input_y.bin` | half | `[8192, 8192]` | 在区间`[-0.1, 0.1)`内随机生成。 |
| z | 输出 | `output/output.bin` | half | `[8192, 8192]` | Kernel计算结果。 |
| golden | 真值 | `output/golden.bin` | half | `[8192, 8192]` | 按Kernel相同的递推顺序生成，每次乘法和加法后转换为half。 |

两个场景使用相同的切核和分块参数。矩阵沿M轴分给64个核，每核处理连续128行。具体计算见表3。

**表3**  多核切分和分块参数

| 参数 | 取值 | 说明 |
| --- | --- | --- |
| 矩阵规模 | `[8192, 8192]` | 沿M轴切分。 |
| 核数 | 64 | 每个核处理连续的128行。 |
| 每核处理元素数 | 1048576 | 对应128行数据。 |
| 满块有效元素数 | 21760 | 每轮处理一个满块。 |
| 尾块有效元素数 | 4096 | 最后一轮处理。 |
| 每核循环次数 | 49 | 包括48个满块和1个尾块。 |

编译参数`SCENARIO_NUM`决定使用单Buffer还是Ping/Pong Double Buffer，`FUSED_ITERATIONS`决定融合递推次数。对比Buffer场景时，两个场景必须使用相同的`FUSED_ITERATIONS`。

### 场景0：未开启Ping/Pong Buffer优化

`add_baseline`只分配一组连续排列的x、y、z Buffer。每个Buffer预留21840个half元素，实际读写长度由`current_length`指定。

<a id="figure-1"></a>
<p align="center">
  <img src="figures/sequential_pipeline.png" width="50%">
</p>
<p align="center">图1 Ping/Pong Buffer流水示意图</p>

**表4**  未开启Ping/Pong Buffer优化时的UB布局

| UB缓冲区 | 起始偏移（half元素） | 满块有效元素数 | 用途 |
| --- | --- | --- | --- |
| `x_local` | 0 | 21760 | 保存当前轮从GM搬入的x。 |
| `y_local` | 21840 | 21760 | 保存当前轮从GM搬入的y。 |
| `z_local` | 43680 | 21760 | 保存当前轮的融合计算结果。 |

每轮先由MTE2将x和y搬入UB，再由Vector完成融合计算，最后由MTE3将z搬回GM。

三阶段复用同一组UB Buffer，当前轮完成后下一轮才能开始处理，因此不同轮次之间不能重叠。这一实现作为性能基线。

### 场景1：开启Ping/Pong Buffer优化

`add_double_buffer`在UB中分配Ping和Pong两组x、y、z Buffer。偶数轮使用Ping，奇数轮使用Pong。每组使用两个Mutex：输入Mutex管理x、y的搬入和读取，输出Mutex管理z的写入和搬出。

<a id="figure-2"></a>
<p align="center">
  <img src="figures/overlapped_pipeline.png" width="50%">
</p>
<p align="center">图2 Ping/Pong Buffer流水示意图</p>

**表5**  开启Ping/Pong Buffer优化时的缓冲区布局

| 轮次 | 缓冲区组 | 输入Mutex ID | 输出Mutex ID | x起始偏移 | y起始偏移 | z起始偏移 |
| --- | --- | --- | --- | --- | --- | --- |
| 偶数轮 | Ping | 0 | 2 | 0 | 21840 | 43680 |
| 奇数轮 | Pong | 1 | 3 | 65520 | 87360 | 109200 |

表中的偏移以half元素为单位，每个Buffer预留21840个元素。Double Buffer不改变每轮的搬运量和计算量，`asc_lock`和`asc_unlock`用于控制两组Buffer的读写顺序。

**表6**  开启Ping/Pong Buffer优化时的同步关系

| 流水阶段 | Mutex ID | 作用 |
| --- | --- | --- |
| MTE2搬入 | 输入：Ping为0，Pong为1 | 保护同组x、y Buffer的复用。 |
| Vector计算 | 当前组输入和输出Mutex | 等待输入就绪和z Buffer可写。 |
| MTE3搬出 | 输出：Ping为2，Pong为3 | 等待当前轮z计算完成。 |

所有轮次下发后调用`asc_sync_pipe(PIPE_ALL)`等待流水结束。接口说明参见[asc_lock](../../../../../../docs/zh/api/SIMD-API/c_api/sync/asc_lock.md)和[asc_unlock](../../../../../../docs/zh/api/SIMD-API/c_api/sync/asc_unlock.md)。

Ping/Pong Buffer使用两组独立的x、y、z Buffer交替处理数据块，使MTE2搬入、Vector计算和MTE3搬出能够跨轮重叠，但需要额外占用一组UB Buffer。

<a id="figure-3"></a>
<p align="center">
  <img src="figures/capi_double_buffer.png" width="50%">
</p>
<p align="center">图3 Ping/Pong Buffer流水示意图</p>

如[图3](#figure-3)所示，相邻数据块在MTE2、Vector和MTE3流水上重叠执行。

## 性能对比方法

同一组`FUSED_ITERATIONS`测试中，两个场景只改变Buffer数量和同步方式。融合计算量、输入数据和分块参数保持一致。

### 测试条件

**表7**  性能测试配置

| 配置项 | 配置值 |
| --- | --- |
| 芯片版本 | Ascend 950PR |
| CANN软件版本 | CANN 9.2.0 |
| NPU架构 | `dav-3510` |
| 输入/输出数据类型 | half |
| 输入/输出Shape | `[8192, 8192]` |
| AI Vector Core数 | 64 |
| 单次分块长度 | 21760个half元素，尾块为4096个half元素 |
| 融合递推次数 | 正整数；同一取值分别测试两个Buffer场景 |
| 测试次数 | 每个场景预热1次，正式采集至少5次，取Task Duration中位数 |

测试期间保持设备频率和功耗模式不变，避免其他任务占用NPU。两个场景使用同一组输入数据，并采用相同的核数、分块参数和编译选项。

### 性能指标

使用msOpProf采集性能数据，表8中的指标取自`PipeUtilization.csv`。

**表8**  性能指标说明

| 指标 | 说明 |
| --- | --- |
| `Task Duration(μs)` | 整个Task的执行时间，端到端性能对比以该指标为准。 |
| `aiv_time(μs)` | Task在AI Vector Core上的执行时间。 |
| `aiv_vec_time(μs)` / `aiv_vec_ratio` | Vector计算指令耗时及其占比。 |
| `aiv_scalar_time(μs)` / `aiv_scalar_ratio` | Scalar指令耗时及其占比。 |
| `aiv_mte2_time(μs)` / `aiv_mte2_ratio` | MTE2流水执行GM到UB搬入的耗时及其占比。 |
| `aiv_mte3_time(μs)` / `aiv_mte3_ratio` | MTE3流水执行UB到GM搬出的耗时及其占比。 |

Ping/Pong Double Buffer会使不同流水重叠，各流水耗时或占比不能直接相加。端到端性能以`Task Duration`为准，其他指标用于定位主要耗时。

### Ascend 950PR性能对比

原始纯Add配置对应`FUSED_ITERATIONS=0`，由于计算量较小，Double Buffer收益不明显，本节不再列出该组历史数据。

**表9**  `FUSED_ITERATIONS=12`性能数据

| 场景 | 优化配置 | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 场景0 | 未开启Ping/Pong Buffer优化 | 285.401 | 284.24 | 120.175 | 0.423 | 2.12 | 0.007 | 143.849 | 0.506 | 16.041 | 0.056 |
| 场景1 | 开启Ping/Pong Buffer优化 | 269.418 | 268.35 | 118.935 | 0.443 | 1.959 | 0.007 | 262.743 | 0.979 | 142.026 | 0.529 |

**表10**  `FUSED_ITERATIONS=14`性能数据

| 场景 | 优化配置 | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 场景0 | 未开启Ping/Pong Buffer优化 | 296.725 | 295.62 | 142.654 | 0.483 | 1.842 | 0.006 | 132.634 | 0.449 | 16.228 | 0.055 |
| 场景1 | 开启Ping/Pong Buffer优化 | 271.788 | 270.69 | 142.011 | 0.525 | 2.216 | 0.008 | 263.457 | 0.973 | 138.192 | 0.511 |

**表11**  `FUSED_ITERATIONS=24`性能数据

| 场景 | 优化配置 | Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 场景0 | 未开启Ping/Pong Buffer优化 | 433.267 | 432.46 | 306.585 | 0.709 | 1.72 | 0.004 | 105.532 | 0.244 | 16.266 | 0.038 |
| 场景1 | 开启Ping/Pong Buffer优化 | 306.767 | 305.87 | 298.238 | 0.975 | 2.401 | 0.008 | 150.555 | 0.492 | 15.939 | 0.052 |

#### 性能分析

以`FUSED_ITERATIONS=14`为例，场景0中VEC与MTE2的阶段平衡误差为：

$$
\left|T_{\text{vec}} - T_{\text{mte2}}\right| = \left|142.654 - 132.634\right| = 10.020\ \mu s
$$

该误差在已测档位中最小，说明计算与搬入的耗时较为平衡。开启Ping/Pong Buffer后的端到端耗时降幅为：

$$
\frac{T_{\text{baseline}} - T_{\text{ping-pong}}}{T_{\text{baseline}}} \times 100\%
= \frac{296.725 - 271.788}{296.725} \times 100\%
= 8.40\%
$$

场景1的`aiv_mte2_ratio`为97.3%，当前流水仍主要受MTE2阶段限制。各流水指标包含重叠和等待时间，不能相加或用于比较单次搬运速度；性能结论以`Task Duration`为准。

#### 融合计算分档测试

表12汇总了三个代表性档位的实测结果。阶段平衡误差取场景0中`aiv_vec_time`与`aiv_mte2_time`的差值绝对值。

**表12**  融合计算性能记录

| `FUSED_ITERATIONS` | 每个VF迭代的算术指令数 | 场景0阶段平衡误差(μs) | 场景0 Task Duration(μs) | 场景1 Task Duration(μs) | 同档耗时降幅 |
| --- | --- | --- | --- | --- | --- |
| 12 | 25 | 23.674 | 285.401 | 269.418 | 5.60% |
| 14 | 29 | 10.020 | 296.725 | 271.788 | 8.40% |
| 24 | 49 | 201.053 | 433.267 | 306.767 | 29.20% |

`12`表示搬运耗时略高于计算耗时，`14`是当前实测中最接近平衡的档位，`24`用于观察计算明显占优时的表现。`FUSED_ITERATIONS=14`的阶段平衡误差最小，因此作为主要分析配置。`24`的同档耗时降幅更高，但融合计算量也更大；不同融合次数之间只能观察负载变化，不能直接比较`Task Duration`。

当前测试使用正整数的`FUSED_ITERATIONS`，每个取值都必须重新生成匹配的`golden.bin`，并分别完成场景0和场景1的精度校验。不同`FUSED_ITERATIONS`对应不同计算任务，不能直接用它们的Task Duration计算Double Buffer加速比。

#### 总结

- **实际收益**：`FUSED_ITERATIONS=14`时，`Task Duration`从296.725μs降至271.788μs，缩短24.937μs，降幅为8.40%。
- **收益来源**：两个场景的融合计算和数据搬运量相同，且`aiv_vec_time`基本不变，收益来自Buffer布局和流水重叠，而不是减少了计算量。
- **流水优势**：Double Buffer额外使用一组UB Buffer，使MTE2搬入下一数据块、Vector计算当前数据块、MTE3搬出上一数据块可以交错执行；MTE2搬入与MTE3搬出也可同时进行。

## 编译运行

配置CANN环境变量：

```bash
source ${install_path}/cann/set_env.sh
```

在本样例目录编译并运行指定场景：

```bash
SCENARIO_NUM=0
FUSED_ITERATIONS=8
RUN_MODE=npu
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 \
    -DCMAKE_ASC_RUN_MODE=${RUN_MODE} \
    -DSCENARIO_NUM=${SCENARIO_NUM} \
    -DFUSED_ITERATIONS=${FUSED_ITERATIONS} ..
make -j
python3 ../scripts/gen_data.py --fused-iterations ${FUSED_ITERATIONS}
./demo
python3 ../scripts/verify_result.py output/output.bin output/golden.bin
```

默认使用NPU运行模式。进行NPU仿真时，将`RUN_MODE`设置为`sim`，其余编译、数据生成、运行和精度校验步骤不变：

```bash
RUN_MODE=sim
```

NPU仿真模式用于检查Kernel功能和计算精度，仿真耗时不能用于评估Double Buffer的性能收益。性能对比应使用NPU运行模式下的上板数据。

修改`FUSED_ITERATIONS`后，CMake参数和`gen_data.py`参数必须保持一致。将`SCENARIO_NUM`改为`1`即可测试相同融合计算下的Ping/Pong Double Buffer场景。切换运行模式或编译参数前需要清理build目录中的CMake缓存。

**表13**  编译选项

| 编译选项 | 可选值 | 说明 |
| --- | --- | --- |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | 指定NPU架构。 |
| `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 选择NPU运行或NPU仿真模式。 |
| `SCENARIO_NUM` | `0`、`1`（默认） | 选择是否开启Ping/Pong Buffer优化。 |
| `FUSED_ITERATIONS` | 正整数，默认`8` | $t_{i+1}=t_i \times x+y$的递推次数。 |

精度校验成功时输出：

```text
test pass!
```

## 功能调试

### printf

该接口提供CPU域/NPU域调试场景下的格式化输出功能。本样例在NPU运行和NPU仿真模式下，可以在Kernel侧需要输出日志的位置直接调用[printf](../../../../../../docs/zh/api/Utils-API/tuning_interface/printf.md)接口。使用该接口需要包含头文件`utils/debug/asc_printf.h`。

示例如下：

```cpp
printf("add blockIdx=%ld\n", static_cast<int64_t>(block_idx));
```

> **注意：** `printf`接口会增加算子的执行时间并影响性能测试结果，通常只在功能调试阶段使用。进行性能采集前应删除或关闭打印代码，也可以按需设置`ASCENDC_DUMP=0`关闭打印功能。

### msOpProf工具介绍

msOpProf工具是单算子性能分析工具。包含msopprof和msopprof simulator两种使用方式。该工具协助用户定位算子内存、算子代码以及算子指令的异常，实现全方位的算子调优。当前支持基于不同运行模式（上板或仿真）和不同文件形式（可执行文件或算子二进制.o文件）进行性能数据的采集和自动解析。

#### 上板性能采集

通过上板性能采集，可以直接测定算子昇腾AI处理器上的运行时间。该方式适合在板环境中快速定位算子性能问题。

基于可执行文件`demo`通过msopprof执行算子调优：

```bash
msopprof ./demo
```

**性能数据说明**

命令完成后，会在默认目录下生成以“OPPROF_{timestamp}_XXX”命名的文件夹，性能数据文件夹结构示例如下：

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
