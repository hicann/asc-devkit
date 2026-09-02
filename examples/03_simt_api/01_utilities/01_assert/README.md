# SIMT assert断言功能实现样例

## 概述

本样例演示在SIMT编程下使用`assert()`接口实现上板进行功能调试的方法。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本
- \>= CANN 9.1.0

## 目录结构介绍

```text
├── 01_assert
│   ├── CMakeLists.txt         // cmake编译文件
│   ├── assert.asc             // Ascend C算子实现加assert断言的调用样例
│   └── README.md
```

## 算子描述

- 算子功能:

  本样例详细展示了在SIMT实现函数中使用```assert()```接口的实践方式，实现对算子执行过程中断言的调试。


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
          assert(in_shape > 1);
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

  编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |

  执行后有如下打印信息，说明功能正常。
  ```
  [SIMT] trap check 2!
  [SIMT] trap check 2!
  [SIMT] trap check 2!
  [SIMT] trap check 2!
  [SIMT] trap check 2!
  [SIMT] trap check 2!
  [SIMT] trap check 2!
  [SIMT] trap check 2!
  [SIMT] trap check start 1!
  [SIMT] trap check start 1!
  [SIMT] trap check start 2!
  [SIMT] trap check start 2!
  [SIMT] trap check start 3!
  [SIMT] trap check start 3!
  [ASSERT] xxx/assert.asc:32: void simt_assert(float *, uint32_t): Assertion `in_shape < 1' failed.
  [ASSERT] xxx/assert.asc:32: void simt_assert(float *, uint32_t): Assertion `in_shape < 1' failed.
  ```
