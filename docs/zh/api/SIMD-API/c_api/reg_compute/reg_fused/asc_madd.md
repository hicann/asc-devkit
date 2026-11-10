---
okftype: reference.api
description: "以目的矢量数据寄存器为累加器的Reg融合乘加接口，按掩码计算dst乘src0加src1并写回dst，支持half、bfloat16_t与float，仅在AIV上生效。"
tag:
  - roles: [compute]
  - operations: [fused_multiply_add]
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
    kind: overload_added
    scope: "overload:asc_madd__Rvbf16_vbf16_vbf16_vb"
    scope_basis: resolved
    note: "新增 bfloat16_t 数据的重载 asc_madd(vector_bfloat16_t& dst, vector_bfloat16_t src0, vector_bfloat16_t src1, vector_bool mask)；依据 CANN_9_2_0 Release Notes 第 58 行（asc_madd 新增 bfloat16_t 重载）与 upstream/9.2.0 首次出现的该声明（9.1.0 的 reg_vector.h 仅有 half/float 两种）。"
---
# asc_madd

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

根据`mask`对目的操作数`dst`与源操作数`src0`按元素相乘后，与源操作数`src1`中的对应元素相加，将结果写入`dst`。其中，`dst`既作为输入参与计算，又用于存储最终结果。计算公式如下：

$$
dst_i = dst_i \times src0_i + src1_i
$$

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```c
__simd_callee__ inline void asc_madd(vector_<dtype>& dst,
                                     vector_<dtype> src0,
                                     vector_<dtype> src1,
                                     vector_bool mask)
```

### dtype支持数据类型

`dtype`取值为：`half`、`bfloat16_t`、`float`。

### 函数原型典型示例

```c
// 示例：对float矢量数据寄存器执行乘加计算
__simd_callee__ inline void asc_madd(vector_float& dst,
                                     vector_float src0,
                                     vector_float src1,
                                     vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| ------ | --------- | ---------------------------------------------------------------------------------------------------------------- |
| dst | 输入/输出 | 目的操作数（矢量数据寄存器）。 |
| src0 | 输入 | 源操作数0（矢量数据寄存器）。 |
| src1 | 输入 | 源操作数1（矢量数据寄存器）。 |
| mask | 输入 | 掩码寄存器，用于控制各元素是否参与计算。`mask`中与元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- `dst`、`src0`和`src1`的数据类型需要保持一致。
- `mask`掩码位为0时，`dst`对应元素置0。

## 调用示例

将以下代码保存为`example.asc`后，可通过`bisheng`命令编译运行。其中，`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
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
bool compare_data(const std::vector<T>& actual, const std::vector<T>& expected, double tolerance = 0.0)
{
    if (actual.size() != expected.size()) return false;
    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] == expected[i]) continue;
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t DATA_BYTES = 137 * sizeof(float);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(float);
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// 按完整寄存器块扩展UB容量，避免尾轮整寄存器访问越界；有效范围仍由掩码控制。
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;

__simd_vf__ inline void compute(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_float dst_reg;
    vector_float src0_reg;
    vector_float src1_reg;
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(dst_reg, dst + offset);
        asc_loadalign(src0_reg, src0 + offset);
        asc_loadalign(src1_reg, src1 + offset);
        asc_madd(dst_reg, src0_reg, src1_reg, mask);
        asc_storealign(dst + offset, dst_reg, mask);
    }
}

__global__ __vector__ void asc_madd_kernel(__gm__ float* dst, __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float dst_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float src0_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float src1_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(dst_local, dst, TOTAL_LENGTH * sizeof(float));
    asc_copy_gm2ub_align(src0_local, src0, TOTAL_LENGTH * sizeof(float));
    asc_copy_gm2ub_align(src1_local, src1, TOTAL_LENGTH * sizeof(float));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<compute>(dst_local, src0_local, src1_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst, dst_local, TOTAL_LENGTH * sizeof(float));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> initial(TOTAL_LENGTH);
    std::vector<float> src0(TOTAL_LENGTH);
    std::vector<float> src1(TOTAL_LENGTH);
    std::vector<float> output(TOTAL_LENGTH);
    std::vector<float> golden(TOTAL_LENGTH);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        initial[i] = static_cast<float>(i) * 0.125f;
        src0[i] = static_cast<float>(i % 8) * 0.25f;
        src1[i] = static_cast<float>((i + 1) % 8) * 0.25f;
        output[i] = initial[i];
        golden[i] = initial[i] * src0[i] + src1[i];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    float* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), (TOTAL_LENGTH) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* src0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), (TOTAL_LENGTH) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* src1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), (TOTAL_LENGTH) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(dst_device, initial.size() * sizeof(float), initial.data(), initial.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src0_device, src0.size() * sizeof(float), src0.data(), src0.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, src1.size() * sizeof(float), src1.data(), src1.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_madd_kernel<<<1, 0>>>(dst_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), dst_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    const bool passed = compare_data(output, golden, 1e-6);
    std::cout << (passed ? "[Success] asc_madd passed." : "[Failed] asc_madd failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
