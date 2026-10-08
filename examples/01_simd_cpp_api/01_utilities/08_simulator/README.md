# 基于MatmulLeakyRelu的CAmodel仿真样例说明


## 概述

本样例以MatmulLeakyRelu融合计算为载体，展示Ascend C程序在CAmodel仿真模式下的编译、运行、结果校验与性能数据采集流程。用户可通过`msopprof simulator`获取仿真性能数据，用于分析性能瓶颈、提升性能分析效率。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── 08_simulator
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读写辅助函数
│   ├── README.md               // 样例说明
│   ├── scripts                 // 输入数据生成和结果校验脚本
│   └── simulator.asc           // Ascend C样例实现和Host侧调用样例
```

## 样例描述

- 样例功能：
  实现Matmul和LeakyRelu融合计算，计算公式如下：

  Matmul计算：
  $$
  C_{ij} = \sum_{k} A_{ik} \times B_{kj}
  $$

  LeakyRelu计算：
  $$
  C_{ij} = \begin{cases}
  C_{ij} & \text{if } C_{ij} \geq 0 \\
  C_{ij} \times 0.001 & \text{if } C_{ij} < 0
  \end{cases}
  $$

  其中，A为左矩阵，形状为[M, K]；B为右矩阵，形状为[K, N]；C为输出矩阵，形状为[M, N]。

- 样例规格：
  本样例参数M = 512, K = 128, N = 128，调用2个Cube核和4个Vector核完成计算，输入规格如下表所示：

  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Matmul+LeakyRelu融合</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A（左矩阵）</td><td align="center">[512, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">B（右矩阵）</td><td align="center">[128, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">C</td><td align="center">[512, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
  </table>

## 编译运行

在本样例根目录下执行如下步骤，编译并执行程序。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  > 💡 使用 msOpProf 工具需安装 CANN 商用/社区版，详细信息可参考[msOpProf工具安装指南](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/install_guide/msopprof_install_guide.md)。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j
  python3 ../scripts/gen_data.py
  msopprof simulator --soc-version=Ascend910B1 ./demo
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin
  ```

  请根据实际测试的NPU硬件架构选择对应的`CMAKE_ASC_ARCHITECTURES`参数。

  | 选项 | 说明 |
  |------|------|
  | `CMAKE_ASC_RUN_MODE` | 指定为`sim`，开启NPU仿真模式 |
  | `CMAKE_ASC_ARCHITECTURES` | 指定NPU架构版本号。`dav-2201`对应Atlas A2系列产品和Atlas A3系列产品，`dav-3510`对应Ascend 950PR&950DT系列产品 |

  精度对比成功时，输出如下：

  ```bash
  test pass!
  ```

## 仿真调优

基于`./demo`可通过`msopprof simulator`进行仿真性能分析，生成可视化的指令流水图等信息，指令如下：

```bash
msopprof simulator --soc-version=<soc_version> ./demo
```

> AI处理器的型号`<soc_version>`请通过如下方式获取：
>
>- Atlas A2系列产品
>   - 针对以上产品型号：在安装昇腾AI处理器的服务器执行`npu-smi info`命令进行查询，获取**Name**信息。实际配置值为AscendName，例如**Name**取值为xxxyy，实际配置值为Ascendxxxyy。
>

> - Ascend 950PR&950DT系列产品
> - Atlas A3系列产品
>   - 针对以上产品型号，在安装昇腾AI处理器的服务器执行`npu-smi info -t board -i <id> -c <chip_id>`命令进行查询，获取**Chip Name**和**NPU Name**信息，实际配置值为Chip Name_NPU Name。例如**Chip Name**取值为Ascendxxx，**NPU Name**取值为1234，实际配置值为Ascendxxx_1234。
>
> 其中，`id`为设备ID，通过`npu-smi info -l`命令查出的NPU ID即为设备ID；`chip_id`为芯片ID，通过`npu-smi info -m`命令查出的Chip ID即为芯片ID。

命令完成后，会在当前目录下生成以`OPPROF_{timestamp}_XXX`命名的文件夹，产物结构如下：

```
OPPROF_{timestamp}_XXX/
├── dump                    // 原始性能数据，用户无需关注
└── simulator
    ├── core*.veccore*/     // 各向量核的仿真指令流水图文件
    ├── trace.json          // 仿真流水图和热点函数可视化呈现文件
    └── visualize_data.bin  // MindStudio Insight呈现文件
```

执行后可通过以下方式查看指令流水图：

- **MindStudio Insight**：打开`visualize_data.bin`或`trace.json`进行可视化呈现。

  更多详细内容可以查看msOpProf工具使用方式，[MindStudio工具调优（msOpProf）快速入门](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/quick_start/msopprof_quick_start.md)。
