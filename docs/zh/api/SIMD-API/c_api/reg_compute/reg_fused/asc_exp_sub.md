---
okftype: reference.api
description: "对两个源矢量逐元素相减并计算以差值为指数的自然指数函数的Reg融合计算接口，float输入走asc_exp_sub，half输入走asc_exp_sub_half2float并可选奇偶索引，仅在AIV上生效。"
tag:
  - roles: [compute]
  - operations: [exponential, subtract]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "矢量数据寄存器与掩码寄存器的类型定义"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "Reg矢量计算C API通用约束见该页通用约束节"
  - path: docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md
    kind: related
    condition: "产品型号与NPU架构映射说明"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: capability_extended
    scope: "page"
    scope_basis: kind_default
    note: "新增 asc_exp_sub_half2float 接口：half 源操作数转换为 float 后计算以差值为指数的自然指数函数，并支持通过 asc_position_mode 常量显式选择读取偶数或奇数索引元素（CANN_9_2_0 Release Notes 第 43 行）；该接口的返回值与 dst 引用两种形式均随本次新增。"
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_exp_sub__vf32_vf32_vb"
    scope_basis: resolved
    note: "新增 float 输入的函数返回值形式重载 asc_exp_sub(vector_float, vector_float, vector_bool)，原 dst 引用形式保留（CANN_9_2_0 Release Notes 第 54 行的函数返回值形式补充批次）。"
---
# asc_exp_sub

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/reg_compute/compute/reg_fused.h"`。

根据`mask`将`src0`与`src1`按元素相减，并计算以差值为指数的自然指数函数，计算结果通过函数返回值返回或写入`dst`。

- `asc_exp_sub`用于`float`类型输入。计算公式如下：

  $$
  result_i = e^{src0_i - src1_i}
  $$

- `asc_exp_sub_half2float`用于`half`类型输入。该接口支持读取源操作数中偶数索引或奇数索引的元素，将其转换为`float`类型后进行计算。计算公式如下：

  $$
  \begin{cases}
  result_i = e^{float(src0_{2i}) - float(src1_{2i})} & \text{提取偶数索引元素} \\
  result_i = e^{float(src0_{2i+1}) - float(src1_{2i+1})} & \text{提取奇数索引元素}
  \end{cases}
  $$

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

- 用于处理`float`数据类型的源操作数。

  ```c
  // 通过函数返回值返回结果
  __simd_callee__ inline vector_float asc_exp_sub(vector_float src0,
                                                  vector_float src1,
                                                  vector_bool mask)

  // 通过引用参数输出结果
  __simd_callee__ inline void asc_exp_sub(vector_float& dst,
                                          vector_float src0,
                                          vector_float src1,
                                          vector_bool mask)
  ```

- 处理`half`数据类型的源操作数，支持读取偶数索引或奇数索引的源数据。

  ```c
  // 通过函数返回值返回结果（占位符形式）
  __simd_callee__ inline vector_float asc_exp_sub_half2float(vector_half src0,
                                                             vector_half src1,
                                                             vector_bool mask,
                                                             std::integral_constant<asc_position_mode, <src_pos>> src_pos)

  // 通过引用参数输出结果（占位符形式）
  __simd_callee__ inline void asc_exp_sub_half2float(vector_float& dst,
                                                     vector_half src0,
                                                     vector_half src1,
                                                     vector_bool mask,
                                                     std::integral_constant<asc_position_mode, <src_pos>> src_pos)
  ```

  **占位符说明如下：**

  - `<src_pos>`支持`ASC_POSITION_EVEN`和`ASC_POSITION_ODD`。

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| ------ | --------- | ---- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src0 | 输入 | 源操作数0（矢量数据寄存器）。 |
| src1 | 输入 | 源操作数1（矢量数据寄存器）。 |
| mask | 输入 | 掩码寄存器，用于控制各元素是否参与计算。`mask`中与输出元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |
| src_pos | 输入 | 指示源操作数读取位置。取值为`ASC_POSITION_EVEN`时读取偶数索引元素，取值为`ASC_POSITION_ODD`时读取奇数索引元素。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 通过引用参数输出结果的函数原型无返回值，计算结果写入`dst`。
- 通过函数返回值输出结果的函数原型返回计算结果，返回值类型为`vector_float`。

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- `mask`掩码位为0时，结果对应元素置0。
- 只有当输出数据类型位宽大于输入时，计算时才会有精度提升。

## 调用示例

