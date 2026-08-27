# 基于Add的CAmodel仿真样例说明

## 概述

本样例以Add计算为载体，展示Ascend C程序在CAmodel仿真模式下的编译、运行、结果校验与性能数据采集流程。用户可通过`msopprof simulator`获取仿真性能数据，用于分析性能瓶颈，提升性能分析效率。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本
- \>= CANN 9.1.0

## 目录结构介绍

```
├── 07_simulator
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读写辅助函数
│   ├── README.md               // 样例说明
│   ├── scripts                 // 输入数据生成和结果校验脚本
│   └── add.asc                 // SIMT实现Add调用样例
```

## 样例描述

- 样例功能

  本样例基于Ascend C SIMT编程方式实现两个输入张量逐元素相加得到输出张量，计算公式如下：

  ```text
  z = x + y
  ```

  样例中`x`和`y`的shape为`[48,256]`，数据类型为`float`。`scripts/gen_data.py`用于生成输入数据和golden数据，样例执行后将结果写入`output/output.bin`，再通过`scripts/verify_result.py`完成结果校验。

- 样例规格

  <table border="2">
  <caption>表1：Add样例规格描述</caption>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Add</td></tr>
  <tr><td align="center">字段</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">x</td><td align="center">[48,256]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[48,256]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">样例输出</td><td align="center">z</td><td align="center">[48,256]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">add_custom</td></tr>
  </table>

## 编译运行

在本样例根目录下执行如下步骤，编译并执行程序。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j
  python3 ../scripts/gen_data.py
  msopprof simulator --soc-version=<soc_version> ./demo
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin
  ```

  请根据实际测试的NPU硬件架构选择对应的`CMAKE_ASC_ARCHITECTURES`参数。

  | 选项 | 说明 |
  |------|------|
  | `CMAKE_ASC_RUN_MODE` | 指定为`sim`，开启NPU仿真模式 |
  | `CMAKE_ASC_ARCHITECTURES` | 指定NPU架构版本号。本样例仅支持`dav-3510`（Ascend 950PR/Ascend 950DT） |

  > 💡 CMakeLists.txt中已添加`-g`编译选项，用于生成调试信息，使仿真器能够采集指令流水图。如需在自有工程中使用仿真调优功能，请确保编译时同样添加该选项。

  精度对比成功时，输出如下：

  ```bash
  test pass!
  ```

## 仿真调优

基于`./demo`，可通过`msopprof simulator`进行仿真性能分析，生成可视化的指令流水图等信息，指令如下：

```bash
msopprof simulator --soc-version=<soc_version> ./demo
```

> AI处理器的型号`<soc_version>`请通过如下方式获取：
>
> - Ascend 950PR/Ascend 950DT
>   - 针对以上产品型号，在安装昇腾AI处理器的服务器执行`npu-smi info -t board -i <id> -c <chip_id>`命令进行查询，获取**Chip Name**和**NPU Name**信息，实际配置值为Chip Name_NPU Name。例如**Chip Name**取值为Ascendxxx，**NPU Name**取值为1234，实际配置值为Ascendxxx_1234。
>
> 其中，`id`为设备ID，通过`npu-smi info -l`命令查出的NPU ID即为设备ID；`chip_id`为芯片ID，通过`npu-smi info -m`命令查出的Chip ID即为芯片ID。

命令完成后，会在当前目录下生成以`OPPROF_{timestamp}_XXX`命名的文件夹，产物结构如下：

```
OPPROF_{timestamp}_XXX/
├── dump                    // 原始性能数据，用户无需关注
└── simulator
    ├── core*.veccore*/     // 各向量核的仿真指令流水图文件
    └── visualize_data.bin  // MindStudio Insight呈现文件
```

执行后可通过以下方式查看指令流水图：

- **MindStudio Insight**：打开`visualize_data.bin`进行可视化呈现。

  更多详细内容可以查看msOpProf工具使用方式，[MindStudio工具调优（msOpProf）快速入门](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/quick_start/msopprof_quick_start.md)。
