# Testing Standards

## Testing Framework Overview

### Framework Type

C/C++ unit tests are built on Google Test, using its native parameterized testing capabilities. Python tests use pytest, registered via `run_python_llt_test` in CMake.

### Test Entry Points

| File | Purpose |
|------|------|
| `tests/main.cpp` | Main test entry |
| `tests/main_global.cpp` | Global test entry (used by some modules) |

### Test Registration

Tests are registered into executables for each product type via the `run_llt_test()` custom function in CMake.

---

## Test Levels

| Test Type | Purpose | Environment |
|---------|------|---------|
| Header Checker | Verify header files can be compiled independently | Host / NPU compiler |
| UT (unit test) | Verify API compilation correctness and parameter validation | Host (CPU Debug) |
| ST (system test) | Verify API functional correctness | Device (NPU, not in this repository) |
| Single operator test | Verify API end-to-end functionality in real operators | Device (NPU, not in this repository) |

---

## Test Directory Structure

```text
tests/
├── CMakeLists.txt
├── main.cpp
├── main_global.cpp
├── api/
│   ├── adv_api/                  # High-level API
│   │   ├── CMakeLists.txt
│   │   ├── math/
│   │   │   ├── axpy/
│   │   │   │   └── test_operator_axpy.cpp
│   │   │   ├── exp/
│   │   │   │   └── test_operator_exphighprecision.cpp
│   │   │   └── sin/
│   │   │       └── test_operator_sin.cpp
│   │   ├── matmul/
│   │   │   ├── test_operator_matmul_v200.cpp
│   │   │   ├── test_operator_matmul_v220.cpp
│   │   │   └── scheduler/
│   │   │       └── test_scheduler_norm.cpp
│   │   ├── normalization/
│   │   │   └── layernorm/
│   │   │       └── test_operator_layernorm.cpp
│   │   ├── reduce/
│   │   ├── tiling/
│   │   │   └── test_tiling.cpp       # General Tiling-side UT (also v310/HCCL/Matmul/Conv dedicated files)
│   │   └── ...
│   ├── basic_api/                     # Basic API
│   │   ├── ascendc_header_checker/    # Header compile check (standalone project)
│   │   │   ├── CMakeLists.txt
│   │   │   └── kernel_*.cpp
│   │   ├── ascendc_case_ascend610/    # Functional tests by product type
│   │   ├── ascendc_case_ascend910b1/
│   │   └── ...
│   ├── c_api/
│   ├── aicpu_api/
│   ├── common/               # Common test utilities
│   ├── reg_compute_api/
│   ├── simt_api/
│   ├── tensor_api/
│   └── utils/
└── python/
```

---

## Test File Naming

| Naming Pattern | Example | Description |
|---------|------|------|
| `test_operator_<api>.cpp` | `test_operator_sin.cpp` | Operator functional test |
| `test_operator_<api>_<variant>.cpp` | `test_operator_matmul_v220.cpp` | Variant-specific test |
| `test_<component>.cpp` | `test_tiling.cpp`, `test_scheduler_norm.cpp` | Non-operator test |
| `test_<api>_instr.cpp` | `test_add_instr.cpp` | C API instruction-level test (Mock) |
| `test_asc_<api>.cpp` | `test_asc_add.cpp` | C API functional test (Mock) |

---

## Kernel Function UT Writing Standards

### File Structure (Three-Part)

