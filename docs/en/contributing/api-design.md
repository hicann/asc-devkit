# API Interface Design Standards

## Comparison of Three API Types

| Dimension | Basic API (basic_api) | High-level API (adv_api) | C API (c_api) |
|------|---------------------|-------------------|---------------|
| Function naming | UpperCamelCase: `Add`, `Mul` | UpperCamelCase: `Exp`, `Axpy` | `asc_` prefix + snake_case: `asc_add` |
| Abstraction level | Direct mapping to hardware instructions | Encapsulates basic APIs to implement algorithms | Direct mapping to hardware instructions |
| Parameter style | `LocalTensor<T>&` | `LocalTensor<T>&` + `sharedTmpBuffer` | `__ubuf__ T*` pointer |
| Documentation style | `@ingroup` / `@brief` | `\brief` / `\param` / `\note` | Concise comments |
| Public entry | `kernel_operator.h` | `kernel_api.h` | `asc_simd.h` |
| Implementation location | `impl/basic_api/` | `impl/adv_api/detail/<category>/<api>/` | `impl/c_api/` |
| Namespace | `AscendC` | `AscendC` | None (C style) |

---

## Interface Design Principles

### Output Parameters First

Output parameters precede input parameters:

```cpp
// Correct: dst (output) comes first
void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, ...);
```

> **Exception:** In Tiling interfaces, output parameters are reference types (`uint32_t&`) and are placed at the end of the parameter list by C++ convention:
>
> ```cpp
> void GetAxpyMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize,
>     const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue);
> ```

### Data Flow Mode Support

Three data flow modes are supported:

1. **Continuous mode**: fixed count parameter
2. **High-dimensional tiling mode**: repeat + block_stride + repeat_stride parameters
3. **Synchronous mode**: Depending on API type and actual need, C API uses `_sync` suffix, some C++ APIs use `Sync` suffix

### Parameter Naming Consistency

Parameters with the same semantics keep consistent naming across different APIs:

| Semantic | Parameter name |
|------|--------|
| Destination operand | `dstTensor` / `dst` |
| Source operand | `srcTensor` / `src0`, `src1` |
| Element count | `calCount` / `count` |
| Temporary buffer | `sharedTmpBuffer` / `tmpBuffer` |
| Iteration count | `repeat` / `repeatTime` |
| Address stride | `block_stride` / `dstBlkStride` |
| Mask | `mask` / `mask0`, `mask1` |

---

## Template Parameter Conventions

### Type Parameters

- High-level API: `T` is the dst type, `U` is the src type
- Basic API: `T` is the data type

```cpp
// High-level API — T=dst, U=src
template <typename T, typename U, bool isReuseSource = false>
__aicore__ inline void Axpy(const LocalTensor<T>& dstTensor, const LocalTensor<U>& srcTensor, ...);

// Basic API — T is unified
template <typename T>
__aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0, ...);
```

### Template Parameter Declaration

`AlwaysBreakTemplateDeclarations: true` (clang-format configuration) requires a line break between the template declaration and the subsequent function/class declaration:

```cpp
template <typename T, typename U, bool isReuseSource = false>
__aicore__ inline void Axpy(...);
```

### Reserved Parameters

Parameters that may be used in the future are given default values, with names starting with `is` to indicate boolean configuration:

```cpp
bool isReuseSource = false
```

### Algorithm Parameters

Use explicit types and meaningful names:

```cpp
uint8_t taylorExpandLevel     // Taylor expansion order
ReduceDim reduceDim           // Reduction dimension
EngineType engine             // Compute engine
```

---

## Companion Attributes

| Attribute | Semantic | Example |
|------|------|------|
| `__gm__` | global memory address space | `__gm__ uint8_t* ptr` |
| `__inout_pipe__(V)` | pipeline input/output annotation | `__aicore__ inline __inout_pipe__(MTE2) void DataCopy(...)` |
| `__in_pipe__(V)` | pipeline input annotation | `__aicore__ inline __in_pipe__(V) void Copy(...)` |
| `__out_pipe__(V)` | pipeline output annotation | `__aicore__ inline __out_pipe__(MTE3) void Copy(...)` |
| `__BLOCK_LOCAL__ __inline__` | block-level thread-local variable | `__BLOCK_LOCAL__ __inline__ TPipe* g_tPipePtr` |
| `__ubuf__` | Unified Buffer address space | `__ubuf__ half* dst` |

