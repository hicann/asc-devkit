# Dump接口功能说明

## 概述

本样例介绍asc_dump_xxx系列接口使用方法，通过该接口实现核函数中不同物理位置的张量数据可视化。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── simple_dump
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── dump.asc                // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：

  使用静态Tensor编程模式实现矩阵乘法，展示asc_dump_xxx系列接口的基本使用方法。
  该系列接口兼容AscendC::DumpTensor接口。但后续开发中，建议优先使用asc_dump系列接口；若需Dump指定偏移位置的数据，由于asc_dump系列暂不支持该能力，可继续使用DumpAccChkPoint接口。

  矩阵乘法的计算公式为：

  ```
  C = A * B
  ```

- 样例规格：

  样例参数为：M = 256, N = 256, K = 64，shape信息如下表所示：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">mmad_custom</td></tr>
  </table>

- asc_dump接口用法：

  | 接口 | 内存层级 | 地址类型 | 示例 |
  |------|----------|----------|------|
  | `asc_dump_gm` | GM (Global Memory) | `__gm__` | `asc_dump_gm<half>((__gm__ half*)addr, id, size);` |
  | `asc_dump_l1buf` | L1 Buffer | `__cbuf__` | `asc_dump_l1buf<half>((__cbuf__ half*)addr, id, size);` |
  | `asc_dump_cbuf` | L0C Buffer | `__cc__` | `asc_dump_cbuf<float>((__cc__ float*)addr, id, size);` |
  | `asc_dump_ubuf` | UB | `__ubuf__` | `asc_dump_ubuf<half>((__ubuf__ half*)addr, id, size);` |

  参数说明：
  - 模板参数：指定数据类型（half/float等）
  - id：自定义标识符，用于区分不同dump数据
  - dumpSize：dump数据元素个数

- 调用实现

  使用内核调用符<<<>>>调用核函数。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # 编译工程
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果
  最终执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
