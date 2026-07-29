# Directory Structure and File Naming Standards

## Top-Level Directory Overview

```text
asc-devkit/
├── include/              # Public API declarations (.h)
├── impl/                 # Internal implementation (.h + .cpp)
├── tests/                # Test code
├── examples/             # Example code (.asc + .cpp)
├── docs/                 # Documentation
├── tools/                # Build tools
├── cmake/                # CMake modules and macros
├── scripts/              # Auxiliary scripts
├── build.sh              # Build entry script
├── CMakeLists.txt        # Root build file
├── .clang-format         # C++ formatting configuration
├── .pre-commit-config.yaml # Pre-commit hook
├── CONTRIBUTING.md       # Contribution process
└── README.md             # Project description
```

---

## include/ ↔ impl/ Mirror Relationship

The `include/` and `impl/` top-level directory structures have a one-to-one correspondence:

| include/ | impl/ | Description |
|----------|-------|------|
| `include/basic_api/` | `impl/basic_api/` | Basic API (kernel operator interfaces and structs) |
| `include/adv_api/` | `impl/adv_api/` | High-level API (math library, normalization, matmul, etc.) |
| `include/c_api/` | `impl/c_api/` | C API (instruction layer encapsulation) |
| `include/simt_api/` | `impl/simt_api/` | SIMT API |
| `include/tensor_api/` | `impl/tensor_api/` | Tensor API |
| `include/utils/` | `impl/utils/` | Utility classes |
| `include/aicpu_api/` | `impl/aicpu_api/` | AI CPU API |

### Detailed Mapping Rules

| include path | impl path | Description |
|-------------|----------|------|
| `include/basic_api/xxx.h` | No matching `xxx_impl.h` (e.g. `kernel_common.h`); some have corresponding `xxx_impl.h` (e.g. `kernel_tensor.h` → `kernel_tensor_impl.h`) | Only a few basic utility headers are self-contained |
| `include/basic_api/xxx_intf.h` | `impl/basic_api/xxx_intf_impl.h` | Interface → architecture dispatch implementation |
| `include/adv_api/math/xxx.h` | `impl/adv_api/detail/math/xxx/xxx_common_impl.h` | |
| `include/adv_api/math/xxx_tiling.h` | `impl/adv_api/tiling/math/xxx_tiling_impl.cpp` | |
| `include/adv_api/<cat>/xxx.h` | `impl/adv_api/detail/<cat>/xxx/xxx_common_impl.h` | |
| `include/c_api/<category>/<category>.h` | `impl/c_api/instr_impl/npu_arch_<NNNN>/<category>_impl/asc_<api>_impl.h` | C API uses `npu_arch_` prefix directory |

---

## File Naming Conventions

### Suffix Conventions

| Suffix | Meaning | Directory |
|------|------|---------|
| `_intf.h` | Interface declaration | `include/` |
| `_impl.h` | Implementation definition | `impl/` |
| `_intf_impl.h` | Interface + architecture dispatch implementation | `impl/` |
| `_tiling.h` | Tiling interface declaration | `include/` |
| `_tiling_intf.h` | Tiling interaction interface declaration | `include/` |
| `_tiling_impl.cpp` | Tiling host-side implementation | `impl/adv_api/tiling/` |
| `_tiling.cpp` | Tiling implementation (variant without `_impl` suffix) | `impl/adv_api/tiling/` |
| `_struct_*.h` | Type/struct definitions | `include/` |
| `_common_impl.h` | Common algorithm implementation | `impl/adv_api/detail/` |
| `_check.h` | Parameter validation logic | `impl/adv_api/detail/api_check/` |
| `_config.h` | Configuration parameter definitions | `include/` or `impl/` |
| `_utils.h` | Utility functions | `include/` or `impl/` |

### Naming Rules

- Source files and custom directories use **snake_case** (lowercase letters + underscores)
- No spaces, no hyphens, no uppercase
- English words, avoid abbreviations (except widely accepted ones like `vec`, `intf`)
- Exception: tool convention files such as `CMakeLists.txt`, `README.md`, `CONTRIBUTING.md` retain their standard naming

**Correct Examples:**

