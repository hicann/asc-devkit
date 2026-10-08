# MxFP4 Matmul 性能调优样例


## 概述

本样例以 MxFP4 矩阵乘法为例，介绍基于 Ascend C `Matmul` 高阶 API 的 MxMatmul 性能调优方法。样例包含两个场景（Case 1-2），均采用常量化 tiling，使用模板常量化 `MatmulApiStaticTiling`（静态 tiling）替代 runtime tiling 拷贝与计算。

**优化路径**：
- Case 1: 多核 MDL 常量化 tiling（scaleA/B 与 A/B 同步搬运）
- Case 2: 多核 MDL 常量化 tiling（在**GM→L1搬运中**，scaleA/B 相对 A/B 按照倍数多倍搬运，`mxTypePara`）

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

```
├── matmul_mxfp4_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── figures                 // 图示
│   ├── matmul_mx.asc           // Ascend C样例实现（包含2个优化case）
│   ├── matmul_mx.h             // 样例头文件（静态Tiling模板与kernel实现）
│   └── README.md               // 样例说明文档
```

## 样例描述

### 样例功能

  样例实现的是固定 shape 为 8192×8192 的 MxFP4 矩阵乘法（带 scale 量化系数输入）。

### MxMatmul 简介

MxMatmul（Matrix Multiply with Scale）是一种**带量化缩放系数的矩阵乘法**，是 Ascend C Matmul API
 在 MX（Mixed-Precision）量化场景下的扩展能力。与基础 Matmul 相比，MxMatmul 的核心区别在于引入了**scale 输入**。

#### 计算公式

$$
C = (\text{scaleA} \otimes A) \times (\text{scaleB} \otimes B)
$$

其中 $\otimes$ 表示广播乘（broadcast），左/右矩阵在左/右量化系数矩阵做乘积时，K 方向上每 32 个元素共享一个量化因子。

#### 参数说明

| 输入 | 名称 | 形状 | 数据类型 | 数据排布类型 | 说明 |
|------|------|------|----------|--------------|------|
| A | 左矩阵 | [8192, 8192] | `fp4x2_e1m2_t` | `ND` | MX FP4 左矩阵 |
| scaleA | 左量化系数矩阵 | [8192, 256] | `fp8_e8m0_t` | `ND` | A 矩阵的缩放因子矩阵，A矩阵 K 方向每 32 个元素共享一个缩放因子 |
| B | 右矩阵 | [8192, 8192] | `fp4x2_e1m2_t` | `ND` | MX FP4 右矩阵 |
| scaleB | 右量化系数矩阵 | [256, 8192] | `fp8_e8m0_t` | `ND` | B 矩阵的缩放因子矩阵，B矩阵 K 方向每 32 个元素共享一个缩放因子 |
| C | 输出 | [8192, 8192] | `bfloat16_t` | `ND` | 计算结果 |

  <img src="figures/MxMatmul.png">

#### 四路输入说明

- 样例中 `sK = ceil(K / 64) * 2`，当 `K=8192` 时，`sK=256`
- 因此 `scaleA` 形状为 `[M, sK] = [8192, 256]`，`scaleB` 形状为 `[sK, N] = [256, 8192]`
- `scale` 的 ND 需要特别说明：`scaleA` 按常规 row-major `[M, sK]` 写入；`scaleB` 的写盘顺序等价于 `[sK/2, N, 2]`，即先在 K 方向连续 `2 Byte`，再沿 N 方向推进，四路输入的 `ND` 排布如下图所示：
  <img src="figures/NDformat.png">

- 四路输入的搬运如下图所示：

  <img src="figures/InputOfMxMatmul.png">

## 样例实现

### 实现要点

本样例将 tiling 参数统一在 `matmul_mx.h` 中编译期确定，通过模板常量 `CONSTANT_CFG` 传给 `Matmul`：

