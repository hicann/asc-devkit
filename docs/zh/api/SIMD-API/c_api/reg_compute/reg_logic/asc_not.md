# asc_not

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

根据`mask`对源操作数`src`执行按位取反操作，将结果作为返回值返回或写入目的操作数`dst`。

- 矢量数据寄存器按位取反：对矢量数据寄存器执行按`dtype`位宽按位取反，结果为矢量数据寄存器。

- 掩码寄存器按位取反：对掩码寄存器按位取反，结果为掩码寄存器。

计算公式如下：

$$
dst_i = \sim src_i
$$

本接口仅在AIV上生效。

## 函数原型

### 矢量数据寄存器按位取反

```c
// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dtype> asc_not(vector_<dtype> src,
                                              vector_bool mask)

// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_not(vector_<dtype>& dst,
                                    vector_<dtype> src,
                                    vector_bool mask)
```

#### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`half`、`int32_t`、`uint32_t`、`float`。

#### 函数原型典型示例

```c
// 示例：对half矢量数据寄存器执行按位取反
__simd_callee__ inline vector_half asc_not(vector_half src,
                                           vector_bool mask)

__simd_callee__ inline void asc_not(vector_half& dst,
                                    vector_half src,
                                    vector_bool mask)
```

### 掩码寄存器按位取反

```c
// 通过函数返回值返回结果
__simd_callee__ inline vector_bool asc_not(vector_bool src,
                                           vector_bool mask)

// 通过引用参数输出结果
__simd_callee__ inline void asc_not(vector_bool& dst,
                                    vector_bool src,
                                    vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量数据寄存器或掩码寄存器），用于存储取反结果，仅用于无返回值原型。|
| src | 输入 | 源操作数（矢量数据寄存器或掩码寄存器）。|
| mask | 输入 | 源操作数掩码（掩码寄存器）。<br>&bull;源操作数为矢量数据寄存器时，对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。<br>&bull;源操作数为掩码寄存器时，指示在计算过程中哪些bit有效。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 通过函数返回值返回结果的函数原型返回按位取反结果，返回类型与`src`的数据类型一致。
- 通过引用参数输出结果的函数原型无返回值。

## 约束说明

- 通过函数返回值返回结果的函数原型在非AIV上调用返回对应矢量类型的默认构造值。
- 通过引用参数输出结果的函数原型在非AIV上调用直接返回。
- `mask`需通过[掩码设置接口](../../defs/type/data_type_definition.md#掩码寄存器)预先赋值后再传入；未赋值的掩码寄存器内容不确定，会导致有效元素位置错误。
- 参与计算的元素个数由矢量长度（VL）决定：
    - 矢量数据寄存器按位取反中元素个数 = VL ÷ sizeof(dtype)；
    - 掩码寄存器按位取反中比特个数 = VL。
- `mask`比特位为0时，计算结果对应比特位写0。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

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

__simd_vf__ inline void compute(__ubuf__ int32_t* dst, __ubuf__ int32_t* src)
{
    vector_int32_t dst_reg;
    vector_int32_t src_reg;
    uint32_t count = ELEMENT_COUNT;
    vector_bool mask = asc_update_mask_b32(count);
    asc_loadalign(src_reg, src);
    asc_not(dst_reg, src_reg, mask);
    asc_storealign(dst, dst_reg, mask);
}

__global__ __vector__ void asc_not_kernel(__gm__ int32_t* dst, __gm__ int32_t* src)
{
    asc_init();
    __ubuf__ int32_t dst_local[ELEMENT_COUNT];
    __ubuf__ int32_t src_local[ELEMENT_COUNT];
    asc_copy_gm2ub_align(src_local, src, ELEMENT_COUNT * sizeof(int32_t));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    compute(dst_local, src_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst, dst_local, ELEMENT_COUNT * sizeof(int32_t));
    asc_sync();
}
} // namespace

int main()
{
    std::vector<int32_t> src(ELEMENT_COUNT);
    std::vector<int32_t> output(ELEMENT_COUNT, 0);
    std::vector<int32_t> golden(ELEMENT_COUNT);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        src[i] = static_cast<int32_t>(0x0f0f0000U + i);
        golden[i] = ~src[i];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    int32_t* src_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src_device), (ELEMENT_COUNT) * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    int32_t* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), (ELEMENT_COUNT) * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src_device, src.size() * sizeof(int32_t), src.data(), src.size() * sizeof(int32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_not_kernel<<<1, 0>>>(dst_device, src_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int32_t), dst_device, output.size() * sizeof(int32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", src);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_not passed." : "[Failed] asc_not failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