```text
kernel_operator_vec_binary_intf.h
kernel_struct_fixpipe.h
kernel_operator_common_intf_impl.h
axpy_common_impl.h
exp_check_common.h
```

### Naming Differences Across API Types

| API type | Header file | Implementation file | Example |
|---------|--------|---------|------|
| Basic API | `kernel_<name>_intf.h` | Dispatch: `kernel_<name>_intf_impl.h`; arch impl: `dav_<arch>/kernel_<name>_impl.h` | `kernel_operator_vec_binary_intf.h` |
| High-level API | `<name>.h` | `<name>_common_impl.h` | `axpy.h` / `axpy_common_impl.h` |
| C API | `<category>.h` | `asc_<api>_impl.h` | `vector_compute.h` / `asc_add_impl.h` |
| Tiling | `<name>_tiling.h` | `<name>_tiling_impl.cpp` | `axpy_tiling.h` |

---

## Architecture Directory Naming

### Naming Format

Architecture-specific implementations for basic API are placed under the `impl/basic_api/dav_<code>/` directory, in the format `dav_` + architecture code. For high-level API, architecture-specific implementations reside in `impl/adv_api/detail/<category>/<api>/<api>_<arch>_impl.h`, dispatched by the public header via `#if __NPU_ARCH__`. C API uses the `npu_arch_<NNNN>/` prefix directory; see the C API mapping table above.

### Architecture Code Mapping (Examples)

The following lists some common architecture directories; for the complete list, see the `impl/basic_api/dav_*/` directories:

| Directory | `__NPU_ARCH__` value | Corresponding Products |
|------|-------------------|---------|
| `dav_c220` | `2201` | Atlas A2 training series products / Atlas A2 inference series products, Atlas A3 training series products / Atlas A3 inference series products |
| `dav_3510` | `3510` | Ascend 950PR / Ascend 950DT |

