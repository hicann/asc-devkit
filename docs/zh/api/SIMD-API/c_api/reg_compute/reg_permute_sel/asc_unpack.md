# asc_unpack

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

将src中低半部分或高半部分的元素以扩充位宽的方式写入dst或通过函数返回值返回，支持：

- asc_unpack_lower：取src的低半部分（LOWER），每个元素高位填0扩充位宽后写入dst或通过函数返回值返回。
- asc_unpack_upper：取src的高半部分（HIGHER），每个元素高位填0扩充位宽后写入dst或通过函数返回值返回。

不同数据类型的扩充方式：

- 无符号整型：高位填0扩充。例如vector_uint8_t到vector_uint16_t，src中每个uint8_t元素高位补0扩展为uint16_t。
- 有符号整型：保持符号位扩充。例如vector_int8_t到vector_int16_t，src中每个int8_t元素按符号位扩展为int16_t。
- bool类型（掩码寄存器）：将每bit展开为2bit，高位填0。

矢量数据寄存器unpack流程如图1所示：

**图1** 矢量数据寄存器unpack流程

![矢量数据寄存器unpack流程](../../figures/capi_unpack_reg.png)

掩码数据寄存器unpack流程如图2所示：

**图2** 掩码数据寄存器unpack流程

![掩码数据寄存器unpack流程](../../figures/capi_unpack_mask.png)

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

### 矢量数据寄存器解压缩

```c
// 解压缩低位，通过函数返回值返回结果（占位符形式）。
__simd_callee__ inline vector_<dtype_dst> asc_unpack_lower(vector_<dtype_src> src)
// 解压缩高位，通过函数返回值返回结果（占位符形式）。
__simd_callee__ inline vector_<dtype_dst> asc_unpack_upper(vector_<dtype_src> src)

// 解压缩低位，通过引用参数输出结果（占位符形式）。
__simd_callee__ inline void asc_unpack_lower(vector_<dtype_dst>& dst,
                                             vector_<dtype_src> src)
// 解压缩高位，通过引用参数输出结果（占位符形式）。
__simd_callee__ inline void asc_unpack_upper(vector_<dtype_dst>& dst,
                                             vector_<dtype_src> src)
```

#### dtype支持数据类型
dtype_src与dtype_dst支持的数据类型对如下：

| dtype_src | dtype_dst |
|---|---|
| uint8_t | uint16_t |
| int8_t | int16_t |
| uint16_t | uint32_t |
| int16_t | int32_t |

#### 函数原型典型示例

```c
// 示例：将uint16_t矢量数据寄存器解压缩为uint32_t，通过函数返回值返回结果。
__simd_callee__ inline vector_uint32_t asc_unpack_lower(vector_uint16_t src)

// 示例：将uint16_t矢量数据寄存器解压缩为uint32_t。
__simd_callee__ inline void asc_unpack_lower(vector_uint32_t& dst,
                                             vector_uint16_t src)
```

### 掩码寄存器解压缩

```c
// 解压缩低位，通过函数返回值返回结果。
__simd_callee__ inline vector_bool asc_unpack_lower(vector_bool src)
// 解压缩高位，通过函数返回值返回结果。
__simd_callee__ inline vector_bool asc_unpack_upper(vector_bool src)

// 解压缩低位，通过引用参数输出结果。
__simd_callee__ inline void asc_unpack_lower(vector_bool& dst,
                                             vector_bool src)
// 解压缩高位，通过引用参数输出结果。
__simd_callee__ inline void asc_unpack_upper(vector_bool& dst,
                                             vector_bool src)
```

## 参数说明

**表1** 参数说明（操作数为矢量数据寄存器）

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的矢量数据寄存器，仅无返回值类型接口包含该参数，读取src低半段或高半段元素，扩展位宽后写入dst全部有效位置，dst全部有效位置均被改写。 |
| src | 输入 | 源矢量数据寄存器，取src低半段或高半段元素扩展位宽写入dst。 |

