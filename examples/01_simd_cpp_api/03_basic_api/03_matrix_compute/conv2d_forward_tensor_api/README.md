# Conv2D Forward Tensor API 样例

## 概述

本样例介绍如何使用Tensor API实现Conv2D Forward计算，展示FeatureMap从Global Memory搬入L1 Buffer、通过`CopyL12L0A`（Img2Col）完成空间展开、使用`Mmad`进行矩阵乘累加，并通过`CopyL0C2GM`将结果搬回Global Memory的完整流程。

本样例支持三种Global Memory输入/输出数据格式（NC1HWC0、NCHW、NHWC），通过编译时参数`SCENARIO_NUM`切换。选用3x3卷积核、padding、stride=2、dilation=2和多C1通道组合，演示常见卷积场景和Tensor API接口使用。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构介绍

```text
├── conv2d_forward_tensor_api
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   │   └── verify_result.py           // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── conv2d_forward_tensor_api.asc  // Ascend C Tensor API样例实现与调用样例
│   └── README.md                      // 样例说明文档
```

## 样例规格

本样例固定使用如下Conv2D规格，核函数名为 `conv2d_forward_tensor_api_custom`。每个AI Core处理一个batch，共使用4个AI Core。

<a name="表1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表1：样例规格</span></caption>
<tr><td rowspan="1" align="center">参数</td><td align="center">取值</td><td align="center">说明</td></tr>
<tr><td align="center">FeatureMap</td><td align="center">[N, H, W, C] = [4, 9, 9, 32]</td><td>Global Memory输入为half类型，按场景选择NC1HWC0/NCHW/NHWC排布</td></tr>
<tr><td align="center">Weight</td><td align="center">[Cout, C, Kh, Kw] = [16, 32, 3, 3]</td><td>Global Memory输入为half类型，按[K, Cout]二维排布展开</td></tr>
<tr><td align="center">卷积参数</td><td align="center">stride = [2, 2]，dilation = [2, 2]，padding = [2, 2, 2, 2]，padValue = 1</td><td>padding顺序为[top, bottom, left, right]</td></tr>
<tr><td align="center">Output</td><td align="center">[N, HOut, WOut, Cout] = [4, 5, 5, 16]</td><td>Global Memory输出为half类型，格式与输入一致</td></tr>
</table>

其中，`C1`和`C0`是FeatureMap在L1 Buffer上采用NC1HWC0格式时的通道拆分维度。`C0`表示每个通道块包含的元素个数，本样例中为16；`C1`表示通道块个数。由于`C = 32`，因此`C1 = C / C0 = 2`、`C0 = 16`。

有效卷积核尺寸计算如下：

$$
K_{\text{effective}} = dilation \times (K_h - 1) + 1 = 2 \times (3 - 1) + 1 = 5
$$

输出H/W计算如下：

$$
\begin{aligned}
H_{out} &= \frac{H + pad_{top} + pad_{bottom} - K_{\text{effective}}}{stride_h} + 1 = \frac{9 + 2 + 2 - 5}{2} + 1 = 5 \\
W_{out} &= \frac{W + pad_{left} + pad_{right} - K_{\text{effective}}}{stride_w} + 1 = \frac{9 + 2 + 2 - 5}{2} + 1 = 5
\end{aligned}
$$

矩阵乘规格为`M = HOut * WOut = 25`，`K = C * Kh * Kw = 288`，`N = Cout = 16`。

## 场景详细说明

本样例通过编译参数`SCENARIO_NUM`选择不同的Global Memory数据格式，`SCENARIO_NUM`不同取值对应的含义如下表所示。

