# Mmad 计算方向优化样例

## 概述

本样例为Mmad计算方向优化样例，展示在开启UnitFlag的场景下，Mmad计算方向对性能的影响，当结果分形的生成顺序与搬出指令的搬出顺序保持一致时，可以获得更好的性能表现。

在开启UnitFlag时，当Mmad计算的结果分形生成顺序与搬出指令读取顺序一致时，搬出指令能更早读取到的结果分形，使计算与搬出可以按分形粒度重叠执行；当顺序不一致时，搬出指令需要先等待尚未生成的结果分形，造成额外的流水等待开销。计算与搬出指令的读取和写入方向示意图如下：

![计算和搬出同时沿M方向写入和读取](figures/mmad_fixpipe_direction.png)

本样例包含五组场景，每组同时提供推荐的正例和仅改变Mmad计算方向配置的反例：

1. 结果为float，Nz2Nz搬出：推荐使用`asc_set_mmad_direction_m()`；
2. 开启B4量化，Nz2Nz搬出，硬件自动使能ChannelMerge：推荐使用`asc_set_mmad_direction_n()`；
3. 结果为float，开启Nz2ND随路格式转换：推荐使用`asc_set_mmad_direction_n()`;
4. 结果为float，Nz2DN：推荐使用`asc_set_mmad_direction_m()`；
5. 开启B8量化，Nz2Nz搬出，硬件自动使能ChannelMerge：推荐使用`asc_set_mmad_direction_n()`。

## 支持的产品及CANN软件版本

**表1** 支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构

```text
01_mmad_direction
├── figures
│   └── mmad_fixpipe_direction.png    // 图示
├── scripts
│   ├── gen_data.py                   // 生成随机输入数据和真值数据
│   └── verify_result.py              // 校验输出结果
├── CMakeLists.txt                    // 编译工程文件
├── data_utils.h                      // 二进制数据读写函数
├── mmad_direction.asc                // Kernel及Host侧实现
├── README.md
└── README_en.md
```

## 样例描述

样例计算`C = A × B`。A和B的数据类型均为half，C在L0C Buffer中的数据类型为float。

**表2** 输入输出数据规格

| 数据 | 输入/输出 | 数据类型 | 维度 | 数据排布 |
| --- | --- | --- | --- | --- |
| A | 输入 | half | `[M, K]` | ND |
| B | 输入 | half | `[N, K]` | ND |
| C，场景1/2 | 输出 | float | `[M, N]` | Nz |
| C，场景3/4 | 输出 | int4b_t | `[M, N]` | Nz |
| C，场景5/6 | 输出 | float | `[M, N]` | ND |
| C，场景7/8 | 输出 | float | `[M, N]` | DN |
| C，场景9/10 | 输出 | int8_t | `[M, N]` | Nz |

Kernel按照以下顺序执行：

1. 调用`asc_set_gm2l1_nz_para`和`asc_copy_gm2l1_nd2nz`，将A和B从GM搬入L1 Buffer并转换为Nz；
2. 调用`asc_copy_l12l0a`和`asc_copy_l12l0b`，将A和B从L1 Buffer加载到L0A Buffer和L0B Buffer；
3. 根据搬出场景预置`asc_set_l0c_copy_prequant`或`asc_set_l0c_copy_nz_para`；
4. 在`asc_mmad`前调用方向配置接口，并使用`asc_unit_flag_mode::ENABLE_UPDATE`开启UnitFlag；
5. 调用一次`asc_mmad`完成矩阵乘，随后调用一次`asc_copy_l0c2gm`并使用`asc_unit_flag_mode::ENABLE_UPDATE`开启UnitFlag，将结果搬出。

### 场景配置

编译参数`SCENARIO_NUM`选择正例或反例。每组场景除Mmad计算方向外，其他各项配置均相同。

**表3** 各场景信息

| `SCENARIO_NUM` | 类型 | 搬出场景 | `[M, N, K]` | MMAD方向 | 说明 |
| ---: | --- | --- | --- | --- | --- |
| 1 | 正例 | float Nz2Nz + UnitFlag | `[256, 256, 128]` | `asc_set_mmad_direction_m()` | 普通Nz搬出按M方向读取。 |
| 2 | 反例 | float Nz2Nz + UnitFlag | `[256, 256, 128]` | `asc_set_mmad_direction_n()` | 生成顺序与搬出顺序不一致。 |
| 3 | 正例 | B4 ChannelMerge + UnitFlag | `[192, 64, 128]` | `asc_set_mmad_direction_n()` | ChannelMerge按N方向读取。 |
| 4 | 反例 | B4 ChannelMerge + UnitFlag | `[192, 64, 128]` | `asc_set_mmad_direction_m()` | 生成顺序与搬出顺序不一致。 |
| 5 | 正例 | float Nz2ND + UnitFlag | `[256, 256, 128]` | `asc_set_mmad_direction_n()` | Nz2ND按N方向读取。 |
| 6 | 反例 | float Nz2ND + UnitFlag | `[256, 256, 128]` | `asc_set_mmad_direction_m()` | 生成顺序与搬出顺序不一致。 |
| 7 | 正例 | float Nz2DN + UnitFlag | `[256, 256, 128]` | `asc_set_mmad_direction_m()` | Nz2DN按M方向读取。 |
| 8 | 反例 | float Nz2DN + UnitFlag | `[256, 256, 128]` | `asc_set_mmad_direction_n()` | 生成顺序与搬出顺序不一致。 |
| 9 | 正例 | B8 ChannelMerge + UnitFlag | `[256, 256, 64]` | `asc_set_mmad_direction_n()` | ChannelMerge按N方向读取。 |
| 10 | 反例 | B8 ChannelMerge + UnitFlag | `[256, 256, 64]` | `asc_set_mmad_direction_m()` | 生成顺序与搬出顺序不一致。 |

