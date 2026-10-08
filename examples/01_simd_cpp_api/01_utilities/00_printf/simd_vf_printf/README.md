# SimdVF Printf接口功能说明

## 概述

本样例介绍simd_vf侧printf接口使用方法。核函数使用固定输入（不依赖外部数据），通过`printf`接口在vector函数中打印调试信息及计算结果。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

```
├── simd_vf_printf
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── simd_vf_printf.asc      // Ascend C样例实现&调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：

  使用vector编程模式，核函数内使用固定输入展示simd_vf printf接口的基本使用方法。核函数依次调用simd_vf侧printf接口打印不同类型的数据和计算结果。

  本样例展示以下printf功能：
  1. `printf`打印int/uint/float/string格式数据
  2. `printf`打印hex/ptr格式数据
  3. simd_vf侧`printf`打印纯字符串
  4. 在`.asc`内使用固定输入写入静态`LocalTensor`，通过AICore侧`AscendC::Add`完成tensor加法，并通过simd_vf侧`printf`打印计算结果
  5. aicore侧`AscendC::printf`打印核函数开始、blockIdx和结束信息

  | 格式符 | 说明 | 示例 |
  |--------|------|------|
  | `%d` | 整型打印 | `printf(fmt, 10);` |
  | `%u` | 无符号整型打印 | `printf(fmt, 20U);` |
  | `%x` | 十六进制打印 | `printf(fmt, 255);` |
  | `%f` | 浮点型打印(float) | `printf(fmt, 3.14f);` |
  | `%s` | 字符串打印 | `printf(fmt, "test");` |
  | `%p` | 指针打印 | `printf(fmt, ptr);` |

- 调用实现

  在`__global__ __vector__`核函数内使用`asc_vf_call<FuncName>()`调用simd_vf函数。核函数使用固定输入值，不依赖外部数据或host侧数据搬运。Add演示中，AICore侧将固定数据写入静态UB `LocalTensor`，调用`AscendC::Add`生成结果，再将UB地址传给simd_vf函数打印。

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # 编译工程
  ./demo                                                                     # 执行样例
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU架构：dav-3510对应Ascend 950PR&950DT系列产品 |

- 执行结果
  执行日志中可以看到aicore侧和simd_vf侧printf输出。
  ```bash
  [AIV Block 0/1] [aicore] simd_vf printf demo start ...
  [AIV Block 0/1] [aicore] blockIdx=0
  [simd_vf] int=10, uint=20, float=3.140000, string=hello
  [simd_vf] hex=ff, ptr=0x12ff
  [simd_vf] This is a simd_vf printf demo string.
  [simd_vf] add[0]: 1.250000 + 10.000000 = 11.250000
  [AIV Block 0/1] [aicore] simd_vf printf demo end ...
  ```
