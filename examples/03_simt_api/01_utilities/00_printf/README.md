# SIMT printf功能实现样例

## 概述

本样例演示在SIMT编程下使用`printf()`接口实现上板打印进行功能调试的方法。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本
- \>= CANN 9.1.0

## 目录结构介绍

```text
├── 00_printf
│   ├── CMakeLists.txt         // cmake编译文件
│   ├── printf.asc             // Ascend C算子实现加printf打印的调用样例
│   └── README.md
```

## 算子描述

- 算子功能:

  本样例详细展示了在SIMT核函数中使用`printf()`接口的实践方式，实现在算子执行过程中打印线程的变量信息（示例通过`threadIdx.x < 3`限制仅打印部分线程）。


- 算子实现:
  ```cpp
  __global__ void simt_printf(float* input, uint32_t in_shape)
  {
      // Calculate global thread ID
      int32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
      if (threadIdx.x < 3) {
          printf("[SIMT %s] thread index[%u], input data shape: %u\n", "print 1", idx, in_shape);
          printf("[SIMT %s] input addr: %p value[%u]: %f\n", "print 2", input, idx, input[idx]);
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

  执行后有如下打印信息，说明打印功能正常。
  ```
  [SIMT print 1] thread index[34], input data shape: 128
  [SIMT print 1] thread index[2], input data shape: 128
  [SIMT print 1] thread index[33], input data shape: 128
  [SIMT print 1] thread index[1], input data shape: 128
  [SIMT print 1] thread index[32], input data shape: 128
  [SIMT print 1] thread index[0], input data shape: 128
  [SIMT print 2] input addr: 0x120000016000 value[2]: 3.118000
  [SIMT print 2] input addr: 0x120000016000 value[1]: 2.118000
  [SIMT print 2] input addr: 0x120000016000 value[0]: 1.118000
  [SIMT print 2] input addr: 0x120000016000 value[34]: 35.118000
  [SIMT print 2] input addr: 0x120000016000 value[33]: 34.118000
  [SIMT print 2] input addr: 0x120000016000 value[32]: 33.118000
  ```