### 场景1/2：float Nz输出

场景1和场景2目的类型为float，量化模式为`NoQuant`，且未使能Nz2ND和Nz2DN，因此结果保持Nz排布且不使能ChannelMerge。普通Nz搬出按M方向读取L0C Buffer，因此场景1使用`asc_set_mmad_direction_m()`；场景2使用N方向作为反例。

### 场景3/4：B4 ChannelMerge

场景3和场景4使用`QF322S4_PRE`量化模式将float量化为`int4b_t`，并保持Nz2NZ输出。硬件自动使能ChannelMerge，将N方向连续的四个16×16分形合并为一个16×64分形。ChannelMerge按N方向读取L0C Buffer，因此场景3使用`asc_set_mmad_direction_n()`；场景4使用M方向作为反例。

### 场景5/6：float Nz2ND

场景5和场景6通过`asc_set_l0c_copy_nz_para`配置随路格式转换参数，并在`asc_copy_l0c2gm`中使能Nz2ND。Nz2ND按N方向读取L0C Buffer，因此场景5使用`asc_set_mmad_direction_n()`；场景6使用M方向作为反例。

### 场景7/8：float Nz2DN

场景7和场景8通过`asc_set_l0c_copy_nz_para`配置随路格式转换参数，并在`asc_copy_l0c2gm`中使能Nz2DN。Nz2DN按M方向读取L0C Buffer，因此场景7使用`asc_set_mmad_direction_m()`；场景8使用N方向作为反例。

### 场景9/10：B8 ChannelMerge

场景9和场景10使用`QF322B8_PRE`量化模式将float量化为`int8_t`，并保持Nz2NZ输出。硬件自动使能ChannelMerge，将N方向连续的两个16×16分形合并为一个16×32分形。ChannelMerge按N方向读取L0C Buffer，因此场景9使用`asc_set_mmad_direction_n()`；场景10使用M方向作为反例。

## 性能对比

**表4** 测试配置

| 配置项 | 配置值 |
| --- | --- |
| Block数 | 1 |
| Kernel内流水重复次数 | 50 |
| 均值来源 | msOpProf工具采集的样例端到端执行时间均值 |
| 优化降幅 | `(反例均值-正例均值)/反例均值×100%` |

**表5** 正例与反例的多次均值

| 场景 | 正例均值（μs） | 反例均值（μs） | 优化降幅 |
| --- | ---: | ---: | ---: |
| float Nz2Nz + UnitFlag | 61.295 ± 0.084 | 61.948 ± 0.141 | **1.055%** |
| B4 ChannelMerge + UnitFlag | 13.154 ± 0.053 | 15.149 ± 0.043 | **13.172%** |
| float Nz2ND + UnitFlag | 61.935 ± 0.113 | 112.891 ± 0.056 | **45.137%** |
| float Nz2DN + UnitFlag | 60.937 ± 0.170 | 61.653 ± 0.191 | **1.162%** |
| B8 ChannelMerge + UnitFlag | 60.909 ± 0.157 | 61.174 ± 0.112 | **0.435%** |

注意性能收益与数据量、Shape、切分方式和完整算子流水相关，表5中的百分比不能直接外推到其他场景，仅用于展示计算方向与性能优化的关系。

### 性能测试方法

### msOpProf工具

msOpProf可采集真实NPU或仿真环境中的单算子性能数据。命令完成后会生成`OPPROF_{timestamp}_*`目录，其中：

- `OpBasicInfo.csv`记录Task基础信息和端到端执行时间；
- `PipeUtilization.csv`记录Cube、Scalar、MTE和Fixpipe流水的耗时及占比；
- `Memory.csv`、`MemoryL0.csv`记录各级存储器访问指标。

使用msOpProf采集本样例Kernel性能数据的方式如下：

```bash
SCENARIO_NUM=1
rm -rf build
mkdir build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..
make -j
python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM
msopprof ./c_api_mmad_direction_example
python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM \
    output/output.bin output/golden.bin
```

## 编译运行

配置CANN环境变量：

```bash
source $INSTALL_PATH/cann/set_env.sh
```

其中`$INSTALL_PATH`为CANN包安装目录。在本样例目录编译并运行指定场景：

```bash
SCENARIO_NUM=1
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..
make -j
python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM
./c_api_mmad_direction_example
python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM \
    output/output.bin output/golden.bin
```

切换场景前需清理`build`目录中的CMake缓存。

**表6** 编译选项

| 编译选项 | 可选值 | 说明 |
| --- | --- | --- |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构。 |
| `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | NPU运行或NPU仿真；不支持CPU调试。 |
| `SCENARIO_NUM` | `1`至`10`，默认`1` | 选择表3中的场景。 |

精度校验成功时输出：

```text
test pass!
```
