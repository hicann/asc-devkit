# LoadData(L1->L0) 性能测试样例

## 概述

本样例用于 LoadData 相关接口的性能测试，覆盖矩阵 A/B 从 L1 搬运到 L0A/L0B 的数据搬运路径。
本样例为无业务测试，不包含矩阵计算指令。

## 本样例支持的产品及CANN软件版本

| 产品 | 架构代号 | CANN软件版本 |
|------|----------|-------------|
| Ascend 950PR&950DT系列产品 | dav-3510 | >= CANN 9.1.0 |
| Atlas A3系列产品 | dav-2201 | >= CANN 9.0.0 |
| Atlas A2系列产品 | dav-2201 | >= CANN 9.0.0 |

## 目录结构介绍

```text
├── load_data_perf
│   ├── CMakeLists.txt                    // 编译工程文件
│   ├── load_data_perf.asc                // LoadData 性能测试实现与调用入口
│   ├── perf.sh                           // 性能测试脚本
│   ├── generate_roofline_with_latency.py // Roofline 生成脚本
│   └── README.md                         // 样例说明文档
```

## 样例描述

本样例通过运行参数 `SCENARIO_NUM` 选择不同搬运路径，矩阵规格通过 `./demo SCENARIO_NUM M K N` 在运行时传入，核函数名为 `load_data_perf_custom`。

根据平台架构，支持的测试场景如下：

| 平台架构 | 支持场景 | 说明 |
|----------|----------|------|
| Atlas A3/A2 训练/推理平台（dav-2201） | 1-9 | Atlas A3/A2 训练/推理平台支持的 LoadData 接口场景 |
| Ascend 950PR&950DT系列产品 平台（dav-3510） | 11-19 | Ascend 950PR&950DT系列产品 平台支持的 LoadData 接口场景 |

### Atlas A3/A2 训练/推理平台场景（1-9）

| SCENARIO_NUM | 输入数据类型 | 数据源 | 执行路径 | 说明 | 理论带宽(Byte/cycle) | 带宽延迟(cycle) |
|--------------|--------------|--------|----------|------|----------------------|-----------------|
| 1 | bfloat16 | A矩阵 | L1 -> L0A | LoadData（2D矩阵搬运） 从 L1 加载到 L0A | 256 | 28 |
| 2 | bfloat16 | B矩阵 | L1 -> L0B | LoadData（2D矩阵搬运） 从 L1 加载到 L0B | 128 | 28 |
| 3 | bfloat16 | A矩阵 | L1 -> L0A（转置） | LoadDataWithTranspose 从 L1 加载到 L0A（转置） | 256 | 28 |
| 4 | bfloat16 | B矩阵 | L1 -> L0B（转置） | LoadDataWithTranspose 从 L1 加载到 L0B（转置） | 128 | 28 |
| 5 | bfloat16 | A矩阵 | L1 -> L0A | LoadData（卷积数据搬运）v2 从 L1 加载到 L0A | 32-256 | 50 |
| 6 | bfloat16 | B矩阵 | L1 -> L0B | LoadData（卷积数据搬运）v2 从 L1 加载到 L0B | 28.4-128 | 50 |
| 7 | int8_t | B矩阵 | L1 -> L0B | LoadDataWithSparse 从 L1 加载到 L0B（稀疏加载） | 128 | 30 |
| 8 | float | Bias | L1 -> BiasTable Buffer | 从 L1 加载到 BiasTable Buffer | 32 | 20 |
| 9 | uint64_t | FixPipe | L1 -> Fixpipe Buffer | 从 L1 加载到 Fixpipe Buffer | 32 | 20 |

### Ascend 950PR&950DT系列产品 平台场景（11-19）

