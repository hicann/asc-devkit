# asc_pack_to_low

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

头文件路径为：`"c_api/reg_compute/compute/reg_permute_sel.h"`。

将源操作数中的元素选取偶数位bit(bool)、低8位（b16）、低16位（b32）写入目的操作数的低半部分。
当源操作数为vector_bool时，其按bit位进行压缩，每2bit将其中的低位bit存储至目的操作数的低半部分。

![](../../../../figures/pack_low_draft.png)

## 函数原型

```c
// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_pack_to_low(vector_<dtype_dst>& dst,
                                            vector_<dtype_src> src)

// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dtype_dst> asc_pack_to_low(vector_<dtype_src> src)

// 操作数为掩码寄存器
__simd_callee__ inline void asc_pack_to_low(vector_bool& dst,
                                            vector_bool src)

// 操作数为掩码寄存器，通过函数返回值返回结果
__simd_callee__ inline vector_bool asc_pack_to_low(vector_bool src)
```

### dtype支持数据类型

dtype_src与dtype_dst支持的数据类型对如下：

| dtype_src | dtype_dst |
|---|---|
| uint16_t | uint8_t |
| int16_t | uint8_t |
| uint32_t | uint16_t |
| int32_t | uint16_t |

### 函数原型典型示例

```c
// 示例：将uint16_t矢量数据寄存器压缩为uint8_t。
__simd_callee__ inline void asc_pack_to_low(vector_uint8_t& dst,
                                            vector_uint16_t src)

// 示例：将uint16_t矢量数据寄存器压缩为uint8_t，通过函数返回值返回结果。
__simd_callee__ inline vector_uint8_t asc_pack_to_low(vector_uint16_t src)
```

## 参数说明

**表1** 参数说明

| 参数名       | 输入/输出 | 描述               |
| --------- | ----- | ---------------- |
| dst       | 输出    | 目的操作数（矢量数据寄存器/掩码寄存器）。            |
| src | 输入    | 源操作数（矢量数据寄存器/掩码寄存器）。            |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 通过引用参数输出结果的函数原型无返回值，压缩结果写入`dst`。
- 通过函数返回值返回结果的函数原型返回压缩结果，类型为矢量数据寄存器或掩码寄存器，与`dst`的数据类型一致。

## 约束说明

- 本接口非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用，源操作数和目的操作数为矢量数据寄存器或掩码寄存器。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
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
void PrintData(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

template <typename T>
bool CompareRangeData(const std::vector<T>& actual, const std::vector<T>& expected,
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

constexpr uint32_t SRC_COUNT = 128;
constexpr uint32_t DST_COUNT = 256;

__simd_vf__ inline void Pack(__ubuf__ uint8_t* low, __ubuf__ uint8_t* high, __ubuf__ uint16_t* src)
{
    vector_uint8_t low_reg;
    vector_uint8_t high_reg;
    vector_uint16_t src_reg;
    uint32_t count = DST_COUNT;
    vector_bool dst_mask = asc_update_mask_b8(count);
    asc_loadalign(src_reg, src);
    asc_duplicate_scalar(low_reg, static_cast<uint8_t>(0), dst_mask);
    asc_duplicate_scalar(high_reg, static_cast<uint8_t>(0), dst_mask);
    asc_pack_to_low(low_reg, src_reg);
    
    asc_storealign(low, low_reg, dst_mask);
    
}

__global__ __vector__ void AscPackToLowKernel(__gm__ uint8_t* low, __gm__ uint8_t* high, __gm__ uint16_t* src)
{
    asc_init();
    __ubuf__ uint8_t low_local[DST_COUNT];
    __ubuf__ uint8_t high_local[DST_COUNT];
    __ubuf__ uint16_t src_local[SRC_COUNT];
    asc_copy_gm2ub_align(src_local, src, SRC_COUNT * sizeof(uint16_t));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    Pack(low_local, high_local, src_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(low, low_local, DST_COUNT);
    asc_copy_ub2gm_align(high, high_local, DST_COUNT);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint16_t> src(SRC_COUNT);
    std::vector<uint8_t> low(DST_COUNT, 0);
    std::vector<uint8_t> high(DST_COUNT, 0);
    std::vector<uint8_t> golden_low(DST_COUNT, 0);
    std::vector<uint8_t> golden_high(DST_COUNT, 0);
    for (uint32_t i = 0; i < SRC_COUNT; ++i) {
        src[i] = static_cast<uint16_t>(0x1200U + i);
        golden_low[i] = static_cast<uint8_t>(src[i]);
        
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* src_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src_device), (SRC_COUNT) * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* low_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&low_device), (DST_COUNT) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* high_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&high_device), (DST_COUNT) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src_device, src.size() * sizeof(uint16_t), src.data(), src.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    AscPackToLowKernel<<<1, 0>>>(low_device, high_device, src_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(low.data(), low.size() * sizeof(uint8_t), low_device, low.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(high.data(), high.size() * sizeof(uint8_t), high_device, high.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    PrintData("Input", src);
    PrintData("Low output", low);
    PrintData("Low Golden", golden_low);
    PrintData("High output", high);
    PrintData("High Golden", golden_high);
    const bool passed = CompareRangeData(low, golden_low, 0, SRC_COUNT);
    std::cout << (passed ? "[Success] asc_pack_to_low passed." : "[Failed] asc_pack_to_low failed.") << std::endl;
    aclrtFree(high_device);
    aclrtFree(low_device);
    aclrtFree(src_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
