# Fixpipe（L0C搬出）性能测试样例

## 概述

本样例用于矩阵计算搬出路径的性能测试，覆盖把Cube计算结果从L0C Buffer搬出到L1 Buffer或Unified Buffer（简称UB）的数据通路。

本样例为无业务测试，不校验计算结果，只采集Fixpipe搬出耗时。

## 本样例支持的产品及CANN软件版本

| 产品 | 架构代号 | CANN软件版本 |
|------|----------|-------------|
| Ascend 950PR&950DT系列产品 | dav-3510 | >= CANN 9.1.0 |
| Atlas A3系列产品 | dav-2201 | >= CANN 9.0.0 |
| Atlas A2系列产品 | dav-2201 | >= CANN 9.0.0 |

## 目录结构介绍

```text
├── fixpipe_perf
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── fixpipe_perf.asc        // Fixpipe搬出性能测试实现与调用入口
│   ├── perf.sh                 // 性能测试脚本
│   ├── generate_roofline.py    // Roofline 生成脚本
│   ├── README.md               // 样例说明文档
```

## 样例描述

本样例通过运行参数`SCENARIO_NUM`选择不同的搬出通路与数据类型，矩阵规格通过`./demo SCENARIO_NUM M K N`在运行时传入。

两条搬出通路使用不同接口：

| 通路 | 接口 | 头文件路径 | 支持架构 |
|------|------|-----------|----------|
| L0C Buffer到L1 Buffer | `DataCopy` | `basic_api/kernel_operator_data_copy_intf.h` | dav-2201、dav-3510 |
| L0C Buffer到UB | `Fixpipe` | `basic_api/kernel_operator_fixpipe_intf.h` | 仅dav-3510 |

根据平台架构，支持的测试场景如下：

### Atlas A3/A2训练/推理平台场景

| SCENARIO_NUM | 输入数据类型 | 数据源 | 执行路径 | 说明 | 理论带宽(Byte/cycle) | 带宽延迟(cycle) |
|--------------|--------------|--------|----------|------|----------------------|-----------------|
| 1 | float | L0C Buffer | L0C Buffer -> L1 Buffer | DataCopy 搬出，随路 F322F16 转 half | 128 | 20 |
| 2 | float | L0C Buffer | L0C Buffer -> L1 Buffer | DataCopy 搬出，随路 QF322B8_PRE 量化为 int8_t | 64 | 20 |

### Ascend 950PR&950DT系列产品平台场景

| SCENARIO_NUM | 输入数据类型 | 数据源 | 执行路径 | 说明 | 理论带宽(Byte/cycle) | 带宽延迟(cycle) |
|--------------|--------------|--------|----------|------|----------------------|-----------------|
| 11 | float | L0C Buffer | L0C Buffer -> L1 Buffer | DataCopy 搬出，随路 F322F16 转 half | 128 | 26 |
| 12 | float | L0C Buffer | L0C Buffer -> L1 Buffer | DataCopy 搬出，随路 QF322B8_PRE 量化为 int8_t | 64 | 26 |
| 13 | float | L0C Buffer | L0C Buffer -> UB | Fixpipe 搬出 float，非双目标模式 | 128 | 26 |
| 14 | float | L0C Buffer | L0C Buffer -> UB | Fixpipe 搬出 float，双目标模式按 M 维度拆分 | 256 | 26 |

> 理论带宽基于硬件搬出并行度计算：搬出单元每 cycle 处理 64 个 out 元素，理论带宽 = 64 * sizeof(目的数据类型)（Byte/cycle）。双目标模式拆两个子核并行，等效并行度翻倍。L0C Buffer 到 UB 仅 dav-3510 支持（场景 13、14）；场景 14 把 L0C Buffer 的 M×N 矩阵按 M 维度拆成两半，同时写入两个Vec核的 UB。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

### 配置环境变量

