# Matmul C-API高性能样例

## 概述

本样例使用Ascend C C-API实现half矩阵乘，对比单缓冲基线和L1双缓冲两种配置。L1 Ping/Pong Buffer使MTE2搬入能够与后续流水在不同K轮之间重叠。

## 支持的产品及CANN软件版本

**表1**  支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
| --- | --- |
| Ascend 950PR/Ascend 950DT | == CANN 9.2.0 |

## 目录结构

```text
00_mmad_double_buffer
├── scripts
│   ├── gen_data.py          # 生成输入数据和真值数据
│   └── verify_result.py     # 校验输出结果
├── CMakeLists.txt           # 编译工程文件
├── data_utils.h             # 二进制数据读写工具
├── mmad_double_buffer.asc   # Kernel及Host侧实现
├── README.md
└── README_en.md
```

## 样例描述

样例计算`C = A x B`。输入A和B使用固定随机种子生成。B写入文件前先转置，使K轴数据在GM中连续，便于按K块搬运。输入、输出和真值数据见表2。

**表2**  输入输出数据规格

| 数据 | 输入/输出 | 数据文件 | 数据类型 | 逻辑Shape | 文件及GM排布 | 数据说明 |
| --- | --- | --- | --- | --- | --- | --- |
| A | 输入 | `input/x1_gm.bin` | half | `[M, K] = [1024, 1024]` | ND，按`[M, K]`存储 | 在区间`[-0.1, 0.1)`内随机生成。 |
| B | 输入 | `input/x2_gm.bin` | half | `[K, N] = [1024, 1024]` | 转置后的ND，按`[N, K]`存储 | 在区间`[-0.1, 0.1)`内随机生成。 |
| C | 输出 | `output/output.bin` | float | `[M, N] = [1024, 1024]` | ND，按`[M, N]`存储 | Kernel计算结果。 |
| golden | 真值 | `output/golden.bin` | float | `[M, N] = [1024, 1024]` | ND，按`[M, N]`存储 | 使用float输入执行`np.matmul`生成。 |

样例计算`C = A x B`，其中`M`、`N`和`K`均为1024。矩阵按`[128, 128, 64]`分块，使用64个逻辑Cube Block，每个Block负责一个`[128, 128]`输出块。`block_idx`用于分配输出块，Block内沿K轴执行16轮MMAD累加。

数据依次经过GM、L1 Buffer、L0A Buffer/L0B Buffer和L0C Buffer，最后写回GM。各阶段的数据排布见表3。

**表3**  数据搬运路径及数据排布

| 存储单元 | 数据 | 数据排布 |
| --- | --- | --- |
| GM | A、转置后的B | ND |
| L1 Buffer | A、B数据块 | Nz |
| L0A Buffer | A数据块 | Nz |
| L0B Buffer | B数据块 | Zn |
| L0C Buffer | C输出块 | Nz，float |
| GM | C | ND，float |

编译参数`SCENARIO_NUM`选择单缓冲基线或L1双缓冲优化。两个场景使用相同的矩阵规模、Tile、K循环次数、Block数和MMAD参数，只改变L1 Buffer的数量。

**表4**  场景配置

| `SCENARIO_NUM` | Kernel | 场景 | L1 Buffer | L0A Buffer/L0B Buffer | L0C Buffer | 流水关系 |
| --- | --- | --- | --- | --- | --- | --- |
| 0 | `mmad_baseline` | 单缓冲基线 | A/B各一组 | A/B各一组 | 一组，用于结果累加 | 同一Mutex串联每个K块的搬入、加载和MMAD，完成后再处理下一块。 |
| 1 | `mmad_l1_double_buffer` | 仅L1双缓冲 | A/B Ping/Pong两组 | A/B各一组 | 一组，用于结果累加 | MTE2可提前准备后续K块；MTE1仍需等待单组L0可复用。 |

### 场景0：单缓冲基线

`mmad_baseline`在L1 Buffer、L0A Buffer和L0B Buffer中只为A/B各分配一块空间，16轮K循环重复使用这些地址。各Buffer的大小和用途见表5。

**表5**  单缓冲配置

