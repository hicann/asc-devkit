# Basic API Contribution Guide

## Overview

Basic API is the instruction-level API layer in the Ascend C programming framework. It directly wraps hardware instructions of Ascend AI processors and uses C++ style function interfaces. Basic API serves as the foundation for building high-level APIs. Developers can implement complex algorithm logic by combining basic APIs.

**Core Features of Basic API:**

- **Instruction-level encapsulation**: Each API maps to one or more hardware instructions.
- **LocalTensor abstraction**: Uses `LocalTensor<T>` type to operate memory.
- **Template design**: Supports multiple data types (half, float, int16_t, int32_t, and so on).
- **Dual interfaces**: High-dimensional tiling computation (fine control) and first-n elements computation (simplified invocation).
- **Architecture adaptation**: Supports different NPU architectures through architecture macro definitions.

---

## Development Process

### Requirement Analysis

- Define API functionality (for example, Add, Mul, Relu).
- Determine supported data types.
- Analyze hardware instruction support.

### API Design

- Define function prototypes (using LocalTensor).
- Design high-dimensional tiling computation and first-n elements computation interfaces.
- Define parameter specifications (mask, repeat, stride, and so on).

### Implementation Development

- Write interface declarations (include/basic_api/).
- Implement core logic (impl/basic_api/).
- Handle architecture differences.

### Test and Verification

- Write unit tests.
- Verify functional correctness.
- Check boundary conditions.

### Documentation

- Complete API documentation.
- Provide usage examples.
- Explain constraints.

---

## API Introduction

### High-dimensional Tiling Computation vs First-n Elements Computation Interface

#### High-dimensional Tiling Computation (Fine Control)

```cpp
// Requires manual setting of mask and repeat parameters
template <typename T, bool isSetMask = true>
__aicore__ inline void Add(const LocalTensor<T>& dst,
                           const LocalTensor<T>& src0,
                           const LocalTensor<T>& src1,
                           uint64_t mask[],           // mask array
                           const uint8_t repeatTime,  // repeat count
                           const BinaryRepeatParams& repeatParams);  // stride parameters
```

**Applicable Scenarios:**

- Require fine control over computation process.
- Non-contiguous memory access.
- Performance optimization.

#### First-n Elements Computation (Simplified Invocation)

```cpp
// Automatically handles mask and repeat
template <typename T>
__aicore__ inline void Add(const LocalTensor<T>& dst,
                           const LocalTensor<T>& src0,
                           const LocalTensor<T>& src1,
                           const int32_t& count);  // only element count needed
```

**Applicable Scenarios:**

- Contiguous memory block computation.
- Simplified code.
- Rapid development.

---

## Directory Planning

### Directory Structure

```text
asc-devkit/
├── include/
│   └── basic_api/                    # Basic API header files
│       ├── kernel_operator_common_intf.h        # Common interface
│       ├── kernel_operator_vec_binary_intf.h    # Vector binary operations
│       ├── kernel_operator_vec_unary_intf.h     # Vector unary operations
│       ├── kernel_operator_data_copy_intf.h     # Data movement
│       ├── kernel_operator_fixpipe_intf.h       # Fixpipe
│       ├── kernel_operator_mm_intf.h            # Matrix multiplication
│       ├── kernel_operator_scalar_intf.h        # Scalar operations
│       ├── kernel_operator_sys_var_intf.h       # System variables
│       ├── kernel_operator_atomic_intf.h        # Atomic operations
│       ├── kernel_tensor.h                      # Tensor definition
│       └── kernel_struct_*.h                    # Parameter structures
│
├── impl/
│   └── basic_api/                    # Basic API implementation
│       ├── dav_m200/                 # NPU ARCH 200x architecture
│       │   ├── kernel_operator_vec_binary_impl.h
│       │   └── ...
│       ├── dav_c220/                 # NPU ARCH 220x architecture
│       │   ├── kernel_operator_vec_binary_impl.h
│       │   └── ...
│       └── CMakeLists.txt
│
├── tests/
│   └── api/
│       └── basic_api/                # Basic API tests
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
            └── ...              # Basic API documentation
```

