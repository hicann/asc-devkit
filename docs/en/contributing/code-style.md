# C++ Code Style Standards

## Naming Conventions

### File Naming

Source files and custom directories use **snake_case** (lowercase letters + underscores); tool-convention files (e.g., `CMakeLists.txt`, `README.md`, `CONTRIBUTING.md`) keep their standard naming. Suffix conventions are described in [directory-structure.md](directory-structure.md#file-naming-conventions).

### Function Naming

**Basic API (`basic_api/`):** PascalCase

```text
Add, Sub, Mul, DataCopy, SetVectorMask, PipeBarrier
```

**C API (`c_api/`):** snake_case, `asc_` prefix

```text
asc_add, asc_axpy, asc_axpy_sync
```

**High-level API (`adv_api/`):** PascalCase

```text
Axpy, Exp, LayerNorm, GroupedMatmul
```

Synchronous versions are named by API style (depending on API type and actual need): C API uses the `_sync` suffix (e.g., `asc_add_sync`). Not all APIs have synchronous variants.

```text
asc_add_sync, WarpReduceAddSync
```

### Macro Naming

All uppercase letters + underscores.

| Type | Prefix | Example |
|------|--------|---------|
| Include guard | `ASCENDC_` | `ASCENDC_KERNEL_COMMON_H` |
| Internal sentinel | `__ASCENDC_INCLUDE_INTERNAL_HEADERS__` | Used only within the guard system |
| General-purpose macro | `ASCENDC_` | `ASCENDC_ASSERT`, `ASCENDC_CHECK_VALUE_RANGE` |
| Logging macro | `KERNEL_LOG_` | `KERNEL_LOG_KERNEL_ERROR` |
| Constant/enum macro | All uppercase | `DT_FLOAT`, `QUE_MAX_DEPTH`, `FORMAT_NCHW` |

**Rules:**

- Custom `__` double-underscore prefixes are prohibited (reserved by the C++ standard)
- `__ASCENDC_INCLUDE_INTERNAL_HEADERS__` and `__UNDEF_*` are the only exceptions

### Type/Struct Naming

PascalCase:

```cpp
LocalTensor, GlobalTensor, TPipe, TQue, FixpipeConfig, DivConfig
```

### Variable Naming

camelCase:

```text
dstTensor, srcTensor, calCount, stackSize, sharedTmpBuffer, repeatTime
```

Global configuration constants use a lowercase `k` prefix or all uppercase:

```text
kDefaultNdDmaConfig, DEFAULT_BINARY_CONFIG, QUE_MAX_DEPTH
```

### Template Parameter Naming

Single uppercase letter or CamelCase:

```text
T, U, isReuseSource
```

---

## Header Guard Standards

### Public Header Files (Three-Layer Guard System)

Basic API and high-level API public headers that use the internal header sentinel employ three-layer protection:

```cpp
// Layer 1: Internal header sentinel (prevents recursive include)
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_<FILENAME_UPPER>__
#endif

// Layer 2: Traditional include guard
#ifndef ASCENDC_<MODULE>_H
#define ASCENDC_<MODULE>_H

// ... file content ...

#endif  // ASCENDC_<MODULE>_H

// Layer 3: Sentinel cleanup
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_<FILENAME_UPPER>__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_<FILENAME_UPPER>__
#endif
```

**Guard macro naming formula:**

```text
File name: kernel_common.h
  → Layer 2 guard: ASCENDC_KERNEL_COMMON_H
  → Layer 1 undef: __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_COMMON_H__

File name: kernel_operator_vec_binary_intf.h
  → Layer 2 guard: ASCENDC_MODULE_OPERATOR_VEC_BINARY_INTERFACE_H
  → Layer 1 undef: __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_INTF_H__
```

**Naming rule:** `ASCENDC_` + module path (slashes converted to underscores, all uppercase) + `_H`

> **Note:** Some high-level API header files (`adv_api/`) use the `LIB_` or `ADV_API_` prefix (e.g., `LIB_MATH_SIN_H`, `ADV_API_CONV_COMMON_CONV_COMMON_H`) as a legacy convention. New code should prefer the `ASCENDC_` prefix.
>
> **Note:** Some basic API header files use `MODULE`/`INTERFACE` instead of `KERNEL`/`INTF` in the Layer 2 guard (e.g., `kernel_operator_vec_binary_intf.h` has guard `ASCENDC_MODULE_OPERATOR_VEC_BINARY_INTERFACE_H`), while the Layer 1 undef uses `KERNEL`/`INTF`. This is a legacy naming convention; new code should use macro names derived from the filename.

### Implementation Header Files (Additional `#pragma message` Warning)

When the implementation contained in `impl/` is included at the bottom of a public header file, the implementation header file itself should add:

```cpp
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message("impl/basic_api/kernel_operator_vec_binary_intf_impl.h "
    "is an internal header file and must not be used directly. "
    "Functions or variables defined in this file may be removed in the future. "
    "Please use \"#include \\\"basic_api/kernel_operator_vec_binary_intf.h\\\"\" "
    "and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_INTF_IMPL_H__
#endif
```

### C API Header Files

C API uses a separate sentinel namespace:

```cpp
#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif
```

### `#pragma once` Prohibited

This project does not use `#pragma once` in principle; public header files for basic API, high-level API, and C API use the `#ifndef` / `#define` / `#endif` style.

---

## Include Standards

### Quotes vs Angle Brackets

| Symbol | Purpose | Example |
|--------|---------|---------|
| `""` | Internal project header files | `#include "kernel_tensor.h"` |
| `<>` | Standard library header files (recommended inside the `ASCENDC_CPU_DEBUG` guard) | `#include <cstdint>` |

### Four Include Path Modes

| Mode | Example | Applicable Scenario |
|------|---------|---------------------|
| Bare filename | `#include "kernel_tensor.h"` | Headers in the same directory or dependencies |
| Subdirectory path | `#include "reg_compute/kernel_reg_compute_utils.h"` | Different subdirectory within the same module |
| `include/` prefix absolute path | `#include "include/adv_api/kernel_api.h"` | Cross-module references |
| `../../impl/` relative backtrack | `#include "../../impl/basic_api/kernel_operator_vec_binary_intf_impl.h"` | Interface header referencing implementation header |

### Include Order

Organize includes in the following order, with a blank line separating each group:

1. **Core Project Dependencies**

   ```cpp
   #include "kernel_macros.h"
   #include "kernel_tensor.h"
   ```

2. **Same-Module Struct/Type Headers**

   ```cpp
   #include "kernel_struct_binary.h"
   #include "kernel_struct_unary.h"
   ```

3. **Architecture-Conditional Includes**

   ```cpp
   #if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
   #include "reg_compute/kernel_reg_compute_utils.h"
   #endif
   ```

4. **CPU Debug Conditional Includes (Standard Library)**

   ```cpp
   #if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
   #include <cstdint>
   #include "stub_def.h"
   #endif
   ```

5. **Impl Header References (placed at the bottom, before the `#endif` of the Layer 2 guard)**

   ```cpp
   #include "../../impl/basic_api/kernel_operator_vec_binary_intf_impl.h"

   #endif  // ASCENDC_<MODULE>_H
   ```

### Public Entry Header Files

Unified entry files for high-level APIs (e.g., `kernel_api.h`) include sub-header files directly; `__NPU_ARCH__` architecture guards are managed internally by each sub-header file:

```cpp
#include "math/axpy.h"
```

---

## Namespace Standards

### Main Namespace

All public C++ APIs reside under `namespace AscendC { }`; C API `asc_*` interfaces are in the global namespace.

```cpp
namespace AscendC {
__aicore__ inline void Add(...);
}  // namespace AscendC
```

### Sub-Namespaces

| Sub-Namespace | Purpose |
|---------------|---------|
| `AscendC::Std` | Standard library compatibility layer |
| `AscendC::Simt` | SIMT programming model |
| `AscendC::Te` | Tensor Engine |
| `AscendC::Reg` | Register Compute |

### Namespace Closing Comments

A comment is recommended after the closing brace:

```cpp
}  // namespace AscendC::Std
}  // namespace AscendC
```

### `using namespace` Prohibited

`using namespace` is not allowed in header files.

### `#pragma begin_pipe` / `#pragma end_pipe`

Pipeline function blocks are wrapped with the following pragmas:

```cpp
#pragma begin_pipe(V)
namespace AscendC {
    // vector pipeline functions...
}  // namespace AscendC
#pragma end_pipe
```

---

## Macro Standards

### Function-like Macros

Wrap multi-statement macros with `do { ... } while (0)`:

```cpp
#define ASCENDC_ASSERT(cond, behavior)                          \
    do {                                                        \
        if (!(cond)) {                                          \
            behavior;                                           \
            AscendC::KernelRaise::GetInstance().Raise(SIGABRT); \
        }                                                       \
    } while (0)
```

### Logging Macros

```cpp
#define KERNEL_LOG(level, format, ...) KERNEL_LOG_##level(format, ##__VA_ARGS__)
```

Log levels: `KERNEL_DEBUG`, `KERNEL_INFO`, `KERNEL_WARN`, `KERNEL_ERROR`

### Assertion and Validation Macros

| Macro | Purpose |
|-------|---------|
| `ASCENDC_ASSERT(cond, behavior)` | Executes behavior and aborts on assertion failure (only effective in CPU Debug / Host builds; expands to empty on device) |
| `ASCENDC_ASSERT_WARNING(cond, behavior)` | Executes behavior only on assertion failure, without abort |
| `ASCENDC_HOST_ASSERT(cond, ret, format, ...)` | Tiling-side (host-side) assertion, executes ret and outputs diagnostic message on failure |
| `ASCENDC_CHECK_VALUE_RANGE(v, lo, hi, name, msg)` | Parameter value range validation |
| `ASCENDC_CHECK_TENSOR_PTR_ALIGN(ptr, pos, align, name, msg)` | Address alignment validation |
| `ASCENDC_REPORT_NOT_SUPPORT(cond, msg)` | Unsupported condition report |
| `ASCENDC_REPORT_OVERFLOW_MEM(cond)` | Memory overflow report |

### Architecture Conditional Macros

```cpp
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
    // architecture-specific code
#endif

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    // CPU debug code
#endif

#if (defined(UT_TEST) || defined(ST_TEST))
    // test code
#endif
```

### Macro Naming Prefix Conventions

- `ASCENDC_`: unified prefix for all functional macros
- `KERNEL_LOG_`: prefix for logging macros
- `__ASCENDC_INCLUDE_INTERNAL_HEADERS__` and the companion `__UNDEF_*` cleanup macros: guard system uses the `__` prefix

---

## constexpr and Compile-Time Programming

### Global Configuration Constants

```cpp
constexpr FixpipeConfig CFG_NZ_FIX = {CO2Layout::NZ, false, true};
constexpr FixpipeConfig CFG_NZ = {CO2Layout::NZ, false};
constexpr DivConfig DEFAULT_DIV_CONFIG = {DivAlgo::INTRINSIC};
constexpr BinaryConfig DEFAULT_BINARY_CONFIG = {1};
```

### constexpr + `__aicore__` on Member Functions

`constexpr` appears before `__aicore__` (the recommended unified style):

```cpp
constexpr __aicore__ static inline T Max()
constexpr __aicore__ static inline T Min()
constexpr __aicore__ inline int16_t GetDataBlockSizeInBytes()
```

> **Note:** The `__aicore__ constexpr` style still predominates in the current codebase. New code should consistently place `constexpr` first, as the migration target for new code.

### `if constexpr` Compile-Time Branching

```cpp
if constexpr (sizeof(U) == sizeof(float)) {
    Axpy<T, U>(dstTensor, srcTensor, scalarValue, calCount);
} else {
    AxpySub<T, U, isReuseSource>(dstTensor, srcTensor, scalarValue, sharedTmpBuffer, calCount);
}
```

```cpp
if constexpr (AscendC::Std::is_same<T, uint8_t>::value) {
    // ...
} else if constexpr (AscendC::Std::is_same<T, float>::value) {
    // ...
}
```

### `static_assert` Type Constraints

```cpp
static_assert(SupportType<T, uint8_t, int8_t, uint16_t, int16_t, half, float>(),
    "current data type is not supported on current device!");
```

### `static constexpr` Class Members

```cpp
static constexpr TQueConfig config = GetTQueConfig(mask);
static constexpr TPosition srcPosition = src;
static constexpr Hardware srcHardType = GetPhyType(src);
```

---

## Error Handling Standards

### `ASCENDC_ASSERT`

Perform parameter validation at the beginning of functions:

```cpp
__aicore__ inline void Add(const LocalTensor<T>& dst, const LocalTensor<T>& src0,
    const LocalTensor<T>& src1, ...)
{
    ASCENDC_ASSERT((dst.GetSize() == src0.GetSize()), {
        KERNEL_LOG(KERNEL_ERROR, "Tensor size mismatch: dst=%u, src0=%u",
            dst.GetSize(), src0.GetSize());
    });
    // ...
}
```

### `CHECK_FUNC_HIGHLEVEL_API`

In high-level API implementation functions or interface functions, call it on the first line of the function body:

```cpp
template <typename T, typename U, bool isReuseSource>
__aicore__ inline void AxpyImpl(...)
{
    CHECK_FUNC_HIGHLEVEL_API(
        Axpy, (T, U, isReuseSource), (dstTensor, srcTensor, scalarValue, sharedTmpBuffer, calCount));
    // ...
}
```

### `KERNEL_LOG` Log Level Selection

| Level | Usage Scenario |
|-------|----------------|
| `KERNEL_DEBUG` | Development debug information |
| `KERNEL_INFO` | General runtime information |
| `KERNEL_WARN` | Potential issue warning |
| `KERNEL_ERROR` | Error condition |

### `#pragma message` Deprecation Warnings

Warning prohibiting direct inclusion of impl headers:

```cpp
#pragma message("impl/xxx.h is an internal header file and must not be used directly. Please use include/xxx.h.")
```

---

## Formatting

### clang-format Configuration

The `.clang-format` file in the project root directory configures a unified code style. Key configuration items:

| Configuration Item | Value | Description |
|--------------------|-------|-------------|
| BasedOnStyle | Google | Based on Google C++ Style |
| ColumnLimit | 120 | Maximum 120 characters per line |
| IndentWidth | 4 | 4-space indentation |
| UseTab | Never | Do not use Tab |
| PointerAlignment | Left | Pointer symbol left-aligned |
| BreakBeforeBraces | Custom | Custom brace wrapping (see BraceWrapping below) |
| BraceWrapping.AfterFunction | true | Function braces wrapped |
| BraceWrapping.AfterControlStatement | false | Control-flow braces not wrapped |
| SortIncludes | false | Do not auto-sort includes |
| AccessModifierOffset | -4 | Access modifier indentation |
| AlwaysBreakTemplateDeclarations | true | Break between template declaration and subsequent declaration |

### Automatic Formatting

The project automatically formats C/C++/`.asc` source files at commit time via a pre-commit hook (the `clang-format` hook in `.pre-commit-config.yaml`). Refer to the actual repository file for the specific configuration; developers do not need to maintain it manually.

Enabling steps:

1. Install pre-commit: `pip install pre-commit`
2. Run `pre-commit install` in the repository root to install the git hook
3. Afterwards, each `git commit` will automatically trigger clang-format; files that do not conform to the style will be fixed in place and the commit blocked. Re-run `git add` and commit again.

You can also skip the hook and manually format a single file:

```bash
clang-format -i --style=file <file>
```

---

## Code Comment Standards

### Comment Style Overview

The project uses three comment styles for different API types:

| Style | Prefix Symbol | Applicable Scenario | Example Location |
|-------|---------------|---------------------|------------------|
| `\brief` / `\param` / `\note` | Backslash `\` | High-level API public header files | `include/adv_api/math/sin.h` |
| `@ingroup` / `@brief` | `@` symbol | Basic API public header files | `include/basic_api/kernel_operator_vec_binary_intf.h` |
| `ingroup` / `brief` | No prefix | impl internal header files | `impl/basic_api/kernel_operator_common_intf_impl.h` |

### File Header Comments

Every public header file must begin with a `\file` block:

```cpp
/*!
 * \file kernel_common.h
 * \brief
 */
```

- Fill in the filename after `\file`
- `\brief` may be left empty (basic API) or contain a one-line functional description (high-level API)

High-level APIs may include formula descriptions:

```cpp
/*!
 * \file sin.h
 * \brief Defines a series of interface used to do elementwise math sin calculation.
 * Formula: sin(x) = (-1)^k*sin(x0), sin(x) = xP(x)
 */
```

### Function/Class Comments

**High-level API style (`\brief` / `\param` / `\note`)**

```cpp
/*!
 * \ingroup Sin
 * \brief compute Sin elementwisely
 *
 * \tparam T: half/float
 * \tparam isReuseSource: whether allows API to modify source data during calculation
 *
 * \param [out] dstTensor: output LocalTensor
 * \param [in] srcTensor: input LocalTensor
 * \param [in] sharedTmpBuffer: extra temporary shared space for intermediate
 * \param [in] calCount: number of elements to be calculated
 *
 * \note src/dst Tensor must be 32B aligned
 * \note The calculation may allocate temporary space through sharedTmpBuffer
 */
```

**Elements:**

- `\ingroup` + functional group name (first line)
- `\brief` + brief functional description
- `\tparam` + template parameter description (one line per parameter)
- `\param [out]` / `\param [in]` + parameter description
- `\note` + constraints and notes

**Basic API style (`@ingroup` / `@brief` + separator line)**

```cpp
/* **************************************************************************************************
 * Add                                             *
 * ************************************************************************************************* */

/*
 * @ingroup Add Level 0
 * @brief dst = src0 + src1
 *
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.src0BlkStride src0 block stride
 * @param [in] repeatParams.src1BlkStride src1 block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src0 repeat stride
 * @param [in] repeatParams.src1RepStride src1 repeat stride
 */
```

**Elements:**

- `*` separator line, with the function name in the middle
- `@ingroup` + group + Level (first line)
- `@brief` + formula
- `@param [out]` / `@param [in]` + parameter description
- Nested parameters separated by `.` (e.g., `repeatParams.dstBlkStride`)

**C API style**

```cpp
// ==========asc_add(half/float)==========
// Continuous mode
__aicore__ inline void asc_add(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

// Configuration mode
__aicore__ inline void asc_add(__ubuf__ half* dst, ...);
```

### Parameter Annotation Conventions

**Direction Annotations**

| Annotation | Meaning |
|------------|---------|
| `\param [out]` | Output parameter |
| `\param [in]` | Input parameter |
| `\param [in/out]` | Input/output parameter |

**Template Parameters**

```text
\tparam T: target operand data type, supports half/float
\tparam U: source operand data type, supports half/float
\tparam isReuseSource: whether allows API to modify source data during calculation
```

**Parameter Description Format**

- Parameter names match the actual formal parameter names
- Descriptions should be concise, explaining type constraints and value ranges
- Use English or Chinese consistently within a single file

### Special Annotations

**`\note` Constraint Notes**

```cpp
/*!
 * \note src/dst Tensor must be 32B aligned.
 * \note CalCount must be less than or equal to the element count of src/dst Tensor.
 * \note support data type: half and float
 */
```

Common uses include alignment requirements, data range limits, supported data types, and performance notes.

**Deprecation Annotations**

Method one `[[deprecated]]` attribute:

```cpp
[[deprecated("NOTICE: SetDumpWorkSpacePtr has been deprecated and will be removed "
             "in the next version. Please do not use it!")]]
__aicore__ inline __gm__ uint8_t* SetDumpWorkSpacePtr(__gm__ uint8_t* workspace)
```

Method two line comment:

```cpp
// NOTICE: InitOutput has been deprecated and will be removed in the next version.
// Please use Fill instead!
```

**Function Group Separator (Basic API)**

```cpp
/* **************************************************************************************************
 * Add                                             *
 * ************************************************************************************************* */
```

- 100 columns wide, with the function name in the middle
- Used to delineate functional blocks in large interface files

### Chinese-English Language Conventions

| Content Type | Language | Example |
|--------------|----------|---------|
| Public API comments (`\brief`, `\param`) | English | `\brief compute Sin elementwisely` |
| impl internal comments | Chinese | `// 计算循环次数` |
| Log messages | English | `"Tensor size mismatch"` |
| Assertion messages | English | `"current data type is not supported"` |
| File/function/variable names | English | `kernel_operator_vec_binary_intf.h` |
| Documentation descriptions (.md) | Match the document locale | `docs/zh`: Chinese; `docs/en` and `*_en.md`: English |
