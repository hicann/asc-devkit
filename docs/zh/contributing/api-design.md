# API接口设计规范

## 三种API类型对比

| 维度 | 基础API (basic_api) | 高阶API (adv_api) | C API (c_api) |
|------|---------------------|-------------------|---------------|
| 函数命名 | 大驼峰：`Add`、`Mul` | 大驼峰：`Exp`、`Axpy` | `asc_` 前缀+ snake_case：`asc_add` |
| 抽象层级 | 直接映射硬件指令 | 封装基础API实现算法 | 直接映射硬件指令 |
| 参数风格 | `LocalTensor<T>&` | `LocalTensor<T>&` + `sharedTmpBuffer` | `__ubuf__ T*` 指针 |
| 文档风格 | `@ingroup` / `@brief` | `\brief` / `\param` / `\note` | 简洁注释 |
| 公开入口 | `kernel_operator.h` | `kernel_api.h` | `asc_simd.h` |
| 实现位置 | `impl/basic_api/` | `impl/adv_api/detail/<category>/<api>/` | `impl/c_api/` |
| 命名空间 | `AscendC` | `AscendC` | 无（C风格） |

---

## 接口设计原则

### 输出参数优先

输出参数位于输入参数之前：

```cpp
// 正确：dst（输出）在前
void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, ...);
```

> **例外：** Tiling接口中输出参数为引用类型（`uint32_t&`），按C++惯例放在参数列表末尾：
>
> ```cpp
> void GetAxpyMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize,
>     const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue);
> ```

### 数据流模式支持

支持三种数据流模式：

1. **连续模式**：固定count参数
2. **高维切分模式**：repeat + block_stride + repeat_stride参数
3. **同步模式**：视API类型和实际需求，C API使用 `_sync` 后缀，部分C++ API使用 `Sync` 后缀

### 参数命名一致性

相同语义的参数在不同API中保持一致的命名：

| 语义 | 参数名 |
|------|--------|
| 目的操作数 | `dstTensor` / `dst` |
| 源操作数 | `srcTensor` / `src0`、`src1` |
| 元素个数 | `calCount` / `count` |
| 临时缓存 | `sharedTmpBuffer` / `tmpBuffer` |
| 迭代次数 | `repeat` / `repeatTime` |
| 地址步长 | `block_stride` / `dstBlkStride` |
| Mask | `mask` / `mask0`、`mask1` |

---

## 模板参数约定

### 类型参数

- 高阶API：`T` 为dst类型，`U` 为src类型
- 基础API：`T` 为数据类型

```cpp
// 高阶API — T=dst, U=src
template <typename T, typename U, bool isReuseSource = false>
__aicore__ inline void Axpy(const LocalTensor<T>& dstTensor, const LocalTensor<U>& srcTensor, ...);

// 基础API — T统一
template <typename T>
__aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, ...);
```

### 模板参数声明

`AlwaysBreakTemplateDeclarations: true`（clang-format配置）要求模板声明与后续函数/类声明分行：

```cpp
template <typename T, typename U, bool isReuseSource = false>
__aicore__ inline void Axpy(...);
```

### 预留参数

未来可能使用的参数设置默认值，命名以 `is` 开头表示布尔配置：

```cpp
bool isReuseSource = false
```

### 算法参数

使用明确的类型和有意义的名字：

```cpp
uint8_t taylorExpandLevel     // Taylor展开级数
ReduceDim reduceDim           // 归约维度
EngineType engine             // 计算引擎
```

---

## 配套属性

| 属性 | 语义 | 示例 |
|------|------|------|
| `__gm__` | 全局内存地址空间 | `__gm__ uint8_t* ptr` |
| `__inout_pipe__(V)` | 流水线输入输出标注 | `__aicore__ inline __inout_pipe__(MTE2) void DataCopy(...)` |
| `__in_pipe__(V)` | 流水线输入标注 | `__aicore__ inline __in_pipe__(V) void Copy(...)` |
| `__out_pipe__(V)` | 流水线输出标注 | `__aicore__ inline __out_pipe__(MTE3) void Copy(...)` |
| `__BLOCK_LOCAL__ __inline__` | Block级线程局部变量 | `__BLOCK_LOCAL__ __inline__ TPipe* g_tPipePtr` |
| `__ubuf__` | Unified Buffer地址空间 | `__ubuf__ half* dst` |

