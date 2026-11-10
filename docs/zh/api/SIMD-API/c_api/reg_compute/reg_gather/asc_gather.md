---
okftype: reference.api
description: "根据索引矢量寄存器把源矢量元素按元素收集到目的矢量的Reg收集接口，dst的每个元素取自src中索引指定的位置，提供写入dst引用与函数返回值两种形式，支持8/16/32位多种数据与索引类型，仅在AIV上生效。"
tag:
  - roles: [compute]
  - operations: [gather]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "矢量数据寄存器的类型定义与Vector Length说明"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "Reg矢量计算C API通用约束见该页通用约束节"
  - path: docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md
    kind: related
    condition: "产品型号与NPU架构映射说明"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "overload:asc_gather__Rvf32_vf32_vu32"
    scope_basis: resolved
    note: "寄存器源收集模式新增 float 数据重载：dst/src 为 vector_float、index 为 vector_uint32_t；依据 CANN_9_2_0 Release Notes 第 55 行（寄存器源 asc_gather 新增 vector_float 重载）与 upstream/9.2.0 首次出现的该声明（9.1.0 及更早无该组合）。"
  - since: "9.2.0"
    kind: overload_added
    scope: "section:函数原型"
    scope_basis: resolved
    note: "新增 13 种 dtype/index_dtype 组合的函数返回值形式重载（asc_gather__vi8_vu8 至 asc_gather__vf32_vu32），原 dst 引用形式保留；这批重载属 CANN_9_2_0 为 Reg 收集等接口补充函数返回值形式的批次（Release Notes 第 54 行），并由 upstream/9.2.0 首次出现这些声明印证；因同一批共 13 条重载，按函数原型小节聚合登记。"
---
# asc_gather

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

头文件路径为：`"c_api/reg_compute/compute/reg_gather.h"`。

根据索引位置`index`将源操作数`src`按元素收集到目的操作数`dst`中。

**寄存器源收集模式**：源操作数与目的操作数均为矢量数据寄存器。按 `index` 矢量寄存器中保存的逐元素索引，从`src`矢量数据寄存器中选择对应位置的元素写入`dst`对应位置，不使用掩码。

**图 1** 寄存器源收集模式

![寄存器源收集模式](../../../../figures/asc_gather_vec.png)

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```c
// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_gather(vector_<dtype>& dst,
                                       vector_<dtype> src,
                                       vector_<index_dtype> index)

// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dtype> asc_gather(vector_<dtype> src,
                                                 vector_<index_dtype> index)
```

### 支持数据类型列表

**表1** 寄存器源收集模式支持数据类型列表

| dtype | index_dtype |
| :---------- | :-------------- |
| int8_t | uint8_t |
| uint8_t | uint8_t |
| hifloat8_t | uint8_t |
| fp8_e8m0_t | uint8_t |
| fp8_e5m2_t | uint8_t |
| fp8_e4m3fn_t | uint8_t |
| int16_t | uint16_t |
| uint16_t | uint16_t |
| half | uint16_t |
| bfloat16_t | uint16_t |
| int32_t | uint32_t |
| uint32_t | uint32_t |
| float | uint32_t |

### 函数原型典型示例

```c
// 示例：dtype/index_dtype数据类型分别为: int16_t/uint16_t
// 通过引用参数输出结果
__simd_callee__ inline void asc_gather(vector_int16_t& dst,
                                       vector_int16_t src,
                                       vector_uint16_t index)

// 通过函数返回值返回结果
__simd_callee__ inline vector_int16_t asc_gather(vector_int16_t src,
                                                 vector_uint16_t index)
```

## 参数说明

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量数据寄存器），dtype须与dst一致。 |
| index | 输入 | 数据索引（矢量数据寄存器）。单位是元素个数，dst[i] = src[index[i]]。 |

矢量数据寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 通过引用参数输出结果的函数原型无返回值。
- 通过函数返回值输出结果的函数原型返回收集结果，返回值类型与对应引用输出函数原型中`dst`参数的类型一致（去除引用）。

## 约束说明

### 通用约束

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 源/目的均为矢量数据寄存器。

### 寄存器源收集模式约束

- `src`为矢量数据寄存器类型，位宽是固定的[Vector Length (VL)](../../defs/type/data_type_definition.md)，存储的元素个数固定。如果`index`中索引值超出当前矢量数据寄存器中能存储的最大元素个数时，按照如下方式处理：设定当前矢量数据寄存器所能存储的最大数据元素个数为`VL_T`, `index`中索引值为`i`，索引值更新为`i % VL_T`。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)。

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

constexpr uint32_t ELEMENTS_PER_REGISTER = 128;
constexpr uint32_t DATA_BYTES = 133 * sizeof(uint16_t);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint16_t);
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// 按完整寄存器块扩展UB容量，避免尾轮整寄存器访问越界；有效范围仍由掩码控制。
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t PADDED_BUFFER_BYTES = PADDED_ELEMENT_COUNT * sizeof(uint16_t);

__simd_vf__ inline void gather_vf(__ubuf__ uint16_t* output, __ubuf__ uint16_t* input, __ubuf__ uint16_t* indices, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b16(remaining_length);
        vector_uint16_t src;
        asc_loadalign(src, input + offset);
        vector_uint16_t index;
        asc_loadalign(index, indices + offset);
        vector_uint16_t dst;
        asc_gather(dst, src, index);
        asc_storealign(output + offset, dst, mask);
    }
}

__global__ __vector__ void asc_gather_kernel(__gm__ uint16_t* output, __gm__ uint16_t* input,
                                             __gm__ uint16_t* indices)
{
    asc_init();
    __ubuf__ uint16_t output_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint16_t input_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint16_t index_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, PADDED_BUFFER_BYTES);
    asc_copy_gm2ub_align(index_local, indices, PADDED_BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint16_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<gather_vf>(output_local, input_local, index_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, PADDED_BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint16_t> input(PADDED_ELEMENT_COUNT, 0);
    std::vector<uint16_t> indices(PADDED_ELEMENT_COUNT, 0);
    std::vector<uint16_t> output(PADDED_ELEMENT_COUNT, 0xff);
    std::vector<uint16_t> golden(PADDED_ELEMENT_COUNT, 0xff);
    for (uint32_t i = 0; i < PADDED_ELEMENT_COUNT; ++i) {
        input[i] = static_cast<uint16_t>(i);
    }
    for (uint32_t offset = 0; offset < TOTAL_LENGTH; offset += ELEMENTS_PER_REGISTER) {
        const uint32_t valid_count = (TOTAL_LENGTH - offset > ELEMENTS_PER_REGISTER) ?
            ELEMENTS_PER_REGISTER : TOTAL_LENGTH - offset;
        for (uint32_t i = 0; i < valid_count; ++i) {
            indices[offset + i] = static_cast<uint16_t>(valid_count - 1 - i);
            golden[offset + i] = input[offset + indices[offset + i]];
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), PADDED_BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint16_t* index_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&index_device), PADDED_BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint16_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), PADDED_BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint16_t), input.data(), input.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(index_device, indices.size() * sizeof(uint16_t), indices.data(), indices.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_gather_kernel<<<1, 0>>>(output_device, input_device, index_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint16_t), output_device, output.size() * sizeof(uint16_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    print_data("Golden", golden);
    bool passed = true;
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) passed = passed && output[i] == golden[i];
    std::cout << (passed ? "[Success] asc_gather completed." : "[Failed] asc_gather output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(index_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
