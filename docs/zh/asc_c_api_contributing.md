# C API贡献指南

## 概述

C API 是Ascend C编程框架中的指令层API，直接对应昇腾AI处理器的硬件指令集。C API采用C语言风格的函数接口，为算子开发提供最接近硬件的编程能力。

C API的主要特点：

- **指令级映射**：每个API直接映射到一条或多条硬件指令。
- **指针式编程**：直接操作内存地址，无抽象层开销。
- **高性能**：零开销封装，最大化硬件性能。
- **精细控制**：支持流水线、mask、stride等底层控制。

本文档介绍如何基于Ascend C框架开发新的C API，包括编程基础、目录规划、架构设计和回归验证要求。

---

## 开发流程

C API的开发流程主要包括以下步骤：

- **需求分析**：明确API功能。
- **接口设计**：定义C风格的函数接口。
- **实现开发**：编写指令封装和参数转换代码。
- **测试验证**：完成Mock测试和功能测试。
- **文档编写**：完成API文档和约束说明。

---

## API介绍

### 硬件指令基础

C API直接封装硬件指令，需要理解：

#### 指令类型

- **矢量指令**：VADD、VMUL、VCONV、VREDUCE等
- **标量指令**：SADD、SMUL等
- **数据搬运指令**：VMOV、DCOPY等
- **控制指令**：SETMASK、PIPEBARRIER等

#### 指令属性

- **流水线类型**：V、M、S、MTE1/2/3等
- **数据类型**：half、float、int16、int32等
- **操作数个数**：一元、二元、三元运算
- **同步特性**：异步/同步执行

### 编程模型

C API采用**指针式编程模型**：

```cpp
// C API编程示例
constexpr uint32_t count = 64;
__ubuf__ half dst[count];   // Unified Buffer（UB）内存
__ubuf__ half src0[count];
__ubuf__ half src1[count];


// 直接调用C API
asc_add(dst, src0, src1, count);
```

#### 数据流模式

C API支持三种数据流模式：

模式1：前n个数据计算

```cpp
asc_add(dst, src0, src1, count);
```

- 连续内存块计算
- 自动处理mask
- 简单易用

模式2：高维切分计算

```cpp
asc_add(dst, src0, src1, repeat,
        dst_block_stride, src0_block_stride, src1_block_stride,
        dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
```

- 非连续内存访问
- 支持多维数据
- 灵活控制

模式3：同步计算

```cpp
asc_add_sync(dst, src0, src1, count);
```

- 自动同步流水线
- 保证数据一致性
- 用于关键路径

### 参数约定

#### 通用参数

| 参数名 | 类型 | 说明 |
| -------- | ------ | ------ |
| dst | `__ubuf__ T*` | 输出地址 |
| src0, src1 | `__ubuf__ T*` | 输入地址 |
| count | `uint32_t` | 元素个数 |
| repeat | `uint8_t` | 迭代次数 |
| block_stride | `uint8_t` | Block内地址步长 |
| repeat_stride | `uint8_t` | Repeat间地址步长 |
| mask0, mask1 | `uint64_t` | 计算mask |

#### Mask计算

- **半精度**：每repeat处理256字节（128个half）
  - mask0对应低64个元素，mask1对应高64个元素
  - count=64时，mask0=0xFFFFFFFFFFFFFFFF, mask1=0

- **单精度**：每repeat处理256字节（64个float）
  - mask0对应全部64个元素
  - count=32时，mask0=0x00000000FFFFFFFF

#### Stride计算

- **Block Stride**：单次迭代内不同DataBlock的地址偏移
  - 通常为1（连续访问）
  - 可用于转置、分块访问

- **Repeat Stride**：相邻迭代间相同DataBlock的地址偏移
  - 半精度：8（256字节对齐）
  - 单精度：4（256字节对齐）

---

## 目录规划

### 目录结构