```cpp
// Part 1: Include headers
#include <gtest/gtest.h>
#include "kernel_operator.h"

// Part 2: Define test parameter struct and kernel function
struct AxpyTestParams {
    int32_t dataSize;
    int32_t dataBitSize;
    void (*calFunc)(uint8_t*, uint8_t*, int32_t);
    void (*goldenFunc)(uint8_t* srcGm, uint8_t* expectedGm, int32_t dataSize);
};

template <typename T, typename U>
void AxpyKernel(uint8_t* srcGm, uint8_t* dstGm, int32_t dataSize)
{
    // Kernel function implementation (see Kernel Function Implementation Template below)
}

// Golden computation: element-wise dst = src * scalar + dst per T/U type
template <typename T, typename U>
void AxpyGolden(uint8_t* srcGm, uint8_t* expectedGm, int32_t dataSize)
{
    U scalar = static_cast<U>(2);
    T* typedDst = reinterpret_cast<T*>(expectedGm);
    U* typedSrc = reinterpret_cast<U*>(srcGm);
    for (int32_t i = 0; i < dataSize; i++) {
        typedDst[i] = static_cast<T>(typedSrc[i] * scalar + typedDst[i]);
    }
}

// Part 3: Implement test class and test cases
class AxpyTestsuite : public testing::Test,
                      public testing::WithParamInterface<AxpyTestParams> {
protected:
    void SetUp() {}
    void TearDown() {}
};

INSTANTIATE_TEST_CASE_P(TEST_AXPY, AxpyTestsuite,
    ::testing::Values(
        AxpyTestParams{256, 2, AxpyKernel<half, half>, AxpyGolden<half, half>},
        AxpyTestParams{256, 4, AxpyKernel<float, float>, AxpyGolden<float, float>}
    ));

TEST_P(AxpyTestsuite, AxpyTestCase)
{
    // Test logic...
}
```

### Kernel Function Implementation Template

```cpp
template <typename T, typename U>
void AxpyKernel(uint8_t* srcGm, uint8_t* dstGm, int32_t dataSize)
{
    // 1. Initialize variables
    AscendC::TPipe tpipe;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> vecInQue;
    AscendC::TQue<AscendC::TPosition::VECOUT, 1> vecOutQue;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> vecTmpQue;
    U scalar = static_cast<U>(2);

    AscendC::GlobalTensor<U> inputGlobal;
    AscendC::GlobalTensor<T> outputGlobal;
    inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ U*>(srcGm), dataSize);
    outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);

    tpipe.InitBuffer(vecInQue, 1, dataSize * sizeof(U));
    tpipe.InitBuffer(vecOutQue, 1, dataSize * sizeof(T));
    tpipe.InitBuffer(vecTmpQue, 1, dataSize * 4 * sizeof(half));

    // 2. Copy data from GlobalMemory to LocalMemory
    AscendC::LocalTensor<U> inputLocal = vecInQue.AllocTensor<U>();
    AscendC::LocalTensor<T> outputLocal = vecOutQue.AllocTensor<T>();
    AscendC::LocalTensor<uint8_t> tmpLocal = vecTmpQue.AllocTensor<uint8_t>();

    AscendC::DataCopy(inputLocal, inputGlobal, dataSize);
    AscendC::DataCopy(outputLocal, outputGlobal, dataSize);

    // 3. Call the API under test
    AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

    AscendC::Axpy<T, U, false>(outputLocal, inputLocal, scalar, tmpLocal, dataSize);

    // 4. Copy data from LocalMemory back to GlobalMemory
    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::DataCopy(outputGlobal, outputLocal, dataSize);

    // 5. Synchronize and release
    AscendC::PipeBarrier<AscendC::PIPE_ALL>();
    vecInQue.FreeTensor(inputLocal);
    vecOutQue.FreeTensor(outputLocal);
    vecTmpQue.FreeTensor(tmpLocal);
}
```

### Test Assertions

```cpp
TEST_P(AxpyTestsuite, AxpyTestCase)
{
    auto param = GetParam();
    uint8_t srcGm[param.dataSize * param.dataBitSize] = {0};
    uint8_t dstGm[param.dataSize * param.dataBitSize] = {0};

    // Fill with non-zero, distinguishable test data
    for (int32_t i = 0; i < param.dataSize * param.dataBitSize; i++) {
        srcGm[i] = static_cast<uint8_t>(i + 1);
        dstGm[i] = static_cast<uint8_t>(i + 2);
    }

    // Before calling the API under test, save originalDst and compute golden buffer per T/U type (dst = src * scalar + dst)
    uint8_t expectedGm[param.dataSize * param.dataBitSize] = {0};
    memcpy(expectedGm, dstGm, param.dataSize * param.dataBitSize);
    param.goldenFunc(srcGm, expectedGm, param.dataSize);

    param.calFunc(srcGm, dstGm, param.dataSize);

    // Compare actual results with golden buffer element-wise
    for (int32_t i = 0; i < param.dataSize * param.dataBitSize; i++) {
        EXPECT_EQ(dstGm[i], expectedGm[i]);
    }
}
```

---

## Tiling-Side UT Writing Standards

### Registration

