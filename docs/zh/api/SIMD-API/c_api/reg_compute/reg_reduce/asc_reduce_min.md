---
okftype: reference.api
description: "按掩码对源矢量寄存器做归约求最小值，结果与最小值在源中的索引原始位模式分别写入目的寄存器第 0、1 个元素，其余元素置零。"
tag:
  - roles: [compute]
  - operations: [minimum, reduction]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: requires
    condition: "矢量数据寄存器与掩码寄存器的类型定义"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "Reg 矢量计算 C API 通用约束"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "section:函数原型"
    scope_basis: resolved
    note: "9.2.0 为 asc_reduce_min 的 6 种 dtype 补充 6 个函数返回值形式重载，9.0.0/9.1.0 只有把结果写入 dst 的引用输出形式；当前 include 中该名字共 6 条引用输出 + 6 条返回值声明。"
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "声明位置由 include/c_api/reg_compute/reg_vector.h（9.1.0 及更早）统一到 include/c_api/reg_compute/compute/reg_reduce.h（9.2.0 起），与正文头文件路径一致。"
---
# asc_reduce_min

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

头文件路径为：`"c_api/reg_compute/compute/reg_reduce.h"`。

根据`mask`对源操作数`src`进行归约最小值操作，得到归约结果。结果保存在`dst`的第0个元素，最小值在`src`中的索引原始位模式保存在`dst`的第1个元素，`dst`中的其他元素置0。如果存在多个最小值，则保留最小的索引。计算公式如下：

$$
\begin{aligned}
dst_0 &= \min\{src_i \mid mask_i = 1\} \\
dst_1 &= \operatorname{argmin}\{src_i \mid mask_i = 1\}
\end{aligned}
$$

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```c
// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dtype> asc_reduce_min(vector_<dtype> src,
                                                     vector_bool mask)

// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_reduce_min(vector_<dtype>& dst,
                                           vector_<dtype> src,
                                           vector_bool mask)
```

### dtype支持数据类型

`dtype`取值为：`int16_t`、`uint16_t`、`half`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```c
// 示例：对half矢量数据寄存器执行归约最小值操作
__simd_callee__ inline void asc_reduce_min(vector_half& dst,
                                           vector_half src,
                                           vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| ------ | --------- | ---------------------------------------- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| mask | 输入 | 掩码寄存器，用于控制各元素是否参与归约。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 通过引用参数输出结果：归约结果写入`dst`。
- 通过函数返回值返回结果：返回归约结果，类型为矢量数据寄存器，与`src`的数据类型一致。

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 未被`mask`选中的元素被视为对应数据类型的最大值，浮点数类型的最大值为`+inf`。如果`src`中的所有元素均未被`mask`选中，则将该最大值写入`dst`的第0个元素，并将其余元素置0。
- 比较时遵循$min(-0, +0) = -0$。
- 如果输入数据中存在nan，则将nan写入`dst`的第0个元素，并将第一个nan的索引写入`dst`的第1个元素。

## 关键特性

**规约产生值+索引两个结果，索引值需要强制类型转换**：

`dst`的索引按照`dst`的数据类型存储，比如`dst`为half类型时，索引按照half类型存储，因此读取索引需要使用reinterpret\_cast方法转换到整数类型。若数据类型是half，需要使用reinterpret\_cast\<uint16_t\*\>；若数据类型是float，需要使用reinterpret\_cast\<uint32\_t\*\>。值+索引的分别提取方式请参见[调用示例](#调用示例)。

## 调用示例

将以下代码保存为`example.asc`后，可通过`bisheng`命令编译运行。其中，`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
// 本例使用half类型演示归约结果"值+索引"的分别提取，
// 归约结果中dst的第0个元素为最小值，第1个元素为最小值索引的原始位模式。
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"
namespace {
constexpr uint8_t MUTEX_ID = 0;

constexpr uint32_t ELEMENTS_PER_REGISTER = 128;
constexpr uint32_t DATA_BYTES = 133 * sizeof(half);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(half);
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// 按完整寄存器块扩展UB容量，避免尾轮整寄存器访问越界；有效范围仍由掩码控制。
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
__simd_vf__ inline void reduce(__ubuf__ half* dst, __ubuf__ half* src, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_half dst_reg, src_reg;
        vector_bool mask = asc_update_mask_b16(remaining_length);
        asc_loadalign(src_reg, src + offset);
        asc_reduce_min(dst_reg, src_reg, mask);
        asc_storealign(dst + offset, dst_reg, mask);
    }
}
__global__ __vector__ void asc_reduce_min_kernel(__gm__ half* dst, __gm__ half* src)
{
    asc_init();
    __ubuf__ half dst_local[PADDED_ELEMENT_COUNT], src_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src_local, src, TOTAL_LENGTH * sizeof(half));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(half));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<reduce>(dst_local, src_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst, dst_local, TOTAL_LENGTH * sizeof(half));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace
int main()
{
    std::vector<half> src(TOTAL_LENGTH), output(TOTAL_LENGTH, half(0));
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        src[i] = half(static_cast<float>(i) + 1.0f);  // 1, 2, ..., 64
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    half* src_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src_device), (TOTAL_LENGTH) * sizeof(half),
        ACL_MEM_MALLOC_HUGE_FIRST);
    half* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), (TOTAL_LENGTH) * sizeof(half),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src_device, src.size() * sizeof(half), src.data(), src.size() * sizeof(half),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_reduce_min_kernel<<<1, 0>>>(dst_device, src_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(half), dst_device, output.size() * sizeof(half),
        ACL_MEMCPY_DEVICE_TO_HOST);
    half min_value = output[0];                                      // 提取最小值
    uint16_t min_index = *reinterpret_cast<uint16_t*>(&output[1]);   // 提取最小值索引：索引按half类型存储，需强制转换到整数类型
    const bool passed = (static_cast<float>(min_value) == 1.0f) && (min_index == 0);
    std::cout << (passed ? "[Success] asc_reduce_min passed." : "[Failed] asc_reduce_min failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
