# Aclrtc运行时编译+静态Tensor编程Add样例

## 概述

本样例展示了如何使用AscendC的 **Aclrtc（运行时编译）** 模式，在Host侧动态编译核函数源码并执行。核函数内部采用 **静态Tensor编程范式**（LocalMemAllocator+SetFlag/WaitFlag事件同步），将核函数源码以字符串形式嵌入Host代码中，通过aclrtc API在运行时编译并执行。

与传统自定义算子工程不同，Aclrtc无需提前编译算子包（.run），适合快速验证和原型开发。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.1.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

```
├── rtc_template_add
│   ├── CMakeLists.txt                   // 编译工程文件
│   ├── data_utils.h                     // 数据生成、精度校验工具函数+ASCENDC_CHECK宏
│   ├── rtc_template_add.cpp             // Host侧代码（含RTC核函数源码字符串）
│   └── README.md                        // 样例说明文档
```

## 样例描述

- 样例功能：  
  Add计算公式：

  ```
  z = x + y
  ```

- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Add</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">Kernel::add_custom&lt;float&gt;</td></tr>
  </table>

### 样例实现

### Kernel实现

核函数源码以字符串形式嵌入Host代码中，通过LocalMemAllocator接口完成内存管理，通过SetFlag/WaitFlag接口完成事件同步，实现数据搬运、计算和结果搬出的完整流程。

### 调用实现

本样例的核心流程分为两个阶段：**编译阶段**（aclrtc 运行时编译模板核函数源码为 deviceELF）和 **执行阶段**（aclrt 加载 deviceELF 到设备并启动核函数，Host侧完成数据准备与精度校验）。

**1. 编译阶段：aclrtc 接口**

- `aclrtcCreateProg` — 创建编译程序对象，传入核函数源码字符串
- `aclrtcAddNameExpr` — 注册需要导出的模板核函数名（如`Kernel::add_custom<float>`）
- `aclrtcCompileProg` — 执行运行时编译，通过options传入`--npu-arch`指定NPU架构
- `aclrtcGetBinDataSize` / `aclrtcGetBinData` — 获取编译产物的二进制大小及数据（deviceELF）
- `aclrtcGetLoweredName` — 获取模板核函数编译后的mangled name，供后续`aclrtBinaryGetFunction`查找
- `aclrtcDestroyProg` — 销毁编译程序对象

**2. 执行阶段：关键 aclrt 接口**

- `aclrtMallocHost` / `aclrtMalloc` — 分配Host/Device内存，用于输入输出数据
- `aclrtMemcpy` — Host与Device间数据拷贝
- `aclrtBinaryLoadFromData` — 将编译产物加载到设备
- `aclrtBinaryGetFunction` — 通过编译阶段获取的mangled name查找核函数句柄
- `aclrtLaunchKernelWithArgsArray` — 通过Host侧参数数组传入三个Device内存指针参数（`x`、`y`、`z`），并在指定stream上启动核函数
- `aclrtSynchronizeStream` — 等待stream上核函数执行完成

Host侧C++内完成数据生成与精度校验，不依赖外部脚本。

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
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # 编译工程，支持dav-3510, dav-2201
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

  > [!WARNING] 注意
  Aclrtc仅支持NPU运行模式，不支持cpu调试或sim仿真模式。

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
