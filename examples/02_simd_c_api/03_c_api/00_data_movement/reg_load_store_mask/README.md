# reg_load_store_mask样例

## 概述

本样例演示Ascend C C API中掩码寄存器的加载、存储和使用方式，包含基础搬运和基于掩码寄存器的数据选择两个场景。

## 支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构

```text
reg_load_store_mask/
├── figures/
│   └── reg_load_store_mask.png    // 场景示意图
├── scripts/
│   ├── gen_data.py                // 生成输入和golden数据
│   └── verify_result.py           // 校验输出数据
├── CMakeLists.txt                 // 编译配置
├── data_utils.h                   // 输入输出文件工具
├── reg_load_store_mask.asc        // Ascend C C API实现和调用入口
├── README.md                      // 中文说明文档
└── README_en.md                   // 英文说明文档
```

## 场景详细说明

输入数据先由MTE2从GM（Global Memory）搬运至UB（Unified Buffer），入口函数通过`asc_vf_call`调用SIMD VF函数加载或生成掩码寄存器并完成寄存器操作，最后由MTE3将输出从UB搬运至GM；内核末尾通过`asc_sync_pipe(PIPE_ALL)`完成所有流水的收尾同步。

### 场景1：基础搬运场景

| 数据 | 名称 | 长度 | 数据类型 |
| --- | --- | --- | --- |
| 输入 | `src` | 1024 | `uint8_t` |
| 输出 | `dst` | 1024 | `uint8_t` |

实现过程如下：

![场景1掩码寄存器操作示意图](./figures/reg_load_store_mask.png)

1. 使用`asc_loadalign`将输入前32B数据加载至掩码寄存器。生成的输入中第1和第32个字节为1，其余字节为0，因此掩码寄存器中第1和第249个位置有效。
2. 使用`asc_duplicate_scalar`根据掩码寄存器，在矢量数据寄存器的指定位置填充数值2，再使用`asc_storealign`写回UB。输出中仅第1和第249个元素为2，其余元素保持为0。
3. 使用`asc_create_mask_b8(PAT_ALL)`生成全1掩码寄存器，再通过`asc_storealign`将对应的32B 0xFF数据写入UB中第257个至第288个元素。

### 场景2：复合计算场景

| 数据 | 名称 | 长度 | 数据类型 |
| --- | --- | --- | --- |
| 输入 | `src0` | 256 | `float` |
| 输入 | `src1` | 256 | `float` |
| 输入 | `mask` | 128 | `uint8_t` |
| 输出 | `dst` | 256 | `float` |

实现过程如下：

1. 将`src0`、`src1`和掩码数据从GM搬运至UB。
2. SIMD VF函数循环4次，每次使用`asc_loadalign_postupdate`加载64个float元素及对应的32B掩码数据至矢量数据寄存器和掩码寄存器。
3. 使用`asc_select`按掩码寄存器的比特位选择数据：掩码位为1时选择`src0`中的元素，为0时选择`src1`中的元素。
4. 使用`asc_storealign_postupdate`将每轮的64个float结果连续写回UB，随后由MTE3搬运至GM。

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
| `SCENARIO_NUM` | `1`、`2` | 选择掩码寄存器加载、存储和选择场景。 |
| `CMAKE_ASC_RUN_MODE` | `npu`、`sim` | 选择NPU运行或NPU仿真模式。 |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | 指定NPU架构，对应Ascend 950PR/Ascend 950DT。 |

## 运行结果

输出结果与golden数据一致时，校验脚本输出如下信息：

```bash
test pass!
```
