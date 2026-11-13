---
okftype: reference.api
description: "把float矢量按rd、rn、rna、ru、rz舍入与饱和或非饱和模式转换为bfloat16_t，并按dst_pos标签写入目的寄存器偶数或奇数索引位置、其余位置清零，结果也可作为返回值，仅在AIV生效。"
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, rounding_mode, saturation_mode]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "需遵循Reg矢量计算C API通用约束"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "舍入与饱和模式说明"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_saturation_flag.md
    kind: requires
    condition: "使用饱和或非饱和模式前需通过asc_set_saturation_flag接口配置"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: parameter_added
    scope: "section:函数原型"
    scope_basis: resolved
    note: "新增显式编译期位置标签dst_pos（std::integral_constant<asc_position_mode, EVEN/ODD>），2:1位宽比下选择写入目的寄存器偶数或奇数索引位置、其他位置清零；原位置后缀形式自9.2.0起废弃，见deprecated_interface对应页。"
  - since: "9.2.0"
    kind: overload_added
    scope: "section:函数原型"
    scope_basis: resolved
    note: "新增通过函数返回值返回结果的重载（返回vector_bfloat16_t），9.2.0两形式合计40条声明（5种舍入×饱和与非饱和×2个位置×2种结果形式）。"
---
# asc_float2bfloat16

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

根据`mask`将`src`中的每个`float`类型数据转换成`bfloat16_t`类型，并支持多种舍入模式与饱和模式，结果写入`dst`。
由于源操作数与目的操作数类型位宽比为2:1，写入数据时需要将一个`VL`大小的数据分为两部分，根据参数`dst_pos`选择写入`dst`索引为偶数的位置或奇数的位置。伪代码如下：

```python
# 位置判断仅用于说明接口语义，实际位置参数为编译期常量。
# VL表示矢量数据寄存器位宽，取值256字节。
def asc_float2bfloat16_rn(dst, src, mask, dst_pos):
    offset = 0 if dst_pos == ASC_POSITION_EVEN else 1
    for i in range(VL // 4):
        dst[2 * i] = 0
        dst[2 * i + 1] = 0
        if mask[i]:
            dst[2 * i + offset] = bfloat16(src[i])  # float -> bfloat16，RINT舍入
```

关于舍入模式的详细说明，请参见[舍入模式与饱和模式](rounding_mode.md)。

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```c
// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_float2bfloat16_<round_mode><sat_mode>(vector_bfloat16_t& dst,
                                                                      vector_float src,
                                                                      vector_bool mask,
                                                                      std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)

__simd_callee__ inline void asc_float2bfloat16_<round_mode><sat_mode>(vector_bfloat16_t& dst,
                                                                      vector_float src,
                                                                      vector_bool mask,
                                                                      std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)

// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_<round_mode><sat_mode>(vector_float src,
                                                                                   vector_bool mask,
                                                                                   std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)

__simd_callee__ inline vector_bfloat16_t asc_float2bfloat16_<round_mode><sat_mode>(vector_float src,
                                                                                   vector_bool mask,
                                                                                   std::integral_constant<asc_position_mode, asc_position_mode::ODD> dst_pos)
```

**占位符说明如下：**

- `<round_mode>`表示支持的舍入模式，支持`rd`（`FLOOR`）、`rn`（`RINT`）、`rna`（`ROUND`）、`ru`（`CEIL`）和`rz`（`TRUNC`）。
- `<sat_mode>`表示饱和/非饱和模式，为空时表示非饱和模式，取值为`_sat`时表示饱和模式。

### 函数原型典型示例

