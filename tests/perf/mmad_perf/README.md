# Mmad（Cube矩阵乘）性能测试样例

## 概述

本样例用于Mmad系列接口的矩阵乘加计算性能测试，覆盖矩阵A、B从L0A Buffer/L0B Buffer经Cube计算单元做矩阵乘、结果写入L0C Buffer的计算路径。本样例为无业务测试，不校验计算结果，只采集Cube计算耗时。

## 本样例支持的产品及CANN软件版本

| 产品 | 架构代号 | CANN软件版本 |
|------|----------|-------------|
| Ascend 950PR&950DT系列产品 | dav-3510 | >= CANN 9.1.0 |
| Atlas A3系列产品 | dav-2201 | >= CANN 9.0.0 |
| Atlas A2系列产品 | dav-2201 | >= CANN 9.0.0 |

## 目录结构介绍

```text
├── mmad_perf
│   ├── CMakeLists.txt                    // 编译工程文件
│   ├── mmad_perf.asc                     // Mmad性能测试实现与调用入口
│   ├── perf.sh                           // 性能测试脚本
│   ├── generate_roofline.py              // Roofline 生成脚本
│   ├── README.md                         // 样例说明文档
```

## 样例描述

本样例通过运行参数`SCENARIO_NUM`选择不同的数据类型与计算接口，矩阵规格通过`./demo SCENARIO_NUM M K N`在运行时传入，核函数名为`mmad_perf_custom`。

根据平台架构，支持的测试场景如下：

| 平台架构 | 支持场景 | 说明 |
|----------|----------|------|
| Atlas A3/A2训练/推理平台（dav-2201） | 1-4 | 普通Mmad的b8、b16、b32与结构化稀疏b8 |
| Ascend 950PR&950DT系列产品平台（dav-3510） | 11-15 | 普通Mmad的b8、b16、b32与MX量化mxfp8、mxfp4 |

### Atlas A3/A2训练/推理平台场景

| SCENARIO_NUM | 接口 | 左矩阵A | 右矩阵B | 结果矩阵C | 说明 |
|--------------|------|---------|---------|-----------|------|
| 1 | `Mmad`           | int8_t | int8_t | int32_t | b8普通矩阵乘 |
| 2 | `Mmad`           | half   | half   | float   | b16普通矩阵乘 |
| 3 | `Mmad`           | float  | float  | float   | b32普通矩阵乘 |
| 4 | `MmadWithSparse` | int8_t | int8_t | int32_t | b8的4:2结构化稀疏矩阵乘 |

### Ascend 950PR&950DT系列产品平台场景

| SCENARIO_NUM | 接口 | 左矩阵A | 右矩阵B | Scale | 结果矩阵C | 说明 |
|--------------|------|---------|---------|-------|-----------|------|
| 11 | `Mmad`   | int8_t       | int8_t       | -          | int32_t | b8普通矩阵乘 |
| 12 | `Mmad`   | half         | half         | -          | float   | b16普通矩阵乘 |
| 13 | `Mmad`   | float        | float        | -          | float   | b32普通矩阵乘 |
| 14 | `MmadMx` | fp8_e4m3fn_t | fp8_e4m3fn_t | fp8_e8m0_t | float   | mxfp8量化矩阵乘 |
| 15 | `MmadMx` | fp4x2_e2m1_t | fp4x2_e2m1_t | fp8_e8m0_t | float   | mxfp4量化矩阵乘 |

所有场景均不带bias。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

### 配置环境变量

