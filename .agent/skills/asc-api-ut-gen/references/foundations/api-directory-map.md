# API 目录映射表

本表基于当前仓内 `include/`、`impl/`、`tests/api/` 的实际目录扫描整理，用于生成或补齐 UT 前定位 API 类别、实现目录和测试目录。新增 API 时先按本表定位，再回到目标 API 的声明、实现、已有同类 UT 和 `CMakeLists.txt` 确认具体文件名与构建目标。

## 使用规则

- 不要只按 API 名推断测试目录；先确认 API 声明所在目录，再映射到实现目录和 UT 目录。
- `tests/api/common/`、`tests/api/*/stub/`、`tests/api/basic_api/common/`、`tests/api/simt_api/common_simt/` 是测试支撑目录，不是独立 API 类别。
- `tests/api/basic_api/ascendc_header_checker/` 是头文件编译检查目录，不是普通功能 UT 目录。
- `include/basic_api/reg_compute/` 的 UT 目录是 `tests/api/reg_compute_api/`，不要放进 `tests/api/basic_api/`。

## 顶层映射

| API 类别 | 声明目录 | 实现目录 | UT 测试目录 | 说明 |
|---------|----------|----------|-------------|------|
| 高阶 API (`adv`) | `include/adv_api/` | `impl/adv_api/detail/`, `impl/adv_api/tiling/` | `tests/api/adv_api/` | 覆盖算子功能 UT、tiling UT 和 `api_check` UT。`impl/adv_api/cmake/` 是构建辅助目录，不作为 API 实现目录。 |
| 高阶 API 参数/合法性检查 | `include/adv_api/` | `impl/adv_api/detail/api_check/kernel_check/` | `tests/api/adv_api/api_check/` | 属于高阶 API 的检查分支，目录结构与功能 UT 并行。 |
| membase 基础 API - AIV/AIC/common/framework (`aiv`, `aic`) | `include/basic_api/`, `include/basic_api/core_mng/`, `include/basic_api/op_frame/` | `impl/basic_api/dav_*`, `impl/basic_api/utils/` | `tests/api/basic_api/ascendc_case_*` | 排除 `include/basic_api/reg_compute/`。按芯片、AIV/AIC/framework/basic 子目录拆分。 |
| regbase 基础 API (`reg`) | `include/basic_api/reg_compute/` | `impl/basic_api/reg_compute/` | `tests/api/reg_compute_api/` | 当前有 `ascendc_case_ascend950pr_9599_reg_compute/`。 |
| C API (`c_api`) | `include/c_api/` | `impl/c_api/memory_base_impl/`, `impl/c_api/reg_base_impl/` | `tests/api/c_api/npu_arch_2201/`, `tests/api/c_api/npu_arch_3510/` | AIC/AIV 由 CMake 按产品目标拆分；`tests/api/c_api/stub/` 为支撑目录。 |
| C API register 子类 | `include/c_api/reg_compute/` | `impl/c_api/reg_base_impl/reg_arith_intf_impl.h`, `impl/c_api/reg_base_impl/npu_arch_3510/reg_convert_intf_impl.h`, 现用接口与废弃接口按重载分别归属 | `tests/api/c_api/npu_arch_3510/vector_datamove/`, `tests/api/c_api/npu_arch_3510/vector_compute/`；2201 现有 reg 相关用例在 `tests/api/c_api/npu_arch_2201/vector_compute/` | 这是 C API 内部 register 风格接口，不等同于 `tests/api/reg_compute_api/`。 |
| SIMT API (`simt`) | `include/simt_api/`, `include/simt_api/cpp/` | `impl/simt_api/cpp/dav_3510/` | `tests/api/simt_api/ascendc_case_ascend950pr_9599_simt/`, `tests/api/simt_api/math*` | `common_simt/` 是共用测试支撑目录。默认 CMake 初始化产品为 `ascend950pr_9599`。 |
| 工具类 API (`utils`) | `include/utils/` | `impl/utils/` | `tests/api/utils/` | 包含 context、debug、std、tiling、stub 等子类；`tests/api/utils/std/` 和 `tests/api/utils/tiling/` 有独立 CMake。 |
| AICPU API | `include/aicpu_api/` | `impl/aicpu_api/` | `tests/api/aicpu_api/` | 独立于当前 `asc-api-ut-gen` 默认 API 类型。 |