### File Naming Conventions

| File Type | Naming Convention | Example |
| --------- | --------- | ------ |
| Interface header | `kernel_operator_<category>_intf.h` | `kernel_operator_vec_binary_intf.h` |
| Implementation file | `kernel_operator_<category>_impl.h` | `kernel_operator_vec_binary_impl.h` |
| Test file | `test_operator_<category>.cpp` | `test_operator_vec_binary.cpp` |
| Documentation file | `<api>.md` | `Add.md` |

### API Categories

| Category | Description | Example APIs |
| ------ | ------ | --------- |
| vec_binary | Vector binary operations | Add, Sub, Mul, Div, Max, Min |
| vec_unary | Vector unary operations | Relu, Exp, Cast, Abs |
| vec_reduce | Vector reduction | Sum, Max, Mean |
| data_copy | Data movement | DataCopy, LoadData |
| fixpipe | Pipeline control | Fixpipe |
| mm | Matrix multiplication | Mmad, Conv2D |
| scalar | Scalar operations | ToFloat |
| atomic | Atomic operations | AtomicAdd, AtomicCAS |

---

## Architecture Design

### Implementation Layers

#### Layer 1: Interface Declaration Layer (include/basic_api/)

```cpp
// include/basic_api/kernel_operator_vec_binary_intf.h
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BINARY_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_VEC_BINARY_INTERFACE_H

#include "kernel_tensor.h"
#include "kernel_struct_binary.h"

namespace AscendC {

// Add - High-dimensional tiling computation
template <typename T, bool isSetMask = true>
__aicore__ inline void Add(const LocalTensor<T>& dst,
                           const LocalTensor<T>& src0,
                           const LocalTensor<T>& src1,
                           uint64_t mask[],
                           const uint8_t repeatTime,
                           const BinaryRepeatParams& repeatParams);

// Add - First-n elements computation
template <typename T>
__aicore__ inline void Add(const LocalTensor<T>& dst,
                           const LocalTensor<T>& src0,
                           const LocalTensor<T>& src1,
                           const int32_t& count);

}  // namespace AscendC

#include "impl/basic_api/kernel_operator_vec_binary_intf_impl.h"
#endif
```

#### Layer 2: Instruction Implementation Layer (impl/basic_api/)

```cpp
// impl/basic_api/dav_c220/kernel_operator_vec_binary_impl.h
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BINARY_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BINARY_IMPL_H

namespace AscendC {

// Add implementation - First-n elements computation
template <typename T>
__aicore__ inline void AddImpl(__ubuf__ T* dst, __ubuf__ T* src0,
                               __ubuf__ T* src1, const int32_t& count) {
    if ASCEND_IS_AIV {
        // 1. Set mask
        set_mask_count();
        set_vector_mask(0, count);

        // 2. Call underlying instruction
        vadd(dst, src0, src1, 1,
             DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE,
             DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE);

        // 3. Restore mask
        set_mask_norm();
        set_vector_mask(static_cast<uint64_t>(-1), static_cast<uint64_t>(-1));
    }
}

// Add implementation - High-dimensional tiling computation
template <typename T, bool isSetMask = true>
__aicore__ inline void AddImpl(__ubuf__ T* dst, __ubuf__ T* src0,
                               __ubuf__ T* src1, const uint64_t mask[],
                               const uint8_t repeatTime,
                               const BinaryRepeatParams& repeatParams) {
    if ASCEND_IS_AIV {
        // Set mask (if needed)
        if (isSetMask) {
            AscendCUtils::SetMask<T, isSetMask>(mask[1], mask[0]);
        }

        // Call underlying instruction
        vadd(dst, src0, src1, repeatTime,
             repeatParams.dstBlkStride, repeatParams.src0BlkStride, repeatParams.src1BlkStride,
             repeatParams.dstRepStride, repeatParams.src0RepStride, repeatParams.src1RepStride);
    }
}

}  // namespace AscendC

#endif
```

#### Layer 3: Interface Wrapper Layer