```cpp
constexpr static auto CONSTANT_CFG = GetMxConstantCFG<aType, bType, cType, EnableScaleCache>();
AscendC::Matmul<aType, bType, cType, cType, CONSTANT_CFG,
                    AscendC::MatmulCallBackFunc<nullptr, nullptr, nullptr>,
                    AscendC::Impl::Detail::MatmulWithScalePolicy>
    matmulObj;
REGIST_MATMUL_OBJ(pipe, GetSysWorkSpacePtr(), matmulObj, (TCubeTiling*)nullptr);
```

说明：
- kernel 侧不做 `TCubeTiling` 运行时拷贝/计算。
- `SCENARIO_NUM` 只决定模板实例：`case1 -> MatmulKernel<false>`，`case2 -> MatmulKernel<true>`。

### Case1 与 Case2 差异

两种场景均为常量化 tiling，L1 参数一致：`depthA1/depthB1=4`、`stepKa/stepKb=2`、`stepM/stepN=1`、`dbL0A/dbL0B=2`。
唯一差异是 `mxTypePara`：

| 场景 | `mxTypePara` | 语义 |
|------|--------------|------|
| Case 1 (`SCENARIO_NUM=1`) | `CASE1_MX_TYPE_PARA = 0x01010101` | scaleA/B 与 A/B 同步搬运 |
| Case 2 (`SCENARIO_NUM=2`) | `CASE2_MX_TYPE_PARA = 0x01010404` | scaleA/B 在 K 方向相对 A/B 多搬运 |

`mxTypePara`  定义：

- 在MxMatmul中，可以通过设置mxTypePara来控制Scale矩阵和矩阵A、B在L1中加载的比例。
- **MX Scale 缩放因子**：`scaleFactorKa=4` 表示 scaleA 数据在 K 方向的加载比例为 A 矩阵的 4 倍；`scaleFactorKb=4` 表示 scaleB 数据在 K 方向的加载比例为 B 矩阵的 4 倍

    - **mxTypePara**：组合参数，在 MxMatmul 场景使用，表示 scaleA/scaleB 载入 L1 的大小与 A/B 矩阵载入 L1 大小的倍数：
      - **bit [0:6]** `scaleFactorKa`：scaleA 与 A 矩阵在 K 方向载入数据量的比例系数，范围 [1, 127]
      - **bit [8:14]** `scaleFactorKb`：scaleB 与 B 矩阵在 K 方向载入数据量的比例系数，范围 [1, 127]
      - **bit [16:22]** `scaleFactorM`：scaleA 与 A 矩阵在 M 方向载入数据量的比例系数，范围 [1, 127]
      - **bit [24:30]** `scaleFactorN`：scaleB 与 B 矩阵在 N 方向载入数据量的比例系数，范围 [1, 127]
    - 使用约束：
      - 仅当 Ka 方向全载时（`baseK * stepKa * scaleFactorKa >= singleCoreK`），才能设置 `scaleFactorM > 1`
      - 仅当 Kb 方向全载时（`baseK * stepKb * scaleFactorKb >= singleCoreK`），才能设置 `scaleFactorN > 1`
      - scaleA、scaleB 在 M、N、K 方向的载入数据量不能超过实际大小
      - 该参数仅在 MDL 模式下生效

### 参数设置与搬运数据量计算

以下统计搬运路径为 **GM->L1**，按当前固定参数计算：

| 参数 | 值 |
|------|----|
| `M` | `8192` |
| `N` | `8192` |
| `K` | `8192` |
| `singleCoreM` | `2048` |
| `singleCoreN` | `1024` |
| `singleCoreK` | `8192` |
| `baseM` | `256` |
| `baseN` | `256` |
| `baseK` | `256` |
| `stepKa` | `2` |
| `stepKb` | `2` |
| `scaleFactorKa (case1)` | `1` |
| `scaleFactorKb (case1)` | `1` |
| `scaleFactorKa (case2)` | `4` |
| `scaleFactorKb (case2)` | `4` |
| `数据类型` | A/B: `fp4x2` (`0.5 Byte/elem`) |
| `数据类型` | scale: `fp8` (`1 Byte/elem`) |

