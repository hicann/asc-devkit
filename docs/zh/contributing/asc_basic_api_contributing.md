# 基础API贡献指南

## 概述

基础API是Ascend C编程框架中的指令级API层，直接封装昇腾AI处理器的硬件指令，采用C++语言风格的函数接口。基础API是高阶API的构建基础，开发者可以通过组合基础API实现复杂的算法逻辑。

**基础API的核心特点：**

- **指令级封装**：每个API对应一条或多条硬件指令
- **LocalTensor抽象**：使用`LocalTensor<T>`类型操作内存
- **模板化设计**：支持多种数据类型（half、float、int16_t、int32_t等）
- **双层接口**：高维切分计算（精细控制）和 前n个数据计算（简化调用）
- **架构适配**：通过架构宏定义支持不同NPU架构

---

## 开发流程

### 需求分析

- 明确API功能（如Add、Mul、Relu等）
- 确定支持的数据类型
- 分析硬件指令支持情况

### API设计

- 定义函数原型（使用LocalTensor）
- 设计高维切分计算和前n个数据计算接口
- 确定参数规范（mask、repeat、stride等）

### 实现开发

- 编写接口声明（include/basic_api/）
- 实现核心逻辑（impl/basic_api/）
- 处理架构差异

### 测试验证

- 编写单元测试
- 验证功能正确性
- 检查边界条件

### 文档编写

- 完成API文档
- 提供使用示例
- 说明约束条件

---

## API介绍

### 高维切分计算 vs 前n个数据计算接口

#### 高维切分计算（精细控制）

```cpp
// 需要手动设置mask和repeat参数
template <typename T, bool isSetMask = true>
__aicore__ inline void Add(const LocalTensor<T>& dst,
                           const LocalTensor<T>& src0,
                           const LocalTensor<T>& src1,
                           uint64_t mask[],           // mask数组
                           const uint8_t repeatTime,  // repeat次数
                           const BinaryRepeatParams& repeatParams);  // stride参数
```

**适用场景**：

- 需要精细控制计算过程
- 非连续内存访问
- 性能优化

#### 前n个数据计算（简化调用）

```cpp
// 自动处理mask和repeat
template <typename T>
__aicore__ inline void Add(const LocalTensor<T>& dst,
                           const LocalTensor<T>& src0,
                           const LocalTensor<T>& src1,
                           const int32_t& count);  // 仅需元素个数
```

**适用场景**：

- 连续内存块计算
- 简化代码
- 快速开发

---

## 目录规划

### 目录结构

```text
asc-devkit/
├── include/
│   └── basic_api/                    # 基础API头文件
│       ├── kernel_operator_common_intf.h        # 通用接口
│       ├── kernel_operator_vec_binary_intf.h    # 矢量二元运算
│       ├── kernel_operator_vec_unary_intf.h     # 矢量一元运算
│       ├── kernel_operator_data_copy_intf.h     # 数据搬运
│       ├── kernel_operator_fixpipe_intf.h       # Fixpipe
│       ├── kernel_operator_mm_intf.h            # 矩阵乘法
│       ├── kernel_operator_scalar_intf.h        # 标量运算
│       ├── kernel_operator_sys_var_intf.h       # 系统变量
│       ├── kernel_operator_atomic_intf.h        # 原子操作
│       ├── kernel_tensor.h                      # Tensor定义
│       └── kernel_struct_*.h                    # 参数结构体
│
├── impl/
│   └── basic_api/                    # 基础API实现
│       ├── dav_m200/                 # NPU ARCH 200x 架构
│       │   ├── kernel_operator_vec_binary_impl.h
│       │   └── ...
│       ├── dav_c220/                 # NPU ARCH 220x 架构
│       │   ├── kernel_operator_vec_binary_impl.h
│       │   └── ...
│       └── CMakeLists.txt
│
├── tests/
│   └── api/
│       └── basic_api/                # 基础API测试
│           ├── tikcpp_case_common/
│           │   └── test_operator_axpy.cpp
│           ├── tikcpp_case_ascend910/
│           │   └── ...
│           └── tikcpp_case_ascend910b1/
│               └── ...
│
└── docs/
    └── api/
        └── context/
            └── ...              # 基础API文档
```

### 文件命名规范

| 文件类型 | 命名规范 | 示例 |
| --------- | --------- | ------ |
| 接口头文件 | `kernel_operator_<category>_intf.h` | `kernel_operator_vec_binary_intf.h` |
| 实现文件 | `kernel_operator_<category>_impl.h` | `kernel_operator_vec_binary_impl.h` |
| 测试文件 | `test_operator_<category>.cpp` | `test_operator_vec_binary.cpp` |
| 文档文件 | `<api>.md` | `Add.md` |

