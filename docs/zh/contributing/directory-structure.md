# 目录结构与文件命名规范

## 顶层目录总览

```text
asc-devkit/
├── include/                # 公开API声明(.h)
├── impl/                   # 内部实现(.h + .cpp)
├── tests/                  # 测试代码
├── examples/               # 示例代码(.asc + .cpp)
├── docs/                   # 文档
├── tools/                  # 构建工具
├── cmake/                  # CMake模块和宏
├── scripts/                # 辅助脚本
├── build.sh                # 构建入口脚本
├── CMakeLists.txt          # 根构建文件
├── .clang-format           # C++格式化配置
├── .pre-commit-config.yaml # Pre-commit钩子
├── CONTRIBUTING.md         # 贡献总流程
└── README.md               # 项目说明
```

---

## include/↔impl/镜像关系

`include/` 和 `impl/` 顶层目录结构一一对应：

| include/ | impl/ | 说明 |
|----------|-------|------|
| `include/basic_api/` | `impl/basic_api/` | 基础API（kernel operator接口与结构体） |
| `include/adv_api/` | `impl/adv_api/` | 高阶API（数学库、归一化、矩阵乘等） |
| `include/c_api/` | `impl/c_api/` | C API（指令层封装） |
| `include/simt_api/` | `impl/simt_api/` | SIMT API |
| `include/tensor_api/` | `impl/tensor_api/` | Tensor API |
| `include/utils/` | `impl/utils/` | 工具类 |
| `include/aicpu_api/` | `impl/aicpu_api/` | AI CPU API |

### 详细映射规则

| include路径 | impl路径 | 说明 |
|-------------|----------|------|
| `include/basic_api/xxx.h` | 无对应 `xxx_impl.h`（如 `kernel_common.h`）；部分头文件有对应 `xxx_impl.h`（如 `kernel_tensor.h` → `kernel_tensor_impl.h`） | 自包含的仅有少数基础工具头 |
| `include/basic_api/xxx_intf.h` | `impl/basic_api/xxx_intf_impl.h` | 接口 → 架构分发实现 |
| `include/adv_api/math/xxx.h` | `impl/adv_api/detail/math/xxx/xxx_common_impl.h` | |
| `include/adv_api/math/xxx_tiling.h` | `impl/adv_api/tiling/math/xxx_tiling_impl.cpp` | |
| `include/adv_api/<cat>/xxx.h` | `impl/adv_api/detail/<cat>/xxx/xxx_common_impl.h` | |
| `include/c_api/<category>/<category>.h` | `impl/c_api/instr_impl/npu_arch_<NNNN>/<category>_impl/asc_<api>_impl.h` | C API使用 `npu_arch_` 前缀目录 |

---

## 文件命名规范

### 后缀约定

| 后缀 | 含义 | 所在目录 |
|------|------|---------|
| `_intf.h` | 接口声明（interface） | `include/` |
| `_impl.h` | 实现定义（implementation） | `impl/` |
| `_intf_impl.h` | 接口+架构分发实现 | `impl/` |
| `_tiling.h` | Tiling接口声明 | `include/` |
| `_tiling_intf.h` | Tiling交互接口声明 | `include/` |
| `_tiling_impl.cpp` | Tiling宿主侧实现 | `impl/adv_api/tiling/` |
| `_tiling.cpp` | Tiling实现（无 `_impl` 后缀的变体） | `impl/adv_api/tiling/` |
| `_struct_*.h` | 类型/结构体定义 | `include/` |
| `_common_impl.h` | 通用算法实现 | `impl/adv_api/detail/` |
| `_check.h` | 参数校验逻辑 | `impl/adv_api/detail/api_check/` |
| `_config.h` | 配置参数定义 | `include/` 或 `impl/` |
| `_utils.h` | 工具函数 | `include/` 或 `impl/` |

### 命名规则

- 源码文件和自定义目录使用 **snake_case**（小写字母+下划线）
- 无空格、无连字符、无大写
- 英文单词，避免缩写（除非是广泛认可的缩写如 `vec`、`intf`）
- 例外：工具约定文件如 `CMakeLists.txt`、`README.md`、`CONTRIBUTING.md` 保持各自标准命名

**正确示例：**

