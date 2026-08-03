# 测试规范

## 测试框架概述

### 框架类型

C/C++单元测试基于Google Test构建，使用其原生的参数化测试能力。Python测试基于pytest，通过CMake中的 `run_python_llt_test` 注册。

### 测试入口

| 文件 | 用途 |
|------|------|
| `tests/main.cpp` | 测试主入口 |
| `tests/main_global.cpp` | 全局测试入口（部分模块使用） |

### 测试注册

测试通过CMake中 `run_llt_test()` 自定义函数注册到各product type的executable中。

---

## 测试层次

| 测试类型 | 目的 | 运行环境 |
|---------|------|---------|
| Header Checker | 验证头文件可独立编译 | Host / NPU编译器 |
| UT（单元测试）| 验证API编译正确和参数校验 | Host（CPU Debug）|
| ST（系统测试）| 验证API功能正确 | Device（NPU，不在本仓内）|
| 单算子测试 | 在真实算子中验证API端到端功能 | Device（NPU，不在本仓内）|

---

## 测试目录结构

```text
tests/
├── CMakeLists.txt
├── main.cpp
├── main_global.cpp
├── api/
│   ├── adv_api/                  # 高阶API
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
│   │   │   └── test_tiling.cpp       # 通用Tiling侧UT（另含v310/HCCL/Matmul/Conv等专用文件）
│   │   └── ...
│   ├── basic_api/                     # 基础API
│   │   ├── ascendc_header_checker/    # Header编译检查（独立project）
│   │   │   ├── CMakeLists.txt
│   │   │   └── kernel_*.cpp
│   │   ├── ascendc_case_ascend610/    # 按产品类型的功能测试
│   │   ├── ascendc_case_ascend910b1/
│   │   └── ...
│   ├── c_api/
│   ├── aicpu_api/
│   ├── common/               # 公共测试工具
│   ├── reg_compute_api/
│   ├── simt_api/
│   ├── tensor_api/
│   └── utils/
└── python/
```

---

## 测试文件命名

| 命名模式 | 示例 | 说明 |
|---------|------|------|
| `test_operator_<api>.cpp` | `test_operator_sin.cpp` | 算子功能测试 |
| `test_operator_<api>_<variant>.cpp` | `test_operator_matmul_v220.cpp` | 特定变体的测试 |
| `test_<component>.cpp` | `test_tiling.cpp`、`test_scheduler_norm.cpp` | 非算子测试 |
| `test_<api>_instr.cpp` | `test_add_instr.cpp` | C API指令级测试（Mock） |
| `test_asc_<api>.cpp` | `test_asc_add.cpp` | C API功能测试（Mock） |

---

## Kernel侧UT编写规范

### 文件结构（三段式）

```cpp
// 第一部分：引入头文件
#include <gtest/gtest.h>
#include "kernel_operator.h"

// 第二部分：定义测试参数结构体和Kernel函数
struct AxpyTestParams {
    int32_t dataSize;
    int32_t dataBitSize;
    void (*calFunc)(uint8_t*, uint8_t*, int32_t);
    void (*goldenFunc)(uint8_t* srcGm, uint8_t* expectedGm, int32_t dataSize);
};

template <typename T, typename U>
void AxpyKernel(uint8_t* srcGm, uint8_t* dstGm, int32_t dataSize)
{
    // Kernel实现（见下方Kernel函数实现模板）
}

// 期望值计算：按T/U类型逐元素执行dst = src * scalar + dst
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

// 第三部分：实现测试类和测试用例
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
    // 测试逻辑...
}
```

### Kernel函数实现模板

```cpp
template <typename T, typename U>
void AxpyKernel(uint8_t* srcGm, uint8_t* dstGm, int32_t dataSize)
{
    // 1. 初始化变量
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

    // 2. 数据从GlobalMemory拷贝到LocalMemory
    AscendC::LocalTensor<U> inputLocal = vecInQue.AllocTensor<U>();
    AscendC::LocalTensor<T> outputLocal = vecOutQue.AllocTensor<T>();
    AscendC::LocalTensor<uint8_t> tmpLocal = vecTmpQue.AllocTensor<uint8_t>();

    AscendC::DataCopy(inputLocal, inputGlobal, dataSize);
    AscendC::DataCopy(outputLocal, outputGlobal, dataSize);

    // 3. 调用被测API
    AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

    AscendC::Axpy<T, U, false>(outputLocal, inputLocal, scalar, tmpLocal, dataSize);

    // 4. 数据从LocalMemory拷贝回GlobalMemory
    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::DataCopy(outputGlobal, outputLocal, dataSize);

    // 5. 同步并释放
    AscendC::PipeBarrier<AscendC::PIPE_ALL>();
    vecInQue.FreeTensor(inputLocal);
    vecOutQue.FreeTensor(outputLocal);
    vecTmpQue.FreeTensor(tmpLocal);
}
```

### 测试断言

