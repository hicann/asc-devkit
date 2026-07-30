# reg_load_gather样例

## 概述

本样例使用Ascend C C API的Reg矢量计算接口，将Unified Buffer（UB）中的非连续数据加载到矢量数据寄存器，再写回UB并搬运至GM（Global Memory）。通过CMake参数`SCENARIO_NUM`选择以下场景：

| `SCENARIO_NUM` | 离散搬入场景 |
| --- | --- |
| 1 | 使用`asc_gather`按元素索引离散搬入数据。 |
| 2 | 使用`asc_gather_datablock`按32B DataBlock离散搬入数据。 |

## 支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构

```text
reg_load_gather/
├── scripts/
│   ├── gen_data.py                // 生成输入和golden数据
│   └── verify_result.py           // 校验输出数据
├── CMakeLists.txt                 // 编译配置
├── data_utils.h                   // 输入输出文件工具
├── reg_load_gather.asc            // Ascend C C API实现和调用入口
├── README.md                      // 中文说明文档
└── README_en.md                   // 英文说明文档
```

## 场景详细说明

输入数据先由MTE2从GM（Global Memory）搬运到UB（Unified Buffer）；随后，入口函数通过`asc_vf_call`调用SIMD VF函数，按单个矢量数据寄存器对应的元素数分块完成离散搬入和矢量数据寄存器到UB的存储；SIMD VF函数执行完成后，MTE3将结果从UB搬运回GM。内核末尾调用`asc_sync_pipe(PIPE_ALL)`，确保全部流水完成。

### 场景1：按元素索引离散搬入

- 使用`asc_loadalign`分两次将256个`uint16_t`索引加载到类型为`vector_uint16_t`的矢量数据寄存器中。
- 使用`asc_gather`按元素索引进行离散搬入，结果满足`dst[i] = src[index[i]]`。
- 使用`asc_storealign`将类型为`vector_half`的矢量数据寄存器中的结果写回UB。

| 项目 | 名称 | Shape | 数据类型 |
| --- | --- | --- | --- |
| 输入 | `src` | `[1, 1024]` | `half` |
| 输入 | `index` | `[1, 256]` | `uint16_t` |
| 输出 | `dst` | `[1, 256]` | `half` |

索引值表示相对于`src`首地址的元素下标，取值范围为`[0, 1023]`。

### 场景2：按DataBlock离散搬入

- 一个DataBlock为32B，对应16个`half`元素。
- 使用`asc_loadalign`分两次将16个有效`uint32_t`索引加载到类型为`vector_uint32_t`的矢量数据寄存器中。
- 使用`asc_gather_datablock`加载16个DataBlock，并使用`asc_storealign`连续写回256个`half`结果。

| 项目 | 名称 | Shape | 数据类型 |
| --- | --- | --- | --- |
| 输入 | `src` | `[1, 1024]` | `half` |
| 输入 | `index` | `[1, 16]` | `uint32_t` |
| 输出 | `dst` | `[1, 256]` | `half` |

`index`中的每个值表示相对于`src`首地址的字节偏移，必须为32B的整数倍。`src`包含1024个`half`元素，共2048B，因此`index`的合法取值为`0, 32, ..., 2016`。

## 编译与运行

### 配置环境变量

请根据当前环境中CANN开发套件包的安装方式配置环境变量：

```bash
source ${install_path}/cann/set_env.sh
```

> **说明：** `${install_path}`为CANN包安装目录；root用户默认为`/usr/local/Ascend`，非root用户默认为`${HOME}/Ascend`。

### 运行样例

在样例根目录执行以下命令。`SCENARIO_NUM`可取1或2，默认值为1。

```bash
SCENARIO_NUM=1                                                                    # 选择场景1
mkdir -p build && cd build;                                                        # 创建并进入build目录
cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # 编译工程，默认NPU模式
python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM                         # 生成测试输入数据和golden数据
./demo                                                                             # 执行样例
python3 ../scripts/verify_result.py output/output.bin output/golden.bin           # 校验输出数据与golden数据
```

默认使用NPU运行模式。使用NPU仿真模式时，增加`-DCMAKE_ASC_RUN_MODE=sim`：

```bash
cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
```

切换场景或运行模式前，请在`build`目录删除`CMakeCache.txt`后重新执行CMake，避免复用缓存配置。

### 编译选项

| 选项 | 可选值 | 说明 |
| --- | --- | --- |
| `SCENARIO_NUM` | `1`、`2` | 选择按元素索引或按DataBlock加载场景。 |
| `CMAKE_ASC_RUN_MODE` | `npu`、`sim` | 选择NPU运行或NPU仿真模式。 |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | 指定NPU架构，对应Ascend 950PR/Ascend 950DT。 |

## 运行结果

输出结果与golden数据一致时，校验脚本输出如下信息：

```bash
test pass!
```
