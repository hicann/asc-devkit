# pattern_transformation 兼容性样例


## 概述

本样例基于基础的mmad样例，演示了 L1 Buffer -> L0A Buffer 通路的分形转换逻辑，通过编译时宏 `__NPU_ARCH__` 隔离不同硬件实现。

- 在 Atlas A2/A3 系列产品中，L0A Buffer 的数据排布为 Zz 分形
- 在 Ascend 950PR/Ascend 950DT 系列产品中，L0A Buffer 的数据排布为 Nz 分形

分形的转换涉及到以下两种典型场景的兼容性适配：

1. L0A不复用。对一整块A矩阵进行一次mmad计算。
2. L0A复用。切分M轴，A矩阵分为两块子矩阵，进行两次mmad计算。

**对于场景1**，需要修改L1>L0A搬运过程的逻辑，由Nz->Zz分形转换变为Nz->Nz。具体可参考[样例实现](./README.md#样例实现)。  
**对于场景2**，在Atlas A2/A3 系列产品上，由于Zz分形本身在M轴上连续，所以L1->L0A搬运逻辑和场景1相同，而对于Ascend 950PR/Ascend 950DT 系列产品，切分M轴会导致矩阵不连续，需要拆分为两个Nz矩阵。具体可参考[样例实现](./README.md#样例实现)。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
| ------ | ------------- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```text
├── pattern_transformation
│   ├── scripts
│   │   ├── gen_data.py                    // 输入数据和真值数据生成脚本
│   │   └── verify_result.py               // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                     // 编译工程文件
│   ├── data_utils.h                       // 数据读入写出函数
│   ├── figures                            // README中图片资源目录
│   ├── pattern_transformation.asc         // AscendC算子实现 & 调用样例
│   └── README.md                          // 样例说明文档
```

## 样例规格

<table>
<caption>样例规格表</caption>
<tr><td rowspan="1" align="center">类别</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td rowspan="3" align="center">样例输入</td></tr>
<tr><td align="center">x</td><td align="center">[64, 64]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[64, 64]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[64, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">pattern_transformation</td></tr>
</table>

## 样例实现

***图1 L0A不复用场景概览***

![L0A不复用场景](./figures/普通兼容场景.png)

图1 展示了 L0A 不复用场景下两种芯片的 L1→L0A 数据搬运差异：
- Atlas A2/A3 系列：L0A 为 Zz 分形，需通过 LoadData（2D矩阵搬运） 完成 Nz→Zz 分形转换。
- Ascend 950PR/Ascend 950DT：L0A 为 Nz 分形，与 L1 一致，无需分形转换，LoadData（2D矩阵搬运） 参数更简洁。

***图2 L0A复用场景概览***

![L0A切分兼容场景](./figures/L0A切分兼容场景.png)

图2 展示了 L0A 复用（M 轴切分）场景下两种芯片的适配逻辑：
- Atlas A2/A3 系列：Zz 分形下，M 轴切分后 A1 和 A2 在 L0A 中依然连续，仅需调整第二次 Mmad 的偏移量即可。
- Ascend 950PR/Ascend 950DT：Nz 分形下，M 轴切分后 A1 和 A2 不连续，需通过两次 LoadData（2D矩阵搬运） 分别搬运上半和下半 Nz 子矩阵，不改变 Mmad 计算逻辑及流水排布。

### GM->L1

GM到L1的搬运过程，**无需兼容性适配**，使用DataCopy接口将GM中的ND矩阵加载到L1，相关代码片段如下：

```cpp
__aicore__ inline void CopyIn(AscendC::LocalTensor<T> a1Local, AscendC::LocalTensor<U> b1Local)
{
    ...
    AscendC::DataCopy(a1Local, aGM, nd2nzA1Params);
    ...
    AscendC::DataCopy(b1Local, bGM, nd2nzB1Params);
}
```

DataCopy 将 GM 中 ND 格式的矩阵 x[M, K] 和 y[K, N] 分别搬运到 L1 Buffer 中，存储为 Nz 分形格式，供后续 Cube 运算使用。

### L1->L0A

Atlas A2/A3 系列产品中 L0A 数据排布为 Zz 分形，Ascend 950PR/Ascend 950DT 中变更为 Nz 分形，以下分场景介绍兼容性差异。通过编译宏 `SCENARIO_NUM` 区分场景：`SCENARIO_NUM=1` 为 L0A 不复用场景，`SCENARIO_NUM=2` 为 L0A 复用场景（M 轴切分）。

#### L0A不复用场景

在 Atlas A2/A3 系列产品中，L1->L0A 需要由 Nz 分形转换成 Zz 分形，通过 LoadData（2D矩阵搬运） 接口实现，相关代码如下：

```cpp
constexpr uint32_t mBlocks = M / CUBE_BLOCK;
constexpr uint32_t kBlocks = K * sizeof(T) / C0_SIZE;
int srcOffset = 0;
int dstOffset = 0;
for (uint32_t i = 0; i < mBlocks; ++i) {
    AscendC::LoadData2DParams loadDataParams;
    loadDataParams.repeatTimes = kBlocks;
    loadDataParams.srcStride = mBlocks;
    loadDataParams.ifTranspose = false;
    AscendC::LoadData(a2[dstOffset], a1[srcOffset], loadDataParams);
    srcOffset += CUBE_BLOCK * CUBE_BLOCK;
    dstOffset += K * CUBE_BLOCK;
}
```

在 Ascend 950PR/Ascend 950DT 中，L1->L0A不需要做分形转换，保持Nz分形，通过LoadData（2D矩阵搬运） 接口实现，相关代码如下：

```cpp
constexpr uint32_t mBlocks = M / CUBE_BLOCK;
constexpr uint32_t kBlocks = K * sizeof(T) / C0_SIZE;
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;  // M轴起始位置，单位为16个元素
loadDataParams.kStartPosition = 0;  // K轴起始位置，单位为32字节
loadDataParams.mStep = mBlocks;     // M轴搬运范围，单位为16个元素
loadDataParams.kStep = kBlocks;     // K轴搬运范围，单位为32字节
loadDataParams.srcStride = mBlocks; // 源相邻K轴分形间隔，单位为512字节
loadDataParams.dstStride = mBlocks; // 目的相邻K轴分形间隔，单位为512字节
loadDataParams.ifTranspose = false;
loadDataParams.sid = 0;
AscendC::LoadData(a2, a1, loadDataParams);
```


#### L0A复用场景

在分析前，先介绍数学背景。对于矩阵乘法 C = A × B，当 A 可拆分为上下两块时：

$$
A = \begin{bmatrix} A_1 \\ A_2 \end{bmatrix}, \quad
A \cdot B = \begin{bmatrix} A_1 \\ A_2 \end{bmatrix} B
= \begin{bmatrix} A_1 B \\ A_2 B \end{bmatrix}
= \begin{bmatrix} C_1 \\ C_2 \end{bmatrix} = C
$$

即可以将 A 按行拆分为 A1 和 A2，分别与 B 相乘得到 C1 和 C2，最终拼接为 C。
在Atlas A2/A3系列产品中，由于L0A为Zz分形，因此切分M轴后，A1和A2依然连续，第二次mmad计算只需要修改A矩阵的偏移量即可。
在Ascend 950PR/Ascend 950DT中，由于L0A为Nz分形，切分M轴后A1和A2不连续，为了不改变mmad计算的逻辑及后续的流水排布，需要通过LoadData（2D矩阵搬运）搬运指令，将整块Nz矩阵切分成两块子Nz矩阵，相关代码如下：

```cpp
constexpr uint32_t mBlocks = M / CUBE_BLOCK;
constexpr uint32_t kBlocks = K * sizeof(T) / C0_SIZE;
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;      // 从A矩阵上半部分开始，单位为16个元素
loadDataParams.kStartPosition = 0;      // K轴起始位置，单位为32字节
loadDataParams.mStep = mBlocks / 2;     // 每次搬运一半M轴，单位为16个元素
loadDataParams.kStep = kBlocks;         // K轴搬运范围，单位为32字节
loadDataParams.srcStride = mBlocks;     // 源相邻K轴分形间隔，单位为512字节
loadDataParams.dstStride = mBlocks / 2; // 子矩阵相邻K轴分形间隔，单位为512字节
loadDataParams.ifTranspose = false;
loadDataParams.sid = 0;
// 搬运上半部分Nz分形
AscendC::LoadData(a2, a1, loadDataParams);
// 搬运下半部分Nz分形
loadDataParams.mStartPosition = mBlocks / 2;
AscendC::LoadData(a2[M * K / 2], a1, loadDataParams);
```


### L1->L0B

各产品的L0B数据排布均为Zn分形，L1到L0B的分形转换逻辑相同，但3510架构需要将LoadData（2D矩阵搬运）的参数结构体切换为`LoadData2DParamsV2`。

Atlas A2/A3系列产品使用`LoadData2DParams`，相关代码如下：

```cpp
constexpr uint32_t srcStride = 512 / sizeof(U);
constexpr uint32_t dstStride = N * CUBE_BLOCK;
constexpr uint32_t kBlocks = K / CUBE_BLOCK;
constexpr uint32_t nBlocks = N * sizeof(U) / C0_SIZE;
AscendC::LoadData2DParams loadDataParams;
loadDataParams.repeatTimes = nBlocks;
loadDataParams.srcStride = kBlocks;
loadDataParams.dstGap = 0;
loadDataParams.ifTranspose = true;
for (uint32_t i = 0; i < kBlocks; ++i) {
    AscendC::LoadData(b2[i * dstStride], b1[i * srcStride], loadDataParams);
}
```

Ascend 950PR/Ascend 950DT系列产品使用`LoadData2DParamsV2`，相关代码如下：

```cpp
constexpr uint32_t kBlocks = K / CUBE_BLOCK;
constexpr uint32_t nBlocks = N * sizeof(U) / C0_SIZE;
AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;  // K轴起始位置，单位为16个元素
loadDataParams.kStartPosition = 0;  // N轴起始位置，单位为32字节
loadDataParams.mStep = kBlocks;     // K轴搬运范围，单位为16个元素
loadDataParams.kStep = nBlocks;     // N轴搬运范围，单位为32字节
loadDataParams.srcStride = kBlocks; // 源相邻N轴分形间隔，单位为512字节
loadDataParams.dstStride = nBlocks; // 目的相邻K轴分形间隔，单位为512字节
loadDataParams.ifTranspose = true;
loadDataParams.sid = 0;
AscendC::LoadData(b2, b1, loadDataParams);
```

### Compute

Compute 阶段使用 Mmad 接口完成矩阵乘法，Mmad 接口在两种架构上通用，**无需兼容性适配**。

- `SCENARIO_NUM=1`（L0A 不复用）：直接调用一次 Mmad 完成完整矩阵乘法 C=A×B。

```cpp
AscendC::MmadParams mmadParams;
mmadParams.m = M;
mmadParams.n = N;
mmadParams.k = K;
AscendC::Mmad(co1Local, a2, b2, mmadParams);
```

- `SCENARIO_NUM=2`（L0A 复用）：将 A 矩阵按 M 轴拆分为上下两半，分别计算 C1=A1×B 和 C2=A2×B。

```cpp
mmadParams.m = M / 2;  // M 轴拆分为上下两半，每次计算半个 M
mmadParams.n = N;
mmadParams.k = K;
mmadParams.cmatrixInitVal = true;
mmadParams.isBias = false;
AscendC::Mmad(co1Local, a2, b2, mmadParams);
AscendC::Mmad(co1Local[M * N / 2], a2[M * K / 2], b2, mmadParams);  // 下半部分 A2[32, 64]
```

### L0C->GM

L0C 到 GM 的搬运，使用 AscendC::FixpipeParamsV220 接口将 L0C 中的 float 结果写回 GM，该接口在两种架构上通用，**无需兼容性适配**。

- `SCENARIO_NUM=1`（L0A 不复用）：直接一次 Fixpipe 搬出完整结果矩阵。

```cpp
AscendC::FixpipeParamsV220 fixpipeParams;
fixpipeParams.mSize = M;
fixpipeParams.srcStride = M;
AscendC::Fixpipe(cGM, co1Local, fixpipeParams);
```

- `SCENARIO_NUM=2`（L0A 复用）：将结果矩阵按 M 轴拆分为上下两半分别搬出。

```cpp
AscendC::FixpipeParamsV220 fixpipeParams;
fixpipeParams.mSize = M / 2;
fixpipeParams.srcStride = M / 2;
AscendC::Fixpipe(cGM, co1Local, fixpipeParams);
AscendC::Fixpipe(cGM[M * N / 2], co1Local[M * N / 2], fixpipeParams);
```

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上 CANN 开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为 CANN 包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=1 .. && make -j       # 编译（默认 NPU 模式）
  python3 ../scripts/gen_data.py                                # 生成测试输入数据
  ./demo                                                        # 执行编译生成的可执行程序
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # 验证结果
  ```

  使用 CPU 调试或 NPU 仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可，示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=1 .. && make -j   # CPU 调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=1 .. && make -j   # NPU 仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU 调试、NPU 仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2/A3 系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`、`2` | 场景编号：1 表示 L0A 不复用场景，2 表示 L0A 复用场景（M 轴切分） |

- 执行结果

  执行结果如下，说明精度对比成功：

  ```bash
  test pass!
  ```