```cpp
TEST_P(AxpyTestsuite, AxpyTestCase)
{
    auto param = GetParam();
    uint8_t srcGm[param.dataSize * param.dataBitSize] = {0};
    uint8_t dstGm[param.dataSize * param.dataBitSize] = {0};

    // 填充非零且可区分的测试数据
    for (int32_t i = 0; i < param.dataSize * param.dataBitSize; i++) {
        srcGm[i] = static_cast<uint8_t>(i + 1);
        dstGm[i] = static_cast<uint8_t>(i + 2);
    }

    // 在调用被测函数前，保存originalDst并按T/U类型计算golden buffer（dst = src * scalar + dst）
    uint8_t expectedGm[param.dataSize * param.dataBitSize] = {0};
    memcpy(expectedGm, dstGm, param.dataSize * param.dataBitSize);
    param.goldenFunc(srcGm, expectedGm, param.dataSize);

    param.calFunc(srcGm, dstGm, param.dataSize);

    // 逐元素比较实际结果与golden buffer
    for (int32_t i = 0; i < param.dataSize * param.dataBitSize; i++) {
        EXPECT_EQ(dstGm[i], expectedGm[i]);
    }
}
```

---

## Tiling侧UT编写规范

### 注册方式

Tiling接口的UT由 `tests/api/adv_api/CMakeLists.txt` 注册，按类别放入对应测试文件或子目录：通用测试在 `tiling/test_tiling.cpp`，架构特定测试在 `tiling/test_tiling_v310.cpp`，HCCL在 `tiling/test_hccl_tiling.cpp`，Matmul在 `tiling/test_matmul_api_tiling.cpp`，Conv类在 `tiling/conv/` 和 `tiling/conv_backprop/` 子目录。新增Tiling UT时放入对应类别的文件或子目录，并确保被CMake GLOB收集。

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

### 编写要求

- 每个API一个 `TEST_F(TestTiling, Test<Api>Tiling)` 函数
- 覆盖正常输入和边界条件
- `EXPECT_EQ` 校验maxValue和minValue

---

## Header Checker

### 目的

验证公开头文件可正常编译。基础API为每个公开头文件生成独立翻译单元，验证其不依赖隐式的include顺序；高阶API将所有公开头集中到聚合文件中编译，验证整体可编译性（不保证每个头独立编译）。

### 独立项目

Header Checker是一个独立的CMake项目：

```cmake
project(HeaderChecker)
```

位置：`tests/api/basic_api/ascendc_header_checker/CMakeLists.txt`

### 四种检查模式

| 模式 | Target命名 | 编译器 |
|------|------------|--------|
| CPU normal | `check_cpu_${CONFIG_NAME}_${ID}` | Host编译器 |
| CPU single-header | 同上变体 | Host编译器 |
| NPU normal | `check_npu_${CONFIG_NAME}_${ID}` | Bisheng编译器 |
| NPU single-header | 同上变体 | Bisheng编译器 |

### 新增要求

Header Checker的接入方式因API类型而异：

- **基础API**：每新增一个公开头文件，在 `tests/api/basic_api/ascendc_header_checker/` 下添加对应的 `<头文件_base_name>.cpp`（如 `kernel_operator_vec_binary_intf.cpp`）。
- **高阶API**：在 `tests/api/adv_api/api_host_check/api_host_check.cpp` 聚合文件中添加 `#include`。

---

## 运行测试

### 全量测试

```bash
bash build.sh -t
```

### 仅编译特定test target

修改 `build.sh` 中 `build_test()` 函数：

```bash
function build_test() {
    cmake_config
    build ascendc_ut_adv_api_kernel_ascend910B1_AIV     # 替换为目标target
}
```

然后执行 `bash build.sh -t`。

### 单算子测试

完成Kernel侧代码后，通过搭建自定义算子工程进行端到端测试，详见 [Ascend C编程指南](https://www.hiascend.com/document/redirect/CannCommunityOpdevAscendC)。

---

## CI/CD与覆盖率要求

### 最低要求

| 场景 | 要求 |
|------|------|
| 新增API | 必须包含Kernel侧UT |
| 新增API（需要临时空间）| 必须包含Tiling侧UT |
| Bug修复 | 应补充回归测试用例 |
| 新增公开头文件 | 必须添加Header Checker |

### 测试覆盖

- **正常路径**：典型输入下的功能正确性
- **边界条件**：最小/最大输入、空输入、对齐/非对齐
- **错误输入**：参数校验断言触发

### Pre-commit钩子

提交前自动运行的检查以 `.pre-commit-config.yaml` 配置为准，主要包括以下类别：

- 基础文件检查（trailing-whitespace、end-of-file-fixer、check-yaml等）
- `clang-format` — C/C++代码格式化
- Ruff（check + format）— Python代码检查与格式化
- `codespell` — 拼写检查
- `oat-check` — 许可证合规检查
- `markdown-link-check` — Markdown链接有效性检查

---

## Mock测试（C API专用）

C API使用gTest + MockCPP框架验证指令映射：

```cpp
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "c_api/stub/cce_stub.h"
#include "c_api/asc_simd.h"
```

验证C API是否正确调用了底层硬件指令，但不验证计算结果。
