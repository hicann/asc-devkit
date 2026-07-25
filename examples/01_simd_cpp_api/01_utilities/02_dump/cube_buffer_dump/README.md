# Cube Buffer DumpTensor接口功能说明

## 概述

本样例以Cube编程模式的`QuantMatmulHifp8`计算为背景，通过`DumpTensor`打印HiFloat8矩阵A、Bias和
scale在L1 Buffer、Bias Table Buffer和Fixpipe Buffer中的数据，展示dav-3510 Cube Buffer的数据打印方法。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 | HDK版本 |
|------|-------------|---------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 | >= 25.7.0 |

## 目录结构介绍

```text
├── cube_buffer_dump
│   ├── scripts
│   │   ├── gen_data.py          // 生成输入数据和golden数据
│   │   └── verify_result.py     // 校验计算输出和dump日志
│   ├── CMakeLists.txt           // 编译工程文件
│   ├── data_utils.h             // 二进制文件读写函数
│   ├── cube_buffer_dump.asc     // 量化MatMul计算、DumpTensor调用和Host调用
│   ├── README.md                // 中文样例说明文档
│   └── README_en.md             // 英文样例说明文档
```

## 样例描述

- 样例功能：

  以带Bias和per-channel scale的HiFloat8量化矩阵乘为背景。对于输出矩阵中的元素
  `C[m, n]`，计算过程如下：

  ```text
  C[m, n] = (sum(A[m, k] * B[k, n]) + bias[n]) * scale[n]
  ```

  MMAD使用Bias Table Buffer中的float Bias完成HiFloat8矩阵乘累加，Fixpipe使用Fixpipe Buffer中的
  per-channel scale将float累加结果转换为bfloat16。

- 样例规格：

  样例使用固定参数`M=16, K=32, N=16`，shape信息如下表所示：

  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">QuantMatmulHifp8</td></tr>
  <tr><td rowspan="5" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">hifloat8</td><td align="center">ND</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">hifloat8</td><td align="center">ND</td></tr>
  <tr><td align="center">bias</td><td align="center">[N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">scale</td><td align="center">[N]</td><td align="center">uint64_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">bfloat16</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">QuantMatmulHifp8</td></tr>
  </table>

- 计算流程：

  计算由单个AI Core执行，处理流程如下：

  1. 将HiFloat8矩阵A、B从Global Memory搬入L1，再搬入L0A、L0B。
  2. 将float Bias从Global Memory搬入L1，再搬入Bias Table Buffer。MMAD使用Bias Table Buffer
     作为C矩阵初值，完成`A * B + bias`。
  3. 将per-channel `uint64_t` scale从Global Memory搬入L1，再搬入Fixpipe Buffer。
  4. Fixpipe读取scale，将L0C中的float结果转换为bfloat16并写回Global Memory。

  输入A、B均为HiFloat8数值`1.0`，Bias为`0~15`，偶数列scale为`1.0`，奇数列scale为`0.5`。
  因此每行第`n`列的期望结果为`(32 + bias[n]) * scale[n]`。

- DumpTensor打印位置：

  本样例打印以下数据：

  | desc | 数据位置 | 计算中的用途 |
  |------|----------|--------------|
  | 100 | L1 Buffer | HiFloat8矩阵A的前32个元素 |
  | 101 | L1 Buffer | 搬入Bias Table Buffer前的float Bias |
  | 102 | L1 Buffer | 搬入Fixpipe Buffer前的per-channel `uint64_t` scale |
  | 201 | Bias Table Buffer | MMAD用于初始化C矩阵的float Bias |
  | 301 | Fixpipe Buffer | Fixpipe量化实际读取的`uint64_t` scale |

  `scripts/verify_result.py`校验计算输出和五组`DumpTensor`日志。

## Fixpipe Buffer打印特性

本样例从Fixpipe Buffer量化参数区的地址`0`读取scale。在dav-3510上，从L1 Buffer搬入Fixpipe Buffer前后的
数据关系如下：

```text
fixpipe_value = l1_value & 0x00007fe0ffffe0ff
```

该掩码保留位`[7:0]`、`[31:13]`和`[46:37]`，清零位`[12:8]`、`[36:32]`和`[63:47]`。
`desc=301`的结果应等于`desc=102`的数据与该掩码按位与后的结果。

相关规则参见[L1与Fixpipe Buffer数据搬运](../../../../../docs/zh/api/SIMD-API/基础API/数据搬运导览/L1与Fixpipe-Buffer数据搬运.md)和
[随路量化](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵搬出关键特性说明/随路量化.md)。
其中，随路量化文档的3510量化参数位域表将`[12:0]`标记为无效位；本样例的dav-3510打印结果中
`[7:0]`保留输入值，仅`[12:8]`清零。该差异仅表示无效位的打印可见值不同，不改变其量化语义。

## 编译运行

在本样例根目录下执行：

```bash
source ${install_path}/cann/set_env.sh
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 .. && make -j
python3 ../scripts/gen_data.py
./demo 2>&1 | tee run.log
python3 ../scripts/verify_result.py output/output.bin output/golden.bin run.log \
    input/a_gm.bin input/bias_gm.bin input/scale_gm.bin
```

`${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

校验通过时输出：

```text
Output result check: PASS
HiFloat8 A L1 data check: PASS
Bias L1 raw input check: PASS
Bias Table raw data check: PASS
Fixpipe L1 raw input check: PASS
Fixpipe quant visible-mask check: PASS
test pass!
```
