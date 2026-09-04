
# Batch Matmul样例

## 概述

本样例介绍在输入为float数据类型并且左、右矩阵均不转置的场景下，带batch的矩阵乘法，其中GM -> L1通路采用DataCopy（Nd2NzParams）批量搬运，L0C -> GM、L0C -> L1通路采用Fixpipe批量搬运，L1 -> L0A / L0B搬运和Mmad矩阵乘循环执行batch次。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```text
├── batch_matmul
│   ├── scripts
│   │   ├── gen_data.py             // 输入数据和真值数据生成脚本
│   │   └── verify_result.py        // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读入写出函数
│   ├── batch_matmul.asc            // Ascend C样例实现 & 调用样例
│   └── README.md                   // 样例说明文档
```

## 样例描述

本样例的数据流如下：

1. GM -> L1：调用DataCopy（Nd2NzParams），一次性搬入B对A、B矩阵并完成ND -> Nz随路转换。
2. L1 -> L0A / L0B：循环batch次，每次搬运当前batch的一对A、B矩阵。
3. Mmad：循环batch次，每次计算一对A、B矩阵乘结果，并写入L0C对应位置。
4. L0C -> GM / L1：调用Fixpipe一次性搬出B个C矩阵；Atlas A2/A3系列产品额外演示L0C -> L1搬出路径。

### batch mmad定义

批量矩阵乘法（batch mmad）是普通矩阵乘法在批次维度（Batch Dimension）上的扩展，核心逻辑是：对一个包含多个矩阵的批次数据，逐一对批次内的矩阵执行标准矩阵乘法，最终输出同批次数量的结果矩阵。

简单来说，若有两个批量矩阵 A 和 B，它们的形状分别为 [B, M, K] 和 [B, K, N]（其中 B 是批次大小，M/K/N 是矩阵维度），批量矩阵乘法会为每一个批次索引 i（i ∈ [0, B-1]），取 A[i]（形状 [M, K]）和 B[i]（形状 [K, N]）执行普通矩阵乘法，最终得到形状为 [B, M, N] 的批量结果矩阵 C。对任意批次 i（0 ≤ i < B），C 的第 i 个矩阵满足：
C[i]=A[i]×B[i]。

需要注意的是，不同批次的矩阵之间不会互相计算。

### 样例规格

本样例中输入输出矩阵的规格，如下表1所示：

<a name="表1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">📌 表1：输入输出的规格</span></caption>
  <tr>
    <td >输入输出</td>
    <td>数据类型</td>
    <td>Shape</td>
    <td>是否转置</td>
  </tr>
  <tr>
    <td>输入矩阵A</td>
    <td>float</td>
    <td>[4, 30, 40]</td>
    <td>false</td>
  </tr>
  <tr>
    <td>输入矩阵B</td>
    <td>float</td>
    <td>[4, 40, 70]</td>
    <td>false</td>
  </tr>
  <tr>
    <td>输出矩阵C</td>
    <td>float</td>
    <td>[4, 30, 70]</td>
    <td>-</td>
  </tr>
</table>

### 矩阵批量搬入（GM -> L1）

根据batch Mmad的定义可知，共计B对A、B矩阵进行矩阵乘法。GM -> L1通路调用DataCopy（Nd2NzParams）完成ND -> Nz随路转换，通过配置`nd2nzA1Params.ndNum = B`，实现一次性搬入B对A、B矩阵。