```c
// RINT舍入模式，非饱和模式，数据写入索引为偶数的位置
__simd_callee__ inline void asc_float2bfloat16_rn(vector_bfloat16_t& dst,
                                                  vector_float src,
                                                  vector_bool mask,
                                                  std::integral_constant<asc_position_mode, asc_position_mode::EVEN> dst_pos)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| ------ | --------- | ------------------------------------------------------------------------------------------------------------------ |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| mask | 输入 | 掩码寄存器，用于控制各元素是否参与计算。`mask`中与元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |
| dst_pos | 输入 | 位置选择标签（编译期标签分发，通过编译期重载选择对应实现），类型为`std::integral_constant<asc_position_mode, asc_position_mode::EVEN>`或`std::integral_constant<asc_position_mode, asc_position_mode::ODD>`。取`ASC_POSITION_EVEN`时选择将结果写入目的操作数索引为偶数的位置，其他位置清零；取`ASC_POSITION_ODD`时选择将结果写入目的操作数索引为奇数的位置，其他位置清零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 通过引用参数输出结果的函数原型无返回值。
- 通过函数返回值输出结果的函数原型返回计算结果，返回值类型与对应引用输出函数原型中`dst`参数的类型一致（去除引用）。

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 位置选择标签参数仅能使用编译期常量，编译器据此在编译期分发至对应的重载。
- 位置选择标签选择目的操作数的写入地址，其他位置清零。
- `src`与`dst`的数据类型需要与函数原型匹配。
- `mask`掩码位为0时，`dst`对应元素置0。
- 结果写入`dst`的奇数索引位置时，偶数索引位置置0；结果写入偶数索引位置时，奇数索引位置置0。
- 使用饱和或非饱和模式时，需要通过[asc_set_saturation_flag](../../spr/asc_set_saturation_flag.md)接口配置，具体配置要求请参见该接口说明。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```c
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

constexpr uint32_t ELEMENTS_PER_REGISTER = 256;
constexpr uint32_t DATA_BYTES = 260;
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint8_t);
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
constexpr uint32_t PADDED_BUFFER_BYTES = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
__simd_vf__ inline void convert(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = TOTAL_LENGTH / sizeof(uint8_t);
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b8(remaining_length);
        vector_bfloat16_t dst_0;
        vector_float src_0;
        asc_loadalign(dst_0, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_0, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_rd_sat(dst_0, src_0, mask, ASC_POSITION_EVEN);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_0, mask);
        vector_bfloat16_t dst_1;
        vector_float src_1;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_1, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_1, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_rd(dst_1, src_1, mask, ASC_POSITION_ODD);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_1, mask);
        vector_bfloat16_t dst_2;
        vector_float src_2;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_2, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_2, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_rd_sat(dst_2, src_2, mask, ASC_POSITION_ODD);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_2, mask);
        vector_bfloat16_t dst_3;
        vector_float src_3;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_3, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_3, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_rn(dst_3, src_3, mask, ASC_POSITION_EVEN);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_3, mask);
        vector_bfloat16_t dst_4;
        vector_float src_4;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_4, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_4, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_rn_sat(dst_4, src_4, mask, ASC_POSITION_EVEN);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_4, mask);
        vector_bfloat16_t dst_5;
        vector_float src_5;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_5, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_5, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_rn(dst_5, src_5, mask, ASC_POSITION_ODD);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_5, mask);
        vector_bfloat16_t dst_6;
        vector_float src_6;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_6, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_6, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_rn_sat(dst_6, src_6, mask, ASC_POSITION_ODD);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_6, mask);
        vector_bfloat16_t dst_7;
        vector_float src_7;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_7, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_7, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_rna(dst_7, src_7, mask, ASC_POSITION_EVEN);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_7, mask);
        vector_bfloat16_t dst_8;
        vector_float src_8;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_8, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_8, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_rna_sat(dst_8, src_8, mask, ASC_POSITION_EVEN);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_8, mask);
        vector_bfloat16_t dst_9;
        vector_float src_9;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_9, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_9, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_rna(dst_9, src_9, mask, ASC_POSITION_ODD);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_9, mask);
        vector_bfloat16_t dst_10;
        vector_float src_10;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_10, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_10, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_rna_sat(dst_10, src_10, mask, ASC_POSITION_ODD);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_10, mask);
        vector_bfloat16_t dst_11;
        vector_float src_11;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_11, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_11, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_ru(dst_11, src_11, mask, ASC_POSITION_EVEN);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_11, mask);
        vector_bfloat16_t dst_12;
        vector_float src_12;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_12, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_12, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_ru_sat(dst_12, src_12, mask, ASC_POSITION_EVEN);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_12, mask);
        vector_bfloat16_t dst_13;
        vector_float src_13;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_13, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_13, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_ru(dst_13, src_13, mask, ASC_POSITION_ODD);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_13, mask);
        vector_bfloat16_t dst_14;
        vector_float src_14;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_14, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_14, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_ru_sat(dst_14, src_14, mask, ASC_POSITION_ODD);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_14, mask);
        vector_bfloat16_t dst_15;
        vector_float src_15;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_15, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_15, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_rz(dst_15, src_15, mask, ASC_POSITION_EVEN);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_15, mask);
        vector_bfloat16_t dst_16;
        vector_float src_16;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_16, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_16, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_rz_sat(dst_16, src_16, mask, ASC_POSITION_EVEN);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_16, mask);
        vector_bfloat16_t dst_17;
        vector_float src_17;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_17, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_17, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_rz(dst_17, src_17, mask, ASC_POSITION_ODD);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_17, mask);
        vector_bfloat16_t dst_18;
        vector_float src_18;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_18, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_18, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_rz_sat(dst_18, src_18, mask, ASC_POSITION_ODD);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_18, mask);
        vector_bfloat16_t dst_19;
        vector_float src_19;
        // 不同目的寄存器复用同一段UB，等前一次写入完成后再读取。
        asc_mem_bar(VST_VLD);
        asc_loadalign(dst_19, reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset));
        asc_loadalign(src_19, reinterpret_cast<__ubuf__ float*>(input + offset));
        asc_float2bfloat16_rd(dst_19, src_19, mask, ASC_POSITION_EVEN);
        asc_storealign(reinterpret_cast<__ubuf__ bfloat16_t*>(output + offset), dst_19, mask);
    }
}
__global__ __vector__ void asc_float2bfloat16_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
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
    asc_vf_call<convert>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, PADDED_BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace
