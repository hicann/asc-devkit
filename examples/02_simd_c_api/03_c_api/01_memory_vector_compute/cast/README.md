# cast样例

## 概述

本样例基于C API编程接口实现Cast运算，主要调用[asc_half2int4_ru](../../../../../docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_half2int4.md)和[asc_half2int32_ru](../../../../../docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_half2int32.md)接口实现数据类型转换。

本样例支持两种类型转换场景，通过CMake编译参数`SCENARIO_NUM`选择。

  | SCENARIO_NUM | 数据类型转换 |
  | --- | --- |
  | 1 | 数据类型位宽大转小，以half转int4b_t为例 |
  | 2 | 数据类型位宽小转大，以half转int32_t为例 |

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```text
├── cast
│   ├── scripts
│   │   └── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── cast.asc                       // Ascend C样例实现 & 调用样例
│   ├── README.md                      // 样例介绍
│   └── README_en.md                   // 样例介绍（英文版）
```

## 样例描述

本样例对输入向量做数据类型转换，具体说明如下：

**场景1：数据类型位宽大转小**

- 样例功能：将half类型的数据转换成int4b_t类型的数据。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 512]</td><td align="center">half</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 512]</td><td align="center">int4b_t</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">cast_kernel</td></tr>
  </table>

- 样例实现：
  `cast_kernel`函数内调用`asc_half2int4_ru`接口，将512个half类型的数据转换为int4b_t类型的数据，具体流程如下：
  - 搬入：调用`asc_copy_gm2ub_align`将512个half元素从GM搬入UB。
  - 计算：调用`asc_half2int4_ru`接口进行数据类型转换。
  - 搬出：调用`asc_copy_ub2gm_align`接口将结果从UB搬回GM。
  - 调用实现：使用内核调用符`<<<>>>`调用核函数。

**场景2：数据类型位宽小转大**

- 样例功能：将half类型的数据转换成int32_t类型的数据。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 512]</td><td align="center">half</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 512]</td><td align="center">int32_t</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">cast_kernel</td></tr>
  </table>

- 样例实现：
  `cast_kernel`函数内调用`asc_half2int32_ru`接口，将512个half类型的数据转换为int32_t类型的数据，具体流程如下：
  - 搬入：调用`asc_copy_gm2ub_align`将512个half元素从GM搬入UB。
  - 计算：调用`asc_half2int32_ru`接口进行数据类型转换。
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
  | `SCENARIO_NUM` | `1`（默认）、`2` | 样例执行场景：场景1：数据类型位宽大转小、场景2：数据类型位宽小转大 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认） | NPU架构：dav-2201对应Atlas A2训练系列产品/Atlas A2推理系列产品和Atlas A3训练系列产品/Atlas A3推理系列产品 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
