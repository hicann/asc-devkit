---
okftype: reference.api
description: "按掩码对源矢量执行Leaky ReLU的Reg融合激活接口，正值原样输出、非正值乘以标量alpha，提供写入dst引用与函数返回值两种形式，支持half与float，仅在AIV上生效。"
tag:
  - roles: [compute]
  - operations: [leaky_relu]
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
    scope: "overload:asc_leakyrelu__vf16_f16_vb"
    scope_basis: resolved
    note: "新增 half 数据的函数返回值形式重载 asc_leakyrelu(vector_half, half, vector_bool)，mask 为 0 的元素在返回值中置 0；依据 CANN_9_2_0 Release Notes 第 54 行（为 Reg 融合等接口补充函数返回值形式重载）与 upstream/9.2.0 首次出现的该声明。"
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_leakyrelu__vf32_f32_vb"
    scope_basis: resolved
    note: "新增 float 数据的函数返回值形式重载 asc_leakyrelu(vector_float, float, vector_bool)，与 half 返回值重载同批加入。"
---
# asc_leakyrelu

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

根据掩码对源矢量数据寄存器中的元素执行Leaky ReLU操作，并将结果通过函数返回值返回或写入目的矢量数据寄存器。源操作数中大于0的元素直接写入输出结果，小于或等于0的元素乘以标量立即数后写入输出结果。掩码对应位置为1的元素参与计算，为0的元素在输出结果中置零。计算公式如下：

$$
dst_i = \begin{cases} src_i & src_i > 0 \\ src_i \times \alpha & src_i \leq 0 \end{cases}
$$

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```c
// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dtype> asc_leakyrelu(vector_<dtype> src,
                                                    <dtype> alpha,
                                                    vector_bool mask)

// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_leakyrelu(vector_<dtype>& dst,
                                          vector_<dtype> src,
                                          <dtype> alpha,
                                          vector_bool mask)
```

### dtype支持数据类型

`dtype`取值为：`half`、`float`。

### 函数原型典型示例

```c
// 通过函数返回值返回结果
__simd_callee__ inline vector_half asc_leakyrelu(vector_half src,
                                                 half alpha,
                                                 vector_bool mask)

// 通过引用参数输出结果
__simd_callee__ inline void asc_leakyrelu(vector_half& dst,
                                          vector_half src,
                                          half alpha,
                                          vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数（矢量数据寄存器）。仅无返回值类型接口包含该参数，数据类型须与`src`一致。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| alpha | 输入 | 负半轴斜率（标量）。数据类型须与`src`一致。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。`mask`未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

对于返回值类型接口，返回保存Leaky ReLU计算结果的矢量数据寄存器，数据类型与`src`保持一致。

## 约束说明

### 通用约束

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 掩码位为0的元素位置不参与运算，输出结果对应位置写0。

### 计算约束

- 源矢量数据寄存器中数据为负零（-0）时按负数处理，乘法满足IEEE 754浮点乘法规则。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

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

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t DATA_BYTES = 137 * sizeof(float);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(float);
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// 按完整寄存器块扩展UB容量，避免尾轮整寄存器访问越界；有效范围仍由掩码控制。
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t BUFFER_BYTES = TOTAL_LENGTH * sizeof(float);
constexpr float ALPHA = 0.1f;

__simd_vf__ inline void leakyrelu_vf(__ubuf__ float* output, __ubuf__ float* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_float src;
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(src, input + offset);
        vector_float dst = asc_leakyrelu(src, ALPHA, mask);
        asc_storealign(output + offset, dst, mask);
    }
}

__global__ __vector__ void asc_leakyrelu_kernel(__gm__ float* output, __gm__ float* input)
{
    asc_init();
    __ubuf__ float output_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float input_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<leakyrelu_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> input(TOTAL_LENGTH);
    std::vector<float> output(TOTAL_LENGTH, 0.0f);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        input[i] = static_cast<float>(static_cast<int32_t>(i) - 64);
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float* input_device = nullptr;
    float* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BUFFER_BYTES, input.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_leakyrelu_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BUFFER_BYTES, output_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        const float expected = input[i] > 0.0f ? input[i] : input[i] * ALPHA;
        if (std::fabs(output[i] - expected) > 1e-6f) {
            passed = false;
            break;
        }
    }
    std::cout << (passed ? "[Success] asc_leakyrelu completed."
                         : "[Failed] asc_leakyrelu output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