```cpp
// GM -> L1，搬运A矩阵
AscendC::Nd2NzParams nd2nzA1Params;
// 传输ND矩阵的数目
nd2nzA1Params.ndNum = B;
// ND矩阵的行数
nd2nzA1Params.nValue = m;
// ND矩阵的列数
nd2nzA1Params.dValue = k;
// 源操作数相邻ND矩阵起始地址间的偏移，单位是元素
nd2nzA1Params.srcNdMatrixStride = m * k;
// 源操作数同一ND矩阵的相邻行起始地址间的偏移，单元是元素
nd2nzA1Params.srcDValue = k;

// ND转换到NZ格式后，源操作数中的一行会转换为目的操作数的多行。
// 该参数表示，目的NZ矩阵中，来自源操作数同一行的多行数据相邻行起始地址间的偏移，单位：C0_SIZE（32B）。
// 数据搬运到L1时会进行对齐
nd2nzA1Params.dstNzC0Stride = CeilAlign(m, cubeShape[0]);
// 目的NZ矩阵中，Z型矩阵相邻行起始地址之间的偏移。单位：C0_SIZE（32B）。
nd2nzA1Params.dstNzNStride = 1;
// 目的NZ矩阵中，相邻NZ矩阵起始地址间的偏移，单位是元素
nd2nzA1Params.dstNzMatrixStride = aSizeAlignL0;
```

### L1 -> L0A / L0B搬运和矩阵乘Mmad循环执行B次

for循环B次，每次从L1 -> L0A / L0B搬运每个batch的A、B矩阵，Mmad指令每次计算一对A、B矩阵乘的结果，存储到L0C的相应位置上，待循环结束后将L0C上的完整结果搬出到GM。

```cpp
for (int32_t batchIndex = 0; batchIndex < B; batchIndex++) {
    SplitA(a1Local[batchIndex * aSizeAlignL0], a2Local);
    SplitBTranspose(b1Local[batchIndex * bSizeAlignL0], b2Local);
    AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID0);

    AscendC::MmadParams params;
    params.m = m;
    params.n = n;
    params.k = k;
    AscendC::Mmad(
        co1Local[batchIndex * CeilAlign(m, cubeShape[0]) * CeilAlign(n, cubeShape[0])], a2Local, b2Local,
        params);

    AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::M_MTE1>(EVENT_ID0);
}
```

### 矩阵批量搬出

L0C -> GM通路调用Fixpipe搬运接口，通过配置`fixpipeParams.ndNum = B`，实现一次性搬出B对C矩阵。需要注意的是，L0C中的C矩阵是对齐后的，搬出到GM的C矩阵是原始非对齐shape。

对于Atlas A2 训练系列产品/Atlas A2 推理系列产品以及Atlas A3 训练系列产品/Atlas A3 推理系列产品，样例还额外演示了L0C -> L1搬出路径：Fixpipe将L0C上的float结果量化为half后写入L1，并通过DumpTensor打印L1上的NZ结果。

```cpp
// 源NZ矩阵在N方向上的大小。
fixpipeParams.nSize = n;
// 源NZ矩阵在M方向上的大小。
fixpipeParams.mSize = m;
// 源NZ矩阵中相邻Z排布的起始地址偏移，单位：C0_Size(16*sizeof(T)，T为src的数据类型)
fixpipeParams.srcStride = CeilAlign(m, cubeShape[0]);
// 使能NZ2ND功能时，代表目的ND矩阵每一行中的元素个数，取值不为0，单位：元素
fixpipeParams.dstStride = n;
// 源NZ矩阵的数目，也就是传输ND矩阵的数目
fixpipeParams.ndNum = B;
// 不同NZ矩阵起始地址之间的间隔，单位：1024B
fixpipeParams.srcNdStride = (CeilAlign(m, cubeShape[0]) * CeilAlign(n, cubeShape[0])) 
                                / (cubeShape[0] * cubeShape[0]);
// 目的相邻ND矩阵起始地址之间的偏移，单位：element
```

### 避免数据占用总内存超过存储空间限制

用户应该保证batch Mmad整个过程中的数据所占总内存不超过存储空间限制。
用户可以通过PlatformAscendC类成员函数GetCoreMemSize，获取硬件平台中L1、L0A、L0B、L0C存储空间的内存大小。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DCMAKE_ASC_RUN_MODE=npu ..;make -j;    # 编译工程
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```
  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```
  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品/Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