<a name="表2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表2：SCENARIO_NUM不同取值的含义</span></caption>
<tr><td rowspan="1" align="center">SCENARIO_NUM</td><td align="center">FeatureMap输入布局</td><td align="center">Output输出布局</td><td align="center">Global Memory→L1 Buffer转换</td><td align="center">L0C Buffer→Global Memory转换</td></tr>
<tr><td align="center">0</td><td align="center">NC1HWC0 [1, C1, H, W, C0]</td><td align="center">NC1HWC0 [1, Cout_C1, HOut, WOut, C0]</td><td align="center">NC1HWC0直搬</td><td align="center">NC1HWC0直搬</td></tr>
<tr><td align="center">1</td><td align="center">NCHW [1, C, H, W]</td><td align="center">NCHW [1, Cout, HOut, WOut]</td><td align="center">NCHW→NC1HWC0（dn2nz）</td><td align="center">NC1HWC0→NCHW（nz2dn）</td></tr>
<tr><td align="center">2</td><td align="center">NHWC [1, H, W, C]</td><td align="center">NHWC [1, HOut, WOut, Cout]</td><td align="center">NHWC→NC1HWC0（nd2nz）</td><td align="center">NC1HWC0→NHWC（nz2nd）</td></tr>
</table>

**场景0：NC1HWC0输入输出**
- 输入：FeatureMap [1, 2, 9, 9, 16] half类型，NC1HWC0格式；Weight [K, Cout] half类型
- 输出：Output [1, 1, 5, 5, 16] half类型，NC1HWC0格式
- 实现：Global Memory→L1 Buffer使用`CopyGM2L1`直搬NC1HWC0数据，L0C Buffer→Global Memory使用`CopyL0C2GM`直搬

**场景1：NCHW输入输出**
- 输入：FeatureMap [1, 32, 9, 9] half类型，NCHW格式；Weight [K, Cout] half类型
- 输出：Output [1, 16, 5, 5] half类型，NCHW格式
- 实现：Global Memory→L1 Buffer使用`CopyGM2L1`将NCHW随路转换为NC1HWC0（dn2nz），L0C Buffer→Global Memory使用`CopyL0C2GM`将NC1HWC0转换为NCHW（nz2dn）

**场景2：NHWC输入输出**
- 输入：FeatureMap [1, 9, 9, 32] half类型，NHWC格式；Weight [K, Cout] half类型
- 输出：Output [1, 5, 5, 16] half类型，NHWC格式
- 实现：Global Memory→L1 Buffer使用`CopyGM2L1`将NHWC随路转换为NC1HWC0（nd2nz），L0C Buffer→Global Memory使用`CopyL0C2GM`将NC1HWC0转换为NHWC（nz2nd）

核心流程如下：

1. 将单batch FeatureMap从Global Memory搬到L1 Buffer，调用Copy接口通过`CopyGM2L1` `Operation`搬运能力，统一为NC1HWC0布局。`CopyGM2L1`根据Global Memory源端格式自动路由搬运路径（NCHW走dn2nz、NHWC走nd2nz、NC1HWC0直搬）。同时将Weight从Global Memory搬运到L1 Buffer，完成ND到ZN格式转换。
2. 将L1 Buffer上的FeatureMap执行img2col空间展开搬到L0A Buffer，调用Copy接口通过`CopyL12L0A` `Operation`的Img2Col搬运能力，完成3x3卷积核在stride=2、dilation=2下的展开与padding填充。
3. 将L1 Buffer上的Weight搬到L0B Buffer，调用Copy接口通过`CopyL12L0B` `Operation`搬运能力。
4. 使用 `Mmad` 完成矩阵乘计算，L0A Buffer(NZ) × L0B Buffer(ZN) → L0C Buffer(NZ)。
5. 使用 `CopyL0C2GM` 将L0C Buffer结果按目的格式搬回Global Memory（NCHW走nz2dn、NHWC走nz2nd、NC1HWC0直搬）。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  SCENARIO_NUM=0
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # 编译工程（默认dav-3510 NPU模式）
  python3 ../scripts/gen_data.py --scenario=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确
  ```

  切换输入/输出格式：将`-DSCENARIO_NUM=0`改为`1`（NCHW）或`2`（NHWC），重新编译运行即可。

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;  # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构：dav-3510对应Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0`（默认）、`1`、`2` | 场景编号：0=NC1HWC0，1=NCHW，2=NHWC |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
