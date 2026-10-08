# 使用TmpBuf实现VectorAdd样例

## 概述

本样例基于TPipe::InitBuffer接口初始化TBuf内存空间，并在计算过程中使用TBuf临时缓冲区进行数据转换，实现了bfloat16_t数据类型的向量加法（Add）样例。

> **注意：** 本样例仅适用于基于TPipe和TQue的编程模式。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── tmp_buffer
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── tmp_buffer.asc          // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能

  本样例调用Cast接口将bfloat16_t类型的输入数据转换为float类型存入TBuf临时缓冲区，完成Add计算后再调用Cast接口转换回bfloat16_t类型。

- 样例规格

  <table>
    <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Add</td></tr>
    <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
    <tr><td align="center">x</td><td align="center">[1, 2048]</td><td align="center">bfloat16_t</td><td align="center">ND</td></tr>
    <tr><td align="center">y</td><td align="center">[1, 2048]</td><td align="center">bfloat16_t</td><td align="center">ND</td></tr>
    <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1, 2048]</td><td align="center">bfloat16_t</td><td align="center">ND</td></tr>
    <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">tmp_buffer_custom</td></tr>
  </table>

- 样例实现

  - Kernel实现

    - 调用TPipe::InitBuffer接口，为TQue和TBuf分配内存空间，其中TBuf用于存储临时数据。

    - 调用DataCopy基础API，将数据从GM（Global Memory）搬运到UB（Unified Buffer）。

    - 调用Cast接口，将bfloat16_t类型数据转换为float类型并存入TBuf临时缓冲区。

    - 调用Add接口，对两个输入tensor执行加法操作。

    - 调用Cast接口，将float类型的计算结果转换为bfloat16_t类型并存入TQue分配的UB（Unified Buffer）空间。

    - 调用DataCopy基础API，将计算结果从UB（Unified Buffer）搬运至GM（Global Memory）。

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
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确
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
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果

  执行结果如下，说明精度对比成功：
  ```bash
  test pass!
  ```