int main()
{
    constexpr uint8_t input_pattern[] = {0x00, 0x00, 0x80, 0x3f};
    constexpr uint8_t golden_pattern[] = {0x80, 0x3f, 0x00, 0x00};
    std::vector<uint8_t> input(PADDED_BUFFER_BYTES);
    std::vector<uint8_t> output(PADDED_BUFFER_BYTES, 0xff);
    std::vector<uint8_t> golden(PADDED_BUFFER_BYTES);
    for (size_t i = 0; i < PADDED_BUFFER_BYTES; ++i) {
        input[i] = input_pattern[i % sizeof(input_pattern)];
        golden[i] = (i < TOTAL_LENGTH) ? golden_pattern[i % sizeof(golden_pattern)] : input[i];
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
    asc_float2bfloat16_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    std::cout << "Conversion: vector_float -> vector_bfloat16_t" << std::endl;
    print_data("Input 1.0 (raw bytes)", input);
    print_data("Output (raw bytes)", output);
    print_data("Golden 1.0 (raw bytes)", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_float2bfloat16_rd/asc_float2bfloat16_rd_sat/"
                           "asc_float2bfloat16_rn/asc_float2bfloat16_rn_sat/"
                           "asc_float2bfloat16_rna/asc_float2bfloat16_rna_sat/"
                           "asc_float2bfloat16_ru/asc_float2bfloat16_ru_sat/"
                           "asc_float2bfloat16_rz/asc_float2bfloat16_rz_sat/"
                           "ASC_POSITION_ODD/ASC_POSITION_EVEN passed."
                         : "[Failed] asc_float2bfloat16_rd/asc_float2bfloat16_rd_sat/"
                           "asc_float2bfloat16_rn/asc_float2bfloat16_rn_sat/"
                           "asc_float2bfloat16_rna/asc_float2bfloat16_rna_sat/"
                           "asc_float2bfloat16_ru/asc_float2bfloat16_ru_sat/"
                           "asc_float2bfloat16_rz/asc_float2bfloat16_rz_sat/"
                           "ASC_POSITION_ODD/ASC_POSITION_EVEN failed.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
