# GroupBarrier样例

## 概述

本样例实现了两组存在依赖关系的AIV之间的正确同步，A组AIV计算完成后，B组AIV依赖该A组AIV的计算结果进行后续的计算，称A组为Arrive组，B组为Wait组。

> **注意：** 本样例仅适用于基于TPipe和TQue的编程模式。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── group_barrier
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── group_barrier.asc       // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：
  GroupBarrier样例实现了两组存在依赖关系的AIV之间的正确同步，A组AIV计算完成后，B组AIV依赖该A组AIV的计算结果进行后续的计算，称A组为Arrive组，B组为Wait组。本样例不进行输入输出计算，仅通过Arrive组写完指定数值之后，Wait组读取该数值，printf打印出正确的数值完成验证。

- 样例规格：
  <table>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">barGm</td><td align="center">[3072]</td><td align="center">uint8_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[8]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">kernel_group_barrier</td></tr>
  </table>

- 样例实现：

  GroupBarrier样例启用8个AIV核，其中2个AIV核作为Arrive组开启原子累加，将指定数值写入Global Memory中，并调用Arrive指令；其余6个AIV核首先调用Wait指令等待Arrive组完成写入，然后读出Global Memory并将结果通过printf打印出来。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j;    # 编译工程
  ./demo                           # 执行编译生成的可执行程序，执行样例
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` |  NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果

  执行结果如下，说明样例执行成功。

  ```bash
  [Block (0/6)]: OUTPUT = 24
  [Block (1/6)]: OUTPUT = 24
  [Block (2/6)]: OUTPUT = 24
  [Block (3/6)]: OUTPUT = 24
  [Block (4/6)]: OUTPUT = 24
  [Block (5/6)]: OUTPUT = 24
  ```