---

## Tiling Interface Design

### Interface Naming

Kernel-side `Axpy` ↔ Tiling-side `GetAxpyMaxMinTmpSize`:

```text
Get<ApiName>MaxMinTmpSize
Get<ApiName>TmpBufferFactorSize
```

### Parameter Conventions

Taking `Axpy` as an example; different APIs may have different Tiling interface signatures—follow the actual design of similar APIs.

**Input Parameters:**

- `const AscendC::TensorShape& srcShape` — input shape information
- `const uint32_t typeSize` — data type size (bytes)
- `const bool isReuseSource` — whether modifying the source operand is allowed (currently false)

**Output Parameters (references):**

- `uint32_t& maxValue` — required maximum temporary space
- `uint32_t& minValue` — required minimum temporary space

**Return value:** `void`

### Implementation Location

- Declaration: `include/adv_api/<category>/<api>_tiling.h`
- Implementation: `impl/adv_api/tiling/<category>/<api>_tiling_impl.cpp` (or `_tiling.cpp`)
- Registration: add the `.cpp` to the `tiling_api` target in `impl/adv_api/tiling/CMakeLists.txt`

### Constant Definitions

Tiling constants are defined with `constexpr` in the implementation file:

```cpp
constexpr uint32_t AXPY_HALF_CALC_PROC = 4;
constexpr uint32_t AXPY_FLOAT_CALC_PROC = 1;
constexpr uint32_t AXPY_ONE_REPEAT_BYTE_SIZE = 256;
```

### Assertion Usage

On the Tiling side, use `ASCENDC_HOST_ASSERT` instead of `ASCENDC_ASSERT`:

```cpp
ASCENDC_HOST_ASSERT(inputSize > 0, return, "Input Shape size must be greater than 0.");
```

---

## Public Header File Registration

### High-level API Registration

After adding a new high-level API, you need to add include in the following files:

**`include/adv_api/kernel_api.h`:**

```cpp
#include "math/axpy.h"
```

Architecture guards are managed internally by each sub-header file; `kernel_api.h` includes them directly.

**`include/adv_api/tiling_api.h`:**

```cpp
#include "math/axpy_tiling.h"
```

### Basic API Registration

Newly added basic APIs are registered in the corresponding main entry, for example `kernel_operator.h`.

### Architecture Guards

- APIs supporting multiple architectures: list all supported architectures in `#if defined(__NPU_ARCH__)`
- APIs supporting only a specific architecture: restrict the architecture scope at include time

---

## Interface and Implementation Separation

### Public Header Files (`include/`)

- Contain interface documentation and inline forwarding definitions
- `#include` the corresponding architecture impl header before the forwarding definitions
- Annotated with `__aicore__`

### Implementation Header Files (`impl/`)

- Contain `#pragma message` warnings
- For high-level API, architecture dispatch is performed by the public header via `#if __NPU_ARCH__` selecting the corresponding impl file (e.g., `axpy_3510_impl.h` or `axpy_common_impl.h`); for basic API, dispatch is performed by the `*_intf_impl.h` header
- Contain the concrete algorithm logic

---

## Interface Implementation Patterns

### High-level API Implementation Template

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
    // Implementation logic...
}
```

### Algorithm Function and Intrinsics Separation

Complex APIs separate the public interface, algorithm parameter decomposition, and low-level intrinsics:

- `ApiName(...)` — public interface (public header)
- `ApiNameImpl(...)` — algorithm parameter decomposition (impl header)
- `ApiNameSub(...)` — tiling loop logic (impl header)
- `ApiNameIntrinsicsImpl(...)` — low-level instruction calls (impl header)