请根据当前环境上CANN开发套件包的[安装方式](../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

```bash
source ${install_path}/cann/set_env.sh
```

`${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

### 编译样例

针对Atlas A3/A2训练/推理平台（dav-2201）编译：

```bash
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..
make -j
cd ..
```

针对Ascend 950PR&950DT系列产品平台（dav-3510）编译：

```bash
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
make -j
cd ..
```
### 运行样例

运行参数顺序为`SCENARIO_NUM M K N`：

```bash
# Atlas A3/A2训练/推理平台示例（场景1、2）
./build/demo 1 128 64 128
./build/demo 2 128 64 128

# Ascend 950PR&950DT系列产品平台示例（场景11-14）
./build/demo 11 128 64 128
./build/demo 12 128 64 128
./build/demo 13 128 64 128
./build/demo 14 128 64 128
```

| 参数 | 说明 |
|------|------|
| `SCENARIO_NUM` | 测试场景编号，Atlas A3/A2训练/推理平台使用1、2，Ascend 950PR&950DT系列产品平台使用11-14 |
| `M` | 矩阵行数 |
| `K` | 矩阵 A 的列数（矩阵 B 的行数） |
| `N` | 矩阵列数 |

矩阵规格需符合对齐要求：M、N取16的倍数；双目标模式按M维度拆分时M须为2的倍数，按N维度拆分时N须为32的倍数。

## 性能数据获取

使用`msopprof`工具获取详细性能数据：

```bash
msopprof build/demo 1 128 64 128
```

> 💡 使用`msopprof`工具需安装CANN商用/社区版，详细信息可参考[msOpProf工具安装指南](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/install_guide/msopprof_install_guide.md)。

命令完成后，会在默认目录下生成以`OPPROF_{timestamp}_XXX`命名的文件夹，性能数据文件夹结构示例如下：

```bash
├── dump                       # 原始性能数据
├── ArithmeticUtilization.csv  # cube/vector指令cycle占比
├── L2Cache.csv                # L2 Cache命中率，影响MTE2，建议合理规划数据搬运逻辑，增加命中率
├── Memory.csv                 # UB、L1 Buffer和主存储器读写带宽速率
├── MemoryL0.csv               # L0A Buffer、L0B Buffer和L0C Buffer读写带宽速率
├── MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
├── OpBasicInfo.csv            # 算子基础信息
├── PipeUtilization.csv        # 计算单元和搬运单元耗时及占比
├── ResourceConflictRatio.csv  # UB bank group、bank conflict和资源冲突率占比
└── visualize_data.bin         # MindStudio Insight呈现文件
```

本样例主要关注L0C Buffer搬出的性能数据，通过如下方法查看具体的性能数据结果：

```bash
cat ./OPPROF_*/PipeUtilization.csv
```

主要关注如下指标：

| 指标 | 说明 |
|------|------|
| `aic_fixpipe_time(us)` | fixpipe类型指令（L0C Buffer搬出）耗时 |

## 性能测试脚本

`perf.sh`用于批量编译、执行`msopprof`、提取Fixpipe搬出耗时并生成CSV汇总结果。

```bash
# 查看帮助
./perf.sh --help

# 测试场景1，默认使用dav-2201
./perf.sh 1

# 测试场景13，默认使用dav-3510
./perf.sh 13

# 明确指定平台，须与场景匹配，否则报错
./perf.sh 1 dav-2201
./perf.sh 13 dav-3510
```

脚本内置的默认shape序列（搬出性能只与M、N相关，K固定取64用于让Mmad前置产出L0C Buffer数据；从很小的M·N起步、逐步增大至满载）。dav-2201的L0C Buffer为128KB、dav-3510为256KB，满载点不同，故两架构末组不同：

dav-2201默认shape序列：

| Test_ID | M | K | N | M·N | L0C Buffer占用（float）|
|---------|---|---|---|-----|------------------------|
| 1 | 16  | 64 | 16  | 256   | 1 KB |
| 2 | 32  | 64 | 32  | 1024  | 4 KB |
| 3 | 64  | 64 | 64  | 4096  | 16 KB |
| 4 | 128 | 64 | 128 | 16384 | 64 KB |
| 5 | 128 | 64 | 256 | 32768 | 128 KB（dav-2201满载）|

dav-3510默认shape序列：

| Test_ID | M | K | N | M·N | L0C Buffer占用（float）|
|---------|---|---|---|-----|------------------------|
| 1 | 16  | 64 | 16  | 256   | 1 KB |
| 2 | 32  | 64 | 32  | 1024  | 4 KB |
| 3 | 64  | 64 | 64  | 4096  | 16 KB |
| 4 | 128 | 64 | 128 | 16384 | 64 KB |
| 5 | 256 | 64 | 256 | 65536 | 256 KB（dav-3510满载）|

其中，L0C Buffer搬出数据量为M * N * sizeof(目的类型)，K仅用于让Mmad前置在L0C Buffer产出M×N结果、不计入搬出量，可以按需调整。

如需针对性补点，可直接用`./build/demo SCENARIO_NUM M K N`跑单个shape。

测试完成后，结果保存在`perf_data_${timestamp}_scenario${SCENARIO}/perf_result_scenario${SCENARIO}.csv`，原始`msopprof`数据保存在同目录下的`test_${id}_${M}_${K}_${N}`子目录。

## 性能指标说明

`perf.sh`从`PipeUtilization.csv`提取`aic_fixpipe_time(us)`，并结合平台主频与搬出量计算带宽。

CSV中的计算列如下：

| 列名 | 计算方式 | 说明 |
|------|----------|------|
| `AIC_FixPipe_Time(us)` | 取自`PipeUtilization.csv`的`aic_fixpipe_time(us)` | Fixpipe搬出耗时 |
| `Cycle` | `Time(us) * Frequency(MHz)` | 根据平台主频折算的Cycle数 |
| `Bandwidth(GB/s)` | `DataSize(bytes) / Time(us) / 1e3` | 数据搬运带宽 |

### 性能指标计算方法

`msopprof`在`PipeUtilization.csv`中采集到的`aic_fixpipe_time(us)`为微秒（us）单位的搬出耗时。`perf.sh`会读取该时间列，并结合平台主频和搬出数据量计算Cycle数与实测带宽。

#### Time转换为Cycle

主频单位为MHz，即每微秒的周期数，因此无需额外换算：

```text
Cycle = Time(us) * Frequency(MHz)
```

例如Atlas A3/A2训练/推理平台主频为1800MHz，若`aic_fixpipe_time(us) = 0.050000`：

```text
Cycle = 0.050000 * 1800 = 90.00 cycles
```

#### 数据搬运量计算

`perf.sh`按场景的目的数据类型计算搬出量，作为带宽计算的`DataSize(bytes)`：

| 场景 | 数据量计算 | 数据类型大小 |
|------|------------|--------------|
| 1、11 | `M * N * sizeof(half)` | 2 bytes |
| 2、12 | `M * N * sizeof(int8_t)` | 1 byte |
| 13、14 | `M * N * sizeof(float)` | 4 bytes |

#### 实测带宽计算

带宽按GB/s输出。由于`Time(us)`是微秒，`DataSize(bytes) / Time(us)`的单位是MB/s，继续除以`1e3`后得到GB/s：

```text
Bandwidth(GB/s) = DataSize(bytes) / Time(us) / 1e3
```

例如场景11，shape为`[128, 64, 128]`，目的类型half：

```text
DataSize = 128 * 128 * 2 = 32768 bytes
Time = 0.050000 us
Bandwidth = 32768 / 0.050000 / 1e3 = 655.360 GB/s
```

#### 理论耗时与带宽利用率

场景表中的“理论带宽(Byte/cycle)”和“带宽延迟(cycle)”可用于估算理论耗时。理论带宽由搬出并行度推导（搬出单元每cycle处理64个out元素，理论带宽=64*sizeof(目的类型)，双目标模式等效翻倍）；固定延迟表示一次搬运的基础启动开销。理论传输时间可按如下方式估算：

```text
TransferCycle = DataSize(bytes) / TheoreticalBandwidth(Byte/cycle)
TheoryCycle = Latency(cycle) + TransferCycle
TheoryTime(us) = TheoryCycle / Frequency(MHz)
TheoryBandwidth(GB/s) = DataSize(bytes) / TheoryTime(us) / 1e3
```

实测带宽与理论带宽的比例可用于评估带宽利用率：

```text
BandwidthUtilization = MeasuredBandwidth(GB/s) / TheoryBandwidth(GB/s) * 100%
```

平台主频由`perf.sh`按场景自动设置：

| 平台 | 架构代号 | 主频 | 适用场景 |
|------|----------|------|----------|
| Atlas A3/A2训练/推理平台 | dav-2201 | 1800MHz | 1、2 |
| Ascend 950PR&950DT系列产品平台 | dav-3510 | 1650MHz | 11-14 |

## Roofline分析

本样例提供`generate_roofline.py`，可基于`perf.sh`生成的CSV输出ASCII报告和图片。

### Python包依赖

`generate_roofline.py`使用Python标准库读取CSV并生成ASCII报告；如果需要同时生成PNG/PDF图表，需要安装`matplotlib`和`numpy`。

```bash
python3 -m pip install --user matplotlib numpy
```

如果未安装上述依赖，脚本仍可生成`.txt`格式的ASCII分析报告，但会跳过图片生成。

```bash
# 自动查找最新perf_data目录中的结果
python3 generate_roofline.py

# 指定CSV文件
python3 generate_roofline.py --csv perf_data_xxx_scenario11/perf_result_scenario11.csv
```

脚本内置各场景的搬出并行度，无需手工传入峰值带宽；首指令开销默认按场景取（dav-2201为20cycle、dav-3510为26cycle），主频按场景自动取。

### 图表示例

以下为场景1生成的Roofline图示例：

![场景1 Roofline图](figures/perf_data_scenario1_roofline.png)

## 注意事项

1. 场景编号与平台必须匹配：dav-2201用场景1、2，dav-3510用场景11-14，`perf.sh`会校验不匹配并报错。
2. L0C Buffer到UB通路（场景13、14）仅dav-3510支持，使用`Fixpipe`接口与混合编程框架。
3. 矩阵规格需符合对齐要求：M、N取16的倍数；双目标模式按M维度拆分时M须为2的倍数，按N维度拆分时N须为32的倍数。
4. 本样例为纯性能测试，不校验计算结果。核函数对L0A Buffer、L0B Buffer数据不初始化，保留一条最小Mmad前置产出L0C Buffer数据。
5. Mmad前置与搬出指令之间插入`PipeBarrier<PIPE_ALL>`，避免流水重叠导致`aic_fixpipe_time`统计不准。