> For the complete mapping between product models and `__NPU_ARCH__`, see the [**NPU_ARCH** documentation](../../zh/guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md#npu-arch).

### Architecture Dispatch Pattern

In impl header files, use an `#if / #elif / #endif` chain to select the implementation by architecture:

```cpp
#if __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_common_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_common_impl.h"
#endif
```

---

## impl/adv_api Internal Organization

```text
impl/adv_api/
├── CMakeLists.txt
├── detail/                   # Core implementation
│   ├── activation/           # Activation functions
│   ├── api_check/            # Parameter validation
│   │   ├── host_apicheck.h
│   │   ├── kernel_api_check.h
│   │   └── kernel_check/     # Validation logic for each API
│   │       └── math/
│   │           └── exp/
│   │               └── exp_check_common.h
│   ├── common/               # Common utilities
│   ├── math/                 # Math operations (50+ subdirectories)
│   │   └── exp/
│   │       ├── exp_common_impl.h
│   │       └── exp_3510_impl.h
│   ├── matmul/               # Matmul
│   ├── normalization/        # Normalization
│   ├── reduce/               # Reduction
│   └── ...
└── tiling/                   # Tiling (host-side memory planning)
    ├── CMakeLists.txt
    └── math/
        └── exp_tiling_impl.cpp
```

---

## New API Operation Checklist

When adding a complete API, create and modify files according to the following checklist:

### High-level API Checklist

```text
□ include/adv_api/<category>/<api>.h
    Public header file, including Doxygen documentation, template declarations, forwarding to impl

□ include/adv_api/<category>/<api>_tiling.h
    Tiling interface declaration (if the API needs temporary space)

□ impl/adv_api/detail/<category>/<api>/<api>_common_impl.h and/or <api>_<arch>_impl.h
    Core algorithm implementation (common impl uses _common_impl.h; arch-specific impl uses <api>_<arch>_impl.h)

□ impl/adv_api/detail/api_check/kernel_check/<category>/<api>/<api>_check.h
    Parameter validation logic (required by CHECK_FUNC_HIGHLEVEL_API)

□ impl/adv_api/tiling/<category>/<api>_tiling_impl.cpp
    Tiling interface implementation (if needed)

□ impl/adv_api/tiling/CMakeLists.txt
    Add the .cpp to the add_library of tiling_api

□ impl/adv_api/detail/<category>/CMakeLists.txt (if any)
    Add subdirectory add_subdirectory(<api>)

□ include/adv_api/kernel_api.h
    Add #include "<category>/<api>.h"

□ include/adv_api/tiling_api.h
    Add #include "<category>/<api>_tiling.h"

□ tests/api/adv_api/<category>/<api>/test_operator_<api>.cpp
    Functional test

□ tests/api/adv_api/api_check/<category>/<api>/kernel_<api>_check.cpp
    Parameter validation UT (CHECK_FUNC_HIGHLEVEL_API check under CPU Debug)

□ tests/api/adv_api/tiling/test_tiling.cpp (or corresponding category file)
    Choose by category: general → test_tiling.cpp, arch-specific → test_tiling_v310.cpp, HCCL → test_hccl_tiling.cpp, Matmul → test_matmul_api_tiling.cpp, Conv → tiling/conv/ or tiling/conv_backprop/

□ tests/api/adv_api/CMakeLists.txt
    Add test files to the GLOB of the corresponding product type

□ tests/api/adv_api/api_host_check/api_host_check.cpp
    Add public header `#include` to the high-level API host checker aggregate file (aggregate compile check)
```

### Basic API Checklist

```text
□ include/basic_api/kernel_operator_<name>_intf.h
    Interface declaration

□ impl/basic_api/kernel_operator_<name>_intf_impl.h
    Interface + architecture dispatch implementation

□ impl/basic_api/dav_<arch>/kernel_operator_<name>_impl.h
    Implementation file for each architecture

□ tests/api/basic_api/ascendc_case_<product>/test_operator_<name>.cpp
    Functional test (place by product; cross-product cases go in ascendc_case_common)

□ include/kernel_operator.h (or other main entry)
    Add include

□ tests/api/basic_api/ascendc_header_checker/kernel_operator_<name>_intf.cpp
    Header Checker compilation check file

```

### C API Checklist

```text
□ include/c_api/<category>/<category>.h or add function declarations to existing file

□ impl/c_api/instr_impl/npu_arch_<NNNN>/<category>_impl/asc_<api>_impl.h
    Architecture-specific implementation

□ impl/c_api/instr_impl/npu_arch_<NNNN>/<category>_impl.h
    Add #include "<category>_impl/asc_<api>_impl.h" in the aggregate header
    (when adding a new category, also register the public header in include/c_api/asc_simd.h)

□ tests/api/c_api/npu_arch_<arch>/<category>/test_asc_<api>.cpp
    Mock test

□ tests/api/c_api/CMakeLists.txt
    When adding a new category, add the test directory to the corresponding product type GLOB

□ docs/zh/api/SIMD-API/C-API/<category>/asc_<api>.md
    API documentation
```

---

## examples/ Directory Organization

### Directory Structure

```text
examples/
├── 01_simd_cpp_api/        # SIMD C++ API examples
│   ├── 00_introduction/    # Introduction examples
│   ├── 01_utilities/       # Utility usage (printf, assert, etc.)
│   ├── 02_features/        # Advanced features
│   ├── 03_basic_api/       # Basic API examples
│   │   └── 00_data_movement/
│   │       └── copy_ub2ub/ # UB to UB copy example
│   │           ├── CMakeLists.txt
│   │           ├── copy.asc          # Kernel source (.asc extension)
│   │           ├── data_utils.h     # Test data utilities
│   │           └── README.md
│   ├── 04_advanced_api/    # High-level API examples
│   ├── 05_best_practices/  # Best practices
│   └── 06_compatibility_guide/  # Compatibility guide
├── 02_simd_c_api/          # SIMD C API examples
├── 03_simt_api/            # SIMT API examples
├── 04_aicpu/               # AI CPU API examples
├── 05_simd_simt_hybrid/   # SIMD/SIMT hybrid programming examples
├── README.md
└── README_en.md
```

### Naming Rules

- Typical directory organization: `<api_type>/<category>/<api_name>/`, with some categories having deeper levels
- Numeric prefixes ensure ordering: `00_introduction/`, `01_utilities/`
- Kernel source files use `.asc` extension
- Each example must have its own `CMakeLists.txt`
- Each example is recommended to include `README.md`

---

## Test File Naming

For test directory structure and file naming standards, see [testing.md](testing.md).
