# C API UT 生成指南

## 1. API 概述

C API 是 AscendC 提供的 C 风格接口，使用指针和基本数据类型，提供底层硬件操作能力。

### 1.1 目录位置

| 类型 | 路径 |
|------|------|
| **头文件** | `{ASC_DEVKIT_PATH}/include/c_api/` |
| **实现文件** | `{ASC_DEVKIT_PATH}/impl/c_api/` |
| **测试目录** | `{ASC_DEVKIT_PATH}/tests/api/c_api/` |

### 1.2 典型 API 示例

| API 类别 | 典型 API |
|---------|---------|
| 向量计算 | `asc_add`, `asc_mul`, `asc_sub` |
| 标量操作 | `asc_add_scalar`, `asc_mul_scalar` |
| 类型转换 | `asc_float2half`, `asc_half2float` |
| 归约操作 | `asc_datablock_reduce_sum` |
| 矩阵运算 | `asc_mmul` |

---

## 2. API 特点

### 2.1 函数式调用

C API 使用 `asc_` 前缀的函数形式：

```cpp
// 向量加法
__aicore__ inline void asc_add(__ubuf__ half* dst, __ubuf__ half* src0,
    __ubuf__ half* src1, uint32_t count);

// 带stride的向量加法
__aicore__ inline void asc_add(__ubuf__ half* dst, __ubuf__ half* src0,
    __ubuf__ half* src1, uint8_t repeat,
    uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride,
    uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

// 标量加法
__aicore__ inline void asc_add_scalar(__ubuf__ half* dst, __ubuf__ half* src,
    half value, uint32_t count);
```

### 2.2 指针类型标记

C API 使用特定指针标记：

| 标记 | 说明 |
|------|------|
| `__ubuf__` | Unified Buffer 指针 |
| `__gm__` | Global Memory 指针 |
| `__cbuf__` | L1 / CBUF 指针 |
| `__fbuf__` | FBUF 指针 |
| `__ca__` | Cube A buffer 指针 |
| `__cb__` | Cube B buffer 指针 |
| `__cc__` | Cube C buffer 指针 |

本表不是全集。生成 UT 时必须以目标 API 头文件中的真实指针空间标记为准；若出现未列出的标记或 typedef/结构体封装，继续读取 `include/c_api/**`、`impl/c_api/**` 和 `tests/api/c_api/stub/cce_stub.h`，不要按本表猜测或替换。

### 2.3 基本类型参数

C API 入参多为基本类型：

- `uint32_t`, `uint16_t`, `uint8_t` - 大小和计数
- `half`, `float` - 数据值
- 指针 - 数据地址

### 2.4 Mock 测试框架

C API 测试通常使用 `mockcpp` 框架进行底层指令 Mock：

```cpp
#include <mockcpp/mockcpp.hpp>
#include "c_api/stub/cce_stub.h"

// Mock 底层指令
MOCKER(cce_instruction, void(...))
    .times(1)
    .will(invoke(cce_instruction_Stub));
```

---

## 3. UT 生成注意事项

### 3.1 核心类型设置

C API 测试按已有同类 UT 设置 `g_coreType`，根据目标接口选择 AIC 或 AIV 类型，并在 `TearDown` 中恢复默认值。

### 3.2 指针类型使用

C API 必须使用目标函数声明中的指针空间标记，例如 `__ubuf__`、`__gm__`、`__cbuf__`、`__fbuf__`、`__ca__`、`__cb__`、`__cc__`。不要为了复用模板而替换指针空间；如果目标函数声明使用其他地址空间标记，必须先读取代码中的真实声明、impl 调用和底层 stub 签名。

### 3.3 Mock 函数验证

Mock 函数签名必须与底层指令完全一致，并在 stub 中断言所有由目标 API 透传或计算出的参数。地址、count、stride、repeat 和标量参数只是常见示例；其他 attribute、attr、params、config、mode、flag、shape、layout、padding、dilation、filter、transpose 等属性也必须按目标函数声明、impl 调用和底层 stub 签名逐项补齐。通用 gtest 骨架见 [测试模板参考](../foundations/test-templates.md)，mockcpp 写法以本 guide 和已有同类 C API UT 为准。

### 3.4 Attribute 与结构体参数

不要在本 guide 固化 C API 的 attribute 字段全集。生成或补齐 UT 时，遇到以下任一情况，必须读取代码中的真实结构体定义和调用链：

- 参数名或类型包含 `attribute`、`attr`、`attrs`、`params`、`param`、`config`、`mode`、`flag` 等语义。
- API 入参、impl 局部变量或底层指令参数使用 `struct`、`union`、`enum`、`typedef` 或 `class` 承载属性。
- impl 将多个散列入参打包成控制字、bit field、attribute 结构体或硬件指令参数。

读取顺序：

1. 目标 API 头文件：`include/c_api/**` 中的函数签名、重载和结构体声明。
2. 目标架构 impl：`impl/c_api/**`、`impl/c_api/{memory_base_impl,reg_base_impl}/**` 中的真实字段赋值、默认值、条件编译和指令调用。
3. 底层 stub：`tests/api/c_api/stub/cce_stub.h` 中的 mock/stub 函数签名。
4. 已有同架构同类 UT：确认字段初始化、mockcpp 写法和断言习惯。

