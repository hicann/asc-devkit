# compare样例

## 概述

本样例基于C API编程接口实现比较运算，主要调用[asc_lt](../../../../../docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_lt.md)、[asc_gt_scalar](../../../../../docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_gt_scalar.md)和[asc_get_cmp_mask](../../../../../docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_get_cmp_mask.md)接口实现数据比较。

本样例支持三种比较场景，通过CMake编译参数`SCENARIO_NUM`选择。

  | SCENARIO_NUM | 比较场景 |
  | --- | --- |
  | 1 | 一个向量和另一个向量逐元素比较 |
  | 2 | 一个向量和另一个向量逐元素比较（通过cmp_mask读取比较结果） |
  | 3 | 一个向量和标量逐元素比较 |

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```text
├── compare
│   ├── scripts
│   │   └── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── compare.asc                    // Ascend C样例实现 & 调用样例
│   ├── README.md                      // 样例介绍
│   └── README_en.md                   // 样例介绍（英文版）
```

## 样例描述

本样例对输入向量做比较运算，具体说明如下：

**场景1：向量比较**

- 样例功能：逐元素比较`x[i] < y[i]`。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 64]</td><td align="center">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 64]</td><td align="center">float</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">output</td><td align="center">[1, 8]</td><td align="center">uint8_t</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">compare_kernel</td></tr>
  </table>

- 样例实现：
  `compare_kernel`函数内调用`asc_lt`接口对两个float类型的向量进行逐元素比较，具体流程如下：
  - 搬入：调用`asc_copy_gm2ub_align`将`x`和`y`从GM搬入UB。
  - 计算：调用带输出参数的`asc_lt`接口完成向量比较。
  - 搬出：调用`asc_copy_ub2gm_align`接口将结果从UB搬回GM。
  - 调用实现：使用内核调用符`<<<>>>`调用核函数。

**场景2：向量比较（cmp_mask读取）**

- 样例功能：逐元素比较`x[i] < y[i]`，并通过cmp_mask读取比较结果。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 64]</td><td align="center">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 64]</td><td align="center">float</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">output</td><td align="center">[1, 8]</td><td align="center">uint8_t</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">compare_kernel</td></tr>
  </table>

- 样例实现：
  `compare_kernel`函数内调用`asc_lt`和`asc_get_cmp_mask`接口对两个float类型的向量进行逐元素比较，具体流程如下：
  - 搬入：调用`asc_copy_gm2ub_align`将`x`和`y`从GM搬入UB。
  - 计算：调用不带输出参数的`asc_lt`接口生成cmp_mask，再调用`asc_get_cmp_mask`接口读取cmp_mask。
  - 搬出：调用`asc_copy_ub2gm_align`接口将结果从UB搬回GM。
  - 调用实现：使用内核调用符`<<<>>>`调用核函数。

**场景3：向量标量比较**

- 样例功能：逐元素比较`x[i] > y[0]`，其中`y[0]`作为标量。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 64]</td><td align="center">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 64]</td><td align="center">float</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">output</td><td align="center">[1, 8]</td><td align="center">uint8_t</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">compare_kernel</td></tr>
  </table>

- 样例实现：
  `compare_kernel`函数内调用`asc_gt_scalar`接口对float类型的向量和标量进行逐元素比较，具体流程如下：
  - 搬入：调用`asc_copy_gm2ub_align`将`x`和`y`从GM搬入UB。
  - 计算：调用`asc_gt_scalar`接口比较`x[i]`与标量`y[0]`。
  - 搬出：调用`asc_copy_ub2gm_align`接口将结果从UB搬回GM。
  - 调用实现：使用内核调用符`<<<>>>`调用核函数。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录；root用户默认为`/usr/local/Ascend`，非root用户默认为`${HOME}/Ascend`。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  SCENARIO_NUM=1                                                                    # 执行场景1
  mkdir -p build && cd build;                                                       # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py -scenario_num=$SCENARIO_NUM                        # 生成测试输入数据和真值数据
  ./demo                                                                            # 执行编译生成的可执行程序，执行样例
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  | --- | --- | --- |
  | `SCENARIO_NUM` | `1`（默认）、`2`、`3` | 样例执行场景：场景1：向量比较、场景2：向量比较（cmp_mask读取）、场景3：向量标量比较 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认） | NPU架构：dav-2201对应Atlas A2训练系列产品/Atlas A2推理系列产品和Atlas A3训练系列产品/Atlas A3推理系列产品 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
