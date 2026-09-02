# Ascend日志功能样例

## 概述

本样例演示在SIMD编程模式下，Ascend日志功能的完整使用方式，包括日志打屏输出、日志落盘存储、日志级别控制等功能。通过配置环境变量，开发者可以灵活控制日志输出行为，辅助样例开发与问题定位。

更多日志功能详情请参考：[Ascend 日志功能参考](https://hiascend.com/document/redirect/CannCommunitylogref)

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── 09_log
│   ├── CMakeLists.txt          // cmake编译文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── log.asc                 // Ascend C Kernel示例代码
│   ├── README.md
│   └── scripts
│       ├── gen_data.py         // 输入数据和真值数据生成脚本
│       └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
```

## 样例描述

- 日志功能说明：
  - **日志打屏**：通过环境变量控制日志是否输出到标准输出（屏幕），便于实时查看
  - **日志落盘**：通过环境变量指定日志文件存储路径，便于后续分析
  - **日志级别**：通过环境变量控制日志输出级别，按需过滤日志内容

  日志级别说明：

  | 值 | 级别 | 说明 |
  |----|------|------|
  | 0 | DEBUG | 输出所有日志（DEBUG/INFO/WARNING/ERROR），信息最详细 |
  | 1 | INFO | 输出INFO及以上级别日志（INFO/WARNING/ERROR） |
  | 2 | WARNING | 输出WARNING及以上级别日志（WARNING/ERROR） |
  | 3 | ERROR | 仅输出ERROR级别日志 |
  | 4 | NULL | 不输出日志 |

- 样例实现：
  本样例使用静态Tensor编程模式实现矩阵乘法（Matmul），通过构造异常场景，演示Kernel侧异常抛出与Host侧错误捕获的完整流程：

  - Kernel实现
    在Kernel函数中使用标准C `assert` 断言，当条件不满足时触发设备侧异常。

  - Host实现
    Kernel执行后，Host侧通过 `ASCENDC_CHECK` 宏捕获并打印错误信息。`ASCENDC_CHECK` 是用于捕获并打印ACL错误的工具宏，定义如下：

    ```c
    #define ASCENDC_CHECK(expr) do { \
        aclError ret = (expr); \
        if (ret != ACL_SUCCESS) { \
            fprintf(stderr, "Ascend Error: %s:%d code=%d %s\n", \
                __FILE__, __LINE__, ret, aclGetRecentErrMsg()); \
        } \
    } while(0)
    ```

    将ACL接口调用作为参数传入，宏会自动检查返回值并在失败时打印错误信息。

    ```c
    ASCENDC_CHECK(aclrtGetLastError(ACL_RT_THREAD_LEVEL));
    ASCENDC_CHECK(aclrtSynchronizeDevice());
    ```

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 配置日志环境变量（仅NPU模式）
  本样例支持通过环境变量控制日志输出行为（以下环境变量仅在NPU模式下生效）：
  ```bash
  export ASCEND_PROCESS_LOG_PATH=./log        # 日志落盘路径
  export ASCEND_SLOG_PRINT_TO_STDOUT=1        # 控制日志是否打印到屏幕（1：开启，0：关闭）
  export ASCEND_GLOBAL_LOG_LEVEL=1            # 控制日志级别
  ```

  > 注意：
  > - 日志环境变量控制Ascend内部库（RUNTIME/ASCENDCL等）的日志输出。Kernel内部的`printf`输出不受这些环境变量影响。
  > - 日志打屏和落盘功能互斥。开启打屏（`ASCEND_SLOG_PRINT_TO_STDOUT=1`）后，即使配置了 `ASCEND_PROCESS_LOG_PATH` 也不会落盘。
  >   - 打屏模式下，可通过 shell 重定向保存日志：`./demo > err.log 2>&1`
  >   - 落盘模式下，需关闭打屏并配置 `ASCEND_PROCESS_LOG_PATH`，日志将写入指定目录

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：<br>dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品<br>dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果（NPU模式，以非打屏模式下为例，落盘文件详情可参考[Ascend 日志功能参考](https://hiascend.com/document/redirect/CannCommunitylogref)）
  执行后会打印类似以下信息：
  ```bash
  [INFO] Matrix shape: M=256, K=64, N=256
  [INFO] Launching kernel with assert(M < 100)...
  Ascend Error: <your_path>/09_log/log.asc:<line> code=507015 EZ9999: Inner Error!
  ...
  rtDeviceSynchronize execution failed, reason=aicore exception
  ...

  [ASSERT] <your_path>/09_log/log.asc:<line>: : Assertion 'M < 100 && "M exceeds expected limit!"' failed.
  [INFO] Execution completed. Check for error messages above.
  ```

  > **说明：**
  > - `<your_path>` 表示样例代码所在目录的绝对路径，实际输出中会替换为具体路径
  > - `<line>` 表示断言或错误捕获所在的代码行号，实际输出中会替换为具体数字
  > - 关键错误信息解读：
  >   - `Ascend Error: ... code=507015 EZ9999: Inner Error!` — ACL接口返回的底层错误码
  >   - `aicore exception` — AI Core执行异常，Kernel任务失败
  >   - `Assertion 'M < 100 ...' failed` — 断言失败的具体位置和条件，帮助定位问题代码