| Buffer | 数组 | 元素数量 | 字节数 | 用途 |
| --- | --- | --- | --- | --- |
| L1 Buffer A | `a_l1` | 8192个half元素 | 16384 | 保存从GM搬入并转换为Nz格式的当前A块。 |
| L1 Buffer B | `b_l1` | 8192个half元素 | 16384 | 保存从GM搬入并转换为Nz格式的当前B块。 |
| L0A Buffer | `a_l0` | 8192个half元素 | 16384 | 保存MMAD使用的当前A块。 |
| L0B Buffer | `b_l0` | 8192个half元素 | 16384 | 保存MMAD使用的当前B块。 |
| L0C Buffer | `c_l0` | 16384个float元素 | 65536 | 在16轮MMAD迭代中累加C输出块。 |

每轮K循环按照以下顺序执行：

1. MTE2调用`copy_a_to_l1`和`copy_b_to_l1`，将当前A/B块从GM搬入L1 Buffer，同时把数据从ND转换为Nz；
2. MTE2释放Mutex后，MTE1获取该Mutex，将A/B加载到L0A Buffer和L0B Buffer；
3. MTE1释放Mutex后，M流水调用`asc_mmad`计算`[128, 64] x [64, 128]`。第0轮初始化L0C Buffer，后续轮次继续累加；
4. M流水释放Mutex后，下一轮MTE2获取同一个Mutex，等待当前MMAD完成后再复用相同的L1 Buffer和L0 Buffer；
5. K循环结束后，FIX获取同一个Mutex，等待最后一轮MMAD完成，再将L0C结果写回GM；最后调用`asc_sync_pipe(PIPE_FIX)`等待写回完成。

单Buffer和同一Mutex使MTE2、MTE1和M流水只能依次处理同一个K块，不能跨K轮并行。该实现作为性能基线。

### 场景1：仅L1 Buffer双缓冲

`mmad_l1_double_buffer`仅为L1中的A/B数据分配Ping/Pong两组空间，L0A/L0B仍各使用一组空间。偶数轮使用L1 Ping和Mutex 0，奇数轮使用L1 Pong和Mutex 1；单组L0始终使用Mutex 2。

MTE1读完某组L1 Buffer后，MTE2即可向该组写入后续K块，不必等待MMAD结束。由于L0只有一组，MTE1仍需等待前一轮MMAD读完L0后才能加载下一块。

各场景均使用[asc_lock](../../../../../docs/zh/api/SIMD-API/c_api/sync/asc_lock.md)和[asc_unlock](../../../../../docs/zh/api/SIMD-API/c_api/sync/asc_unlock.md)控制Buffer复用依赖。这些Mutex保证数据在被读取前已写完、在被覆盖前已读完，不作为独立的性能优化项。

**表6**  Buffer复用依赖

| 依赖方向 | Mutex ID | 作用 |
| --- | --- | --- |
| `MTE2 -> MTE1`、`MTE1 -> MTE2` | L1：单缓冲为`0`，双缓冲为`0/1` | 控制L1数据搬入与MTE1读取，以及L1 Buffer的再次写入。 |
| `MTE1 -> M`、`M -> MTE1` | L0：场景0使用`0`，场景1使用`2` | 控制L0加载与MMAD读取，以及单组L0 Buffer的再次写入。 |
| `M -> FIX` | 最后一轮使用的L0 Mutex | 保证FIX在最后一轮MMAD完成后再读取L0C并写回GM。 |

以场景1的连续三轮为例：第0轮使用L1 Ping，第1轮使用L1 Pong，第2轮重新使用L1 Ping。M流水计算第0轮时，MTE2可向L1 Pong搬入第1轮数据；MTE1读完Ping后，MTE2还可向已释放的L1 Ping搬入第2轮数据。K循环结束后，FIX通过L0 Mutex等待最后一轮MMAD完成，再将结果写回GM。

两个场景分别为单缓冲基线和L1 Ping/Pong双缓冲，用于比较L1双缓冲带来的流水重叠收益。

## 性能对比方法

性能测试保持矩阵、MMAD参数和输入数据一致，Mutex仅用于表达各Buffer的读写和复用依赖。

