# DataSyncBarrier样例

## 概述

本样例介绍DataSyncBarrier的调用，该接口功能：阻塞后续的指令执行，直到所有之前的内存访问指令（需要等待的内存位置可通过参数控制）执行结束。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```txt
├── data_sync_barrier
│   ├── scripts
│   │   ├── gen_data.py            // 输入数据和真值数据生成脚本
│   │   └── verify_result.py       // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt             // 编译工程文件
│   ├── data_utils.h               // 数据读入写出函数
│   ├── data_sync_barrier.asc      // Ascend C样例实现 & 调用样例
│   └── README.md                  // 样例说明文档
```

## 样例规格

<table border="2">
<caption>表1：样例规格对照表</caption>
<tr><th>类型</th><th>名称</th><th>Shape</th><th>数据类型</th><th>格式</th></tr>
<tr><td>输入</td><td>srcGm</td><td>[1, 8]</td><td>int32_t</td><td>ND</td></tr>
<tr><td>输出</td><td>dstGm</td><td>[1, 8]</td><td>int32_t</td><td>ND</td></tr>
<tr><td>核函数名</td><td colspan="4" style="text-align:center;">kernel_data_sync_barrier</td></tr>
</table>

## 样例说明

下面按步骤介绍本样例中 `DataSyncBarrier` 的使用场景：

1. 系统中有两个 AIV 核，分别记为核 0 和核 1。GM 中的两个变量 `x` 和 `y` 初始值都为 1。
2. 核 0 先通过标量流水接口`WriteGmBypassDCache`向 `srcGm[1]` 写入 `x=7`。
3. 核 0 再插入 `DataSyncBarrier<AscendC::MemDsbT::DDR>()`，等待前一次 GM 写操作完成。
4. 核 0 随后向 `srcGm[0]` 写入 `y=6`。
5. 核 1 持续轮询 `srcGm[0]`，直到读到 `y=6`，再读取 `srcGm[1]`，并将 `2 * x` 写入输出。

预期行为是：

- 当核 1 读到 `y=6` 时，`x=7` 必须已经写回 GM。
- 因此核 1 读到的 `x` 应为 7，最终输出应为 14。

如果不加同步，标量流水不保证两次 GM 写入的先后顺序，可能出现 `y` 已更新而 `x` 尚未完成写入的情况。这样核 1 即使已经看到 `y=6`，也可能读到错误的 `x` 值。

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

  使用CPU调试时，添加 `-DCMAKE_ASC_RUN_MODE=cpu`参数即可。

  示例如下：

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # CPU调试模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu` | 运行模式：NPU运行、CPU调试|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
