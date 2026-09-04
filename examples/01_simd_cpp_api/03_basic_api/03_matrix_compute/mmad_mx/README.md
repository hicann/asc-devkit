# Mmad MX样例

## 概述

本样例介绍了输入为ND格式，A、B矩阵为FP4（fp4x2_e1m2_t、fp4x2_e2m1_t）和FP8（fp8_e4m3fn_t、fp8_e5m2_t）数据类型，量化系数矩阵scaleA、scaleB为fp8_e8m0_t数据类型，左矩阵和左量化系数矩阵不转置、右矩阵和右量化系数矩阵转置共 4 种带有量化功能的矩阵乘（简称：MX矩阵乘法）。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```
├── mmad_mx
│   ├── figures                     // 图示
│   ├── scripts
│   │   ├── gen_data.py             // 输入数据和真值数据生成脚本
│   │   └── verify_result.py        // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读入写出函数
│   ├── mmad_mx.asc                 // Ascend C样例实现 & 调用样例
│   └── README.md                   // 样例说明文档
```

## 样例描述

一次完整的MX矩阵乘法涉及的数据搬运过程包括：GM -> L1、L1 -> L0A / L0AMX / L0B / L0BMX、L1 -> BT（BiasTable Buffer）、L0C -> GM，其中不同存储单元的数据排布格式，如下表1所示：

<a name="表1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表1：不同存储单元的数据排布格式</span></caption>
  <tr>
    <td >存储单元</td>
    <td>数据排布格式</td>
  </tr>
  <tr>
    <td>GM</td>
    <td>输入A、B矩阵和输出C矩阵为ND排列。</td>
  </tr>
  <tr>
    <td>L1</td>
    <td>A、B矩阵为Nz排列。</td>
  </tr>
  <tr>
    <td>L0A、L0AMX</td>
    <td>A、ScaleA矩阵分别为Nz、Zz排列。</td>
  </tr>
  <tr>
    <td>L0B、L0BMX</td>
    <td>B、ScaleB矩阵分别为Zn、Nn排列。</td>
  </tr>
  <tr>
    <td>BT（BiasTable Buffer）</td>
    <td>Bias是shape为[N]的一维Tensor。</td>
  </tr>
  <tr>
    <td>L0C</td>
    <td>C矩阵为Nz排列。</td>
  </tr>
</table>

MX矩阵乘法计算公式：$$ C = (ScaleA ⊗ A) x (ScaleB ⊗ B) + Bias $$
其中A、ScaleA、B、ScaleB、Bias、C矩阵的需要满足的shape分别为[M,K]、[M,ScaleK]、[K,N]、[ScaleK, N]、[N]和[M,N]，ScaleK = CeilAlign(CeilDiv(K, 32), 2)。

程序中scenarioNum参数不同取值对应的场景，如下表2所示：

<a name="表2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表2：scenarioNum不同取值的含义</span></caption>
  <tr>
    <td >scenarioNum</td>
    <td>A Type</td>
    <td>ScaleA Type</td>
    <td>B Type</td>
    <td>ScaleB Type</td>
    <td>Bias Type</td>
    <td>C Type</td>
    <td>Bias说明</td>
  </tr>
  <tr>
    <td>1</td>
    <td>fp4x2_e1m2_t</td>
    <td>fp8_e8m0_t</td>
    <td>fp4x2_e2m1_t</td>
    <td>fp8_e8m0_t</td>
    <td>float</td>
    <td>float</td>
    <td>不带Bias，C矩阵的初始值为0</td>
  </tr>
  <tr>
    <td>2</td>
    <td>fp4x2_e2m1_t</td>
    <td>fp8_e8m0_t</td>
    <td>fp4x2_e1m2_t</td>
    <td>fp8_e8m0_t</td>
    <td>float</td>
    <td>float</td>
    <td>带Bias，矩阵计算时不传入biasTensor，C矩阵的初始值来源于C2（BiasTable Buffer），<br>从C2的起始位置0开始读取</td>
  </tr>
  <tr>
    <td>3</td>
    <td>fp8_e4m3fn_t</td>
    <td>fp8_e8m0_t</td>
    <td>fp8_e5m2_t</td>
    <td>fp8_e8m0_t</td>
    <td>float</td>
    <td>float</td>
    <td>不带Bias，C矩阵累加来源于CO1初始值</td>
  </tr>
  <tr>
    <td>4</td>
    <td>fp8_e5m2_t</td>
    <td>fp8_e8m0_t</td>
    <td>fp8_e4m3fn_t</td>
    <td>fp8_e8m0_t</td>
    <td>float</td>
    <td>float</td>
    <td>带Bias，矩阵计算时传入biasTensor，C矩阵初始值来源于传入的biasTensor，<br>从传入biasTensor的起始位置开始读取</td>
  </tr>