```text
kernel_operator_vec_binary_intf.h
kernel_struct_fixpipe.h
kernel_operator_common_intf_impl.h
axpy_common_impl.h
exp_check_common.h
```

### 不同API类型的命名差异

| API类型 | 头文件 | 实现文件 | 示例 |
|---------|--------|---------|------|
| 基础API | `kernel_<name>_intf.h` | 分发层：`kernel_<name>_intf_impl.h`；架构实现：`dav_<arch>/kernel_<name>_impl.h` | `kernel_operator_vec_binary_intf.h` |
| 高阶API | `<name>.h` | `<name>_common_impl.h` | `axpy.h` / `axpy_common_impl.h` |
| C API | `<category>.h` | `asc_<api>_impl.h` | `vector_compute.h` / `asc_add_impl.h` |
| Tiling | `<name>_tiling.h` | `<name>_tiling_impl.cpp` | `axpy_tiling.h` |

---

## 架构目录命名

### 命名格式

基础API的架构特定实现放在 `impl/basic_api/dav_<code>/` 目录下，格式为 `dav_`+架构代码。高阶API的架构特定实现位于 `impl/adv_api/detail/<category>/<api>/<api>_<arch>_impl.h`，由公开头文件通过 `#if __NPU_ARCH__` 分发。C API使用 `npu_arch_<NNNN>/` 前缀目录，详见上文C API映射表。

### 架构代码映射（示例）

以下列出部分常见架构目录，完整列表请查看 `impl/basic_api/dav_*/` 目录：

| 目录 | `__NPU_ARCH__` 值 | 对应产品 |
|------|-------------------|---------|
| `dav_c220` | `2201` | Atlas A2 训练系列产品/Atlas A2 推理系列产品、Atlas A3 训练系列产品/Atlas A3 推理系列产品 |
| `dav_3510` | `3510` | Ascend 950PR/Ascend 950DT |