**说明**：

A/B 的 base 块大小：`baseM * baseK * 0.5 = 256 * 256 * 0.5 = 32,768 B = 32 KB`, scaleA/scaleB 的 base 块大小：`256 * (256/32) * 1 = 2,048 B = 2 KB`。

case1 单次 GM→L1 搬运量：

- A：`stepM * stepKa = 1 * 2 = 2` 个 base 块，字节量 `2 * 32 = 64 KB`
- B：`stepN * stepKb = 1 * 2 = 2` 个 base 块，字节量 `2 * 32 = 64 KB`
- scaleA：`stepM * stepKa * scaleFactorKa = 1 * 2 * 1 = 2` 个 base 块，`4 KB`
- scaleB：`stepN * stepKb * scaleFactorKb = 1 * 2 * 1 = 2` 个 base 块，`4 KB`
- **合计：`64 + 64 + 4 + 4 = 136 KB`**

> **说明**：`dbL0A/dbL0B=2` 表示 L1→L0 间采用 double buffer（L0 计算当前份的同时，下一份已从 L1 就位），因此 **L1 需要同时容纳 `136 × 2 = 272 KB`** 数据，但 GM→L1 每次 MTE2 搬运量仍为 `136 KB`。

case2 单次 GM→L1 搬运量：
- A/B 与 case1 相同：各 `64 KB`（合计 `128 KB`）
- scaleA：`1 * 2 * 4 = 8` 个 base 块，`16 KB`
- scaleB：`1 * 2 * 4 = 8` 个 base 块，`16 KB`
- **合计 MTE2 每次：`64 + 64 + 16 + 16 = 160 KB`**

> 同理，由于 `dbL0A/dbL0B=2`，**L1 驻留总量为 `160 × 2 = 320 KB`**。

Case1/Case2 在 scale 侧的差异主要体现在“单次搬运粒度与搬运次数”：

- Case1：每次少量搬运，scale 搬运次数约为 `16` 次（`8192/512`）
- Case2：每次多量搬运，scale 搬运次数约为 `4` 次（`8192/2048`）
- 在该 shape 下两者 scale 理论总字节量相同，但 Case2 的批次更少、复用窗口更大，更利于降低 MTE2 耗时。

## 性能对比总结

### Ascend 950PR芯片性能数据

| Case version | Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| Case 1 | 750.219 | 32 | 749.13 | 660.15 | 0.881 | 258.354 | 0.345 | 437.64 | 0.584 | 753.906 | 0.982 | 33.257 | 0.044 |
| Case 2 | 693.283 | 32 | 692.34 | 641.444 | 0.926 | 241.563 | 0.349 | 428.914 | 0.62 | 612.536 | 0.885 | 33.965 | 0.049 |

可以看到，Case 2 的 Cube 计算时间占比（即表中的`aic_mac_ratio`）达到 `92.6%`。

### Case 2 收益（相对 Case 1）

两种场景均使用常量化 tiling + 模板常量化。Case 2 相对 Case 1 的收益主要来自 `mxTypePara` 带来的 scale 多搬运能力。

- 端到端时延：`750.219 -> 693.283 μs`，减少 `56.936 μs`，收益 `7.59%`。
- MTE2 绝对耗时：`753.906 -> 612.536 μs`，减少 `141.370 μs`，收益 `18.75%`。
- MTE2 占比：`0.982 -> 0.885`，下降 `9.7` 个百分点。
- MAC 占比：`0.881 -> 0.926`，提升 `4.5` 个百分点。

**调优 Tips**：
> MX Matmul 的关键差异在于 `scale` 与 A/B 的搬运可解耦；当 `aic_mte2_ratio` 偏高时，优先通过 `mxTypePara` 调整 `scale` 搬运比例以提升 L1 复用、减少重复 GM->L1 搬运。

### 理论性能对比

