# SIMT assert断言功能实现样例

## 概述

本样例演示在SIMT编程下使用`assert()`与`ascendc_assert()`接口实现上板进行功能调试的方法。

`ascendc_assert()`支持消息形式：断言条件不成立时，除标准断言头外还输出用户自定义的格式化消息（与`printf`同风格的格式串与变长实参），便于携带业务上下文定位问题。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|---|---|
| Ascend 950PR/Ascend 950DT | \>= CANN 9.2.0 |

## 目录结构介绍

```text
├── 01_assert
│   ├── CMakeLists.txt         // cmake编译文件
│   ├── assert.asc             // Ascend C算子实现加assert断言的调用样例
│   └── README.md
```

## 算子描述

- 算子功能:

  本样例详细展示了在SIMT实现函数中使用`assert()`与`ascendc_assert()`接口的实践方式，实现对算子执行过程中断言的调试。其中`ascendc_assert()`演示消息形式（携带格式串与实参）的用法。


- 算子实现:
  ```cpp
  __global__ void simt_assert(float* input, uint32_t in_shape)
  {
      // Calculate global thread ID
      int32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
      if (threadIdx.x < 1) {
          printf("[SIMT] %s\n", "trap check start 1!");
          printf("[SIMT] %s\n", "trap check start 2!");
          printf("[SIMT] %s\n", "trap check start 3!");
          assert(in_shape < 1);
          printf("[SIMT] %s\n", "trap check 1!");
      } else if(threadIdx.x < 5) {
          printf("[SIMT] %s\n", "trap check 2!");
          // 消息形式：断言失败时在断言头之后输出用户自定义消息，随后trap终止核函数
          ascendc_assert(in_shape >= MIN_SHAPE, "in_shape %u must be >= %u\n", in_shape, MIN_SHAPE);
          ascendc_assert(input != nullptr, "input tensor of thread %u is nullptr\n", threadIdx.x);
          printf("[SIMT] %s\n", "trap check 3!");
      }
  }
  ```

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # 编译工程
  ./demo                        # 执行样例
  ```

  样例通过命令行参数控制`in_shape`，演示断言的不同路径：

  - 不带参数（默认`in_shape=128`）：`ascendc_assert()`条件成立不触发，`assert()`条件不成立触发（无消息形式，仅输出断言条件文本）。
  - `./demo 32`（`in_shape=32 < 64`）：`assert()`条件成立不触发，`ascendc_assert()`条件成立触发消息形式断言，输出断言头与用户自定义消息后trap终止。

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

  编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |

  执行后有如下打印信息，说明功能正常。
  ```
  [SIMT] trap check 2!
  [SIMT] trap check 2!
  [SIMT] trap check start 1!
  [SIMT] trap check start 1!
  [SIMT] trap check start 2!
  [SIMT] trap check start 2!
  [SIMT] trap check start 3!
  [SIMT] trap check start 3!
  [ASSERT] xxx/assert.asc:37: void simt_assert(float *, uint32_t): Assertion `in_shape < 1' failed.
  [ASSERT] xxx/assert.asc:37: void simt_assert(float *, uint32_t): Assertion `in_shape < 1' failed.
  ```

  传入小于64的参数（如`./demo 32`）触发消息形式断言，输出断言头与用户自定义消息（末尾`in_shape 32 must be >= 64`即用户消息）：
  ```
  [ASSERT] xxx/assert.asc:43: void simt_assert(float *, uint32_t): Assertion `in_shape >= MIN_SHAPE' failed. in_shape 32 must be >= 64
  ```
