---
okftype: reference.api
description: "按掩码将矢量数据寄存器中的int4x2_t元素按1:4 读（src 每4个存储单元取一组）规则进行转换，转换为half，支持引用参数与函数返回值两种输出形式。"
tag:
  - roles: [compute]
  - operations: [conversion]
  - traits: [elementwise, split_high_low]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_convert/rounding_mode.md
    kind: related
    condition: "舍入与饱和模式说明"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "page"
    scope_basis: kind_default
    note: "CANN 9.2.0 Release Notes 第61条明确 asc_int4x22int16、asc_int4x22half 等新增显式四分之一位置标签重载；该页的 asc_position_quarter_mode 四种位置值与引用/返回值两种输出形式组合的显式标签重载于 9.2.0 补齐，9.0.0 仅有位置模式重载。"
---
# asc_int4x22half

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

`int4x2_t`将两个连续的4bit有符号整数打包为一个8bit存储单元。本接口根据`mask`将`src`中参与计算的4bit有符号整数转换为`half`类型，可通过引用参数或函数返回值输出结果。

由于每个4bit有符号整数的位宽小于1Byte，实际处理时，同一`int4x2_t`存储单元内的两个4bit有符号整数共同对应`mask`中的一个掩码位。该掩码位为1时，两个4bit有符号整数同时参与转换，转换结果作为两个连续的`half`类型元素写入`dst`；为0时，对应的两个`half`类型元素均置0。

由于单个源数据与目的数据的类型位宽比为1:4，`src`中的`int4x2_t`存储单元按索引分为四组。参数`src_quarter_pos`从`src`中离散选取每隔4个存储单元的位置读取数据，具体为索引0、4、8、…，索引1、5、9、…，索引2、6、10、…或索引3、7、11、…。

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```cpp
// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_int4x22half(vector_half& dst,
                                            vector_int4x2_t src,
                                            vector_bool mask,
                                            std::integral_constant<asc_position_quarter_mode, <quarter_pos>> src_quarter_pos)

// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_half asc_int4x22half(vector_int4x2_t src,
                                                   vector_bool mask,
                                                   std::integral_constant<asc_position_quarter_mode, <quarter_pos>> src_quarter_pos)
```

**占位符说明如下：**

`<quarter_pos>`支持`asc_position_quarter_mode`枚举类的四个取值：

```c
enum class asc_position_quarter_mode {
    DISPERSE_FIRST_QUARTER = 0,
    DISPERSE_SECOND_QUARTER,
    DISPERSE_THIRD_QUARTER,
    DISPERSE_FOURTH_QUARTER
};
```

### 函数原型典型示例

```cpp
// 通过引用参数输出结果
__simd_callee__ inline void asc_int4x22half(vector_half& dst,
                                            vector_int4x2_t src,
                                            vector_bool mask,
                                            std::integral_constant<asc_position_quarter_mode,
                                                                   asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> src_quarter_pos)

// 通过函数返回值返回结果
__simd_callee__ inline vector_half asc_int4x22half(vector_int4x2_t src,
                                                   vector_bool mask,
                                                   std::integral_constant<asc_position_quarter_mode,
                                                                          asc_position_quarter_mode::DISPERSE_FIRST_QUARTER> src_quarter_pos)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| ------ | --------- | ---- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| mask | 输入 | 掩码寄存器，用于控制各元素是否参与计算。`mask`中与元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |
| src_quarter_pos | 输入 | 四分之一位置选择标签（编译期标签分发，通过编译期重载选择对应实现），用于选择源操作数的读取位置。可传入对应的编译期标签：`ASC_DISPERSE_FIRST_QUARTER`、`ASC_DISPERSE_SECOND_QUARTER`、`ASC_DISPERSE_THIRD_QUARTER`和`ASC_DISPERSE_FOURTH_QUARTER`。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 通过引用参数输出结果的函数原型无返回值。
- 通过函数返回值输出结果的函数原型返回计算结果，返回值类型与对应引用输出函数原型中`dst`参数的类型一致（去除引用）。

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 位置选择标签参数仅能使用上述编译期常量。
- `mask`掩码位为0时，`dst`对应元素置0。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

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
        vector_half dst;
        vector_int4x2_t src;
        asc_loadalign(dst, reinterpret_cast<__ubuf__ half*>(output + offset));
        asc_loadalign(src, reinterpret_cast<__ubuf__ int4b_t*>(input + offset));
        asc_int4x22half(dst, src, mask, ASC_DISPERSE_FIRST_QUARTER);
        asc_storealign(reinterpret_cast<__ubuf__ half*>(output + offset), dst, mask);
    }
}
} // namespace

__global__ __vector__ void asc_int4x22half_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[PADDED_BUFFER_BYTES];
    __ubuf__ uint8_t input_local[PADDED_BUFFER_BYTES];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, PADDED_BUFFER_BYTES);
    asc_copy_gm2ub_align(output_local, output, PADDED_BUFFER_BYTES);
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

int main()
{
    constexpr uint8_t input_pattern[] = {0x11};
    constexpr uint8_t golden_pattern[] = {0x00, 0x3c};
    std::vector<uint8_t> input(PADDED_BUFFER_BYTES);
    std::vector<uint8_t> output(PADDED_BUFFER_BYTES, 0);
    std::vector<uint8_t> golden(PADDED_BUFFER_BYTES);
    for (size_t i = 0; i < PADDED_BUFFER_BYTES; ++i) {
        input[i] = input_pattern[i % sizeof(input_pattern)];
        golden[i] = (i < TOTAL_LENGTH) ? golden_pattern[i % sizeof(golden_pattern)] : input[i];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), PADDED_BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), PADDED_BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, PADDED_BUFFER_BYTES, input.data(), PADDED_BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, PADDED_BUFFER_BYTES, output.data(), PADDED_BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_int4x22half_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), PADDED_BUFFER_BYTES, output_device, PADDED_BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (size_t i = 0; i < TOTAL_LENGTH; ++i) passed = passed && (output[i] == golden[i]);
    std::cout << (passed ? "[Success] asc_int4x22half passed."
                         : "[Failed] asc_int4x22half failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
