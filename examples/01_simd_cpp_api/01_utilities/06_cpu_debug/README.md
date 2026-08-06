# CPU Debug直调样例说明

## 概述

本样例以 Add 计算为载体，重点演示 Ascend C CPU Debug 功能的使用流程，包括 CPU 域编译、可执行程序运行、通过 GDB 进入调试模式，以及在核函数中设置断点、单步执行和查看变量。样例用于帮助开发者在不依赖 NPU 运行的情况下，对 Ascend C 核函数逻辑进行本地调试和问题定位。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── 06_cpu_debug
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── cpu_debug.asc           // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- CPU Debug介绍：  
  CPU Debug功能支持对CPU执行过程中的运行状态进行调试，主要通过GDB工具实现。GDB调试支持设置断点、查看寄存器和内存状态、单步执行、查看调用栈等常用调试操作。

- 样例介绍：  
  本样例的Add实现与[Add基础样例](../../00_introduction/01_add/add/README.md)保持一致，默认使用`float`类型。每个block处理`2048`个数据，流程为`DataCopy`输入到UB、执行`Add`、再将结果`DataCopy`回GM。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;
  ./cpu_debug
  ```
  请根据实际测试的 NPU 硬件架构选择对应的 `CMAKE_ASC_ARCHITECTURES` 参数。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `cpu` | 指定为`cpu`，开启CPU域编译 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`、`dav-3510` | 指定NPU架构版本号，CMake会根据该值配置对应的CPU调试依赖库。`dav-2201`对应Atlas A2训练系列产品/Atlas A2推理系列产品和Atlas A3训练系列产品/Atlas A3推理系列产品，`dav-3510`对应Ascend 950PR/Ascend 950DT |

- 执行结果  
  执行结果如下，说明执行成功。
  ```bash
  [Success] Case accuracy is verification passed.
  ```
- 进入gdb模式调试  
  > **说明：** 需使用 GDB 12.1及以上版本。
  
  编译生成的CPU域可执行程序支持通过gdb进行调试。gdb支持设置断点、查看寄存器和内存状态、单步执行、查看调用栈等常用调试操作。在上述指令中"./cpu_debug"前加入"gdb --args"，再次执行指令即可进入gdb模式。
  ```bash
  gdb --args ./cpu_debug
  ```
  CPU Debug通过为每个核函数启动单独的子进程来模拟NPU的执行逻辑，因此使用gdb调试时，需要设置`follow-fork-mode`让gdb跟踪子进程，才能在核函数内部断点调试。进入gdb后，先设置跟踪子进程模式：

  ```text
  (gdb) set follow-fork-mode child
  ```

  然后按需进行调试，常用操作：

  ```text
  # 在核函数入口处设置断点
  (gdb) break add_custom

  # 运行程序
  (gdb) run

  # 单步执行
  (gdb) next

  # 继续执行到下一个断点
  (gdb) continue
  ```