```text
asc-devkit/
├── include/
│   └── c_api/                    # C API头文件
│       ├── vector_compute/       # 矢量计算
│       │   └── vector_compute.h
│       ├── scalar_compute/       # 标量计算
│       │   └── scalar_compute.h
│       ├── vector_datamove/      # 矢量数据搬运
│       │   └── vector_datamove.h
│       ├── cube_compute/         # Cube计算
│       │   └── cube_compute.h
│       ├── cube_datamove/        # Cube数据搬运
│       │   └── cube_datamove.h
│       ├── sync/                 # 同步控制
│       │   └── sync.h
│       ├── cache_ctrl/           # 缓存控制
│       │   └── cache_ctrl.h
│       ├── atomic/               # 原子操作
│       │   └── atomic.h
│       ├── sys_var/              # 系统变量
│       │   └── sys_var.h
│       ├── misc/                 # 杂项
│       │   └── misc.h
│       ├── utils/                # 工具和常量
│       │   ├── debug/
│       │   │   ├── asc_dump.h
│       │   │   ├── asc_printf.h
│       │   │   └── asc_assert.h
│       │   ├── c_api_constants.h
│       │   ├── c_api_union.h
│       │   └── c_api_utils_intf.h
│       ├── asc_simd.h            # 总入口
│       └── c_api_utils_intf.h    # 工具接口
│
├── impl/
│   └── c_api/                    # C API实现
│       ├── instr_impl/           # 指令实现
│       │   ├── npu_arch_2201/    # NPU ARCH 220X架构
│       │   │   ├── vector_compute_impl/
│       │   │   │   ├── asc_abs_impl.h
│       │   │   │   └── ...
│       │   │   ├── vector_datamove_impl/
│       │   │   │   ├── asc_copy_gm2ub_align_impl.h
│       │   │   │   └── ...
│       │   │   ├── vector_compute_impl.h
│       │   │   ├── vector_datamove_impl.h
│       │   │   └── ...
│       │   └── npu_arch_3510/    # NPU ARCH 351X架构
│       │       ├── vector_compute_impl/
│       │       │   ├── asc_abs_impl.h
│       │       │   └── ...
│       │       ├── vector_compute_impl.h
│       │       └── ...
│       ├── utils/
│       │   └── c_api_utils_impl.cpp
│       └── stub/                 # Stub
│           └── cce_stub.h
│
├── tests/
│   └── api/
│       └── c_api/                # C API测试
│           ├── npu_arch_2201/    # NPU ARCH 220X架构
│           │   ├── vector_compute/
│           │   │   ├── test_add_instr.cpp
│           │   │   ├── test_axpy_instr.cpp
│           │   │   └── ...
│           │   ├── scalar_compute/
│           │   └── ...
│           ├── npu_arch_3510/    # NPU ARCH 351X架构
│           │   └── ...
│           └── common/           # 通用测试
│               └── test_utils.cpp
│
└── docs/
    └── api/
        └── context/
            └── c_api/            # C API文档
                ├── vector_compute/
                │   ├── asc_add.md
                │   ├── asc_muls.md
                │   └── ...
                ├── scalar_compute/
                ├── vector_datamove/
                ├── general_instruction.md  # 通用说明
                └── c_api_list.md           # API列表
```

### 文件组织原则

1. **按功能分类**：矢量计算、标量计算、数据搬运等
2. **按架构隔离**：不同NPU架构的实现分开组织
3. **接口与实现分离**：include/存放声明，impl/存放实现
4. **测试按架构组织**：便于多架构测试和维护

### 文件命名规范

- **头文件**：`<category>.h`，如 `vector_compute.h`
- **实现文件**：`<category>_c_api_impl.h`，如 `vector_compute_c_api_impl.h`
- **测试文件**：`test_<api>_instr.cpp`，如 `test_add_instr.cpp`
- **文档文件**：`<api_name>.md`，如 `asc_add.md`

---

## 开发示例：实现asc_axpy C API

### 需求分析

实现矢量乘加C API：`dst = src * scalar + dst`

- 支持数据类型：half, float
- 支持三种调用模式
- 直接映射硬件指令

### 接口设计

在 `include/c_api/vector_compute/vector_compute.h` 中添加：