结构体或 attribute 参数必须按真实字段逐项初始化、逐项断言；不能只验证本 guide 中列出的常见字段。结构体优先使用成员赋值，避免依赖聚合初始化顺序；字段无法从代码中确认时，必须记录缺口并停止补齐该字段，不要猜测。

### 3.5 架构映射

测试目录按 NPU_ARCH 组织：

```
tests/api/c_api/
├── npu_arch_2201/    # ascend910b1
├── npu_arch_3510/    # ascend950pr_9599
└── ...
```

### 3.6 自动生成边界

C API 的 mock 形状必须先按目标架构和真实签名确认。即使 API 名称相近，
2201 与 3510 的底层签名、地址空间和 mock 对象也可能不同；不能把单一
`dst, src0, src1, mask` 的 vector 形状套给归约、scalar、搬运、cube 或
跨架构接口。

在某个 signature family 的可执行模板经过验证前，通用生成器不得默认输出
C API UT。先读 declaration、impl、stub 和同 family UT；若还没有可复用模板，
就写 API-specific UT。

---

## 4. 分支覆盖要点

### 4.1 函数重载

C API 常有多个重载版本：

```cpp
// 重载1: 基本版本
void asc_add(__ubuf__ half* dst, __ubuf__ half* src0,
    __ubuf__ half* src1, uint32_t count);

// 重载2: 带 stride 版本
void asc_add(__ubuf__ half* dst, __ubuf__ half* src0,
    __ubuf__ half* src1, uint8_t repeat, ...);
```

**测试策略**：每个重载版本独立测试

### 4.2 数据类型组合