## 高阶 API 子目录映射

| 子类 | 声明目录 | 实现目录 | UT 目录 | 备注 |
|-----|----------|----------|---------|------|
| activation | `include/adv_api/activation/` | `impl/adv_api/detail/activation/`, `impl/adv_api/detail/api_check/kernel_check/activation/` | `tests/api/adv_api/activation/`, `tests/api/adv_api/api_check/activation/` | softmax 还含 membase/regbase 细分实现。 |
| conv | `include/adv_api/conv/` | `impl/adv_api/detail/conv/`, `impl/adv_api/tiling/conv/` | `tests/api/adv_api/tiling/conv/` | 当前扫描未发现 `tests/api/adv_api/conv/` 功能 UT 目录。 |
| conv_backprop | `include/adv_api/conv_backprop/` | `impl/adv_api/detail/conv_backprop/`, `impl/adv_api/tiling/conv_backprop/` | `tests/api/adv_api/tiling/conv_backprop/` | 当前扫描未发现 `tests/api/adv_api/conv_backprop/` 功能 UT 目录。 |
| cube_group | `include/adv_api/cube_group/` | 未发现独立 `impl/adv_api/detail/cube_group/` | 未发现独立 UT 目录 | 生成 UT 前需回查调用方或同类 API。 |
| filter | `include/adv_api/filter/` | `impl/adv_api/detail/filter/`, `impl/adv_api/detail/api_check/kernel_check/filter/`, `impl/adv_api/tiling/filter/` | `tests/api/adv_api/filter/`, `tests/api/adv_api/api_check/filter/` | `api_check` 下当前目录名为 `droupout`。 |
| hccl | `include/adv_api/hccl/` | `impl/adv_api/detail/hccl/`, `impl/adv_api/tiling/hccl/` | `tests/api/adv_api/hccl/` | 含 internal/common/platform 实现。 |
| hcomm | `include/adv_api/hcomm/` | `impl/adv_api/detail/hcomm/` | `tests/api/adv_api/hcomm/` | 含 common/platform 实现。 |
| index | `include/adv_api/index/` | `impl/adv_api/detail/index/`, `impl/adv_api/detail/api_check/kernel_check/index/`, `impl/adv_api/tiling/index/` | `tests/api/adv_api/index/`, `tests/api/adv_api/api_check/index/` | 当前主要子类为 `arithprogression`。 |
| math | `include/adv_api/math/` | `impl/adv_api/detail/math/`, `impl/adv_api/detail/api_check/kernel_check/math/`, `impl/adv_api/tiling/math/` | `tests/api/adv_api/math/`, `tests/api/adv_api/api_check/math/` | 覆盖 unary/binary/logical/bitwise 等大量子类。 |
| matmul | `include/adv_api/matmul/` | `impl/adv_api/detail/matmul/`, `impl/adv_api/tiling/matmul/` | `tests/api/adv_api/matmul/` | 目录下还有 scheduler/resource/stage/param 等内部子目录。 |
| normalization | `include/adv_api/normalization/` | `impl/adv_api/detail/normalization/`, `impl/adv_api/detail/api_check/kernel_check/normalization/`, `impl/adv_api/tiling/normalization/` | `tests/api/adv_api/normalization/`, `tests/api/adv_api/api_check/normalization/` | layernorm/layernormgrad 存在 regbase 细分实现。 |
| pad | `include/adv_api/pad/` | `impl/adv_api/detail/pad/`, `impl/adv_api/detail/api_check/kernel_check/pad/`, `impl/adv_api/tiling/pad/` | `tests/api/adv_api/pad/`, `tests/api/adv_api/api_check/pad/` | 包含 broadcast/pad/unpad 等。 |
| quantization | `include/adv_api/quantization/` | `impl/adv_api/detail/quantization/`, `impl/adv_api/detail/api_check/kernel_check/quantization/`, `impl/adv_api/tiling/quantization/` | `tests/api/adv_api/quantization/`, `tests/api/adv_api/api_check/quantization/` | antiquant/dequant/quant 及 *ize 子类。 |
| reduce | `include/adv_api/reduce/` | `impl/adv_api/detail/reduce/`, `impl/adv_api/detail/api_check/kernel_check/reduce/`, `impl/adv_api/tiling/reduce/` | `tests/api/adv_api/reduce/`, `tests/api/adv_api/api_check/reduce/` | reduce_* 与 sum/mean 子类并存。 |
| select | `include/adv_api/select/` | `impl/adv_api/detail/select/`, `impl/adv_api/detail/api_check/kernel_check/select/`, `impl/adv_api/tiling/select/` | `tests/api/adv_api/select/`, `tests/api/adv_api/api_check/select/` | 当前主要子类为 `selectwithbytesmask`。 |
| sort | `include/adv_api/sort/` | `impl/adv_api/detail/sort/`, `impl/adv_api/detail/api_check/kernel_check/sort/`, `impl/adv_api/tiling/sort/` | `tests/api/adv_api/sort/`, `tests/api/adv_api/api_check/sort/` | `api_check` 还包含 concat/extract/mrgsort。 |
| transpose | `include/adv_api/transpose/` | `impl/adv_api/detail/transpose/`, `impl/adv_api/detail/api_check/kernel_check/transpose/`, `impl/adv_api/tiling/transpose/` | `tests/api/adv_api/transpose/`, `tests/api/adv_api/api_check/transpose/`, `tests/api/adv_api/api_check/transdata/` | `transdata` 在 api_check 下是独立目录。 |
| utils | `include/adv_api/utils/` | `impl/adv_api/detail/utils/`, `impl/adv_api/detail/api_check/kernel_check/utils/` | `tests/api/adv_api/utils/`, `tests/api/adv_api/api_check/utils/` | 当前主要子类为 `init_global_memory`。 |