### 测试条件

**表7**  性能测试配置

| 配置项 | 配置值 |
| --- | --- |
| 芯片版本 | Ascend 950PR |
| CANN软件版本 | CANN 9.2.0 |
| NPU架构 | `dav-3510` |
| 输入数据类型 | A、B为half |
| 输出数据类型 | C为float |
| 矩阵Shape | `M=N=K=1024` |
| Cube Tile | `TILE_M=128`、`TILE_K=64`、`TILE_N=128` |
| Block数 | 64 |
| K循环次数 | 每个输出Tile执行16轮MMAD累加 |
| 测试次数 | 本次每个场景采集1次；稳定结论建议正式采集至少5次并取Task Duration中位数 |

测试期间保持设备频率和功耗模式不变，避免其他任务占用NPU。两个场景使用同一组输入数据，并采用相同的矩阵Shape、分块参数、Block数和编译选项。

### 性能指标

使用msOpProf采集性能数据，表8中的指标取自生成的`op_summary_*.csv`。

**表8**  性能指标说明

| 指标 | 说明 |
| --- | --- |
| `Task Duration(μs)` | 整个Task的执行时间，端到端性能对比以该指标为准。 |
| `Block Num` | Kernel启动的Block数量，本样例应为64。 |
| `aicore_time(μs)` | Task在AI Core上的平均执行时间。 |
| `aic_mac_time(μs)` / `aic_mac_ratio` | Cube计算单元执行MMAD的耗时及其占比。 |
| `aic_scalar_time(μs)` / `aic_scalar_ratio` | Scalar指令耗时及其占比。 |
| `aic_mte1_time(μs)` / `aic_mte1_ratio` | MTE1流水执行L1 Buffer到L0A Buffer/L0B Buffer加载的耗时及其占比。 |
| `aic_mte2_time(μs)` / `aic_mte2_ratio` | MTE2流水执行GM到L1 Buffer搬入的耗时及其占比。 |
| `aic_fixpipe_time(μs)` / `aic_fixpipe_ratio` | FIX流水执行L0C Buffer到GM搬出的耗时及其占比。 |

启用双缓冲后，部分或全部流水会在不同K轮之间重叠，各流水耗时或占比不能直接相加。端到端性能以`Task Duration`为准，其他指标用于判断时间主要花在哪个阶段。

### Ascend 950PR性能对比

表9列出当前代码在`TILE_M=TILE_N=128`、`TILE_K=64`条件下的场景0和场景1采集数据。各流水耗时用于分析瓶颈，场景间的端到端性能以`Task Duration`为准。

**表9**  L1双缓冲性能数据

| 场景 | 优化配置 | Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 场景0 | 单缓冲基线 | 36.589 | 64 | 35.480 | 7.068 | 0.199 | 1.420 | 0.040 | 3.174 | 0.089 | 20.205 | 0.569 | 1.277 | 0.036 |
| 场景1 | L1双缓冲 | 21.220 | 64 | 20.160 | 7.069 | 0.351 | 1.458 | 0.072 | 4.148 | 0.206 | 14.384 | 0.713 | 1.266 | 0.063 |

#### 性能数据分析

以场景0为基线，场景1启用L1双缓冲后的端到端耗时降幅为

$$
R = \frac{T_{\text{baseline}} - T_{\text{double-buffer}}}{T_{\text{baseline}}} \times 100\%
  = \frac{36.589 - 21.220}{36.589} \times 100\%
  \approx 42.00\%
$$

`Task Duration`从36.589μs降至21.220μs，减少15.369μs，降幅为42.00%。`aicore_time`从35.480μs降至20.160μs，变化趋势与端到端耗时一致。

场景0和场景1的`aic_mac_time`只相差0.001μs，Cube计算工作量基本不变。收益来自不同K轮的MTE2搬入与MTE1加载、Cube计算发生重叠。`aic_mte2_time`从20.205μs降至14.384μs，是变化最明显的流水指标。

场景1的`aic_mte2_ratio`从56.9%升至71.3%，是占比最高的阶段。占比升高是因为总`aicore_time`缩短，不能理解为MTE2变慢。Profiler中的各流水耗时不能直接相加。