> 产品型号与 `__NPU_ARCH__` 的完整对应关系见 [**NPU_ARCH** 文档](../../zh/guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

### 架构分发模式

在impl头文件中使用 `#if / #elif / #endif` 链按架构选择实现：

```cpp
#if __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_common_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_common_impl.h"
#endif
```

---

## impl/adv_api内部组织

```text
impl/adv_api/
├── CMakeLists.txt
├── detail/                   # 核心实现
│   ├── activation/           # 激活函数
│   ├── api_check/            # 参数校验
│   │   ├── host_apicheck.h
│   │   ├── kernel_api_check.h
│   │   └── kernel_check/     # 每个API的校验逻辑
│   │       └── math/
│   │           └── exp/
│   │               └── exp_check_common.h
│   ├── common/               # 公共工具
│   ├── math/                 # 数学运算（50+子目录）
│   │   └── exp/
│   │       ├── exp_common_impl.h
│   │       └── exp_3510_impl.h
│   ├── matmul/               # 矩阵乘
│   ├── normalization/        # 归一化
│   ├── reduce/               # 归约
│   └── ...
└── tiling/                   # Tiling（宿主侧内存规划）
    ├── CMakeLists.txt
    └── math/
        └── exp_tiling_impl.cpp
```

---

## 新增API操作清单

新增一个完整的API时，按以下清单创建和修改文件：

### 高阶API清单

```text
□ include/adv_api/<category>/<api>.h
    公开头文件，包含Doxygen文档、模板声明、到impl的转发

□ include/adv_api/<category>/<api>_tiling.h
    Tiling接口声明（如API需要临时空间）

□ impl/adv_api/detail/<category>/<api>/<api>_common_impl.h和/或<api>_<arch>_impl.h
    核心算法实现（通用实现用_common_impl.h，架构专用实现用<api>_<arch>_impl.h）

□ impl/adv_api/detail/api_check/kernel_check/<category>/<api>/<api>_check.h
    参数校验逻辑（CHECK_FUNC_HIGHLEVEL_API依赖）

□ impl/adv_api/tiling/<category>/<api>_tiling_impl.cpp
    Tiling接口实现（如需要）

□ impl/adv_api/tiling/CMakeLists.txt
    在tiling_api的add_library中添加.cpp

□ impl/adv_api/detail/<category>/CMakeLists.txt（如有）
    添加子目录add_subdirectory(<api>)

□ include/adv_api/kernel_api.h
    添加#include "<category>/<api>.h"

□ include/adv_api/tiling_api.h
    添加#include "<category>/<api>_tiling.h"

□ tests/api/adv_api/<category>/<api>/test_operator_<api>.cpp
    功能测试

□ tests/api/adv_api/api_check/<category>/<api>/kernel_<api>_check.cpp
    参数校验UT（CPU Debug下CHECK_FUNC_HIGHLEVEL_API检查）

□ tests/api/adv_api/tiling/test_tiling.cpp（或对应类别文件）
    按类别选择测试文件：通用用test_tiling.cpp，架构专用用test_tiling_v310.cpp，HCCL用test_hccl_tiling.cpp，Matmul用test_matmul_api_tiling.cpp，Conv用tiling/conv/或tiling/conv_backprop/

□ tests/api/adv_api/CMakeLists.txt
    在对应product type的GLOB中添加测试文件

□ tests/api/adv_api/api_host_check/api_host_check.cpp
    在高阶API host checker聚合文件中加入公开头 `#include`（聚合编译检查）
```

### 基础API清单

```text
□ include/basic_api/kernel_operator_<name>_intf.h
    接口声明

□ impl/basic_api/kernel_operator_<name>_intf_impl.h
    接口+架构分发实现

□ impl/basic_api/dav_<arch>/kernel_operator_<name>_impl.h
    每个架构的实现文件

□ tests/api/basic_api/ascendc_case_<product>/test_operator_<name>.cpp
    功能测试（按产品放置，跨产品用例放ascendc_case_common）

□ include/kernel_operator.h（或其他总入口）
    添加include

□ tests/api/basic_api/ascendc_header_checker/kernel_operator_<name>_intf.cpp
    Header Checker编译检查文件

```

### C API清单

```text
□ include/c_api/<category>/<category>.h或添加函数声明到现有文件

□ impl/c_api/instr_impl/npu_arch_<NNNN>/<category>_impl/asc_<api>_impl.h
    架构特定实现

□ impl/c_api/instr_impl/npu_arch_<NNNN>/<category>_impl.h
    在聚合头中添加#include "<category>_impl/asc_<api>_impl.h"
    （新增category时还需在include/c_api/asc_simd.h中注册公开头）

□ tests/api/c_api/npu_arch_<arch>/<category>/test_asc_<api>.cpp
    Mock测试

□ tests/api/c_api/CMakeLists.txt
    新增category时在对应product type的GLOB中添加测试目录

□ docs/zh/api/SIMD-API/C-API/<category>/asc_<api>.md
    API文档
```

---

## examples/目录组织

### 目录结构

```text
examples/
├── 01_simd_cpp_api/        # SIMD C++ API示例
│   ├── 00_introduction/    # 入门示例
│   ├── 01_utilities/       # 工具使用（printf、assert等）
│   ├── 02_features/        # 高级特性
│   ├── 03_basic_api/       # 基础API示例
│   │   └── 00_data_movement/
│   │       └── copy_ub2ub/ # UB到UB拷贝示例
│   │           ├── CMakeLists.txt
│   │           ├── copy.asc         # Kernel源码（.asc后缀）
│   │           ├── data_utils.h     # 测试数据工具
│   │           └── README.md
│   ├── 04_advanced_api/    # 高阶API示例
│   ├── 05_best_practices/  # 最佳实践
│   └── 06_compatibility_guide/  # 兼容性指南
├── 02_simd_c_api/          # SIMD C API示例
├── 03_simt_api/            # SIMT API示例
├── 04_aicpu/               # AI CPU API示例
├── 05_simd_simt_hybrid/    # SIMD/SIMT混合编程示例
├── README.md
└── README_en.md
```

### 命名规则

- 典型目录组织：`<api_type>/<category>/<api_name>/`，部分类别有更深层级
- 数字前缀确保排序：`00_introduction/`、`01_utilities/`
- Kernel源码文件使用 `.asc` 扩展名
- 每个示例必须有独立的 `CMakeLists.txt`
- 每个示例推荐包含 `README.md`

---

## 测试文件命名

测试目录结构和文件命名规范见[testing.md](testing.md)。
