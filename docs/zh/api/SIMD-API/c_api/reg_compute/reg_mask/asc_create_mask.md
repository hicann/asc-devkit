---
okftype: reference.api
description: "按 pat_mode 指定的元素位置模式生成 b8、b16 或 b32 位宽的掩码寄存器，用于筛选后续 Reg 矢量计算中参与计算的元素。"
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "Reg 矢量计算 C API 通用约束与掩码寄存器数量上限"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "9.2.0 起 include/c_api/reg_compute/compute/reg_mask.h 以对象式宏别名提供该接口（#define asc_create_mask_b8 pset_b8、asc_create_mask_b16 pset_b16、asc_create_mask_b32 pset_b32），9.0.0/9.1.0 的 include 中只有注释形式的调用示例、没有可注解的声明；因此本页三个标识符在 doc_overrides 中显示为未写入头，本页不使用 overload scope。"
---
# asc_create_mask

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

根据传入的pat_mode生成对应的掩码寄存器，支持b8、b16、b32三种位宽模式。pat_mode参数指定mask的模式，即指定哪些位置的元素参与计算。

位宽模式说明：
- b8模式：每个bit对应一个8bit元素（共256元素），用于8bit数据类型的矢量计算。
- b16模式：每2个bit为一组对应一个16bit元素（共128元素），用于16bit数据类型的矢量计算。
- b32模式：每4个bit为一组对应一个32bit元素（共64元素），用于32bit数据类型的矢量计算。

**图1** asc_create_mask原理

![asc_create_mask原理](../../../../figures/capi_create_mask.png)

本接口为Reg数据类型定义接口，仅在AIV上生效。

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```c
asc_create_mask_b8(pat_mode)
asc_create_mask_b16(pat_mode)
asc_create_mask_b32(pat_mode)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| pat_mode | 输入 | mask模式，取值如下：<br>&bull; PAT_ALL：所有元素设置为有效数据，全部参与计算<br>&bull; PAT_VL1：最低1个元素设置为有效数据<br>&bull; PAT_VL2：最低2个元素设置为有效数据<br>&bull; PAT_VL3：最低3个元素设置为有效数据<br>&bull; PAT_VL4：最低4个元素设置为有效数据<br>&bull; PAT_VL8：最低8个元素设置为有效数据<br>&bull; PAT_VL16：最低16个元素设置为有效数据<br>&bull; PAT_VL32：最低32个元素设置为有效数据<br>&bull; PAT_VL64：最低64个元素设置为有效数据<br>&bull; PAT_VL128：最低128个元素设置为有效数据<br>&bull; PAT_M3：下标为3的倍数的元素设置为有效数据<br>&bull; PAT_M4：下标为4的倍数的元素设置为有效数据<br>&bull; PAT_H：低一半的元素设置为有效数据<br>&bull; PAT_Q：低四分之一的元素设置为有效数据<br>&bull; PAT_ALLF：所有元素设置为无效元素，均不参与计算 |

## 返回值说明

vector_bool，掩码寄存器。

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 掩码寄存器的数量上限为8，超过上限的掩码寄存器会写入预留的8K Unified Buffer（UB）内存中，可能引起性能劣化。编译器会自动复用生命周期结束的寄存器和预留内存，若两者均可用，优先复用寄存器。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id10 -->
以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id10 -->

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

constexpr uint32_t CHUNK_BYTES = 768;
constexpr uint32_t DATA_BYTES = 1541;
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT = (DATA_BYTES + CHUNK_BYTES - 1) / CHUNK_BYTES;
constexpr uint32_t BUFFER_BYTES = CHUNK_BYTES * REPEAT_COUNT;

// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint8_t);
__simd_vf__ inline void exercise(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint8_t value8;
    vector_uint16_t value16;
    vector_uint32_t value32;
    uint32_t remaining = DATA_BYTES;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * CHUNK_BYTES;
        const uint32_t valid_count = remaining > CHUNK_BYTES ? CHUNK_BYTES : remaining;
        // 保留剩余有效元素数，供更新掩码处理尾块。
        uint32_t remaining_length = valid_count > 256 ? 256 : valid_count;
        uint32_t remaining_length_2 = valid_count > 256 ? (valid_count - 256) / sizeof(uint16_t) : 0;
        if (remaining_length_2 > 128) remaining_length_2 = 128;
        uint32_t remaining_length_3 = valid_count > 512 ? (valid_count - 512) / sizeof(uint32_t) : 0;
        if (remaining_length_3 > 64) remaining_length_3 = 64;
        vector_bool mask8 = asc_create_mask_b8(PAT_ALL);
        vector_bool mask16 = asc_create_mask_b16(PAT_ALL);
        vector_bool mask32 = asc_create_mask_b32(PAT_ALL);
        asc_duplicate_scalar(value8, static_cast<uint8_t>(1), mask8);
        asc_duplicate_scalar(value16, static_cast<uint16_t>(1), mask16);
        asc_duplicate_scalar(value32, static_cast<uint32_t>(1), mask32);
        asc_storealign(output + offset, value8, asc_update_mask_b8(remaining_length));
        asc_storealign(reinterpret_cast<__ubuf__ uint16_t*>(output + offset + 256), value16,
            asc_update_mask_b16(remaining_length_2));
        asc_storealign(reinterpret_cast<__ubuf__ uint32_t*>(output + offset + 512), value32,
            asc_update_mask_b32(remaining_length_3));
        remaining -= valid_count;
    }
}

__global__ __vector__ void asc_create_mask_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
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
        for (uint32_t i = 0; i < end && i < 256; ++i) golden[offset + i] = 1;
        for (uint32_t i = 256; i < end && i < 512; i += sizeof(uint16_t)) golden[offset + i] = 1;
        for (uint32_t i = 512; i < end; i += sizeof(uint32_t)) golden[offset + i] = 1;
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
    asc_create_mask_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    print_data("Golden bytes", golden);
    bool passed = true;
    for (uint32_t i = 0; i < DATA_BYTES; ++i) passed = passed && output[i] == golden[i];
    std::cout << (passed ? "[Success] asc_create_mask_b8/asc_create_mask_b16/asc_create_mask_b32 passed." : "[Failed] asc_create_mask_b8/asc_create_mask_b16/asc_create_mask_b32 failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