**表2** 参数说明（操作数为掩码寄存器）

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的掩码寄存器，仅无返回值类型接口包含该参数，读取src低半段或高半段元素，扩展位宽后写入dst全部有效位置，dst全部有效位置均被改写。 |
| src | 输入 | 源掩码寄存器，取src低半段或高半段元素扩展位宽写入dst。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

对于返回值类型接口，返回保存解压缩结果的矢量数据寄存器或掩码寄存器，数据类型与`dst`保持一致。

## 约束说明

- 本接口仅在AIV上生效，非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用，dst与src均为矢量数据寄存器或掩码寄存器。
- 低半段解包与高半段解包配合可分别解包源寄存器前半段与后半段，两次调用即可将整个源寄存器的窄类型数据全部扩展为宽类型写入两个目的寄存器。

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

constexpr uint32_t SRC_COUNT = 256;
constexpr uint32_t DST_COUNT = 128;
__simd_vf__ inline void unpack(__ubuf__ uint16_t* lower, __ubuf__ uint16_t* upper, __ubuf__ uint8_t* src)
{
    vector_uint16_t lower_reg;
    vector_uint16_t upper_reg;
    vector_uint8_t src_reg;
    uint32_t count = DST_COUNT;
    vector_bool mask = asc_update_mask_b16(count);
    asc_loadalign(src_reg, src);
    asc_unpack_lower(lower_reg, src_reg);
    asc_unpack_upper(upper_reg, src_reg);
    asc_storealign(lower, lower_reg, mask);
    asc_storealign(upper, upper_reg, mask);
}
__global__ __vector__ void asc_unpack_kernel(__gm__ uint16_t* lower, __gm__ uint16_t* upper, __gm__ uint8_t* src)
{
    asc_init();
    __ubuf__ uint16_t lower_local[DST_COUNT];
    __ubuf__ uint16_t upper_local[DST_COUNT];
    __ubuf__ uint8_t src_local[SRC_COUNT];
    asc_copy_gm2ub_align(src_local, src, SRC_COUNT);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    unpack(lower_local, upper_local, src_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(lower, lower_local, DST_COUNT * sizeof(uint16_t));
    asc_copy_ub2gm_align(upper, upper_local, DST_COUNT * sizeof(uint16_t));
    asc_sync();
}
} // namespace
int main()
{
    std::vector<uint8_t> src(SRC_COUNT);
    std::vector<uint16_t> lower(DST_COUNT, 0), upper(DST_COUNT, 0);
    std::vector<uint16_t> golden_lower(DST_COUNT), golden_upper(DST_COUNT);
    for (uint32_t i = 0; i < SRC_COUNT; ++i) src[i] = static_cast<uint8_t>(i);
    for (uint32_t i = 0; i < DST_COUNT; ++i) { golden_lower[i] = src[i]; golden_upper[i] = src[DST_COUNT + i]; }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* src_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src_device), (SRC_COUNT) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint16_t* lower_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&lower_device), (DST_COUNT) * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint16_t* upper_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&upper_device), (DST_COUNT) * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src_device, src.size() * sizeof(uint8_t), src.data(), src.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_unpack_kernel<<<1, 0>>>(lower_device, upper_device, src_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(lower.data(), lower.size() * sizeof(uint16_t), lower_device, lower.size() * sizeof(uint16_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(upper.data(), upper.size() * sizeof(uint16_t), upper_device, upper.size() * sizeof(uint16_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", src);
    print_data("Lower output", lower);
    print_data("Lower Golden", golden_lower);
    print_data("Upper output", upper);
    print_data("Upper Golden", golden_upper);
    const bool passed = compare_data(lower, golden_lower) && compare_data(upper, golden_upper);
    std::cout << (passed ? "[Success] asc_unpack_lower/asc_unpack_upper passed."
                         : "[Failed] asc_unpack_lower/asc_unpack_upper failed.") << std::endl;
    aclrtFree(lower_device);
    aclrtFree(src_device);
    aclrtFree(upper_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
