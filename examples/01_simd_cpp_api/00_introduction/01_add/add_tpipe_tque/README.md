# 基于TPipe和TQue的Add样例

## 概述

本样例基于TPipe和TQue的内存和同步管理机制实现Add向量加法操作。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── add_tpipe_tque
│   ├── scripts
│   │   ├── gen_data.py             // 输入数据和真值数据生成脚本
│   │   └── verify_result.py        // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读入写出函数
│   ├── add_tpipe_tque.asc          // Ascend C样例实现，TPipe和TQue管理内存和同步 & 调用样例
│   └── README.md                   // 样例说明文档
```

## 样例描述

- 样例功能：  
  计算公式：
  ```
  z = x + y
  ```
  该样例完成两组同形状数据的逐元素相加，并将结果写回输出张量。样例输入是 `x` 和 `y` 两个 `float` 类型张量，形状都是 `[8, 2048]`，输出 `z` 的形状与输入一致。核函数启动 8 个核并行计算，每个核负责处理其中一段连续数据。

- 处理流程：
  1. `add_custom` 作为核入口接收 `totalLength`。
  2. 在 `add_custom` 中通过 `GetBlockNum()` 计算当前 block 的数据长度，通过 `GetBlockIdx()` 计算当前核在 GM 中对应的数据起点。
  3. 在核函数中使用 `DataCopy` 把输入数据从 GM 搬到 UB，并通过 `EnQue` 将输入 `LocalTensor` 放入输入队列。
  4. 通过 `DeQue` 从输入队列取出输入张量，在 UB 中执行 `Add`，再通过 `EnQue` 将结果 `LocalTensor` 放入输出队列。
  5. 通过 `DeQue` 从输出队列取出结果，并使用 `DataCopy` 写回当前核负责的 GM 分片。
- 队列说明：
  本样例使用 `TPipe` 和 `TQue` 演示基础的队列式编程方式。`EnQue` 用于将已经搬到 UB 的 `LocalTensor` 入队，`DeQue` 用于在后续阶段从队列中取出张量继续处理。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Add</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">add_custom</td></tr>
  </table>

- 样例实现：
  - Kernel实现  
    `add_tpipe_tque.asc` 中的核函数入口 `add_custom` 接收 `totalLength`，并在核函数内部根据 `GetBlockNum()` 计算每个核要处理的 `blockLength`，再用 `GetBlockIdx()` 计算当前核在 GM 中对应的数据起点。之后，核函数直接完成输入数据搬入 UB 并入队、出队后执行 `Add`、结果写回 GM 的完整流程。

  - 核入口实现
    `add_custom` 负责创建 `TPipe`、`TQue` 和 `GlobalTensor` 对象，并按顺序执行搬入、计算、搬出处理链路。

  - 调用实现
    使用内核调用符`<<<>>>`调用核函数。调用时运行时参数传入Device侧x、y、z张量地址和总数据长度`totalLength`。

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                     # 执行编译生成的可执行程序，执行样例
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
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
