# C API Contribution Guide

## Overview

C API is the instruction-level API in the Ascend C programming framework. It directly maps to the hardware instruction set of Ascend AI processors. C API uses C-style function interfaces and provides programming capabilities closest to hardware for operator development.

Key features of C API:

- **Instruction-level mapping**: Each API directly maps to one or more hardware instructions.
- **Pointer-based programming**: Directly operates on memory addresses without abstraction layer overhead.
- **High performance**: Zero-overhead encapsulation for maximum hardware performance.
- **Fine-grained control**: Supports low-level control of pipeline, mask, stride, and so on.

This document introduces how to develop new C APIs based on the Ascend C framework, including programming basics, directory planning, architecture design, and regression testing requirements.

---

## Development Process

The C API development process includes the following steps:

- **Requirement analysis**: Define API functionality.
- **Interface design**: Define C-style function interfaces.
- **Implementation development**: Write instruction wrappers and parameter conversion code.
- **Test and verification**: Complete mock testing and functional testing.
- **Documentation**: Complete API documentation and constraint descriptions.

---

## API Introduction

### Hardware Instruction Basics

C API directly wraps hardware instructions. You need to understand:

#### Instruction Types

- **Vector instructions**: VADD, VMUL, VCONV, VREDUCE, and so on.
- **Scalar instructions**: SADD, SMUL, and so on.
- **Data movement instructions**: VMOV, DCOPY, and so on.
- **Control instructions**: SETMASK, PIPEBARRIER, and so on.

#### Instruction Attributes

- **Pipeline type**: V, M, S, MTE1/2/3, and so on.
- **Data type**: half, float, int16, int32, and so on.
- **Operand count**: Unary, binary, ternary operations.
- **Synchronization characteristic**: Asynchronous/synchronous execution.

### Programming Model

C API uses a **pointer-based programming model**:

```cpp
// C API programming example
constexpr uint32_t count = 64;
__ubuf__ half dst[count];   // Unified Buffer (UB) memory
__ubuf__ half src0[count];
__ubuf__ half src1[count];


// Directly call C API
asc_add(dst, src0, src1, count);
```

#### Data Flow Modes

C API supports three data flow modes:

Mode 1: First-n elements computation

```cpp
asc_add(dst, src0, src1, count);
```

- Contiguous memory block computation.
- Automatically handles mask.
- Simple and easy to use.

Mode 2: High-dimensional tiling computation

```cpp
asc_add(dst, src0, src1, repeat,
        dst_block_stride, src0_block_stride, src1_block_stride,
        dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
```

- Non-contiguous memory access.
- Supports multi-dimensional data.
- Flexible control.

Mode 3: Synchronous computation

```cpp
asc_add_sync(dst, src0, src1, count);
```

- Automatically synchronizes pipeline.
- Ensures data consistency.
- Used for critical paths.

### Parameter Conventions

#### Common Parameters

| Parameter | Type | Description |
| -------- | ------ | ------ |
| dst | `__ubuf__ T*` | Output address |
| src0, src1 | `__ubuf__ T*` | Input address |
| count | `uint32_t` | Element count |
| repeat | `uint8_t` | Iteration count |
| block_stride | `uint8_t` | Address stride within block |
| repeat_stride | `uint8_t` | Address stride between repeats |
| mask0, mask1 | `uint64_t` | Computation mask |

#### Mask Calculation

- **Half precision**: Each repeat processes 256 bytes (128 half values).
  - mask0 corresponds to lower 64 elements, mask1 corresponds to upper 64 elements.
  - When count=64, mask0=0xFFFFFFFFFFFFFFFF, mask1=0.

- **Single precision**: Each repeat processes 256 bytes (64 float values).
  - mask0 corresponds to all 64 elements.
  - When count=32, mask0=0x00000000FFFFFFFF.

#### Stride Calculation

- **Block Stride**: Address offset between different DataBlocks within a single iteration.
  - Usually 1 (contiguous access).
  - Can be used for transpose, block access.

- **Repeat Stride**: Address offset of the same DataBlock between adjacent iterations.
  - Half precision: 8 (256-byte aligned).
  - Single precision: 4 (256-byte aligned).

---

## Directory Planning

### Directory Structure

