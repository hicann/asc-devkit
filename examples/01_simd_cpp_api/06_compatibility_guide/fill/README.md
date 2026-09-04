# Fill兼容性样例


## 概述

本样例展示如何使用Fill接口对L0A Buffer和L0B Buffer进行初始化，通过编译时宏隔离不同硬件实现。

- 在Atlas A2/A3 系列产品中，可以直接使用Fill接口进行L0A/L0B Buffer的初始化。
- 但在Ascend 950PR/Ascend 950DT平台中，由于删除了L0A Buffer/L0B Buffer初始化的相关硬件指令，因此无法直接使用Fill接口进行L0A/L0B Buffer的初始化，需要先对L1 Buffer初始化，再将初始化数据搬入L0A/L0B中，来间接完成L0A/L0B Buffer的初始化。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```
├── fill
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── fill.asc                // AscendC样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例规格

<table>
<caption>样例规格表</caption>
<tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[128, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[128, 64]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[128, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">fill</td></tr>
</table>

> **说明：** 规格表中的 x、y 仅用于端到端流程占位（生成输入文件与 golden），核函数实际不读取这两个 GM 输入，而是使用 `Fill` 接口构造全 1 矩阵参与 `Mmad` 计算，因此 x、y 的取值不影响计算结果。

### 样例实现

  1. 首先初始化 L0A Buffer 和 L0B Buffer，不同硬件架构方式不一样：
     - Atlas A2/A3 训练/推理系列产品：调用`Fill`接口直接初始化 L0A Buffer 和 L0B Buffer 为指定值（本样例初始化为1）。
     - Ascend 950PR/950DT：使用`Fill`接口初始化 L1 Buffer 为指定值（本样例初始化为1），再通过`LoadData`接口搬运到 L0A Buffer 和 L0B Buffer。
  2. 调用`Mmad`接口进行矩阵乘计算。
  3. 通过`Fixpipe`接口将结果搬运到Global Memory。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # 编译工程（Atlas A2/A3系列产品）
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用CPU调试或NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=cpu`或`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  针对Ascend 950PR/950DT编译：

  ```bash
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # 编译工程（Ascend 950PR/950DT）
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功：

  ```bash
  test pass!
  ```