### API分类

| 分类 | 说明 | 示例API |
| ------ | ------ | --------- |
| vec_binary | 矢量二元运算 | Add, Sub, Mul, Div, Max, Min |
| vec_unary | 矢量一元运算 | Relu, Exp, Cast, Abs |
| vec_reduce | 矢量归约 | Sum, Max, Mean |
| data_copy | 数据搬运 | DataCopy, LoadData |
| fixpipe | 流水线控制 | Fixpipe |
| mm | 矩阵乘法 | Mmad, Conv2D |
| scalar | 标量运算 | ToFloat |
| atomic | 原子操作 | AtomicAdd, AtomicCAS |

---

## 架构设计

### 实现层次

#### 层次1：接口声明层（include/basic_api/）

```cpp
// include/basic_api/kernel_operator_vec_binary_intf.h
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BINARY_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_VEC_BINARY_INTERFACE_H

#include "kernel_tensor.h"
#include "kernel_struct_binary.h"

namespace AscendC {

// Add - 高维切分计算
template <typename T, bool isSetMask = true>
__aicore__ inline void Add(const LocalTensor<T>& dst,
                           const LocalTensor<T>& src0,
                           const LocalTensor<T>& src1,
                           uint64_t mask[],
                           const uint8_t repeatTime,
                           const BinaryRepeatParams& repeatParams);

// Add - 前n个数据计算
template <typename T>
__aicore__ inline void Add(const LocalTensor<T>& dst,
                           const LocalTensor<T>& src0,
                           const LocalTensor<T>& src1,
                           const int32_t& count);

}  // namespace AscendC

#include "impl/basic_api/kernel_operator_vec_binary_intf_impl.h"
#endif
```

#### 层次2：指令实现层（impl/basic_api/）

```cpp
// impl/basic_api/dav_c220/kernel_operator_vec_binary_impl.h
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BINARY_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BINARY_IMPL_H

namespace AscendC {

// Add实现 - 前n个数据计算
template <typename T>
__aicore__ inline void AddImpl(__ubuf__ T* dst, __ubuf__ T* src0,
                               __ubuf__ T* src1, const int32_t& count) {
    if ASCEND_IS_AIV {
        // 1. 设置mask
        set_mask_count();
        set_vector_mask(0, count);

        // 2. 调用底层指令
        vadd(dst, src0, src1, 1,
             DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE,
             DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE);

        // 3. 恢复mask
        set_mask_norm();
        set_vector_mask(static_cast<uint64_t>(-1), static_cast<uint64_t>(-1));
    }
}

// Add实现 - 高维切分计算
template <typename T, bool isSetMask = true>
__aicore__ inline void AddImpl(__ubuf__ T* dst, __ubuf__ T* src0,
                               __ubuf__ T* src1, const uint64_t mask[],
                               const uint8_t repeatTime,
                               const BinaryRepeatParams& repeatParams) {
    if ASCEND_IS_AIV {
        // 设置mask（如果需要）
        if (isSetMask) {
            AscendCUtils::SetMask<T, isSetMask>(mask[1], mask[0]);
        }

        // 调用底层指令
        vadd(dst, src0, src1, repeatTime,
             repeatParams.dstBlkStride, repeatParams.src0BlkStride, repeatParams.src1BlkStride,
             repeatParams.dstRepStride, repeatParams.src0RepStride, repeatParams.src1RepStride);
    }
}

}  // namespace AscendC

#endif
```

#### 层次3：接口封装层

```cpp
// impl/basic_api/kernel_operator_vec_binary_intf_impl.h
namespace AscendC {

// 前n个数据计算接口封装
template <typename T>
__aicore__ inline void Add(const LocalTensor<T>& dst,
                           const LocalTensor<T>& src0,
                           const LocalTensor<T>& src1,
                           const int32_t& count) {
    AddImpl<T>(dst.GetPtr(), src0.GetPtr(), src1.GetPtr(), count);
}

// 高维切分计算接口封装
template <typename T, bool isSetMask = true>
__aicore__ inline void Add(const LocalTensor<T>& dst,
                           const LocalTensor<T>& src0,
                           const LocalTensor<T>& src1,
                           uint64_t mask[],
                           const uint8_t repeatTime,
                           const BinaryRepeatParams& repeatParams) {
    AddImpl<T, isSetMask>(dst.GetPtr(), src0.GetPtr(), src1.GetPtr(),
                          mask, repeatTime, repeatParams);
}

}  // namespace AscendC
```