| SCENARIO_NUM | 输入数据类型 | 数据源 | 执行路径 | 说明 | 理论带宽(Byte/cycle) | 带宽延迟(cycle) |
|--------------|--------------|--------|----------|------|----------------------|-----------------|
| 11 | bfloat16 | A矩阵 | L1 -> L0A | LoadData（2D矩阵搬运V2） 从 L1 加载到 L0A | 256 | 30 |
| 12 | bfloat16 | B矩阵 | L1 -> L0B | LoadData（2D矩阵搬运V2） 从 L1 加载到 L0B | 256 | 30 |
| 13 | fp8_e4m3fn | A矩阵 + ScaleA | L1 -> L0A + L0A_MX | LoadData（MX矩阵搬运） 从 L1 加载到 L0A 和 L0A_MX（带 scale） | 256/32 | 30 |
| 14 | fp8_e4m3fn | B矩阵 + ScaleB | L1 -> L0B + L0B_MX | LoadData（MX矩阵搬运） 从 L1 加载到 L0B 和 L0B_MX（带 scale） | 256/32 | 30 |
| 15 | bfloat16 | A矩阵 | L1 -> L0A | LoadData（卷积数据搬运）v2 从 L1 加载到 L0A | 32-256 | 50 |
| 16 | bfloat16 | B矩阵 | L1 -> L0B | LoadData（卷积数据搬运）v2 从 L1 加载到 L0B | 32-256 | 50 |
| 17 | bfloat16 | B矩阵 | L1 -> L0B（转置） | LoadDataWithTranspose 从 L1 加载到 L0B（转置） | 256 | 28 |
| 18 | float | Bias | L1 -> BiasTable Buffer | 从 L1 加载到 BiasTable Buffer | 32 | 20 |
| 19 | uint64_t | FixPipe | L1 -> Fixpipe Buffer | 从 L1 加载到 Fixpipe Buffer | 32 | 26 |

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

### 配置环境变量