---

## Tiling接口设计

### 接口命名

Kernel侧 `Axpy` ↔ Tiling侧 `GetAxpyMaxMinTmpSize`：

```text
Get<ApiName>MaxMinTmpSize
Get<ApiName>TmpBufferFactorSize
```

### 参数约定

以 `Axpy` 为例，不同API的Tiling接口签名可能不同，请按同类API的实际设计。

**输入参数：**

- `const AscendC::TensorShape& srcShape` — 输入shape信息
- `const uint32_t typeSize` — 数据类型大小（字节）
- `const bool isReuseSource` — 是否允许修改源操作数（当前传false）

**输出参数（引用）：**

- `uint32_t& maxValue` — 所需最大临时空间
- `uint32_t& minValue` — 所需最小临时空间

**返回值：** `void`

### 实现位置

- 声明：`include/adv_api/<category>/<api>_tiling.h`
- 实现：`impl/adv_api/tiling/<category>/<api>_tiling_impl.cpp`（或 `_tiling.cpp`）
- 注册：在 `impl/adv_api/tiling/CMakeLists.txt` 的 `tiling_api` target中添加 `.cpp`

### 常量定义

Tiling常量在实现文件中以 `constexpr` 定义：

```cpp
constexpr uint32_t AXPY_HALF_CALC_PROC = 4;
constexpr uint32_t AXPY_FLOAT_CALC_PROC = 1;
constexpr uint32_t AXPY_ONE_REPEAT_BYTE_SIZE = 256;
```

### 断言使用

Tiling侧使用 `ASCENDC_HOST_ASSERT` 而非 `ASCENDC_ASSERT`：

```cpp
ASCENDC_HOST_ASSERT(inputSize > 0, return, "Input Shape size must be greater than 0.");
```

---

## 公共头文件注册

### 高阶API注册

新增高阶API后，需要在以下文件中添加include：

**`include/adv_api/kernel_api.h`：**

```cpp
#include "math/axpy.h"
```

架构守卫由各子头文件内部管理，`kernel_api.h` 中直接 `#include` 即可。

**`include/adv_api/tiling_api.h`：**

```cpp
#include "math/axpy_tiling.h"
```

### 基础API注册

新增基础API在对应的总入口中注册，例如 `kernel_operator.h`。

### 架构守卫

- 支持多架构的API：在 `#if defined(__NPU_ARCH__)` 中列出所有支持的架构
- 仅支持特定架构的API：在include时限制架构范围

---

## Interface与Implementation分离

### 公开头文件(`include/`)

- 包含接口文档和inline转发定义
- 在转发定义之前 `#include` 对应架构的impl头
- `__aicore__` 标注

### 实现头文件(`impl/`)

- 包含 `#pragma message` 警告
- 高阶API的架构分发由公开头文件通过 `#if __NPU_ARCH__` 选择对应impl文件（如 `axpy_3510_impl.h` 或 `axpy_common_impl.h`）；基础API的分发由 `*_intf_impl.h` 头文件完成
- 包含具体算法逻辑

---

## 接口实现模式

### 高阶API实现模板

```cpp
// include/adv_api/<category>/<api>.h
template <typename T, bool isReuseSource = false>
__aicore__ inline void ApiName(const LocalTensor<T>& dstTensor,
    const LocalTensor<T>& srcTensor, ...)
{
    ApiNameImpl<T, isReuseSource>(dstTensor, srcTensor, ...);
}

// impl/adv_api/detail/<category>/<api>/<api>_common_impl.h
template <typename T, bool isReuseSource>
__aicore__ inline void ApiNameImpl(const LocalTensor<T>& dstTensor,
    const LocalTensor<T>& srcTensor, ...)
{
    CHECK_FUNC_HIGHLEVEL_API(ApiName, (T, isReuseSource),
        (dstTensor, srcTensor, ...));
    // 实现逻辑...
}
```

### 算法函数与Intrinsics分离

复杂的API将对外接口、算法拆解、底层Intrinsics分离：

- `ApiName(...)` — 对外接口（public header）
- `ApiNameImpl(...)` — 算法参数拆解（impl header）
- `ApiNameSub(...)` — 分块循环逻辑（impl header）
- `ApiNameIntrinsicsImpl(...)` — 底层指令调用（impl header）
