# asc_create_mask

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

根据传入的pat_mode生成对应的掩码寄存器，支持b8、b16、b32三种位宽模式。pat_mode参数指定mask的模式，即指定哪些位置的元素参与计算。

位宽模式说明：
- b8模式：每个bit对应一个8bit元素（共256元素），用于8bit数据类型的矢量计算。
- b16模式：每2个bit为一组对应一个16bit元素（共128元素），用于16bit数据类型的矢量计算。
- b32模式：每4个bit为一组对应一个32bit元素（共64元素），用于32bit数据类型的矢量计算。

**图1** asc_create_mask原理

![asc_create_mask原理](../../figures/capi_create_mask.png)

本接口为Reg数据类型定义接口，仅在AIV上生效。

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

- 本接口仅在AIV上生效，非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用。
- 掩码寄存器的数量上限为8，超过上限的掩码寄存器会写入预留的8K Unified Buffer（UB）内存中，可能引起性能劣化。编译器会自动复用生命周期结束的寄存器和预留内存，若两者均可用，优先复用寄存器。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id10 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510; ./main
```
<!-- end id10 -->

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

constexpr uint32_t BUFFER_BYTES = 768;

__simd_vf__ inline void exercise(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input)
{
    vector_uint8_t value8;
    vector_uint16_t value16;
    vector_uint32_t value32;
    vector_bool mask8 = asc_create_mask_b8(PAT_ALL);
    vector_bool mask16 = asc_create_mask_b16(PAT_ALL);
    vector_bool mask32 = asc_create_mask_b32(PAT_ALL);
    asc_duplicate_scalar(value8, static_cast<uint8_t>(1), mask8);
    asc_duplicate_scalar(value16, static_cast<uint16_t>(1), mask16);
    asc_duplicate_scalar(value32, static_cast<uint32_t>(1), mask32);
    asc_storealign(output, value8, mask8);
    asc_storealign(reinterpret_cast<__ubuf__ uint16_t*>(output + 256), value16, mask16);
    asc_storealign(reinterpret_cast<__ubuf__ uint32_t*>(output + 512), value32, mask32);
}

__global__ __vector__ void asc_create_mask_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
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
    asc_create_mask_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    print_data("Golden bytes", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_create_mask_b8/asc_create_mask_b16/asc_create_mask_b32 passed." : "[Failed] asc_create_mask_b8/asc_create_mask_b16/asc_create_mask_b32 failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