请根据当前环境上CANN开发套件包的[安装方式](../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

```bash
source ${install_path}/cann/set_env.sh
```

`${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

### 编译样例

```bash
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..
make -j
```

针对 Ascend 950PR&950DT系列产品 平台编译：

```bash
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
make -j
```

### 运行样例

```bash
# Atlas A3/A2 训练/推理平台示例（场景 1-9）
./demo 1 64 128 128
./demo 2 256 128 128

# Ascend 950PR&950DT系列产品 平台示例（场景 11-19）
./demo 11 64 128 128
./demo 13 256 128 128
```

运行参数顺序为 `SCENARIO_NUM M K N`：

| 参数 | 说明 |
|------|------|
| `SCENARIO_NUM` | 测试场景编号，Atlas A3/A2 训练/推理平台使用 1-9，Ascend 950PR&950DT系列产品 平台使用 11-19 |
| `M` | 矩阵 A 的行数 |
| `K` | 矩阵 A 的列数（矩阵 B 的行数） |
| `N` | 矩阵 B 的列数 |

## 性能数据获取

使用 `msOpProf` 工具获取详细性能数据：

```bash
msopprof ./demo 1 128 128 128
```

  > 💡 使用 `msOpProf` 工具需安装 CANN 商用/社区版，详细信息可参考[msOpProf工具安装指南](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/install_guide/msopprof_install_guide.md)。

  命令完成后，会在默认目录下生成以`OPPROF_{timestamp}_XXX`命名的文件夹，性能数据文件夹结构示例如下：

  ```bash
  ├── dump                       # 原始性能数据
  ├── ArithmeticUtilization.csv  # cube/vector指令cycle占比
  ├── L2Cache.csv                # L2 Cache命中率，影响MTE2，建议合理规划数据搬运逻辑，增加命中率
  ├── Memory.csv                 # UB、L1和主存储器读写带宽速率
  ├── MemoryL0.csv               # L0A、L0B和L0C读写带宽速率
  ├── MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
  ├── OpBasicInfo.csv            # 算子基础信息
  ├── PipeUtilization.csv        # 计算单元和搬运单元耗时及占比
  ├── ResourceConflictRatio.csv  # UB bank group、bank conflict和资源冲突率占比
  └── visualize_data.bin         # MindStudio Insight呈现文件
  ```

本样例主要关注L1->L0的性能数据，通过如下方法查看具体的性能数据结果：

```bash
cat ./OPPROF_*/PipeUtilization.csv
```

主要关注如下指标：

| 指标                    | 说明                                                                   |
|------------------------|------------------------------------------------------------------------|
| aic_mte1_time(us)      | mte1 类型指令（L1 -> L0A/L0B 搬运以及L1 到 Bias Table Buffer 搬运）耗时   |
| aic_fixpipe_time(us)   | fixpipe 类型指令（L1 到 Fixpipe Buffer 搬运）耗时                        |


## 性能测试脚本

`perf.sh` 用于批量编译、执行 `msopprof`、提取性能指标并生成 CSV 汇总结果。

```bash
# 查看帮助
./perf.sh --help

# 测试场景 1，默认使用 dav-2201
./perf.sh 1

# 测试场景 11，默认使用 dav-3510
./perf.sh 11

# 明确指定平台
./perf.sh 1 dav-2201
./perf.sh 11 dav-3510
```

脚本默认测试 5 组 shape：

| Test_ID | M | K | N | 数据量说明 |
|---------|---|---|---|------------|
| 1 | 32 | 32 | 32 | 小数据量 |
| 2 | 64 | 64 | 64 | 中小数据量 |
| 3 | 64 | 128 | 64 | 中小数据量 |
| 4 | 128 | 128 | 128 | 中等数据量 |
| 5 | 256 | 128 | 256 | 约 64KB，接近 L0 满载 |

其中，A矩阵搬运数据量为M * K，B矩阵搬运数据量为K * N，Bias Table Buffer和Fixpipe Buffer搬运数据量为N，可以按需调整。

测试完成后，结果保存在 `perf_data_${timestamp}_scenario${SCENARIO}/perf_result_scenario${SCENARIO}.csv`，原始 `msopprof` 数据保存在同目录下的 `test_${id}_${M}_${K}_${N}` 子目录。

## 性能指标说明

`perf.sh` 根据场景自动选择性能指标：

| 场景 | 指标 | 说明 |
|------|------|------|
| 9、19 | `aic_fixpipe_time(us)` | FixPipe （L1 到 Fixpipe Buffer 搬运）执行时间 |
| 其他场景 | `aic_mte1_time(us)` | MTE1（L1 到 L0A/L0B 搬运或 L1 到 Bias Table Buffer 搬运）执行时间 |

CSV 中的计算列如下：

| 列名 | 计算方式 | 说明 |
|------|----------|------|
| `Cycle` | `Time(us) * Frequency(MHz)` | 根据平台主频折算的 Cycle 数 |
| `Bandwidth(GB/s)` | `DataSize(bytes) / Time(us) / 1e3` | 数据搬运带宽 |

### 性能指标计算方法

`msopprof` 在 `PipeUtilization.csv` 中采集到的 `aic_mte1_time(us)`、`aic_fixpipe_time(us)` 均为微秒（us）单位的耗时。`perf.sh` 会读取对应时间列，并结合平台主频和测试数据量计算 Cycle 数与实测带宽。

#### Time(us) 转换为 Cycle

主频单位为 MHz，含义是每微秒的周期数。因此 `Time(us)` 转换为 Cycle 时不需要额外换算：

```text
Cycle = Time(us) * Frequency(MHz)
```

例如 当 Atlas A3/A2 训练/推理平台主频为 1800 MHz 时，若 `msopprof` 采集到 `aic_mte1_time(us) = 0.021111`：

```text
Cycle = 0.021111 * 1800 = 38.00 cycles
```

当 Ascend 950PR&950DT系列产品 平台主频为 1650 MHz 时，若 `msopprof` 采集到 `aic_mte1_time(us) = 0.030000`：

```text
Cycle = 0.030000 * 1650 = 49.50 cycles
```

#### 数据搬运量计算

`perf.sh` 按场景选择数据搬运量，作为带宽计算的 `DataSize(bytes)`：

| 场景 | 数据量计算 | 数据类型大小 |
|------|------------|--------------|
| 1、3、5、11、15 | `M * K * sizeof(bfloat16)` | 2 bytes |
| 2、4、6、12、16、17 | `K * N * sizeof(bfloat16)` | 2 bytes |
| 7 | `K * N * sizeof(int8_t)` | 1 byte |
| 8、18 | `N * sizeof(float)` | 4 bytes |
| 13 | `M * K * sizeof(fp8_e4m3fn)` | 1 byte |
| 14 | `K * N * sizeof(fp8_e4m3fn)` | 1 byte |
| 9、19 | `N * sizeof(uint64_t)` | 8 bytes |
| 其他场景 | 默认按 `M * K * sizeof(bfloat16)` 计算 | 2 bytes |

Bias 场景在 kernel 中会根据平台要求做 C1 到 C2 的块长对齐。`perf.sh` 默认按有效 bias 元素量 `N * sizeof(float)` 计算带宽；如果需要分析硬件实际搬运粒度，可结合 `load_data_perf.asc` 中的 `biasSizeAlign` 对齐规则单独折算。

#### 实测带宽计算

带宽按 GB/s 输出。由于 `Time(us)` 是微秒，`DataSize(bytes) / Time(us)` 的单位是 MB/s，继续除以 `1e3` 后得到 GB/s：

```text
Bandwidth(GB/s) = DataSize(bytes) / Time(us) / 1e3
```

例如场景 1，shape 为 `[128, 128, 128]`，数据类型为 bfloat16：

```text
DataSize = 128 * 128 * 2 = 32768 bytes
Time = 0.100000 us
Bandwidth = 32768 / 0.100000 / 1e3 = 327.680 GB/s
```

#### 理论耗时与带宽利用率

场景表中的“理论带宽(Byte/cycle)”和“带宽延迟(cycle)”可用于估算理论耗时。固定延迟表示一次搬运的基础启动开销，理论传输时间可按如下方式估算：

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

当理论带宽为范围值（例如 `32-256` Byte/cycle）或复合值（例如 `256/32` Byte/cycle）时，需要结合具体接口搬运模式、数据排布以及 scale 数据路径选择对应的理论带宽进行估算。

平台主频由脚本按场景自动设置：

| 平台 | 架构代号 | 主频 | 适用场景 |
|------|----------|------|----------|
| Atlas A3/A2 训练/推理平台 | dav-2201 | 1800 MHz | 1-9 |
| Ascend 950PR&950DT系列产品 平台 | dav-3510 | 1650 MHz | 11-19 |

## Roofline 分析

本样例提供 `generate_roofline_with_latency.py`，可基于 `perf.sh` 生成的 CSV 输出 ASCII 报告和图片。

### Python 包依赖

`generate_roofline_with_latency.py` 使用 Python 标准库读取 CSV 并生成 ASCII 报告；如果需要同时生成 PNG/PDF 图表，需要安装 `matplotlib` 和 `numpy`。

```bash
python3 -m pip install --user matplotlib numpy
```

如果未安装上述依赖，脚本仍可生成 `.txt` 格式的 ASCII 分析报告，但会跳过图片生成。

```bash
# 自动查找最新 perf_data 目录中的结果
python3 generate_roofline_with_latency.py

# 指定 CSV 文件
python3 generate_roofline_with_latency.py --csv perf_data_20260613_164620_scenario1/perf_result_scenario1.csv

# 自定义理论参数
python3 generate_roofline_with_latency.py \
  --csv perf_data_20260613_164620_scenario1/perf_result_scenario1.csv \
  --peak-bw 256 \
  --latency 30 \
  --frequency 1800 \
  --l0-size 64
```

默认输出文件前缀为 `${perf_data目录名}_roofline`，生成的 `.txt` 文件为 ASCII 详细分析，安装 `matplotlib` 后会同时生成 `.png` 图表。

### 图表示例

以下为场景 1 生成的 Roofline 图示例：

![场景 1 Roofline 图](figures/perf_data_scenario1_roofline.png)

## 注意事项

1. 场景编号与平台架构必须匹配：Atlas A3/A2 训练/推理平台使用场景 1-9，Ascend 950PR&950DT系列产品 平台使用场景 11-19。
2. 场景 7（LoadSparse）仅支持 dav-2201 架构。
3. 场景 13/14（LoadData（MX矩阵搬运））仅支持 dav-3510 架构。
4. 不同场景对矩阵规格有不同的对齐要求，建议使用符合对齐要求的矩阵规格（如 16、32、64 的倍数）。