### 架构适配

不同NPU架构的硬件可能有差异，需要重新实现

---

## 开发示例：实现Axpy 基础API

### API需求分析

实现矢量乘加：`dst = src * scalar + dst`

- 支持数据类型：half, float
- 接口类型：前n个数据计算（简化调用）
- 硬件支持：确认支持硬件

### 查看现有API结构

基础API使用`LocalTensor<T>`作为参数，前n个数据计算接口仅需count参数：

```cpp
// 参考现有Add接口
template <typename T>
__aicore__ inline void Add(const LocalTensor<T>& dst,
                           const LocalTensor<T>& src0,
                           const LocalTensor<T>& src1,
                           const int32_t& count);
```

### 接口设计

在 `include/basic_api/kernel_operator_vec_binary_intf.h` 中添加：

```cpp
/* **************************************************************************************************
 * Axpy                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Axpy
 * @brief dst = dst + src * scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar scalar value
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__aicore__ inline void Axpy(const LocalTensor<T>& dst,
                           const LocalTensor<U>& src,
                           const U scalar,
                           const int32_t& count);
```

### 实现代码

参考其他接口实现

### 接口封装

在 `impl/basic_api/kernel_operator_vec_binary_intf_impl.h` 中添加：

```cpp
template <typename T, typename U>
__aicore__ inline void Axpy(const LocalTensor<T>& dst,
                           const LocalTensor<U>& src,
                           const U scalar,
                           const int32_t& count) {
    AxpyImpl<T, U>(dst.GetPtr(), src.GetPtr(), scalar, count);
}
```

### 测试代码

增加对应接口的测试代码

---

## 测试验证要求

### 功能测试

验证API计算结果正确性

### 边界测试

```cpp
TEST_F(TestAxpy, BoundaryTest) {
    // 测试count=0, 1, 256, 257等边界值
    // 测试不同数据类型组合
    // 测试特殊值（NaN, Inf）
}
```

### 数据类型测试

```cpp
INSTANTIATE_TEST_CASE_P(TEST_AXPY_TYPES, AxpyTestsuite,
    ::testing::Values(
        BinaryTestParams { 256, 2, 2, main_axpy<half, half> },
        BinaryTestParams { 256, 4, 2, main_axpy<float, half> },
        BinaryTestParams { 256, 4, 4, main_axpy<float, float> }
    )
);
```

---

## 代码规范

### 命名规范

```cpp
// 函数名：大驼峰，首字母大写
void Add(...);
void Relu(...);
void Axpy(...);

// 参数名：小驼峰
LocalTensor<T> dstTensor;
int32_t elementCount;

// 宏定义：全大写+下划线
#define ASCENDC_ASSERT(cond, msg) ...

// 类型名：大驼峰
struct BinaryRepeatParams;
class LocalTensor;
```

### 代码风格

```cpp
// 1. 缩进：4空格
// 2. 大括号：K&R风格
// 3. 空格：操作符前后空格
// 4. 注释：Doxygen风格

/**
 * @brief 矢量加法运算
 * @param dst 目标LocalTensor
 * @param src0 源LocalTensor 0
 * @param src1 源LocalTensor 1
 * @param count 元素个数
 */
template <typename T>
__aicore__ inline void Add(const LocalTensor<T>& dst,
                           const LocalTensor<T>& src0,
                           const LocalTensor<T>& src1,
                           const int32_t& count) {
    // 参数验证
    ASCENDC_ASSERT(count > 0, "count must be positive");

    // 调用实现
    AddImpl<T>(dst.GetPtr(), src0.GetPtr(), src1.GetPtr(), count);
}
```

### 错误处理

```cpp
// 1. 参数验证（Debug模式）
ASCENDC_ASSERT(count > 0, "count must be greater than 0");
ASCENDC_ASSERT(dst != nullptr, "dst cannot be nullptr");
ASCENDC_ASSERT(src != nullptr, "src cannot be nullptr");

// 2. 类型检查
static_assert(SupportType<T, half, float, int16_t, int32_t>(),
              "Unsupported data type");

// 3. 架构检查
#if !defined(__NPU_ARCH__) || (__NPU_ARCH__ != 2201 && __NPU_ARCH__ != 3510)
    #error "Unsupported NPU architecture"
#endif
```

---
