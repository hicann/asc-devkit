# 概述

## 接口分类

SIMT API是面向AI处理器的并行计算编程接口，可以实现高效的数据并行计算。SIMT API支持两种编程模型：SIMT编程、SIMD与SIMT混合编程，用户可先阅读[SIMT编程简介](SIMT_programming_intro/SIMT_programming_intro.md)和[SIMD与SIMT混合编程简介](SIMD_SIMT_hybrid_programming_intro/SIMD_SIMT_hybrid_programming_intro.md)，以了解编程基础，后续章节将详细介绍API接口。

**表1**  SIMT API分类列表

| 类别 | 功能 |
| --- | --- |
| [同步与内存栅栏](sync_and_memory_fence/sync_and_memory_fence.md) | 内存管理与同步接口，解决不同核内的线程间可能存在的数据竞争以及线程的同步问题。 |
| [原子操作](atomic_operations/atomic_operations.md) | 对UB或Global Memory上的数据与指定数据执行原子操作的一系列接口。 |
| [Warp函数](Warp_functions/Warp_functions.md) | 对单个Warp内32个线程的数据进行处理的相关操作的一系列API接口。 |
| [数学函数](math_functions/math_functions.md) | 用于处理数学运算的函数集合以及不同精度、数据类型的转换函数集合 |
| [地址空间谓词函数](address_space_predicate_functions/address_space_predicate_functions.md) | 判断输入指针是否为指定空间的地址。 |
| [地址空间转换函数](address_space_conversion_functions/address_space_conversion_functions.md) | 将指定地址空间的地址值转换为指针，或将输入的指针转换为对应内存空间的地址值的接口。 |
| [访存函数](memory_access_functions/memory_access_functions.md) | 数据加载和数据缓存相关接口。 |
| [协作组](cooperative_groups/cooperative_groups.md) | 提供一套标准且安全的机制，实现更高效的线程并行协作。 |
| [调测接口](../Utils-API/tuning_interface/tuning_interface.md) | SIMT VF调试场景下使用的相关接口。 |

## 调用接口依赖的头文件和库文件说明

安装CANN软件包并配置Ascend C编译环境后，编译SIMT源文件时才能引用SIMT API头文件。SIMT API公开头文件位于`${INSTALL_DIR}/asc/include/simt_api/`目录下，代码中通过`#include "simt_api/xxx.h"`方式引用。`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径，以root用户安装为例，默认路径为`/usr/local/Ascend/cann`。

**表2**  头文件列表

| 头文件 | 定位 |
| --- | --- |
| `simt_api/asc_simt.h` | SIMT基础API聚合头文件，适用于常规SIMT编程场景。 |
| `simt_api/common_functions.h` | SIMT通用能力头文件，提供SIMT VF调用等基础定义。 |
| `simt_api/device_functions.h` | 设备函数头文件，覆盖地址空间谓词、地址空间转换、访存函数、Lane-ID类函数等接口，并包含同步、原子和Warp函数头文件。 |
| `simt_api/device_sync_functions.h` | 同步与内存栅栏接口头文件。 |
| `simt_api/device_atomic_functions.h` | 原子操作接口头文件。 |
| `simt_api/device_warp_functions.h` | Warp级投票、数据交换和规约接口头文件。 |
| `simt_api/math_functions.h` | 数学函数和数值转换函数头文件，主要覆盖除`half`、`half2`、`bfloat16_t`、`bfloat16x2_t`、FP8相关类型外的数据类型接口。 |
| `simt_api/vector_functions.h` | 短向量构造函数头文件。 |
| `simt_api/cooperative_groups.h` | 协作组接口头文件。 |
| `simt_api/asc_fp16.h` | `half`、`half2`类型SIMT接口头文件。 |
| `simt_api/asc_bf16.h` | `bfloat16_t`、`bfloat16x2_t`类型SIMT接口头文件。 |
| `simt_api/asc_fp8.h` | `hifloat8x2_t`、`float8_e4m3x2_t`、`float8_e5m2x2_t`类型SIMT接口头文件。 |

使用SIMT API时，请根据实际调用的接口包含对应头文件。

**表3**  各类SIMT API需要包含的头文件

| 类别 | 需要包含的头文件 |
| --- | --- |
| [同步与内存栅栏](sync_and_memory_fence/sync_and_memory_fence.md) | `#include "simt_api/device_sync_functions.h"` |
| [原子操作](atomic_operations/atomic_operations.md) | `#include "simt_api/device_atomic_functions.h"` |
| [Warp函数](Warp_functions/Warp_functions.md) | `#include "simt_api/device_warp_functions.h"` |
| [数学函数](math_functions/math_functions.md) | 数学函数标准库接口：`#include "simt_api/math_functions.h" `<br><br>数学函数的intrinsic接口：`#include "simt_api/device_functions.h"` |
| [地址空间谓词函数](address_space_predicate_functions/address_space_predicate_functions.md)<br><br>[地址空间转换函数](address_space_conversion_functions/address_space_conversion_functions.md)<br><br>[访存函数](memory_access_functions/memory_access_functions.md) | `#include "simt_api/device_functions.h"` |
| [协作组](cooperative_groups/cooperative_groups.md) | `#include "simt_api/cooperative_groups.h"` |

> [!NOTE]说明
>
> 表3为按接口类别给出的最小依赖头文件；若已包含`simt_api/asc_simt.h`，则无需再单独包含`device_sync_functions.h`、`device_atomic_functions.h`、`device_warp_functions.h`、`math_functions.h`、`device_functions.h`和`vector_functions.h`。

使用`half`、`half2`、`bfloat16_t`、`bfloat16x2_t`、`hifloat8x2_t`、`float8_e4m3x2_t`、`float8_e5m2x2_t`等数据类型相关接口时，还需按表4包含对应扩展头文件。

**表4**  不同数据类型下使用接口需要包含的头文件

| 数据类型 | 需要包含的头文件 |
| --- | --- |
| 除`half`、`half2`、`bfloat16_t`、`bfloat16x2_t`、`hifloat8x2_t`、`float8_e4m3x2_t`、`float8_e5m2x2_t`以外的类型 | `#include "simt_api/asc_simt.h"` |
| `half`、`half2` | `#include "simt_api/asc_fp16.h"` |
| `bfloat16_t`、`bfloat16x2_t` | `#include "simt_api/asc_bf16.h"` |
| `hifloat8x2_t`、`float8_e4m3x2_t`、`float8_e5m2x2_t` | `#include "simt_api/asc_fp8.h"` |

SIMT API属于核函数（Kernel）侧基础接口，使用时无需额外链接库文件。