---
okftype: reference.api
description: "从特殊用途寄存器读取掩码值并按数据类型格式化，b16 模式把每个 bit 扩展为 2bit、b32 模式扩展为 4bit，拼成 256bit 掩码寄存器返回。"
tag:
  - roles: [query]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "Reg 矢量计算 C API 通用约束"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_mask_config/asc_set_vector_mask.md
    kind: related
    condition: "在 VF 场景外由该接口设置掩码寄存器，再配合本接口在 Reg 场景获取"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "声明位置由 include/c_api/reg_compute/reg_load.h（9.1.0 及更早）迁移到 include/c_api/reg_compute/compute/reg_mask.h（9.2.0 起），与正文头文件路径一致；签名始终保持无参返回 vector_bool。"
---
# asc_get_mask_spr

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

头文件路径为：`"c_api/reg_compute/compute/reg_mask.h"`。

从SPR（Special Purpose Register）中读取mask值，并按数据类型格式化后返回。

- 对于b16数据类型：从SPR读取mask共128bit数据，将每个bit扩展为2bit，拼成256bit掩码寄存器返回。
- 对于b32数据类型：从SPR读取mask共64bit数据，将每个bit扩展为4bit，拼成256bit掩码寄存器返回。

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```c
__simd_callee__ inline vector_bool asc_get_mask_spr_b16()
__simd_callee__ inline vector_bool asc_get_mask_spr_b32()
```

## 参数说明

无

## 返回值说明

vector_bool，掩码寄存器。

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 本接口需在Vector Function（`__simd_vf__`标记的函数）内调用。
- [asc_set_vector_mask](../../vector_compute/vector_mask_config/asc_set_vector_mask.md)用于在VF场景外设置掩码寄存器，在Reg矢量计算配置时，通过`asc_get_mask_spr_b16`或`asc_get_mask_spr_b32`配合获取掩码寄存器的值。更推荐用户使用VF场景内的接口（[asc_create_mask](./asc_create_mask.md)和[asc_update_mask.md](./asc_update_mask.md)）完成Reg矢量计算掩码配置。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
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
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

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

template <typename T>
bool compare_range_data(const std::vector<T>& actual, const std::vector<T>& expected,
    size_t begin, size_t count, double tolerance = 0.0)
{
    if (begin + count > actual.size() || begin + count > expected.size()) return false;
    for (size_t i = begin; i < begin + count; ++i) {
        if (actual[i] == expected[i]) continue;
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

constexpr uint32_t CHUNK_BYTES = 768;
constexpr uint32_t DATA_BYTES = 1028;
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT = (DATA_BYTES + CHUNK_BYTES - 1) / CHUNK_BYTES;
constexpr uint32_t BUFFER_BYTES = CHUNK_BYTES * REPEAT_COUNT;

// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint8_t);
__simd_vf__ inline void exercise(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint16_t value16;
    vector_uint32_t value32;
    uint32_t remaining = DATA_BYTES;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * CHUNK_BYTES;
        const uint32_t valid_count = remaining > CHUNK_BYTES ? CHUNK_BYTES : remaining;
        // 保留剩余有效元素数，供更新掩码处理尾块。
        uint32_t remaining_length = valid_count / sizeof(uint16_t);
        if (remaining_length > 128) remaining_length = 128;
        vector_bool seed16 = asc_update_mask_b16(remaining_length);
        vector_bool mask16 = asc_get_mask_spr_b16();
        asc_duplicate_scalar(value16, static_cast<uint16_t>(1), seed16);
        asc_storealign(reinterpret_cast<__ubuf__ uint16_t*>(output + offset), value16, mask16);
        uint32_t remaining_length_2 = valid_count > 256 ? (valid_count - 256) / sizeof(uint32_t) : 0;
        if (remaining_length_2 > 64) remaining_length_2 = 64;
        vector_bool seed32 = asc_update_mask_b32(remaining_length_2);
        vector_bool mask32 = asc_get_mask_spr_b32();
        asc_duplicate_scalar(value32, static_cast<uint32_t>(1), seed32);
        asc_storealign(reinterpret_cast<__ubuf__ uint32_t*>(output + offset + 256), value32, mask32);
        remaining -= valid_count;
    }
}

__global__ __vector__ void asc_get_mask_spr_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[BUFFER_BYTES];
    __ubuf__ uint8_t input_local[BUFFER_BYTES];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_copy_gm2ub_align(output_local, input, BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint16_t one_repeat_size = 3 * asc_get_vf_len() / sizeof(uint8_t);
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<exercise>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(BUFFER_BYTES), output(BUFFER_BYTES, 0xff), golden(BUFFER_BYTES);
    for (uint32_t i = 0; i < BUFFER_BYTES; ++i) input[i] = static_cast<uint8_t>(i % 251);
    for (uint32_t repeat = 0; repeat < REPEAT_COUNT; ++repeat) {
        const uint32_t offset = repeat * CHUNK_BYTES;
        const uint32_t end = DATA_BYTES - offset > CHUNK_BYTES ? CHUNK_BYTES : DATA_BYTES - offset;
        for (uint32_t i = 0; i < end && i < 256; i += sizeof(uint16_t)) golden[offset + i] = 1;
        for (uint32_t i = 256; i < end && i < 512; i += sizeof(uint32_t)) golden[offset + i] = 1;
        if (end > 512) {
            std::copy(input.begin() + offset + 512, input.begin() + offset + end, golden.begin() + offset + 512);
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), (BUFFER_BYTES) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (BUFFER_BYTES) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint8_t), input.data(), input.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_get_mask_spr_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    print_data("Golden bytes", golden);
    const bool passed = compare_range_data(output, golden, 0, DATA_BYTES);
    std::cout << (passed ? "[Success] asc_get_mask_spr_b16/asc_get_mask_spr_b32 passed." : "[Failed] asc_get_mask_spr_b16/asc_get_mask_spr_b32 failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