Tiling interface UTs are registered by `tests/api/adv_api/CMakeLists.txt`, placed in corresponding test files or subdirectories by category: general tests in `tiling/test_tiling.cpp`, arch-specific tests in `tiling/test_tiling_v310.cpp`, HCCL in `tiling/test_hccl_tiling.cpp`, Matmul in `tiling/test_matmul_api_tiling.cpp`, Conv in `tiling/conv/` and `tiling/conv_backprop/` subdirectories. When adding new Tiling UTs, place them in the corresponding file or subdirectory and ensure they are collected by the CMake GLOB.

```cpp
TEST_F(TestTiling, TestAxpyTiling)
{
    uint32_t maxVal = 0;
    uint32_t minVal = 0;

    GetAxpyMaxMinTmpSize(AscendC::TensorShape({128}), 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 0);
    EXPECT_EQ(minVal, 0);

    GetAxpyMaxMinTmpSize(AscendC::TensorShape({256}), 2, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 4 * 2);
    EXPECT_EQ(minVal, 256 * 4);
}
```

### Writing Requirements

- One `TEST_F(TestTiling, Test<Api>Tiling)` function per API
- Cover normal inputs and boundary conditions
- Use `EXPECT_EQ` to validate maxValue and minValue

---

## Header Checker

### Purpose

Verify that public header files compile correctly. Basic API generates an independent translation unit for each public header to verify it does not rely on implicit include order; high-level API compiles all public headers together in an aggregate file to verify overall compilation (independent compilation of each header is not guaranteed).

### Standalone Project

Header Checker is a standalone CMake project:

```cmake
project(HeaderChecker)
```

Location: `tests/api/basic_api/ascendc_header_checker/CMakeLists.txt`

### Four Check Modes

| Mode | Target Naming | Compiler |
|------|------------|--------|
| CPU normal | `check_cpu_${CONFIG_NAME}_${ID}` | Host compiler |
| CPU single-header | variant of the above | Host compiler |
| NPU normal | `check_npu_${CONFIG_NAME}_${ID}` | Bisheng compiler |
| NPU single-header | variant of the above | Bisheng compiler |

### New File Requirements

Header Checker integration differs by API type:

- **Basic API**: For each new public header file, add a corresponding `<header_file_base_name>.cpp` in `tests/api/basic_api/ascendc_header_checker/` (e.g., `kernel_operator_vec_binary_intf.cpp`).
- **High-level API**: Add `#include` of the public header to the aggregate file `tests/api/adv_api/api_host_check/api_host_check.cpp`.

---

## Running Tests

### Full Test Suite

```bash
bash build.sh -t
```

### Build Only a Specific Test Target

Modify the `build_test()` function in `build.sh`:

```bash
function build_test() {
    cmake_config
    build ascendc_ut_adv_api_kernel_ascend910B1_AIV     # replace with target
}
```

Then run `bash build.sh -t`.

### Single Operator Testing

After completing the kernel function code, perform end-to-end testing by setting up a custom operator project. See [Ascend C Programming Guide](https://www.hiascend.com/document/redirect/CannCommunityOpdevAscendC).

---

## CI/CD and Coverage Requirements

### Minimum Requirements

| Scenario | Requirement |
|------|------|
| New API | Must include kernel function UT |
| New API (requires temporary space) | Must include tiling-side UT |
| Bug fix | Should add regression test cases |
| New public header file | Must add Header Checker |

### Test Coverage

- **Normal paths**: Functional correctness with typical inputs
- **Boundary conditions**: minimum/maximum inputs, empty input, aligned/unaligned
- **Error inputs**: parameter validation assertion triggers

### Pre-commit Hooks

Pre-commit checks are configured in `.pre-commit-config.yaml` and mainly include the following categories:

- Basic file checks (trailing-whitespace, end-of-file-fixer, check-yaml, etc.)
- `clang-format` — C/C++ code formatting
- Ruff (check + format) — Python code checking and formatting
- `codespell` — Spell check
- `oat-check` — License compliance check
- `markdown-link-check` — Markdown link validity check

---

## Mock Testing (C API Only)

C API uses the gTest + MockCPP framework to verify instruction mapping:

```cpp
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"
```

Verify that C API correctly calls underlying hardware instructions, but does not verify computation results.
