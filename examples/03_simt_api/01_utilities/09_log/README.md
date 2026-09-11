# SIMT Kernel日志功能样例

## 概述

本样例演示在SIMT编程模式下，Ascend日志功能的完整使用方式，包括日志打屏输出、日志落盘存储、日志级别控制等功能。通过配置环境变量，开发者可以灵活控制日志输出行为，辅助算子开发与问题定位。

更多日志功能详情请参考：[Ascend 日志功能参考](https://hiascend.com/document/redirect/CannCommunitylogref)

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.1.0

## 目录结构介绍

```
├── 09_log
│   ├── CMakeLists.txt         // cmake编译文件
│   ├── log.asc                // SIMT Kernel示例代码
│   └── README.md
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
  本样例通过构造异常场景，演示Kernel侧异常抛出与Host侧错误捕获的完整流程：

  - Kernel实现  
    在Kernel函数中使用标准C `assert` 断言，当条件不满足时触发设备侧异常。为避免所有线程重复打印，通常仅由 `thread 0` 触发断言。

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
    // 捕获Kernel执行错误（包括assert触发的错误）
    ASCENDC_CHECK(aclrtGetLastError(ACL_RT_THREAD_LEVEL));
    ASCENDC_CHECK(aclrtSynchronizeDevice());
    ```

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # 编译工程（默认npu模式）
  ./demo
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明
  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |

- 执行结果（以非打屏模式下为例，落盘文件详情可参考[Ascend 日志功能参考](https://hiascend.com/document/redirect/CannCommunitylogref)）  
  执行后会打印类似以下信息：
  ```bash
  [INFO] Input shape: 12288
  [INFO] Launching kernel with assert(total_length < 100)...
  Ascend Error: <your_path>/09_log/log.asc:88 code=507035 EZ9999: Inner Error!
  ...
  rtDeviceSynchronize execution failed, reason=vector core exception
  ...

  [ASSERT] <your_path>/09_log/log.asc:47: void add_custom(float *, float *, float *, uint64_t): Assertion `total_length < 100 && "Total length exceeds expected limit!"' failed.
  [INFO] Execution completed. Check for error messages above.
  ```

  > **说明：**
  > - `<your_path>` 表示样例代码所在目录的绝对路径，实际输出中会替换为具体路径
  > - 关键错误信息解读：
  >   - `Ascend Error: ... code=507035 EZ9999: Inner Error!` — ACL接口返回的底层错误码
  >   - `vector core exception` — AI Core执行异常，Kernel任务失败
  >   - `Assertion 'total_length < 100 ...' failed` — 断言失败的具体位置和条件，帮助定位问题代码

  同时在`./log`目录下会生成日志文件，包含详细的运行日志。
