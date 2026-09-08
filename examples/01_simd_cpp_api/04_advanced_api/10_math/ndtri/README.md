# Ndtri样例

## 概述

本样例演示 Ndtri 高阶 API 的调用方式和功能、性能验证方法。Ndtri 按元素计算标准正态分布累积分布函数的反函数：给定概率值 p，返回满足 $\Phi(x)=p$ 的分位数 x。

输入 p 为 0 时输出负无穷，为 1 时输出正无穷；p 在 (0, 1) 时返回对应分位数；p 越界或为 NaN、无穷时输出 NaN。

## 支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构介绍

```text
├── ndtri
│   ├── scripts
│   │   └── gen_data.py         // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── ndtri.asc               // Ascend C高阶API用例实现
│   ├── run.sh                  // 编译执行脚本（功能/性能验证）
│   ├── run_all.sh              // 全场景回归脚本
│   └── README.md               // 用例说明文档
```

## 用例规格

- 样例功能：按元素计算标准正态分布 CDF 逆函数 $\Phi^{-1}(p)$，并处理 0、1、越界值、NaN 及 Inf。
  $$dstTensor_i = \Phi^{-1}(srcTensor_i)$$
- 样例规格：

  <table>
  <tr><td rowspan="1" align="center">用例类型</td><td colspan="4" align="center">NdtriCustom</td></tr>

  <tr><td rowspan="3" align="center">用例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[SIZE]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">用例输出</td></tr>
  <tr><td align="center">y</td><td align="center">[SIZE]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">ndtri_custom</td></tr>
  </table>

## 用例说明

默认 shape 为输入 x[2048]、输出 y[2048]，可通过 `--size` 修改数据量。样例使用单核处理，并根据 UB 容量分块搬运和计算。

### 1. 功能验证

CopyIn → Compute(Ndtri) → CopyOut，输出与真值对比验证精度。功能模式支持以下接口场景：

- `api_mode=1`：指定`calCount`，并将调用后的输入LocalTensor搬回GM，逐字节检查源数据完整性。`calCount=0`时接口执行空操作，该场景由Kernel UT覆盖。
- `api_mode=2`：不传`calCount`，验证整Tensor重载与指定`calCount`重载结果一致。
- `api_mode=3`：整Tensor重载的输入输出使用完全相同的LocalTensor，验证原地计算结果。

`tests/api/adv_api/math/ndtri/test_operator_ndtri.cpp` 是纳入 adv_api UT 构建的接口调用检查；本目录的 `run_all.sh` 是样例的 NPU 功能、精度和性能验证入口。

### 2. 性能验证

采用AIV_VEC占比计算法，评估计算时间占比，标准为占比 ≥ 90%。

$$\text{AIV\_VEC占比} = \frac{\text{computeTime}}{\text{total2} - \text{total1}}$$

- `total1`：纯搬运耗时基线；`total2`：搬运+计算1000次耗时；`computeTime`：AIV_VEC计算时间。三者均通过msProf采集。
- 分母 `total2 - total1` 扣除搬运开销，得到纯计算增量时间。

**性能标准：AIV_VEC占比 ≥ 90%。**

### 3. 性能数据

性能验证通过msProf采集AI Core性能数据，存放在 `op_summary_*.csv` 中。主要字段说明如下：

| 字段名 | 字段含义 |
|:---:|:---|
| Task Duration(μs) | Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间。 |
| aiv_time(μs) | Task在AI Vector Core上的执行时间。 |
| aiv_vec_time(μs) | vec类型指令（向量类运算指令）耗时。 |
| aiv_vec_ratio | vec类型指令的cycle数在total cycle数中的占用比。 |
| aiv_scalar_time(μs) | scalar类型指令（标量类运算指令）耗时。 |
| aiv_scalar_ratio | scalar类型指令的cycle数在total cycle数中的占用比。 |
| aiv_mte2_time(μs) | mte2类型指令（GM->UB搬运类指令）耗时。 |
| aiv_mte2_ratio | mte2类型指令的cycle数在total cycle数中的占用比。 |
| aiv_mte3_time(μs) | mte3类型指令（UB->GM搬运类指令）耗时。 |
| aiv_mte3_ratio | mte3类型指令的cycle数在total cycle数中的占用比。 |

本用例性能验证实际采集以下字段用于占比计算：

- `aiv_time`：分别取 TEST_MODE=2（空跑基线）的值作为 `total1`、TEST_MODE=3（计算场景）的值作为 `total2`。
- `aiv_vec_time`：取 TEST_MODE=3 的值作为 `computeTime`（AIV_VEC计算时间）。

### 4. 验证范围

- 功能要求：
  - shape 覆盖对齐、非对齐和较大数据场景，`1`/`32`/`1023`/`2048`/`65536`
- 覆盖`float`类型、均匀概率、极端尾部、两个分段边界相邻float32值、正规数与次正规数交界、0/1、越界值、NaN及`±Inf`
- 验证输入输出地址分离时，Ndtri调用前后的源Tensor数据完全一致
- 验证两个接口重载结果一致，并验证完全同址原地计算
- 性能要求：数据量为 `1k`/`4k`/`8k`/`16k`/`32k`/`64k` 时，AIV_VEC 占比均需满足 `≥ 90%`

## 编译运行

- 配置环境变量

  请先根据当前环境的CANN软件安装指南完成开发套件安装，再配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

  API尚未安装时，可通过源码路径验证：

  ```bash
  export ASC_DEVKIT_PATH=/path/to/asc-devkit
  ```

- 用例执行

  通过 `run.sh` 一键编译并执行用例，脚本会自动完成 cmake 编译、生成测试数据、运行可执行程序。

  ```bash
  bash run.sh -r <cpu|sim|npu> --size <N> --is_perf <0|1> [--api_mode <1|2|3>]
  ```

  参数说明（除`--api_mode`外均为必传）：

  | 参数 | 可选值 | 说明 |
  |------|--------|------|
  | `-r` | `cpu`、`sim`、`npu` | 运行模式：CPU调试、NPU仿真、NPU上板（性能验证仅支持`npu`模式） |
  | `--size` | 正整数 | 计算量，即 TOTAL_LENGTH |
  | `--is_perf` | `0`、`1` | `0` 功能验证；`1` 性能验证 |
  | `--api_mode` | `1`、`2`、`3` | 可选，依次表示指定`calCount`、整Tensor重载和整Tensor重载完全同址原地计算；默认值为`1`，性能模式只支持`1` |

  示例：

  ```bash
  bash run.sh -r cpu  --size 2048 --is_perf 0   # CPU功能验证
  bash run.sh -r sim  --size 4096 --is_perf 0   # NPU仿真功能验证
  bash run.sh -r npu  --size 2048 --is_perf 1   # NPU上板性能验证
  bash run.sh -r npu  --size 1023 --is_perf 0 --api_mode 2  # 整Tensor重载
  bash run.sh -r npu  --size 1023 --is_perf 0 --api_mode 3  # 整Tensor重载完全同址
  ```

  950PR全场景回归：

  ```bash
  bash run_all.sh npu 1
  ```

  > **说明：** `--is_perf 1` 性能验证时，脚本自动完成 msProf 采集并计算 AIV_VEC 占比是否达标。

- 执行结果

  功能验证（`--is_perf 0`）通过时输出：

  ```bash
  test pass!
  ```

  性能验证（`--is_perf 1`）达标时输出：

  ```bash
  性能验证达标，当前占比: xx%
  ```

  > `--is_perf 1` 用于性能验证，输出非计算结果，不做精度对比。