本样例的性能数据在Ascend 950PR上运行得到，该处理器的主频为1.65GHz，对于MX-FP4的数据类型，每cycle处理16×64×16次乘加运算。Cube理论运算时间为
$$
T_{\text{theory}} = \frac{M \times N \times K}{16 \times 64 \times 16 \times 1.65 \times 10^9 \times \text{核数}} = \frac{8192 \times 8192 \times 8192}{16384 \times 1.65 \times 10^9 \times 32} = 635.5 μs
$$
Case 1/Case 2 的 `aic_mac_time` 分别为 `660.150 μs` / `641.444 μs`，相对理论值 `635.5 μs`：
- Case 1 误差：`(660.150 - 635.5) / 635.5 = 3.88%`
- Case 2 误差：`(641.444 - 635.5) / 635.5 = 0.94%`

## 编译运行

- 编译执行

在本样例根目录下执行如下步骤，编译并执行样例：

> **注意事项**：本样例使用的 `ml_dtypes` 库需要版本 `0.2.0`，`en_dtypes` 库需要版本 `0.0.4`。安装命令为

  ```bash
  python3 -m pip install ml_dtypes==0.2.0 en_dtypes==0.0.4
  ```

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  SCENARIO_NUM=2
  mkdir -p build && cd build;  # 创建并进入 build 目录
  cmake .. -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=npu -DCMAKE_ASC_ARCHITECTURES=dav-3510; make -j;
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin
  ```

  使用NPU仿真模式时，设置 `-DCMAKE_ASC_RUN_MODE=sim` 即可
  ```bash
  cmake .. -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=npu -DCMAKE_ASC_ARCHITECTURES=dav-3510; make -j; # npu模式
  cmake .. -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510; make -j; # npu仿真模式
  ```

  编译选项说明：

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `SCENARIO_NUM` | `1` / `2` | 1: 常量化tiling + scale同步搬运；2: 常量化tiling + scale多搬运 |
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）/ `sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | 目标SoC架构（本样例仅支持3510） |

  > **注意：** 切换 `CMAKE_ASC_RUN_MODE` / `CMAKE_ASC_ARCHITECTURES` / `SCENARIO_NUM` 前需清理cmake缓存，可在build目录下执行 `rm CMakeCache.txt` 后重新 cmake。

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```

## 性能分析

### msOpProf工具介绍
msOpProf工具是单算子性能分析工具。包含msopprof和msopprof simulator两种使用方式。该工具协助用户定位算子内存、算子代码以及算子指令的异常，实现全方位的算子调优。当前支持基于不同运行模式（上板或仿真）和不同文件形式（可执行文件或算子二进制.o文件）进行性能数据的采集和自动解析。

- 上板性能采集

    通过上板性能采集，可以直接测定算子昇腾AI处理器上的运行时间。该方式适合在板环境中快速定位算子性能问题。

    基于可执行文件demo通过msopprof执行算子调优：
    ```
    msopprof ./demo
    ```

    - 性能数据说明  
      命令完成后，会在默认目录下生成以“OPPROF_{timestamp}_XXX”命名的文件夹,性能数据文件夹结构示例如下：

      ```bash
      ├──dump                       # 原始的性能数据，用户无需关注
      ├──ArithmeticUtilization.csv  # cube/vector指令cycle占比
      ├──L2Cache.csv                # L2 Cache命中率，影响MTE2，建议合理规划数据搬运逻辑，增加命中率
      ├──Memory.csv                 # UB，L1和主存储器读写带宽速率
      ├──MemoryL0.csv               # L0A，L0B，和L0C读写带宽速率
      ├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
      ├──OpBasicInfo.csv            # 算子基础信息
      ├──PipeUtilization.csv        # 采集计算单元和搬运单元耗时和占比
      ├──ResourceConflictRatio.csv  # UB上的bank group、bank conflict和资源冲突率在所有指令中的占比
      └──visualize_data.bin         # MindStudio Insight呈现文件
      ```

查看具体的性能分析结果：

```bash
# 查看Task Duration 以及各项数据
cat ./OPPROF_*/PipeUtilization.csv
```