```text
asc-devkit/
├── include/
│   └── c_api/                    # C API header files
│       ├── vector_compute/       # Vector computation
│       │   └── vector_compute.h
│       ├── scalar_compute/       # Scalar computation
│       │   └── scalar_compute.h
│       ├── vector_datamove/      # Vector data movement
│       │   └── vector_datamove.h
│       ├── cube_compute/         # Cube computation
│       │   └── cube_compute.h
│       ├── cube_datamove/        # Cube data movement
│       │   └── cube_datamove.h
│       ├── sync/                 # Synchronization control
│       │   └── sync.h
│       ├── cache_ctrl/           # Cache control
│       │   └── cache_ctrl.h
│       ├── atomic/               # Atomic operations
│       │   └── atomic.h
│       ├── sys_var/              # System variables
│       │   └── sys_var.h
│       ├── misc/                 # Miscellaneous
│       │   └── misc.h
│       ├── utils/                # Utilities and constants
│       │   ├── debug/
│       │   │   ├── asc_dump.h
│       │   │   ├── asc_printf.h
│       │   │   └── asc_assert.h
│       │   ├── c_api_constants.h
│       │   ├── c_api_union.h
│       │   └── c_api_utils_intf.h
│       ├── asc_simd.h            # Main entry
│       └── c_api_utils_intf.h    # Utility interface
│
├── impl/
│   └── c_api/                    # C API implementation
│       ├── instr_impl/           # Instruction implementation
│       │   ├── npu_arch_2201/    # NPU ARCH 220X architecture
│       │   │   ├── vector_compute_impl/
│       │   │   │   ├── asc_abs_impl.h
│       │   │   │   └── ...
│       │   │   ├── vector_datamove_impl/
│       │   │   │   ├── asc_copy_gm2ub_align_impl.h
│       │   │   │   └── ...
│       │   │   ├── vector_compute_impl.h
│       │   │   ├── vector_datamove_impl.h
│       │   │   └── ...
│       │   └── npu_arch_3510/    # NPU ARCH 351X architecture
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
│       └── c_api/                # C API tests
│           ├── npu_arch_2201/    # NPU ARCH 220X architecture
│           │   ├── vector_compute/
│           │   │   ├── test_add_instr.cpp
│           │   │   ├── test_axpy_instr.cpp
│           │   │   └── ...
│           │   ├── scalar_compute/
│           │   └── ...
│           ├── npu_arch_3510/    # NPU ARCH 351X architecture
│           │   └── ...
│           └── common/           # Common tests
│               └── test_utils.cpp
│
└── docs/
    └── api/
        └── context/
            └── c_api/            # C API documentation
                ├── vector_compute/
                │   ├── asc_add.md
                │   ├── asc_muls.md
                │   └── ...
                ├── scalar_compute/
                ├── vector_datamove/
                ├── general_instruction.md  # General description
                └── c_api_list.md           # API list
```

### File Organization Principles

1. **Classify by functionality**: Vector computation, scalar computation, data movement, and so on.
2. **Isolate by architecture**: Separate implementations for different NPU architectures.
3. **Separate interface and implementation**: include/ for declarations, impl/ for implementations.
4. **Organize tests by architecture**: Facilitates multi-architecture testing and maintenance.

### File Naming Conventions

- **Header file**: `<category>.h`, for example, `vector_compute.h`.
- **Implementation file**: `<category>_c_api_impl.h`, for example, `vector_compute_c_api_impl.h`.
- **Test file**: `test_<api>_instr.cpp`, for example, `test_add_instr.cpp`.
- **Documentation file**: `<api_name>.md`, for example, `asc_add.md`.

---

## Development Example: Implementing asc_axpy C API

### Requirement Analysis

Implement vector multiply-add C API: `dst = src * scalar + dst`

- Supported data types: half, float.
- Supports three invocation modes.
- Directly maps to hardware instructions.

### Interface Design

Add in `include/c_api/vector_compute/vector_compute.h`:

