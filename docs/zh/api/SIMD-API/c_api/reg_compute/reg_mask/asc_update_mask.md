# asc_update_mask

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/reg_compute/compute/reg_mask.h"`。

根据元素个数value生成mask，并自动将value减去当前向量处理单元的元素个数。支持b8、b16、b32三种位宽模式，由于VL=256B，各模式的向量处理单元元素个数为：

- b8模式：每次处理256个元素，用于8 bit数据类型的矢量计算。
- b16模式：每次处理128个元素，用于16 bit数据类型的矢量计算。
- b32模式：每次处理64个元素，用于32 bit数据类型的矢量计算。

**图1**  asc_update_mask更新流程

![asc_update_mask更新流程](../../figures/capi_update_mask.png)

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```c
__simd_callee__ inline vector_bool asc_update_mask_b8(uint32_t& value)
__simd_callee__ inline vector_bool asc_update_mask_b16(uint32_t& value)
__simd_callee__ inline vector_bool asc_update_mask_b32(uint32_t& value)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| value | 输入/输出 | 元素个数。调用后自动减去当前向量处理单元的元素个数（b8模式、b16模式、b32模式分别为256、128、64）。<br>执行完一次该接口后，value = (value < VL_T) ? 0 : (value - VL_T)，VL_T表示位宽为VL的矢量数据寄存器中，可存放数据类型T的元素个数。<br>例如，有320个b16数据类型的元素，每次处理128个元素：<br>第一次调用接口，生成的mask对应的元素全为有效数据，value = 320-128 = 192。<br>第二次调用接口，生成的mask对应的元素全为有效数据，value = 192-128 = 64。<br>第三次调用接口，生成的mask对应的元素只有低半部分为有效数据，value = 0。|

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

vector_bool，掩码寄存器。

## 约束说明

- 本接口仅在AIV上生效，非AIV调用直接返回。
- 掩码寄存器的数量上限为8，超过上限的掩码寄存器会写入预留的8K UB内存中，可能引起性能劣化。编译器会自动复用生命周期结束的寄存器和预留内存，若两者均可用，优先复用寄存器。
- 本接口需在Vector Function（`__simd_vf__`标记的函数）内调用。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510&& ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
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

constexpr uint32_t BUFFER_BYTES = 768;

__simd_vf__ inline void exercise(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input)
{
    uint32_t count8 = 256, count16 = 128, count32 = 64;
    vector_uint8_t value8;
    vector_uint16_t value16;
    vector_uint32_t value32;
    vector_bool mask8 = asc_update_mask_b8(count8);
    vector_bool mask16 = asc_update_mask_b16(count16);
    vector_bool mask32 = asc_update_mask_b32(count32);
    asc_duplicate_scalar(value8, static_cast<uint8_t>(1), mask8);
    asc_duplicate_scalar(value16, static_cast<uint16_t>(1), mask16);
    asc_duplicate_scalar(value32, static_cast<uint32_t>(1), mask32);
    asc_storealign(output, value8, mask8);
    asc_storealign(reinterpret_cast<__ubuf__ uint16_t*>(output + 256), value16, mask16);
    asc_storealign(reinterpret_cast<__ubuf__ uint32_t*>(output + 512), value32, mask32);
}

__global__ __vector__ void asc_update_mask_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[BUFFER_BYTES];
    __ubuf__ uint8_t input_local[BUFFER_BYTES];
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_copy_gm2ub_align(output_local, input, BUFFER_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    exercise(output_local, input_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(BUFFER_BYTES), output(BUFFER_BYTES, 0xff), golden(BUFFER_BYTES);
    for (uint32_t i = 0; i < BUFFER_BYTES; ++i) input[i] = static_cast<uint8_t>(i % 251);
    for (uint32_t i = 0; i < 256; ++i) golden[i] = 1;
    for (uint32_t i = 256; i < 512; i += sizeof(uint16_t)) golden[i] = 1;
    for (uint32_t i = 512; i < BUFFER_BYTES; i += sizeof(uint32_t)) golden[i] = 1;
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
    asc_update_mask_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    print_data("Golden bytes", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_update_mask_b8/asc_update_mask_b16/asc_update_mask_b32 passed." : "[Failed] asc_update_mask_b8/asc_update_mask_b16/asc_update_mask_b32 failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