不要在本 guide 固化通用 dtype 组合。生成 C API UT 时，先从目标 API 声明、impl 分支、设计文档或已有同类 UT 确认真实支持的指针类型和标量类型；通用基础 dtype 的名称、含义、大小和对齐要求统一回链到 [`asc-npu-arch` 架构指南](../../../asc-npu-arch/references/npu-arch-guide.md#统一数据类型视图)。

对存在多类型重载或模板分支的 C API，使用参数化测试覆盖已确认的类型组合，不要按通用 dtype 列表推断支持范围。

### 4.3 架构条件编译

```cpp
// impl 文件中的架构分支
#if __NPU_ARCH__ == 2201
    // ascend910b1 实现
#elif __NPU_ARCH__ == 3510
    // ascend950pr_9599 实现
#endif
```

### 4.4 参数组合

使用参数化测试覆盖：
- 不同的 count 值
- 不同的 stride 组合
- 边界条件

---

## 5. 测试模板引用

C API 使用 [测试模板参考](../foundations/test-templates.md) 中的通用 gtest 和参数化测试骨架。本 guide 维护 C API 特有要求：

- `g_coreType` 使用 C API 测试已有模式设置为目标 AIC/AIV 类型。
- Mock 函数签名必须与底层指令声明完全一致。
- 指针地址、stride、repeat、count、attribute/结构体字段等参数按目标函数签名、impl 调用和底层 stub 签名逐项断言。

---

## 6. 编译与执行

C API 的编译、执行、失败修复和验证报告统一按 [自动化验证流程](../workflows/automation-guide.md) 处理。执行前根据目标芯片确认 `build/tests/api/c_api/` 下的实际架构目录和测试可执行文件名。

---

## 7. C API 函数签名详解

### 7.1 向量计算类 API

```cpp
// 向量加法
__aicore__ inline void asc_add(__ubuf__ half* dst, __ubuf__ half* src0,
    __ubuf__ half* src1, uint32_t count);

// 带 stride 的向量加法
__aicore__ inline void asc_add(__ubuf__ half* dst, __ubuf__ half* src0,
    __ubuf__ half* src1, uint8_t repeat,
    uint8_t dst_block_stride, uint8_t src0_block_stride, uint8_t src1_block_stride,
    uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

// 标量加法
__aicore__ inline void asc_add_scalar(__ubuf__ half* dst, __ubuf__ half* src,
    half value, uint32_t count);

// 向量乘法
__aicore__ inline void asc_mul(__ubuf__ half* dst, __ubuf__ half* src0,
    __ubuf__ half* src1, uint32_t count);
```

### 7.2 类型转换类 API

```cpp
// float 转 half
__aicore__ inline void asc_float2half(__ubuf__ half* dst, __ubuf__ float* src,
    uint32_t count);

// half 转 float
__aicore__ inline void asc_half2float(__ubuf__ float* dst, __ubuf__ half* src,
    uint32_t count);
```

### 7.3 归约操作类 API

```cpp
// 数据块归约求和
__aicore__ inline void asc_datablock_reduce_sum(__ubuf__ half* dst, __ubuf__ half* src,
    uint32_t count);

// 数据块归约求最大值
__aicore__ inline void asc_datablock_reduce_max(__ubuf__ half* dst, __ubuf__ half* src,
    uint32_t count);
```

### 7.4 参数说明

| 参数名 | 类型 | 说明 |
|-------|------|------|
| `dst` | `__ubuf__ T*` | 目标地址（Unified Buffer） |
| `src0`, `src1` | `__ubuf__ T*` | 源地址（Unified Buffer） |
| `count` | `uint32_t` | 元素数量 |
| `repeat` | `uint8_t` | 重复次数 |
| `*_block_stride` | `uint8_t` | 块间步长 |
| `*_repeat_stride` | `uint8_t` | 重复间步长 |
| `value` | 按 API 声明确认 | 标量值 |

本表不是 C API 参数全集。遇到 attribute、attr、params、config、mode 或结构体参数时，必须回到代码中的结构体定义、impl 字段赋值和底层 stub 签名，补齐所有字段的初始化与断言。

---

## 8. 指针类型标记详解

| 标记 | 说明 | 使用场景 |
|------|------|---------|
| `__ubuf__` | Unified Buffer 指针 | 向量计算、数据搬运 |
| `__gm__` | Global Memory 指针 | 全局内存访问 |
| `__cbuf__` | L1 / CBUF 指针 | L1 与 L0/UB/GM 之间的数据搬运、Cube 数据搬运 |
| `__fbuf__` | FBUF 指针 | `asc_copy_l12fb` 等 FBUF 相关数据搬运 |
| `__ca__` | Cube A buffer 指针 | 矩阵乘法左矩阵 |
| `__cb__` | Cube B buffer 指针 | 矩阵乘法右矩阵 |
| `__cc__` | Cube C buffer 指针 | 矩阵乘法结果 |

本表不是固定全集；它来自当前 C API 头文件和已有 UT 的常见地址空间标记。生成或补齐 UT 时必须先读取目标 API 声明；若参数类型通过 typedef、struct、union 或模板别名间接携带地址空间标记，也要继续读取代码中的真实定义，确保 mock/stub 签名、测试入参和断言中的指针空间完全一致。`__aicore__`、`__simd_callee__` 等函数限定符不是指针类型标记，不应放入指针空间表。

**使用示例：**

```cpp
// 向量计算使用 __ubuf__
__ubuf__ half* dst;
__ubuf__ half* src0;
__ubuf__ half* src1;
asc_add(dst, src0, src1, count);

// L1/CBUF 与 FBUF 数据搬运使用 __cbuf__, __fbuf__
__cbuf__ void* l1;
__fbuf__ void* fbuf;
asc_copy_l12fb(fbuf, l1, size);

// 矩阵计算使用 __ca__, __cb__, __cc__
__ca__ half* a;
__cb__ half* b;
__cc__ float* c;
asc_mmad(c, a, b, m, n, k);
```

---

## 9. 常见问题

### Q1: 指针类型错误？

**问题**：编译警告或运行时错误

**解决**：
- 使用目标 API 声明中的正确指针标记，例如 `__ubuf__`、`__gm__`、`__cbuf__`、`__fbuf__`、`__ca__`、`__cb__`、`__cc__`
- 若声明中出现未列出的标记或封装类型，读取头文件、impl 和 `cce_stub.h` 的真实签名
- 参考已有测试中的指针使用方式

### Q2: 公共排障索引

C API 的 mock/stub 签名和架构目录问题统一查看 [常见问题与解决方案](../troubleshooting/faq.md)：

- [Mock/Stub 函数参数不匹配](../troubleshooting/faq.md#2-mockstub-函数参数不匹配)
- [架构或测试目录不匹配](../troubleshooting/faq.md#5-架构或测试目录不匹配)

---

## 10. 检查清单

### 10.1 分析阶段

- [ ] 已读取 C API 头文件，了解函数签名
- [ ] 已分析 impl 文件，了解底层指令调用
- [ ] 已读取 attribute/params/config/mode 等结构体定义，并补齐所有真实字段
- [ ] 已确认数据类型支持
- [ ] 已参考同架构下已有测试

### 10.2 编写阶段

- [ ] 使用目标 API 声明中的正确指针类型标记（如 `__ubuf__`、`__gm__`、`__cbuf__`、`__fbuf__`、`__ca__`、`__cb__`、`__cc__`）
- [ ] Mock 函数签名正确
- [ ] Stub 已逐项断言所有 attribute/结构体字段，不只断言常见参数
- [ ] SetUp/TearDown 正确设置核心类型
- [ ] 使用参数化测试覆盖分支

### 10.3 验证阶段

- [ ] 编译通过
- [ ] Mock 验证通过
- [ ] 测试执行通过

---

## 11. 相关文件索引

| 文件路径 | 说明 |
|---------|------|
| `include/c_api/vector_compute/vector_compute.h` | C API 向量计算 |
| `include/c_api/asc_simd.h` | SIMD 相关 C API |
| `include/c_api/stub/cce_stub.h` | 底层指令 Stub |

---

## 12. 相关参考

| 文档 | 说明 |
|------|------|
| [测试模板参考](../foundations/test-templates.md) | 通用测试骨架与模板选择索引 |
| [分支覆盖分析指南](../foundations/branch-coverage-guide.md) | 分支分析方法 |
