# 核间顺序同步样例

## 概述

本样例在确定性计算场景下，基于InitDetermineComputeWorkspace、WaitPreBlock和NotifyNextBlock三个接口实现核间顺序同步。**这三个接口必须组合使用**，能够确保多个AIV核严格按照blockIdx的升序执行，适用于要求确定性计算的场景。本样例模拟8个核进行数据处理，使用确定性计算接口保证核间运行顺序，进行原子累加，确保计算结果的确定性。

> **注意：** 本样例仅适用于基于TPipe和TQue的编程模式。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── sequential_block_sync
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   │   └── verify_result.py           // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── sequential_block_sync.asc      // Ascend C样例实现 & 调用样例
│   └── README.md                      // 样例说明文档
```

## 样例功能描述

本样例通过8个核协同工作，每个核处理256个float数据。使用InitDetermineComputeWorkspace初始化GM共享内存的同步状态，然后通过WaitPreBlock和NotifyNextBlock确保核间按照blockIdx升序执行。每个核将输入数据分两次tile（每次128个元素）通过原子累加写入输出缓冲区，保证计算结果的确定性。

### 样例规格

<table>
<caption>表1：样例输入输出规格</caption>
<tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[256]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">workspace</td><td align="center">[256] Bytes</td><td align="center">uint8_t</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[256]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">kernel_sequential_block_sync</td></tr>
<tr><td rowspan="1" align="center">核数量</td><td colspan="5" align="center">8</td></tr>
</table>

### 计算流程

1. **初始化阶段**：调用InitDetermineComputeWorkspace初始化GM共享内存的同步状态
2. **数据搬入**：从GM搬入256个元素到UB，分两次tile（每次128个元素）
3. **核间同步**：通过WaitPreBlock等待前序核完成
4. **原子累加**：开启SetAtomicAdd，将数据原子累加写入GM
5. **通知后序核**：通过NotifyNextBlock通知后序核可以开始执行


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
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j;    # 编译工程
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # 验证输出结果是否正确
  ```

  使用CPU调试或NPU仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # CPU调试模式
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # NPU仿真模式
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