## 基础 API 架构与 UT 目录

| 产品/目标 | NPU_ARCH / 宏 | 实现目录 | UT 目录 |
|----------|---------------|----------|---------|
| ascend910 | `__NPU_ARCH__=1001`, `__DAV_C100__` | `impl/basic_api/dav_c100/` | `tests/api/basic_api/ascendc_case_ascend910/` |
| ascend310p | `__NPU_ARCH__=2002`, `__DAV_M200__` | `impl/basic_api/dav_m200/` | `tests/api/basic_api/ascendc_case_ascend310p/` |
| ascend610 | `__NPU_ARCH__=2002`, `__DAV_M200__` | `impl/basic_api/dav_m200/` | `tests/api/basic_api/ascendc_case_ascend610/` |
| ascend910B1 AIC | `__NPU_ARCH__=2201`, `__DAV_C220__`, `__DAV_C220_CUBE__` | `impl/basic_api/dav_c220/` | `tests/api/basic_api/ascendc_case_ascend910b1/ascendc_case_ascend910b1_aic/` |
| ascend910B1 AIV | `__NPU_ARCH__=2201`, `__DAV_C220__`, `__DAV_C220_VEC__` | `impl/basic_api/dav_c220/` | `tests/api/basic_api/ascendc_case_ascend910b1/ascendc_case_ascend910b1_aiv/` |
| ascend910B1 AIV MSTX | `__NPU_ARCH__=2201`, `__DAV_C220__`, `__DAV_C220_VEC__` | `impl/basic_api/dav_c220/` | `tests/api/basic_api/ascendc_case_ascend910b1/ascendc_case_ascend910b1_aiv_mstx/` |
| ascend310B1 | `__NPU_ARCH__=3002`, `__DAV_M300__` | `impl/basic_api/dav_m300/` | `tests/api/basic_api/ascendc_case_ascend310b1/` |
| ascend950pr_9599 AIC | `__NPU_ARCH__=3510`, `__DAV_C310__`, `__DAV_C310_CUBE__` | `impl/basic_api/dav_3510/` | `tests/api/basic_api/ascendc_case_ascend950pr_9599/ascendc_case_ascend950pr_9599_aic/` |
| ascend950pr_9599 AIV basic | `__NPU_ARCH__=3510`, `__DAV_C310__`, `__DAV_C310_VEC__` | `impl/basic_api/dav_3510/` | `tests/api/basic_api/ascendc_case_ascend950pr_9599/ascendc_case_ascend950pr_9599_aiv_basic/` |
| ascend950pr_9599 AIV framework | `__NPU_ARCH__=3510`, `__DAV_C310__`, `__DAV_C310_VEC__` | `impl/basic_api/dav_3510/` | `tests/api/basic_api/ascendc_case_ascend950pr_9599/ascendc_case_ascend950pr_9599_aiv_framework/` |
| common/header check | 多架构 | `impl/basic_api/`, `include/basic_api/` | `tests/api/basic_api/ascendc_case_common/`, `tests/api/basic_api/common/`, `tests/api/basic_api/ascendc_header_checker/` |