</table>

### 场景详细说明
  本样例通过编译参数 `SCENARIO_NUM` 选择不同的场景，所有场景基于相同的矩阵乘规格：[M, N, K] = [40, 50, 70]，核函数名为 `KernelMmadMXCustom`。

**场景1 C矩阵初始值为0**

- 输入：A不转置 [40, 70] fp4x2_e1m2_t类型，ND格式；ScaleA不转置 [40, 4] fp8_e8m0_t类型，ND格式；B转置 [50, 70] fp4x2_e2m1_t类型，ND格式；ScaleB转置 [50, 4] fp8_e8m0_t类型，ND格式；不带Bias
- 输出：C [40, 50] float类型，ND格式
- 实现：使用`MmadMx`实现矩阵乘法运算，通过参数：`mmadParams.cmatrixInitVal = true`，设置C矩阵初始值为0

**场景2 C矩阵初始值来源于C2**
- 输入：A不转置 [40, 70] fp4x2_e2m1_t类型，ND格式；ScaleA不转置 [40, 4] fp8_e8m0_t类型，ND格式；B转置 [50, 70] fp4x2_e1m2_t类型，ND格式；ScaleB转置 [50, 4] fp8_e8m0_t类型，ND格式；带Bias，C矩阵初始值来源于C2
- 输出：C [40, 50] float类型，ND格式
- 实现：使用`MmadMx`实现矩阵乘法运算，不传入biasTensor，通过参数：`mmadParams.cmatrixInitVal = false、mmadParams.cmatrixSource = true`，设置C矩阵初始值来源于C2

**场景3 C矩阵初始值来源于CO1**
- 输入：A不转置 [40, 70] fp8_e4m3fn_t类型，ND格式；ScaleA不转置 [40, 4] fp8_e8m0_t类型，ND格式；B转置 [50, 70] fp8_e5m2_t类型，ND格式；ScaleB转置 [50, 4] fp8_e8m0_t类型，ND格式；不带Bias，C矩阵初始值来源于CO1
- 输出：C [40, 50] float类型，ND格式
- 实现：使用`MmadMx`实现矩阵乘法运算，通过参数：`mmadParams.cmatrixInitVal = false、mmadParams.cmatrixSource = false`，设置C矩阵初始值来源于CO1

**场景4 C矩阵初始值来源于biasTensor**
- 输入：A不转置 [40, 70] fp8_e5m2_t类型，ND格式；ScaleA不转置 [40, 4] fp8_e8m0_t类型，ND格式；B转置 [50, 70] fp8_e4m3fn_t类型，ND格式；ScaleB转置 [50, 4] fp8_e8m0_t类型，ND格式；带Bias，C矩阵初始值来源于传入的biasTensor
- 输出：C [40, 50] float类型，ND格式
- 实现：使用`MmadMx`实现矩阵乘法运算，传入biasTensor，该场景下`mmadParams.cmatrixSource`参数无效

### 矩阵乘法（MmadMx）

`MmadMx`指令会自动完成左、右矩阵与对应scale矩阵的广播乘法，k方向上每32个元素共享一个量化因子。以A、B矩阵均为FP4类型为例，下图展示了A、ScaleA、B、ScaleB的分形排布格式和量化功能原理：

<p align="center">
  <img src="figures/mmad-mx.png" width="1000">
</p>

<p align="center">
图1：带有量化功能的矩阵乘示意图
</p>

需要注意的是，mmadParams.k取alignK = CeilAlign(k, 64) = 128，而非原始的k=70，这是因为MX矩阵乘法中Mmad指令要求k方向向64对齐。

## 编译运行
在本样例根目录下执行如下步骤，编译并执行算子。
- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 安装Python依赖

  本样例的`gen_data.py`脚本需要使用`ml_dtypes`生成FP8输入数据，使用`en_dtypes`生成FP4输入数据。建议安装如下版本：

  ```bash
  python3 -m pip install ml_dtypes==0.2.0 en_dtypes==0.0.4
  ```

  如果未安装上述依赖，脚本无法正确生成FP4/FP8类型的输入数据和真值数据，可能导致精度校验失败。
  
- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU调试模式
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim`、`cpu` | 运行模式：NPU运行、NPU仿真、CPU调试 |
  | `SCENARIO_NUM` |  `1`（默认）、`2`、`3`、`4` | 场景编号，对应场景说明中的4种场景 |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
