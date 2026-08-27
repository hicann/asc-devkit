# HelloWorld样例

## 概述

本样例为SIMT编程入门样例，通过使用`<<<>>>`内核调用符来完成样例核函数在NPU侧运行验证的基础流程，核函数内通过`printf()`打印输出结果。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本
- \>= CANN 9.1.0

## 目录结构介绍

```
├── hello_world_simt
│   ├── CMakeLists.txt      // 编译工程文件
│   └── hello_world.asc     // Ascend C SIMT编程样例实现 & 调用样例
```

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
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # 编译工程
  ./demo                        # 执行样例
  ```
  
  编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |

- 执行结果
  执行结果如下，说明执行成功。

  ```bash
  [blockIdx (0/2)][threadIdx (2/32)]: Hello World!
  [blockIdx (0/2)][threadIdx (1/32)]: Hello World!
  [blockIdx (0/2)][threadIdx (0/32)]: Hello World!
  [blockIdx (1/2)][threadIdx (2/32)]: Hello World!
  [blockIdx (1/2)][threadIdx (1/32)]: Hello World!
  [blockIdx (1/2)][threadIdx (0/32)]: Hello World!
  ```

  输出共6行，每行对应一个线程的打印结果。`[blockIdx (X/2)]` 表示当前为第X个线程块（共2个线程块），`[threadIdx (Y/32)]` 表示线程块内第Y个线程，说明核函数已成功在AIV Core上执行。