请根据当前环境上CANN开发套件包的[安装方式](../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

```bash
source ${install_path}/cann/set_env.sh
```

`${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。`source`后可通过`echo ${ASCEND_HOME_PATH}`确认CANN包路径已就绪，`perf.sh`编译时默认从该路径推导`ASC_DIR`。

本样例在仿真环境运行，需额外配置仿真器动态库路径。不同架构对应不同仿真器型号：

```bash
# dav-2201（Atlas A3/A2训练/推理平台）
export LD_LIBRARY_PATH=${ASCEND_HOME_PATH}/tools/simulator/Ascend910B3/lib:$LD_LIBRARY_PATH

# dav-3510（Ascend 950PR&950DT系列产品平台）
export LD_LIBRARY_PATH=${ASCEND_HOME_PATH}/tools/simulator/Ascend950PR_9589/lib:$LD_LIBRARY_PATH
```

`perf.sh`会按场景自动选择对应型号并配置该路径，手动运行`msopprof simulator`时需自行export。

### 编译样例

针对Atlas A3/A2训练/推理平台（dav-2201）编译：

```bash
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DCMAKE_ASC_RUN_MODE=sim ..
make -j
cd ..
```

针对Ascend 950PR&950DT系列产品平台（dav-3510）编译：

```bash
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCMAKE_ASC_RUN_MODE=sim ..
make -j
cd ..
```

### 运行样例

运行参数顺序为`SCENARIO_NUM M K N`：

```bash
# Atlas A3/A2训练/推理平台示例（场景1-4）
./build/demo 1 128 128 128
./build/demo 4 128 256 128

# Ascend 950PR&950DT系列产品平台示例（场景11-15）
./build/demo 11 128 128 128
./build/demo 14 128 512 128
```

| 参数 | 说明 |
|------|------|
| `SCENARIO_NUM` | 测试场景编号，Atlas A3/A2训练/推理平台使用1-4，Ascend 950PR&950DT系列产品平台使用11-15 |
| `M` | 左矩阵A的行数 |
| `K` | 左矩阵A的列数，即右矩阵B的行数 |
| `N` | 右矩阵B的列数 |

矩阵规格需符合对齐要求：M、N取16的倍数，K取64的倍数（mx场景K必须为64的倍数，为统一建议所有场景K均取64的倍数）。

## 性能数据获取

使用`msopprof simulator`工具在仿真环境获取详细性能数据：

```bash
msopprof simulator build/demo 1 128 128 128
```

> 💡 使用`msopprof`工具需安装CANN商用/社区版，详细信息可参考[msOpProf工具安装指南](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/install_guide/msopprof_install_guide.md)。

命令完成后，会在默认目录下生成以`OPPROF_{timestamp}_XXX`命名的文件夹，仿真性能数据文件夹结构示例如下：

```text
├── dump                                          # 原始性能数据
└── simulator
    ├── trace.json                                # 整体时序 trace
    ├── visualize_data.bin                        # MindStudio Insight呈现文件
    └── core0.cubecore0
        ├── core0.cubecore0_instr_exe.csv         # Cube核逐指令执行明细（含cycles、running_time）
        ├── core0.cubecore0_code_exe.csv          # 按代码行聚合的执行明细
        └── trace.json                            # Cube核逐指令时序trace（含高精度dur）
```

本样例主要关注MMAD指令的性能数据，通过如下方法查看具体的性能数据结果：

```bash
cat ./OPPROF_*/simulator/core0.cubecore0/core0.cubecore0_instr_exe.csv
```

主要关注如下指标：

| 指标 | 来源 | 说明 |
|------|------|------|
| `dur(us)` | `core0.cubecore0/trace.json`中`MMAD`事件的`dur`字段 | MMAD指令持续时间（高精度）|
| `cycles` | `core0.cubecore0_instr_exe.csv`中`MMAD`行的`cycles`列 | MMAD指令周期数 |

## 性能测试脚本

`perf.sh`用于批量仿真编译、执行`msopprof simulator`、提取MMAD指令的持续时间与周期数并生成CSV汇总结果。

```bash
# 查看帮助
./perf.sh --help

# 测试场景1，默认使用dav-2201
./perf.sh 1

# 测试场景11，默认使用dav-3510
./perf.sh 11

# 明确指定平台，须与场景匹配，否则报错
./perf.sh 1 dav-2201
./perf.sh 14 dav-3510
```

脚本内置的默认shape序列：

| 场景 | 数据类型 | 默认shape序列（M K N） |
|------|------|--------------------------|
| 1 | b8（dav-2201） | (32,32,32) (64,64,64) (128,128,128) (128,256,128) (128,512,128) |
| 2 | b16（dav-2201） | (32,32,32) (64,64,64) (64,128,64) (128,128,128) (128,256,128) |
| 3 | b32（dav-2201） | (32,32,32) (64,64,64) (64,128,64) (128,128,128) |
| 4 | sparse b8（dav-2201） | (64,64,64) (64,128,64) (128,128,128) (128,256,128) (128,512,128) |
| 11 | b8（dav-3510） | (32,32,32) (64,64,64) (128,128,128) (128,256,128) (256,256,256) |
| 12 | b16（dav-3510） | (32,32,32) (64,64,64) (64,128,64) (128,128,128) (256,128,256) |
| 13 | b32（dav-3510） | (32,32,32) (64,64,64) (64,128,64) (128,128,128) |
| 14 | mxfp8（dav-3510） | (64,64,64) (64,128,64) (128,128,128) (128,256,128) (256,256,256) |
| 15 | mxfp4（dav-3510） | (64,64,64) (128,128,128) (128,256,128) (128,512,128) (256,512,256) |

如需针对性补点，可直接用`./build/demo SCENARIO_NUM M K N`跑单个shape。

测试完成后，结果保存在`perf_data_${timestamp}_scenario${SCENARIO}/perf_result_scenario${SCENARIO}.csv`，原始`msopprof`数据保存在同目录下的`test_${id}_${M}_${K}_${N}`子目录。

## 性能指标说明

`perf.sh`从仿真产物`core0.cubecore0/`中提取MMAD指令的持续时间与周期数，落盘到CSV。CSV中的列如下：

| 列名 | 来源 | 说明 |
|------|------|------|
| `MMAD_Dur(us)` | `core0.cubecore0/trace.json`中`MMAD`事件的`dur`字段 | MMAD指令持续时间（高精度，实测）|
| `Cycles` | `core0.cubecore0_instr_exe.csv`中`MMAD`行的`cycles`列 | MMAD指令周期数（实测）|

### 性能指标计算方法

仿真产物中，`Cycles`为MMAD指令实测周期数，`MMAD_Dur(us)`为对应的持续时间。`perf.sh`直接读取这两列落盘，实测性能与算力利用率由`generate_roofline.py`基于硬件并行度从`Cycles`推导，无需按主频折算。

#### Mmad性能计算

Mmad性能用`MAC/cycle`表达，计算量按MAC数`M*N*K`计（乘加算1次MAC）。各数据类型的硬件并行度（每cycle处理的元素块`cube_m*cube_n*cube_k`）与等效峰值算力如下：

| 架构 | 数据类型 | cube_m | cube_n | cube_k | k_divisor | 等效峰值算力（MAC/cycle）|
|------|------|----|----|----|-----------|--------------------------|
| dav-2201 | b8 | 16 | 16 | 32 | 1 | 8192 |
| dav-2201 | b16 | 16 | 16 | 16 | 1 | 4096 |
| dav-2201 | b32 | 16 | 16 | 4  | 1 | 1024 |
| dav-2201 | sparse b8 | 16 | 16 | 32 | 2 | 16384 |
| dav-3510 | b8 | 16 | 16 | 32 | 1 | 8192 |
| dav-3510 | b16 | 16 | 16 | 16 | 1 | 4096 |
| dav-3510 | b32 | 16 | 16 | 1  | 1 | 256 |
| dav-3510 | mxfp8 | 16 | 16 | 32 | 1 | 8192 |
| dav-3510 | mxfp4 | 16 | 16 | 64 | 1 | 16384 |

其中`k_divisor`为K方向有效缩减因子：sparse的4:2结构化稀疏中，右矩阵稠密化为`[K/2, N]`，硬件K方向实际只遍历`K/2`个分形，故计算cycle减半、等效峰值算力翻倍（`16*16*32*2=16384`）；普通场景`k_divisor=1`。

理论与实测计算公式（首指令开销按0计算）：

```text
计算cycle   = ceil(M/cube_m) * ceil(N/cube_n) * ceil(K / (cube_k * k_divisor))
理论总cycle = 计算cycle                   (首指令开销取0)
理论耗时us  = 理论总cycle / 主频(MHz)
等效峰值    = cube_m * cube_n * cube_k * k_divisor     (MAC/cycle)
理论性能    = M*N*K / 理论总cycle           (MAC/cycle, 等于等效峰值)

实测cycle   = Cycles（仿真 instr_exe.csv 中 MMAD 的 cycles，直接读取）
实测性能    = M*N*K / 实测cycle             (MAC/cycle)
算力利用率  = 实测性能 / 等效峰值 * 100%
```

例如场景1（dav-2201的b8，并行度16×16×32，k_divisor=1），shape为`[128, 128, 128]`：

```text
计算cycle   = ceil(128/16) * ceil(128/16) * ceil(128/32) = 8 * 8 * 4 = 256
理论总cycle = 256                              (首指令开销取0)
理论耗时    = 256 / 1800 = 0.1422 us
理论性能    = 128*128*128 / 256 = 8192 MAC/cycle（等于峰值8192）
```

平台主频由`perf.sh`按场景自动设置：

| 平台 | 架构代号 | 主频 | 适用场景 |
|------|----------|------|----------|
| Atlas A3/A2训练/推理平台 | dav-2201 | 1800MHz | 1-4 |
| Ascend 950PR&950DT系列产品平台 | dav-3510 | 1650MHz | 11-15 |

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
python3 generate_roofline.py --csv perf_data_xxx_scenario1/perf_result_scenario1.csv
```

默认输出文件前缀为`${perf_data目录名}_cube_roofline`，生成的`.txt`文件为ASCII详细分析，安装`matplotlib`后会同时生成`.png`图表。

### 图表示例

以下为场景1生成的Roofline图示例：

![场景1 Roofline图](figures/perf_data_scenario1_roofline.png)


## 注意事项

1. 场景编号与平台必须匹配：dav-2201用场景1-4，dav-3510用场景11-15，`perf.sh`会校验不匹配并报错。
2. 场景4（MmadWithSparse）仅支持dav-2201；场景14、15（MmadMx）仅支持dav-3510。
3. MmadMx约束：K必须是64的倍数；A、B起始地址在fp8场景按1024字节对齐，在fp4场景按512字节对齐。
4. 本样例为纯性能测试，不校验计算结果。核函数对L0 Buffer上数据不初始化，Mmad指令的执行与耗时只取决于shape与架构，与数据内容无关。
