# asc_unsqueeze

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

将dst中数据根据mask进行解压缩，或将解压缩结果通过函数返回值返回。解压缩方式：dst中第0个元素置为0，dst中的第i个元素等于mask中从第0个到第(i-1)个元素中1的数量。mask最高位被忽略，不参与统计。

具体算法如图1所示，dst的首位为0。对于后续元素，与dst[i-1]对应的有效mask位为1时，dst[i]的值为dst[i-1] + 1；对应的有效mask位为0时，dst[i]的值为dst[i-1]。

**图1** unsqueeze流程

![unsqueeze流程](../../figures/asc_unsqueeze.png)

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```c
// 通过函数返回值返回结果。
__simd_callee__ inline vector_uint8_t asc_unsqueeze_u8(vector_bool mask)
__simd_callee__ inline vector_int8_t asc_unsqueeze_s8(vector_bool mask)
__simd_callee__ inline vector_uint16_t asc_unsqueeze_u16(vector_bool mask)
__simd_callee__ inline vector_int16_t asc_unsqueeze_s16(vector_bool mask)
__simd_callee__ inline vector_uint32_t asc_unsqueeze_u32(vector_bool mask)
__simd_callee__ inline vector_int32_t asc_unsqueeze_s32(vector_bool mask)

// 通过引用参数输出结果。
__simd_callee__ inline void asc_unsqueeze(vector_<dtype>& dst,
                                          vector_bool mask)
```

#### dtype支持数据类型
dtype支持的数据类型：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t。返回值类型接口通过函数名后缀区分数据类型，对应关系如下：

| 函数名后缀 | 数据类型 |
|---|---|
| u8 | uint8_t |
| s8 | int8_t |
| u16 | uint16_t |
| s16 | int16_t |
| u32 | uint32_t |
| s32 | int32_t |

#### 函数原型典型示例

```c
// 示例：对int8_t矢量数据寄存器执行解压缩，通过函数返回值返回结果。
__simd_callee__ inline vector_int8_t asc_unsqueeze_s8(vector_bool mask)

// 示例：对int8_t矢量数据寄存器执行解压缩。
__simd_callee__ inline void asc_unsqueeze(vector_int8_t& dst,
                                          vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输入/输出 | 源和目的矢量数据寄存器，仅无返回值类型接口包含该参数，接口在掩码前缀计数过程中写入计数结果。 |
| mask | 输入 | 源掩码寄存器，用于提供dst解压缩信息。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

对于返回值类型接口，返回保存解压缩结果的矢量数据寄存器，数据类型由函数名后缀决定（参见函数原型中的对应关系表）。

## 约束说明

- 本接口仅在AIV上生效，非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用，dst与mask为矢量数据寄存器/掩码寄存器。

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

constexpr uint32_t ELEMENT_COUNT = 64;
__simd_vf__ inline void unsqueeze(__ubuf__ uint32_t* dst)
{
    vector_uint32_t dst_reg;
    uint32_t count = ELEMENT_COUNT;
    vector_bool mask = asc_update_mask_b32(count);
    asc_unsqueeze(dst_reg, mask);
    asc_storealign(dst, dst_reg, mask);
}
__global__ __vector__ void asc_unsqueeze_kernel(__gm__ uint32_t* dst)
{
    asc_init();
    __ubuf__ uint32_t dst_local[ELEMENT_COUNT];
    unsqueeze(dst_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst, dst_local, ELEMENT_COUNT * sizeof(uint32_t));
    asc_sync();
}
} // namespace
int main()
{
    std::vector<uint32_t> input = {ELEMENT_COUNT};
    std::vector<uint32_t> output(ELEMENT_COUNT, 0), golden(ELEMENT_COUNT);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) golden[i] = i;
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), (ELEMENT_COUNT) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    asc_unsqueeze_kernel<<<1, 0>>>(dst_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint32_t), dst_device, output.size() * sizeof(uint32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input mask count", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_unsqueeze passed." : "[Failed] asc_unsqueeze failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