将以下代码保存为`example.asc`后，可通过`bisheng`命令编译运行。其中，`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 -lm && ./main
```

**注意：** 本示例主机侧golden计算会调用需由libm提供的数学函数：`std::exp`，编译命令需要保留`-lm`以链接数学库。
<!-- end id8 -->

```cpp
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;


template <typename T>
void print_data(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << static_cast<float>(values[i]);
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

bool compare_data(const std::vector<float>& actual, const std::vector<float>& expected, double tolerance)
{
    if (actual.size() != expected.size()) return false;
    for (size_t i = 0; i < actual.size(); ++i) {
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t FLOAT_COUNT = 133;
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT = (FLOAT_COUNT + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
constexpr uint32_t PADDED_FLOAT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t HALF_COUNT = FLOAT_COUNT * 2;
constexpr uint32_t PADDED_HALF_COUNT = REPEAT_COUNT * 128;

constexpr uint32_t DATA_BYTES = FLOAT_COUNT * sizeof(float);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = FLOAT_COUNT;
__simd_vf__ inline void compute(__ubuf__ float* float_dst, __ubuf__ float* half_even_dst,
    __ubuf__ float* half_odd_dst, __ubuf__ float* float_src0, __ubuf__ float* float_src1,
    __ubuf__ half* half_src0, __ubuf__ half* half_src1, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = FLOAT_COUNT;
    uint32_t remaining_length_2 = HALF_COUNT;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_float float_src0_reg;
        vector_float float_src1_reg;
        vector_half half_src0_reg;
        vector_half half_src1_reg;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        vector_bool mask_b16 = asc_update_mask_b16(remaining_length_2);
        asc_loadalign(float_src0_reg, float_src0 + offset);
        asc_loadalign(float_src1_reg, float_src1 + offset);
        asc_loadalign(half_src0_reg, half_src0 + offset * 2);
        asc_loadalign(half_src1_reg, half_src1 + offset * 2);

        vector_float float_result = asc_exp_sub(float_src0_reg, float_src1_reg, mask);
        vector_float half_even_result;
        asc_exp_sub_half2float(half_even_result, half_src0_reg, half_src1_reg, mask_b16,
            ASC_POSITION_EVEN);
        vector_float half_odd_result = asc_exp_sub_half2float(half_src0_reg, half_src1_reg,
            mask_b16, ASC_POSITION_ODD);

        asc_storealign(float_dst + offset, float_result, mask);
        asc_storealign(half_even_dst + offset, half_even_result, mask);
        asc_storealign(half_odd_dst + offset, half_odd_result, mask);
    }
}

__global__ __vector__ void asc_exp_sub_kernel(__gm__ float* float_dst, __gm__ float* half_even_dst,
    __gm__ float* half_odd_dst, __gm__ float* float_src0, __gm__ float* float_src1,
    __gm__ half* half_src0, __gm__ half* half_src1)
{
    asc_init();
    __ubuf__ float float_dst_local[PADDED_FLOAT_COUNT];
    __ubuf__ float half_even_dst_local[PADDED_FLOAT_COUNT];
    __ubuf__ float half_odd_dst_local[PADDED_FLOAT_COUNT];
    __ubuf__ float float_src0_local[PADDED_FLOAT_COUNT];
    __ubuf__ float float_src1_local[PADDED_FLOAT_COUNT];
    __ubuf__ half half_src0_local[PADDED_HALF_COUNT];
    __ubuf__ half half_src1_local[PADDED_HALF_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(float_src0_local, float_src0, FLOAT_COUNT * sizeof(float));
    asc_copy_gm2ub_align(float_src1_local, float_src1, FLOAT_COUNT * sizeof(float));
    asc_copy_gm2ub_align(half_src0_local, half_src0, HALF_COUNT * sizeof(half));
    asc_copy_gm2ub_align(half_src1_local, half_src1, HALF_COUNT * sizeof(half));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<compute>(float_dst_local, half_even_dst_local, half_odd_dst_local, float_src0_local,
        float_src1_local, half_src0_local, half_src1_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(float_dst, float_dst_local, FLOAT_COUNT * sizeof(float));
    asc_copy_ub2gm_align(half_even_dst, half_even_dst_local, FLOAT_COUNT * sizeof(float));
    asc_copy_ub2gm_align(half_odd_dst, half_odd_dst_local, FLOAT_COUNT * sizeof(float));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> float_src0(FLOAT_COUNT);
    std::vector<float> float_src1(FLOAT_COUNT);
    std::vector<half> half_src0(HALF_COUNT);
    std::vector<half> half_src1(HALF_COUNT);
    std::vector<float> float_output(FLOAT_COUNT);
    std::vector<float> half_even_output(FLOAT_COUNT);
    std::vector<float> half_odd_output(FLOAT_COUNT);
    std::vector<float> float_golden(FLOAT_COUNT);
    std::vector<float> half_even_golden(FLOAT_COUNT);
    std::vector<float> half_odd_golden(FLOAT_COUNT);
    for (uint32_t i = 0; i < FLOAT_COUNT; ++i) {
        float_src0[i] = static_cast<float>(i % 8) * 0.125f;
        float_src1[i] = static_cast<float>((i + 1) % 8) * 0.125f;
        float_golden[i] = std::exp(float_src0[i] - float_src1[i]);
    }
    for (uint32_t i = 0; i < HALF_COUNT; ++i) {
        half_src0[i] = static_cast<half>(static_cast<float>(i % 8) * 0.125f);
        half_src1[i] = static_cast<half>(static_cast<float>((i + 2) % 8) * 0.125f);
    }
    for (uint32_t i = 0; i < FLOAT_COUNT; ++i) {
        half_even_golden[i] = std::exp(static_cast<float>(half_src0[2 * i]) -
                                       static_cast<float>(half_src1[2 * i]));
        half_odd_golden[i] = std::exp(static_cast<float>(half_src0[2 * i + 1]) -
                                      static_cast<float>(half_src1[2 * i + 1]));
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float* float_dst_device = nullptr;
    float* half_even_dst_device = nullptr;
    float* half_odd_dst_device = nullptr;
    float* float_src0_device = nullptr;
    float* float_src1_device = nullptr;
    half* half_src0_device = nullptr;
    half* half_src1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&float_dst_device), FLOAT_COUNT * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&half_even_dst_device), FLOAT_COUNT * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&half_odd_dst_device), FLOAT_COUNT * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&float_src0_device), FLOAT_COUNT * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&float_src1_device), FLOAT_COUNT * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&half_src0_device), HALF_COUNT * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&half_src1_device), HALF_COUNT * sizeof(half), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(float_src0_device, FLOAT_COUNT * sizeof(float), float_src0.data(), FLOAT_COUNT * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(float_src1_device, FLOAT_COUNT * sizeof(float), float_src1.data(), FLOAT_COUNT * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(half_src0_device, HALF_COUNT * sizeof(half), half_src0.data(), HALF_COUNT * sizeof(half),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(half_src1_device, HALF_COUNT * sizeof(half), half_src1.data(), HALF_COUNT * sizeof(half),
        ACL_MEMCPY_HOST_TO_DEVICE);

    asc_exp_sub_kernel<<<1, 0>>>(float_dst_device, half_even_dst_device, half_odd_dst_device,
        float_src0_device, float_src1_device, half_src0_device, half_src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(float_output.data(), FLOAT_COUNT * sizeof(float), float_dst_device, FLOAT_COUNT * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(half_even_output.data(), FLOAT_COUNT * sizeof(float), half_even_dst_device,
        FLOAT_COUNT * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(half_odd_output.data(), FLOAT_COUNT * sizeof(float), half_odd_dst_device,
        FLOAT_COUNT * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);

    print_data("Float src0", float_src0);
    print_data("Float src1", float_src1);
    print_data("Float output", float_output);
    print_data("Float golden", float_golden);
    print_data("Half src0", half_src0);
    print_data("Half src1", half_src1);
    print_data("Half even output", half_even_output);
    print_data("Half even golden", half_even_golden);
    print_data("Half odd output", half_odd_output);
    print_data("Half odd golden", half_odd_golden);
    const bool passed = compare_data(float_output, float_golden, 1e-4) &&
                        compare_data(half_even_output, half_even_golden, 1e-4) &&
                        compare_data(half_odd_output, half_odd_golden, 1e-4);
    std::cout << (passed ? "[Success] asc_exp_sub passed." : "[Failed] asc_exp_sub failed.") << std::endl;
    aclrtFree(float_dst_device);
    aclrtFree(half_even_dst_device);
    aclrtFree(half_odd_dst_device);
    aclrtFree(float_src0_device);
    aclrtFree(float_src1_device);
    aclrtFree(half_src0_device);
    aclrtFree(half_src1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
