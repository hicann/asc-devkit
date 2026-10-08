# AI CPU算子Tiling下沉样例介绍

## 概述

本样例介绍使用AI CPU算子进行tiling下沉计算。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍
```
├── aicpu_device_tiling
│   ├── CMakeLists.txt                     // 编译工程文件
│   ├── aicore_kernel.asc                  // AI Core算子实现
│   ├── kernel_args.h                      // tiling结构体头文件
│   ├── main.asc                           // AI CPU算子与AI Core算子调用
│   ├── aicpu_tiling.aicpu                 // AI CPU算子实现
│   └── README.md                          // 样例说明文档
```

## 样例描述
- main.asc中AI CPU算子与AI Core算子均使用内核调用符<<<...>>>进行调用，AI CPU算子将tiling计算的结果传给AI Core算子。
- AI CPU算子与AI Core算子在不同stream上进行launch，样例中分别为aicpu_stream与aicore_stream，event用于记录stream上已下发的任务。使用aclrtRecordEvent在指定stream中记录event，使用aclrtStreamWaitEvent阻塞指定的stream，直到指定的event完成。

## 编译运行
在本样例根目录下执行如下步骤。
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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程
  ./demo                           # 执行编译生成的可执行程序，执行样例
  ```

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品 与 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果

  执行结果如下，说明执行成功：
  其中，`__mix__(1, 2)`会启动1个Cube执行单元和2个Vector执行单元，因此`Hello World`日志会打印3次。

  ```bash
  MyAicpuKernel inited
  MyAicpuKernel inited type 1 mode 2 len 4 end!
  Hello World: int mode 2 len 4 m 10.
  Hello World: int mode 2 len 4 m 10.
  Hello World: int mode 2 len 4 m 10.
  ```