```cpp
// ==========asc_axpy(half/float)==========
// 连续模式
__aicore__ inline void asc_axpy(__ubuf__ half* dst, __ubuf__ half* src, half scalar, uint32_t count);

__aicore__ inline void asc_axpy(__ubuf__ float* dst, __ubuf__ float* src, float scalar, uint32_t count);

// 配置模式
__aicore__ inline void asc_axpy(__ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat,
    uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

__aicore__ inline void asc_axpy(__ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat,
    uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

// 同步模式
__aicore__ inline void asc_axpy_sync(__ubuf__ half* dst, __ubuf__ half* src, half scalar, uint32_t count);

__aicore__ inline void asc_axpy_sync(__ubuf__ float* dst, __ubuf__ float* src, float scalar, uint32_t count);
```

### 实现代码

参考其他接口实现

---

## 测试验证要求

### 测试框架

C API测试使用 **gTest + MockCPP** 框架：

```cpp
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"
```

### 测试类型

#### Mock测试（指令映射验证）

验证C API是否正确调用底层指令。

#### 功能测试（正确性验证）

验证API计算结果正确。

---

## 代码规范

### 命名规范

```cpp
// 函数名：小写字母+下划线，asc_<operation>
void asc_add(...);
void asc_axpy(...);

// 参数名：小驼峰或下划线
__ubuf__ half* dstTensor;
uint32_t elementCount;

// 宏定义：全大写+下划线
#define ASC_CAPI_MAX_COUNT 4096

// 类型名：大驼峰
struct AscUnaryConfig;
```

### 代码风格

```cpp
// 1. 缩进：4空格
// 2. 大括号：K&R风格
// 3. 空格：操作符前后空格，逗号后空格
// 4. 注释：Doxygen风格

/**
 * @brief 矢量乘加运算
 * @param dst 目标地址
 * @param src 源地址
 * @param scalar 标量值
 * @param tmp 临时buffer
 * @param count 元素个数
 */
__aicore__ inline void asc_axpy(__ubuf__ half* dst, __ubuf__ half* src, half scalar, uint32_t count)
{
    // 参数验证
    ASCENDC_ASSERT(count > 0, "count must be positive");
    ASCENDC_ASSERT(dst != nullptr && src != nullptr && tmp != nullptr,
                   "pointers cannot be null");

    // 计算逻辑
    uint8_t repeat = (count + 127) / 128;
    uint64_t mask0 = static_cast<uint64_t>(-1);
    uint64_t mask1 = static_cast<uint64_t>(-1);

    // 设置mask
    set_vector_mask(mask1, mask0);

    // 执行指令
    vmuls(tmp, src, scalar, repeat, 1, 1, 8, 8);
    pipe_barrier(PIPE_V);

    vadd(dst, dst, tmp, repeat, 1, 1, 1, 8, 8, 8);
    pipe_barrier(PIPE_V);
}
```

### 错误处理

```cpp
// 1. 参数验证（Debug模式）
ASCENDC_ASSERT(count > 0, "count must be greater than 0");
ASCENDC_ASSERT(dst != nullptr, "dst cannot be nullptr");
ASCENDC_ASSERT(src != nullptr, "src cannot be nullptr");
ASCENDC_ASSERT(tmp != nullptr, "tmp cannot be nullptr");

// 2. 地址对齐检查
ASCENDC_ASSERT((reinterpret_cast<uintptr_t>(dst) % 32) == 0,
               "dst must be 32-byte aligned");
ASCENDC_ASSERT((reinterpret_cast<uintptr_t>(src) % 32) == 0,
               "src must be 32-byte aligned");

// 3. 范围检查
ASCENDC_ASSERT(count <= ASC_CAPI_MAX_COUNT,
               "count exceeds maximum supported value");

// 4. 架构检查
#if !defined(__NPU_ARCH__) || (__NPU_ARCH__ != 2201 && __NPU_ARCH__ != 3510)
    #error "Unsupported NPU architecture"
#endif
```

### 宏保护

```cpp
// 头文件保护
#ifndef INCLUDE_C_API_VECTOR_COMPUTE_VECTOR_COMPUTE_H
#define INCLUDE_C_API_VECTOR_COMPUTE_VECTOR_COMPUTE_H

// 条件编译
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    // NPU ARCH 2201架构特定代码
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    // NPU ARCH 3510架构特定代码
#endif

#endif
```

---
