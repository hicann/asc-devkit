# asc_copy_l0c2l1样例

## 概述

本样例介绍如何使用SIMD C API `asc_copy_l0c2l1`将矩阵乘的结果从L0C Buffer搬出到L1 Buffer，支持多种数据类型转换、随路量化和ReLU等功能。该接口用于将L0C Buffer中的矩阵乘计算结果高效地传输到L1 Buffer中，并支持各种数据格式转换和预处理能力。

注意：Ascend 950PR/Ascend 950DT不支持将数据从L1 Buffer直接搬运到GM。本样例调用`asc_copy_l0c2l1`完成L0C Buffer到L1 Buffer的搬运后，直接将第一次矩阵乘产生的L0C Buffer结果搬运到GM。最终生成的`output.bin`为float类型的原始矩阵乘结果，用于验证矩阵乘计算，不对L1 Buffer中的类型转换、量化或ReLU结果进行数值校验。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```
├── asc_copy_l0c2l1
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   │   └── verify_result.py           // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── asc_copy_l0c2l1.asc             // SIMD C API样例实现及调用样例
│   └── README.md                      // 样例说明文档
```

## `asc_copy_l0c2l1`参数说明

本样例面向`dav-3510`，通过[`asc_copy_l0c2l1`](../../../../../docs/zh/api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md)的位置、形状、步长和功能控制参数描述L0C Buffer到L1 Buffer搬运；Scalar和Vector量化参数由配套配置接口设置。

<a name="表1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">表1：主要参数与配置接口</span></caption>
<tr><td align="center">参数或接口</td><td align="center">说明</td></tr>
<tr><td align="center"><code>n_size</code> / <code>m_size</code></td><td>源Nz矩阵在N、M方向上的大小，本样例均为128</td></tr>
<tr><td align="center"><code>src_stride</code></td><td>源Nz矩阵中相邻Z排布的起始地址偏移，单位为64字节</td></tr>
<tr><td align="center"><code>dst_stride</code></td><td>目的L1 Buffer Nz矩阵中相邻Z排布的元素偏移；half和int8_t场景分别按16和32个元素的C0配置</td></tr>
<tr><td align="center"><code>unit_flag_mode</code></td><td>矩阵计算和搬出的细粒度并行控制，本样例使用<code>DISABLE</code></td></tr>
<tr><td align="center"><code>quant_pre_mode</code></td><td>场景1、4使用<code>F322F16</code>，场景2、3分别使用<code>QF322B8_PRE</code>和<code>VQF322B8_PRE</code></td></tr>
<tr><td align="center"><code>relu_pre_mode</code></td><td>场景4使用<code>asc_relu_pre_mode::NORMAL</code>，其余场景使用<code>NONE</code></td></tr>
<tr><td align="center"><code>asc_set_l0c_copy_prequant</code></td><td>配置Scalar量化参数，场景2使用scale 2.0</td></tr>
<tr><td align="center"><code>asc_set_l0c_copy_config</code></td><td>配置Vector量化参数在Fixpipe Buffer中的地址，供场景3使用</td></tr>
</table>

## 场景详细说明

本样例通过编译参数 `SCENARIO_NUM` 选择不同的输出场景，`SCENARIO_NUM` 不同取值对应的含义如下表所示。
所有场景基于相同的矩阵乘规格：[M, N, K] = [128, 128, 128]，核函数名为 `asc_copy_l0c2l1_custom`。

<a name="表2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表2：scenarioNum不同取值的含义</span></caption>
<tr><td rowspan="1" align="center">scenarioNum</td><td align="center">L0C Buffer数据类型</td><td align="center">L1 Buffer数据类型</td><td align="center">L1 Buffer输出格式</td><td align="center">是否使能量化</td><td align="center">是否使能ReLU</td></tr>
<tr><td align="center">1</td><td align="center">float</td><td align="center">half</td><td align="center">Nz</td><td align="center">否(cast)</td><td align="center">否</td></tr>
<tr><td align="center">2</td><td align="center">float</td><td align="center">int8_t</td><td align="center">Nz</td><td align="center">是(scalar)</td><td align="center">否</td></tr>
<tr><td align="center">3</td><td align="center">float</td><td align="center">int8_t</td><td align="center">Nz</td><td align="center">是(vector)</td><td align="center">否</td></tr>
<tr><td align="center">4</td><td align="center">float</td><td align="center">half</td><td align="center">Nz</td><td align="center">否(cast)</td><td align="center">是</td></tr>
</table>

**场景1：输出格式Nz，输出到L1 Buffer的数据类型为half**
- 输入：A [128, 128] half类型，ND格式；B [128, 128] half类型，ND格式
- L1 Buffer输出：C [128, 128] half类型，Nz格式
- 实现：以`asc_quant_mode::F322F16`调用`asc_copy_l0c2l1`，将float累加结果转换为half并输出Nz格式
- 说明：L0C Buffer数据为Nz格式直接输出到L1 Buffer的Nz格式，数据保持原格式不变

**场景2：输出格式Nz，输出到L1 Buffer的数据类型为int8_t，使能Scalar量化**
- 输入：A [128, 128] half类型，ND格式；B [128, 128] half类型，ND格式
- L1 Buffer输出：C [128, 128] int8_t类型，Nz格式
- 实现：调用`asc_set_l0c_copy_prequant(2.0F, 0, true)`配置Scalar量化参数，再以`asc_quant_mode::QF322B8_PRE`调用`asc_copy_l0c2l1`
- 说明：将float类型数据量化为int8_t类型，整个C矩阵使用一个量化参数

**场景3：输出格式Nz，输出到L1 Buffer的数据类型为int8_t，使能Vector量化**
- 输入：A [128, 128] half类型，ND格式；B [128, 128] half类型，ND格式
- L1 Buffer输出：C [128, 128] int8_t类型，Nz格式
- 实现：通过`asc_copy_gm2l1`和`asc_copy_l12fb`将每列量化参数搬到Fixpipe Buffer，调用`asc_set_l0c_copy_config`配置参数地址，再以`asc_quant_mode::VQF322B8_PRE`调用`asc_copy_l0c2l1`
- 说明：将float类型数据量化为int8_t类型，C矩阵的每一列对应一个量化参数，使用的量化参数需要从GM拷贝到L1 Buffer

**场景4：输出格式Nz，输出到L1 Buffer的数据类型为half，使能ReLU**
- 输入：A [128, 128] half类型，ND格式；B [128, 128] half类型，ND格式
- L1 Buffer输出：C [128, 128] half类型，Nz格式
- 实现：以`asc_quant_mode::F322F16`调用`asc_copy_l0c2l1`，并设置`relu_pre_mode = asc_relu_pre_mode::NORMAL`
- 说明：在数据从L0C Buffer搬出到L1 Buffer的过程中执行ReLU操作，即将负值置为0

以上每个场景在完成L0C Buffer到L1 Buffer搬运后，均通过`asc_copy_l0c2gm`将第一次矩阵乘产生的L0C Buffer结果直接搬运到GM，生成float类型、ND格式的`output.bin`。该文件用于验证原始矩阵乘结果，不用于校验L1 Buffer中的类型转换、量化或ReLU结果。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录；root用户默认为`/usr/local/Ascend`，非root用户默认为 `${HOME}/Ascend`。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # 编译工程（默认NPU模式）
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM  # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin # 验证原始矩阵乘结果是否正确
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```
  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU架构，对应Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | 1-4 | 场景编号 |

  执行结果如下，说明原始矩阵乘结果精度对比成功。

  ```bash
  test pass!
  ```