`impl/basic_api/dav_l300/` 和 `impl/basic_api/dav_l311/` 当前也被接口实现聚合头引用，但本次扫描未发现同名 `tests/api/basic_api/ascendc_case_*` 顶层 UT 目录；新增相关 UT 前必须先确认产品目标和 CMake 配置。

## C API 子目录映射

实现目录保持扁平，每个实际声明接口的公开头 `<header>.h` 对应 `<header>_intf_impl.h`。下表列出各类代表文件；atomic 另有 `scalar_atomic_intf_impl.h`，scalar_compute 按 bit/convert/load/store 拆分，vector_compute 按各公开头拆分。公共内部辅助代码位于各架构组的 `utils_impl.h`；聚合头、纯类型头和无独立函数实现的宏别名不生成空实现头。

| C API 子类 | 声明目录 | 2201 实现/UT | 3510 实现/UT |
|-----------|----------|--------------|--------------|
| atomic | `include/c_api/atomic/` | `impl/c_api/memory_base_impl/datamove_atomic_intf_impl.h`; 当前实际 UT 目录为 `tests/api/c_api/npu_arch_2201/simd_atomic/`，但 CMake glob 写的是 `npu_arch_2201/atomic/*.cpp`，新增前需核对 | `impl/c_api/reg_base_impl/datamove_atomic_intf_impl.h`; `tests/api/c_api/npu_arch_3510/atomic/` |
| cache_ctrl | `include/c_api/cache_ctrl/` | `impl/c_api/memory_base_impl/cache_ctrl_intf_impl.h`; `tests/api/c_api/npu_arch_2201/cache_ctrl/` | `impl/c_api/reg_base_impl/cache_ctrl_intf_impl.h`; `tests/api/c_api/npu_arch_3510/cache_ctrl/` |
| cube_compute | `include/c_api/cube_compute/` | `impl/c_api/memory_base_impl/cube_compute_intf_impl.h`; `tests/api/c_api/npu_arch_2201/cube_compute/` | `impl/c_api/reg_base_impl/cube_compute_intf_impl.h`; `tests/api/c_api/npu_arch_3510/cube_compute/` |
| cube_datamove | `include/c_api/cube_datamove/` | `impl/c_api/memory_base_impl/cube_datamove_intf_impl.h`; `tests/api/c_api/npu_arch_2201/cube_datamove/` | `impl/c_api/reg_base_impl/cube_datamove_intf_impl.h`; `tests/api/c_api/npu_arch_3510/cube_datamove/` |
| misc | `include/c_api/misc/` | `impl/c_api/memory_base_impl/sys_init_intf_impl.h`; 当前扫描未发现 `tests/api/c_api/npu_arch_2201/misc/` | `impl/c_api/reg_base_impl/sys_init_intf_impl.h`; `tests/api/c_api/npu_arch_3510/misc/` |
| scalar_compute | `include/c_api/scalar_compute/` | `impl/c_api/memory_base_impl/scalar_bit_intf_impl.h`; `tests/api/c_api/npu_arch_2201/scalar_compute/` | `impl/c_api/reg_base_impl/scalar_bit_intf_impl.h`; `tests/api/c_api/npu_arch_3510/scalar_compute/` |
| sync | `include/c_api/sync/` | `impl/c_api/memory_base_impl/sync_intf_impl.h`; `tests/api/c_api/npu_arch_2201/sync/` | `impl/c_api/reg_base_impl/sync_intf_impl.h`; `tests/api/c_api/npu_arch_3510/sync/` |
| sys_var | `include/c_api/utils/sys_var.h` | `impl/c_api/memory_base_impl/sys_var_intf_impl.h`; `tests/api/c_api/npu_arch_2201/sys_var/` | `impl/c_api/reg_base_impl/sys_var_intf_impl.h`; `tests/api/c_api/npu_arch_3510/sys_var/` |
| utils | `include/c_api/utils/` | `impl/c_api/memory_base_impl/sys_var_intf_impl.h`; `tests/api/c_api/npu_arch_2201/utils/` | `impl/c_api/reg_base_impl/sys_var_intf_impl.h`; `tests/api/c_api/npu_arch_3510/utils/` |
| vector_compute | `include/c_api/vector_compute/` | `impl/c_api/memory_base_impl/vector_arith_intf_impl.h`; `tests/api/c_api/npu_arch_2201/vector_compute/` | `impl/c_api/reg_base_impl/vector_sort_intf_impl.h`; `tests/api/c_api/npu_arch_3510/vector_compute/` |
| vector_datamove | `include/c_api/vector_datamove/` | `impl/c_api/memory_base_impl/vector_datamove_intf_impl.h`; `tests/api/c_api/npu_arch_2201/vector_datamove/` | `impl/c_api/reg_base_impl/vector_datamove_intf_impl.h`; `tests/api/c_api/npu_arch_3510/vector_datamove/` |
| reg_compute headers | `include/c_api/reg_compute/` | reg 相关 2201 现有用例在 `tests/api/c_api/npu_arch_2201/vector_compute/` | 3510 的 reg load/store 实现分别位于 `loadalign_intf_impl.h`、`loadunalign_intf_impl.h`、`storealign_intf_impl.h`、`storeunalign_intf_impl.h`，废弃重载位于 `npu_arch_3510/` 下的同名头，UT 归入 `vector_datamove/` 或 `vector_compute/` |

## 工具类 API 子目录映射

| 子类 | 声明目录 | 实现目录 | UT 目录 |
|-----|----------|----------|---------|
| context | `include/utils/context/` | `impl/utils/context/` | `tests/api/utils/context/` |
| debug | `include/utils/debug/` | `impl/utils/debug/`, `impl/utils/debug/npu_arch_*/` | 当前扫描未发现独立 `tests/api/utils/debug/` 目录，新增前需确认归类 |
| std | `include/utils/std/` | `impl/utils/std/` | `tests/api/utils/std/` |
| tiling | `include/utils/tiling/` | `impl/utils/tiling/` | `tests/api/utils/tiling/` |
| base/log/stub | `include/utils/base/`, `include/utils/log/`, `include/utils/stub/` | `impl/utils/stub/` 或无独立实现 | 当前扫描未发现完全同名 UT 目录，按具体 API 和已有同类 UT 归类 |
