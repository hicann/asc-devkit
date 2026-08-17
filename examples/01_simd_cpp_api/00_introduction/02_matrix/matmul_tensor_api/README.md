# 基于Tensor API实现的静态Matmul算子样例

## 概述

本样例基于静态Tensor API编程范式实现多核矩阵乘计算，使用Tensor API的高层次抽象接口完成矩阵搬运、切片和乘加流程。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | > CANN 9.1.0 |

> **说明：** 该样例依赖尚未正式发布的CANN特性，请使用最新的CANN master包。

## 目录结构介绍

```text
├── matmul_tensor_api
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── matmul.asc              // Ascend C样例实现
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：
  Matmul计算公式：
  $$
  C = A * B
  $$
- 样例规格：
  本样例参数M = 512, N = 1024, K = 512，调用16个核完成计算，输入输出规格如下表所示：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">float16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">matmul_custom</td></tr>
  </table>

- 样例实现：
  - 实现流程：
    <table>
    <tr><th align="left">步骤</th><th align="left">Tensor API操作</th><th align="left">功能</th><th align="left">布局转换</th></tr>
    <tr><td align="left">1</td><td align="left">常量化Tiling参数</td><td align="left">通过模板参数传入kernel</td><td align="left">不涉及</td></tr>
    <tr><td align="left">2</td><td align="left">MakeTensor + Slice</td><td align="left">创建GM张量并切片获取当前核处理的数据块</td><td align="left">ND格式</td></tr>
    <tr><td align="left">3</td><td align="left">Copy(CopyGM2L1)</td><td align="left">将A矩阵和B矩阵数据从GM搬运到L1</td><td align="left">ND->NZ格式转换</td></tr>
    <tr><td align="left">4</td><td align="left">Copy(CopyL12L0A/B)</td><td align="left">将数据从L1搬运到L0A和L0B</td><td align="left">L1->L0A: NZ->NZ<br>L1->L0B: NZ->ZN</td></tr>
    <tr><td align="left">5</td><td align="left">Mmad</td><td align="left">完成矩阵乘加计算</td><td align="left">矩阵乘结果为NZ格式</td></tr>
    <tr><td align="left">6</td><td align="left">Copy(CopyL0C2GM)</td><td align="left">将L0C中的计算结果搬运到GM</td><td align="left">NZ->ND格式转换</td></tr>
    </table>

  - Tensor API核心接口：
    1. **张量创建接口**：使用MakeTensor + MakeMemPtr + MakeFrameLayout创建各级张量
       - GM张量：NDExtLayoutPtn布局（ND格式扩展）
       - L1/L0张量：NZLayoutPtn/ZNLayoutPtn布局（适配cube计算单元）

    2. **数据搬运接口**：使用Copy + CopyAtom抽象
       - CopyGM2L1：GM到L1的数据搬运，自动处理ND→NZ格式转换
       - CopyL12L0A/B：L1到L0A/L0B的数据搬运
       - CopyL0C2GM：L0C到GM的数据搬运，自动处理NZ→ND格式转换

    3. **矩阵乘接口**：使用Mmad + MmadAtom抽象
       - 自动管理累加控制（cmatrixInitVal参数）

    4. **切片接口**：使用Slice + MakeCoord + MakeShape获取张量的子区域
       - 实现分核逻辑：每个核处理不同的数据块
       - 实现分块逻辑：baseM/baseK/baseN的基础块切分

  - 调用实现
    使用内核调用符<<<>>>调用核函数。

## Tensor API实现特点

本样例主要使用以下Tensor API能力完成实现：

| 特性 | 本样例实现 |
|------|------------|
| 内存管理 | 使用数组方式分配内存，自动管理偏移 |
| 张量表示 | 使用张量对象直接描述GM、L1、L0数据 |
| 数据搬运 | 使用Copy完成跨层搬运 |
| 格式转换 | 依赖布局模式自动完成 NZ / ZN 转换 |
| 分核逻辑 | 使用Slice获取当前核处理的数据块 |
| 计算接口 | 使用Mmad完成矩阵乘加 |

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量，**当前仅支持使用[CANN master](../../../../../docs/zh/quick_start.md#cann-install)**。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

  > **说明：** 本样例仅支持dav-3510架构（对应 Ascend 950PR/Ascend 950DT）。

- 执行结果
  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
