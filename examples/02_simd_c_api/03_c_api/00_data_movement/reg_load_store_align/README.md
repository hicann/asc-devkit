# reg_load_store_align样例

## 概述

本样例展示使用Ascend C C API的Reg矢量计算接口，在Unified Buffer（UB）与矢量数据寄存器之间进行对齐数据读写，并通过多个场景演示不同的寻址和数据分布方式。

## 支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构

```text
reg_load_store_align/
├── scripts/
│   ├── gen_data.py                // 生成输入和golden数据
│   └── verify_result.py           // 校验输出数据
├── CMakeLists.txt                 // 编译配置
├── data_utils.h                   // 输入输出文件工具
├── reg_load_store_align.asc       // Ascend C C API实现和调用入口
├── README.md                      // 中文说明文档
└── README_en.md                   // 英文说明文档
```

## 场景详细说明

输入数据先由MTE2从GM（Global Memory）搬运至UB（Unified Buffer），入口函数通过`asc_vf_call`调用SIMD VF函数执行按寄存器位宽分块的加载、加法和存储操作，最后由MTE3将输出从UB搬运至GM；内核末尾通过`asc_sync_pipe(PIPE_ALL)`完成所有流水的收尾同步。

| 场景 | 输入长度（`src0`、`src1`） | 输入数据类型 | 数据读写方式 | 输出长度 | 说明 |
| --- | --- | --- | --- | --- | --- |
| 1 | 1024 | half | 开发者自定义的迭代间偏移 | 1024 | 连续读写1021个元素，最后3个元素保持为0。 |
| 2 | 1024 | half | postupdate模式表示迭代间偏移 | 1024 | 每次读写后自动更新地址，实现连续读写。 |
| 3 | 1024 | half | 地址寄存器（add_reg）表示迭代间偏移 | 1024 | 使用地址寄存器配置每次读写的偏移，实现连续读写。 |
| 4 | 1024 | half | 以DataBlock为单位非连续搬入 | 512 | 搬入时DataBlock的步长为2，即每次搬入完整的32B DataBlock后跳过一个DataBlock；结果连续写出。 |
| 5 | 1024 | half | 广播（broadcast）模式搬入 | 1024 | 将输入数据中每个256B数据段的首个half元素广播至矢量数据寄存器，以进行后续计算。 |
| 6 | 1024 | half | 上采样（upsample）模式搬入 | 2048 | 每次从两个输入分别加载64个half元素，将每个元素复制为两个相邻元素填充至矢量数据寄存器；执行加法后，连续写出128个half结果。 |

## 编译与运行

### 配置环境变量

请根据当前环境中CANN开发套件包的安装方式配置环境变量：

```bash
source ${install_path}/cann/set_env.sh
```

> **说明：** `${install_path}`为CANN包安装目录；root用户默认为`/usr/local/Ascend`，非root用户默认为`${HOME}/Ascend`。

### 运行样例

在样例根目录执行以下命令。`SCENARIO_NUM`可取1到6，默认值为1。

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
| `SCENARIO_NUM` | `1`-`6` | 选择寄存器对齐读写场景。 |
| `CMAKE_ASC_RUN_MODE` | `npu`、`sim` | 选择NPU运行或NPU仿真模式。 |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | 指定NPU架构，对应Ascend 950PR/Ascend 950DT。 |

## 运行结果

输出结果与golden数据一致时，校验脚本输出如下信息：

```bash
test pass!
```