#### 总结

- **优化方式**：L1使用Ping/Pong Buffer后，MTE2可在MTE1和Cube处理当前K块时准备后续K块。双缓冲改变的是各阶段的等待关系，不会减少矩阵乘法的Cube计算量。
- **实测收益**：表9中`Task Duration`减少15.369μs，降幅为42.00%。`aic_mac_time`基本不变，说明收益主要来自流水重叠。
- **后续方向**：场景1中MTE2仍是占比最高的阶段。若继续优化，应结合实际采集的Memory和L2 Cache指标检查输入搬运量、L2复用和Tile大小。本次每个场景只有一条采集记录，正式性能结论应以多次采集的中位数为准。

## 编译运行

配置CANN环境变量：

```bash
source ${install_path}/cann/set_env.sh
```

在本样例目录编译并运行指定场景：

```bash
SCENARIO_NUM=0
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=${SCENARIO_NUM} ..
make -j
python3 ../scripts/gen_data.py
./c_api_mmad_double_buffer_example
python3 ../scripts/verify_result.py output/output.bin output/golden.bin
```

将`SCENARIO_NUM`设置为`0`或`1`后重新执行CMake、编译、运行和校验，即可测试对应场景。切换场景前需要清理build目录中的CMake缓存。

**表10**  编译选项

| 编译选项 | 可选值 | 说明 |
| --- | --- | --- |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构 |
| `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式 |
| `SCENARIO_NUM` | `0`、`1`（默认） | `0`为单缓冲基线，`1`为L1双缓冲。 |

精度校验成功时输出：

```text
test pass!
```

## 功能调试

### printf

该接口提供CPU域/NPU域调试场景下的格式化输出功能。本样例在NPU运行和NPU仿真模式下，可以在Kernel侧需要输出日志的位置直接调用[printf](../../../../../docs/zh/api/Utils-API/tuning_interface/printf.md)接口。使用该接口需要包含头文件`utils/debug/asc_printf.h`。

示例如下：

```cpp
printf("mmad blockIdx=%ld\n", static_cast<int64_t>(block_idx));
```

> **注意：** `printf`接口会增加算子的执行时间并影响性能测试结果，通常只在功能调试阶段使用。进行性能采集前应删除或关闭打印代码，也可以按需设置`ASCENDC_DUMP=0`关闭打印功能。

### msOpProf工具介绍

msOpProf工具是单算子性能分析工具，包含msopprof和msopprof simulator两种使用方式。该工具协助用户定位算子内存、算子代码以及算子指令的异常，实现全方位的算子调优。当前支持基于不同运行模式（上板或仿真）和不同文件形式（可执行文件或算子二进制.o文件）进行性能数据的采集和自动解析。

#### 上板性能采集

通过上板性能采集，可以直接测定算子在昇腾AI处理器上的运行时间。该方式适合在板环境中快速定位算子性能问题。

在build目录中，基于可执行文件`c_api_mmad_double_buffer_example`通过msopprof执行算子调优：

```bash
msopprof ./c_api_mmad_double_buffer_example
```

**性能数据说明**

命令完成后，会在默认目录下生成以`OPPROF_{timestamp}_XXX`命名的文件夹，性能数据文件夹结构示例如下：

```text
├── dump                       # 原始性能数据，用户无需关注
├── ArithmeticUtilization.csv  # Cube/Vector指令cycle占比
├── L2Cache.csv                # L2 Cache命中率
├── Memory.csv                 # UB、L1和主存储器读写带宽速率
├── MemoryL0.csv               # L0A、L0B和L0C读写带宽速率
├── MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
├── OpBasicInfo.csv            # 算子基础信息
├── PipeUtilization.csv        # 计算单元和搬运单元耗时及占比
├── ResourceConflictRatio.csv  # UB上的bank group、bank conflict和资源冲突率
└── visualize_data.bin         # MindStudio Insight呈现文件
```

查看具体的性能分析结果：

```bash
# 查看Task Duration及各流水数据
cat ./OPPROF_*/PipeUtilization.csv
```
