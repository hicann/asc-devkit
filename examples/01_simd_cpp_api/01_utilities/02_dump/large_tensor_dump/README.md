# 超大 Tensor 打印功能说明

## 概述

本样例展示超大 Tensor 打印功能。默认单次传输容量为 32 KiB，无法一次传输的 Tensor 数据由框架自动分片传输。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
| --- | --- |
| Ascend 950PR&950DT系列产品 | >= CANN 9.3.0 |
| Atlas A3系列产品 | >= CANN 9.3.0 |
| Atlas A2系列产品 | >= CANN 9.3.0 |

## 目录结构介绍

```text
large_tensor_dump
├── scripts
│   ├── gen_data.py           # 输入数据和真值数据生成脚本
│   └── verify_result.py      # 验证输出数据和真值数据是否一致
├── CMakeLists.txt            # 编译工程文件
├── data_utils.h              # 数据读入写出函数
├── large_tensor_dump.asc     # Ascend C样例实现及调用
├── README.md                # 样例说明文档
└── README_en.md
```

## 样例描述

- 样例功能：

  在一个 Vector 核上执行向量加标量计算，打印计算过程中的 UB Tensor 和计算完成后的 GM Tensor。

  计算公式为：

  ```text
  y = x + 1
  ```

- 样例规格：

  输入和输出均为 float32，包含 65,536 个元素。输入数据为 `x[i] = i % 256`。打印数据规格如下：

  | 打印对象 | desc | 元素个数 | 数据量 |
  | --- | --- | --- | --- |
  | 第一块计算结果的 UB Tensor | 100 | 16,384 | 64 KiB |
  | 完整计算结果的 GM Tensor | 200 | 65,536 | 256 KiB |

- 打印接口用法：

  本样例使用 `asc_dump` 接口输出 UB 和 GM 数据。

  | 内存位置 | 地址类型 | 调用示例 |
  | --- | --- | --- |
  | UB | `__ubuf__ float *` | `asc_dump<float>(ubPtr, 100, 16384);` |
  | GM | `__gm__ float *` | `asc_dump<float>(gmPtr, 200, 65536);` |

  超大 Tensor 数据量超过 32 KiB 时，仅打印前 32 KiB 数据对应的内容，超出部分以 `...` 表示，并输出告警。完整数据解析参见[工具说明](../../../../../docs/zh/guide/programming_guide/appendix/show_kernel_debug_data_tool.md)及[工具使用样例](https://gitcode.com/cann/asc-tools/tree/master/examples/01_show_kernel_debug_data)。

- 调用实现：

  使用内核调用符 `<<<>>>` 调用核函数。核函数每次处理 16,384 个元素，计算后将结果写回 GM。Host 侧通过 `ReadFile`、`WriteFile` 完成二进制数据的读写。

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
  mkdir -p build && cd build                                                # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 .. && make -j                    # 编译工程
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果
  部分示例输出如下：

  ```text
  [AIV Block 0] DumpTensor: desc=100, addr=0x10000, data_type=float32, position=UB, dump_size=16384
  1.000000,2.000000,3.000000,4.000000,...,253.000000,254.000000,255.000000,256.000000,...
  Warning: the tensor data is too large. Only the first 32768 bytes are printed and the rest is omitted. To view the full data, set the environment variable ASCEND_DUMP_PATH or ASCEND_WORK_PATH to enable dumping it to disk, and then use the show_kernel_debug_data tool to view it.
  [AIV Block 0] DumpTensor: desc=200, addr=0x57000, data_type=float32, position=GM, dump_size=65536
  1.000000,2.000000,3.000000,4.000000,...,253.000000,254.000000,255.000000,256.000000,...
  Warning: the tensor data is too large. Only the first 32768 bytes are printed and the rest is omitted. To view the full data, set the environment variable ASCEND_DUMP_PATH or ASCEND_WORK_PATH to enable dumping it to disk, and then use the show_kernel_debug_data tool to view it.
  ```

  计算结果校验通过时，输出如下：

  ```bash
  test pass!
  ```