```cpp
// impl/basic_api/kernel_operator_vec_binary_intf_impl.h
namespace AscendC {

// First-n elements computation interface wrapper
template <typename T>
__aicore__ inline void Add(const LocalTensor<T>& dst,
                           const LocalTensor<T>& src0,
                           const LocalTensor<T>& src1,
                           const int32_t& count) {
    AddImpl<T>(dst.GetPtr(), src0.GetPtr(), src1.GetPtr(), count);
}

// High-dimensional tiling computation interface wrapper
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

### Architecture Adaptation

Hardware may differ across NPU architectures and requires reimplementation.

---

## Development Example: Implementing Axpy Basic API

### API Requirement Analysis

Implement vector multiply-add: `dst = src * scalar + dst`

- Supported data types: half, float
- Interface type: First-n elements computation (simplified invocation)
- Hardware support: Confirm hardware support

### Review Existing API Structure

Basic API uses `LocalTensor<T>` as parameters. The first-n elements computation interface only requires the count parameter:

```cpp
// Reference existing Add interface
template <typename T>
__aicore__ inline void Add(const LocalTensor<T>& dst,
                           const LocalTensor<T>& src0,
                           const LocalTensor<T>& src1,
                           const int32_t& count);
```

### Interface Design

Add in `include/basic_api/kernel_operator_vec_binary_intf.h`:

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

### Implementation Code

Reference other interface implementations.

### Interface Wrapper

Add in `impl/basic_api/kernel_operator_vec_binary_intf_impl.h`:

```cpp
template <typename T, typename U>
__aicore__ inline void Axpy(const LocalTensor<T>& dst,
                           const LocalTensor<U>& src,
                           const U scalar,
                           const int32_t& count) {
    AxpyImpl<T, U>(dst.GetPtr(), src.GetPtr(), scalar, count);
}
```

### Test Code

Add test code for the corresponding interface.

---

## Test and Verification Requirements

### Functional Testing

Verify API computation correctness.

### Boundary Testing

```cpp
TEST_F(TestAxpy, BoundaryTest) {
    // Test boundary values: count=0, 1, 256, 257
    // Test different data type combinations
    // Test special values (NaN, Inf)
}
```

### Data Type Testing

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

## Code Standards

### Naming Conventions

```cpp
// Function name: PascalCase, first letter uppercase
void Add(...);
void Relu(...);
void Axpy(...);

// Parameter name: camelCase
LocalTensor<T> dstTensor;
int32_t elementCount;

// Macro definition: UPPERCASE_WITH_UNDERSCORES
#define ASCENDC_ASSERT(cond, msg) ...

// Type name: PascalCase
struct BinaryRepeatParams;
class LocalTensor;
```

### Code Style

```cpp
// 1. Indentation: 4 spaces
// 2. Braces: K&R style
// 3. Spaces: Spaces around operators
// 4. Comments: Doxygen style

/**
 * @brief Vector addition operation
 * @param dst Destination LocalTensor
 * @param src0 Source LocalTensor 0
 * @param src1 Source LocalTensor 1
 * @param count Element count
 */
template <typename T>
__aicore__ inline void Add(const LocalTensor<T>& dst,
                           const LocalTensor<T>& src0,
                           const LocalTensor<T>& src1,
                           const int32_t& count) {
    // Parameter validation
    ASCENDC_ASSERT(count > 0, "count must be positive");

    // Call implementation
    AddImpl<T>(dst.GetPtr(), src0.GetPtr(), src1.GetPtr(), count);
}
```

### Error Handling

```cpp
// 1. Parameter validation (Debug mode)
ASCENDC_ASSERT(count > 0, "count must be greater than 0");
ASCENDC_ASSERT(dst != nullptr, "dst cannot be nullptr");
ASCENDC_ASSERT(src != nullptr, "src cannot be nullptr");

// 2. Type checking
static_assert(SupportType<T, half, float, int16_t, int32_t>(),
              "Unsupported data type");

// 3. Architecture checking
#if !defined(__NPU_ARCH__) || (__NPU_ARCH__ != 2201 && __NPU_ARCH__ != 3510)
    #error "Unsupported NPU architecture"
#endif
```

---