---
okftype: reference.api
description: "按掩码对矢量数据寄存器中的half/bfloat16_t/float（同类型）元素执行TRUNC（向零截断），1:1（类型与元素数量不变），支持引用参数与函数返回值两种输出形式。"
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, rounding_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "舍入与饱和模式说明"
version: ">=9.0.0"
version_changes: []
---
# asc_trunc

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

头文件路径为：`"c_api/reg_compute/reg_convert.h"`。

将`src`中的浮点数元素按照TRUNC（向零方向截断）截断到整数值，结果仍保持原浮点数据类型。接口支持通过函数返回值返回结果，也支持通过引用和参数输出结果：前者直接返回截断结果，后者将截断结果写入`dst`。截断规则等价于C标准库`trunc`语义：向零方向舍入，即丢弃小数部分取整，正数向0方向取整（如1.7 -> 1.0），负数向0方向取整（如-1.7 -> -1.0），整数值保持不变。未被`mask`筛选的元素置零。

关于舍入模式的详细说明，请参见[舍入模式与饱和模式](rounding_mode.md)。

计算公式如下：

$$
dst[i] = trunc(src[i])
$$

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```cpp
// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dtype> asc_trunc(vector_<dtype> src,
                                                vector_bool mask)

// 通过引用和参数输出结果（占位符形式）
__simd_callee__ inline void asc_trunc(vector_<dtype>& dst,
                                      vector_<dtype> src,
                                      vector_bool mask)
```

### dtype支持数据类型

`dtype`取值为：`half`、`bfloat16_t`、`float`。

### 函数原型典型示例

```cpp
// 通过函数返回值返回结果
__simd_callee__ inline vector_half asc_trunc(vector_half src,
                                             vector_bool mask)

// 通过引用和参数输出结果
__simd_callee__ inline void asc_trunc(vector_half& dst,
                                      vector_half src,
                                      vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的矢量数据寄存器，dtype须与src一致，掩码为真位置写入向零方向截断结果（仍以原浮点类型表示整数），掩码为假位置清零。 |
| src | 输入 | 源矢量数据寄存器，dtype须与dst一致，提供待截断的浮点元素。 |
| mask | 输入 | 源掩码寄存器，用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。`mask`未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 通过函数返回值返回结果：返回`vector_half`、`vector_bfloat16_t`或`vector_float`类型的矢量数据寄存器，保存舍入结果。
- 通过引用和参数输出结果：无返回值，舍入结果写入`dst`。

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- `dst`与`mask`为矢量数据寄存器/掩码寄存器。
- float类型只支持不饱和模式。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510&& ./main
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

constexpr uint32_t ELEMENTS_PER_REGISTER = 128;
constexpr uint32_t DATA_BYTES = 133 * sizeof(uint8_t);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint8_t);
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// 按完整寄存器块扩展UB容量，避免尾轮整寄存器访问越界；有效范围仍由掩码控制。
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t BUFFER_BYTES = TOTAL_LENGTH * sizeof(uint16_t);
constexpr uint32_t PADDED_BUFFER_BYTES = PADDED_ELEMENT_COUNT * sizeof(uint16_t);

__simd_vf__ inline void trunc_vf(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b16(remaining_length);
        vector_half src;
        asc_loadalign(src, reinterpret_cast<__ubuf__ half*>(input + offset * sizeof(uint16_t)));
        vector_half dst = asc_trunc(src, mask);
        asc_storealign(reinterpret_cast<__ubuf__ half*>(output + offset * sizeof(uint16_t)), dst, mask);
    }
}

__global__ __vector__ void asc_trunc_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[PADDED_BUFFER_BYTES];
    __ubuf__ uint8_t input_local[PADDED_BUFFER_BYTES];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, PADDED_BUFFER_BYTES);
    asc_copy_gm2ub_align(output_local, input, PADDED_BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<trunc_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, PADDED_BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace
int main()
{
    constexpr uint8_t input_pattern[] = {0x00, 0x3c};
    constexpr uint8_t golden_pattern[] = {0x00, 0x3c};
    std::vector<uint8_t> input(PADDED_BUFFER_BYTES);
    std::vector<uint8_t> output(PADDED_BUFFER_BYTES, 0xff);
    std::vector<uint8_t> golden(PADDED_BUFFER_BYTES);
    for (size_t i = 0; i < PADDED_BUFFER_BYTES; ++i) {
        input[i] = input_pattern[i % sizeof(input_pattern)];
        golden[i] = (i < BUFFER_BYTES) ? golden_pattern[i % sizeof(golden_pattern)] : input[i];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), PADDED_BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), PADDED_BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint8_t), input.data(), input.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_trunc_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    std::cout << "Conversion: vector_half -> vector_half" << std::endl;
    print_data("Input 1.0 (raw bytes)", input);
    print_data("Output (raw bytes)", output);
    print_data("Golden 1.0 (raw bytes)", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_trunc passed." : "[Failed] asc_trunc failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