```cpp
// ==========asc_axpy(half/float)==========
// Continuous mode
__aicore__ inline void asc_axpy(__ubuf__ half* dst, __ubuf__ half* src, half scalar, uint32_t count);

__aicore__ inline void asc_axpy(__ubuf__ float* dst, __ubuf__ float* src, float scalar, uint32_t count);

// Configuration mode
__aicore__ inline void asc_axpy(__ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat,
    uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

__aicore__ inline void asc_axpy(__ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat,
    uint16_t dst_block_stride, uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

// Synchronous mode
__aicore__ inline void asc_axpy_sync(__ubuf__ half* dst, __ubuf__ half* src, half scalar, uint32_t count);

__aicore__ inline void asc_axpy_sync(__ubuf__ float* dst, __ubuf__ float* src, float scalar, uint32_t count);
```

### Implementation Code

Reference other interface implementations.

---

## Test and Verification Requirements

### Test Framework

C API testing uses the **gTest + MockCPP** framework:

```cpp
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"
```

### Test Types

#### Mock Testing (Instruction Mapping Verification)

Verify that C API correctly calls underlying instructions.

#### Functional Testing (Correctness Verification)

Verify API computation results are correct.

---

## Code Standards

### Naming Conventions

```cpp
// Function name: lowercase_with_underscores, asc_<operation>
void asc_add(...);
void asc_axpy(...);

// Parameter name: camelCase or snake_case
__ubuf__ half* dstTensor;
uint32_t elementCount;

// Macro definition: UPPERCASE_WITH_UNDERSCORES
#define ASC_CAPI_MAX_COUNT 4096

// Type name: PascalCase
struct AscUnaryConfig;
```

### Code Style

```cpp
// 1. Indentation: 4 spaces
// 2. Braces: K&R style
// 3. Spaces: Spaces around operators, space after comma
// 4. Comments: Doxygen style

/**
 * @brief Vector multiply-add operation
 * @param dst Destination address
 * @param src Source address
 * @param scalar Scalar value
 * @param tmp Temporary buffer
 * @param count Element count
 */
__aicore__ inline void asc_axpy(__ubuf__ half* dst, __ubuf__ half* src, half scalar, uint32_t count)
{
    // Parameter validation
    ASCENDC_ASSERT(count > 0, "count must be positive");
    ASCENDC_ASSERT(dst != nullptr && src != nullptr && tmp != nullptr,
                   "pointers cannot be null");

    // Computation logic
    uint8_t repeat = (count + 127) / 128;
    uint64_t mask0 = static_cast<uint64_t>(-1);
    uint64_t mask1 = static_cast<uint64_t>(-1);

    // Set mask
    set_vector_mask(mask1, mask0);

    // Execute instruction
    vmuls(tmp, src, scalar, repeat, 1, 1, 8, 8);
    pipe_barrier(PIPE_V);

    vadd(dst, dst, tmp, repeat, 1, 1, 1, 8, 8, 8);
    pipe_barrier(PIPE_V);
}
```

### Error Handling

```cpp
// 1. Parameter validation (Debug mode)
ASCENDC_ASSERT(count > 0, "count must be greater than 0");
ASCENDC_ASSERT(dst != nullptr, "dst cannot be nullptr");
ASCENDC_ASSERT(src != nullptr, "src cannot be nullptr");
ASCENDC_ASSERT(tmp != nullptr, "tmp cannot be nullptr");

// 2. Address alignment check
ASCENDC_ASSERT((reinterpret_cast<uintptr_t>(dst) % 32) == 0,
               "dst must be 32-byte aligned");
ASCENDC_ASSERT((reinterpret_cast<uintptr_t>(src) % 32) == 0,
               "src must be 32-byte aligned");

// 3. Range check
ASCENDC_ASSERT(count <= ASC_CAPI_MAX_COUNT,
               "count exceeds maximum supported value");

// 4. Architecture check
#if !defined(__NPU_ARCH__) || (__NPU_ARCH__ != 2201 && __NPU_ARCH__ != 3510)
    #error "Unsupported NPU architecture"
#endif
```

### Macro Guards

```cpp
// Header guard
#ifndef INCLUDE_C_API_VECTOR_COMPUTE_VECTOR_COMPUTE_H
#define INCLUDE_C_API_VECTOR_COMPUTE_VECTOR_COMPUTE_H

// Conditional compilation
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
    // NPU ARCH 2201 architecture specific code
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    // NPU ARCH 3510 architecture specific code
#endif

#endif
```

---
