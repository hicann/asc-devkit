---
okftype: reference.api
description: "从UB的16字节对齐地址读取VL/16长度数据，将每个bit重复两次得到VL/8长度的2倍上采样结果，并通过函数返回值返回掩码寄存器。"
tag:
  - roles: [data_movement]
  - operations: [load]
  - traits: [aligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/load/asc_loadalign_upsample.md
    kind: related
    condition: "对齐搬入模式中目的为掩码寄存器的原型，本接口为其返回值形式"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "Reg矢量计算C API通用约束"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: requires
    condition: "与其他指令存在数据依赖时需插入该同步指令"
version: ">=9.2.0"
version_changes: []
---
# asc_loadalign_mask_upsample

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

头文件路径为：`"c_api/reg_compute/load/loadalign.h"`。

从Unified Buffer（UB）中16字节对齐的起始地址读取VL/16长度数据，将每个bit重复两次，得到VL/8长度数据，并通过函数返回值返回掩码寄存器。

本接口与[asc_loadalign_upsample](asc_loadalign_upsample.md)对齐搬入模式中目的操作数为掩码寄存器的原型功能相同，区别在于本接口通过函数返回值返回结果。

**图1** 上采样搬入掩码寄存器

![](../../../../figures/capi_loadalign_mask_upsample.png)

本接口为Reg矢量搬运接口，仅在AIV上生效。

## 函数原型

```c
__simd_callee__ inline vector_bool asc_loadalign_mask_upsample(__ubuf__ uint32_t* src)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| src | 输入 | 源UB地址，实际读取地址必须按16字节对齐，读取VL/16长度数据。 |

## 返回值说明

返回保存2倍上采样搬入结果的掩码寄存器，类型为`vector_bool`，有效数据长度为VL/8。

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- `src`的实际读取地址必须按16字节对齐，且实际读取范围必须在UB地址空间内且不越界，否则会报错。
- 如果本指令与其他指令存在数据依赖，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化，防止出现异常数据。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
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


constexpr uint32_t MASK_OUTPUT_WORD_COUNT = 11;
constexpr uint32_t MASK_DATA_WORDS_PER_REPEAT = 4;
constexpr uint32_t MASK_STORAGE_WORDS_PER_REPEAT = 8;
constexpr uint32_t MASK_OUTPUT_WORDS_PER_REPEAT = 8;
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT =
    (MASK_OUTPUT_WORD_COUNT + MASK_OUTPUT_WORDS_PER_REPEAT - 1) / MASK_OUTPUT_WORDS_PER_REPEAT;
constexpr uint32_t MASK_STORAGE_WORD_COUNT = MASK_STORAGE_WORDS_PER_REPEAT * REPEAT_COUNT;
constexpr uint32_t MASK_OUTPUT_STORAGE_WORD_COUNT = MASK_OUTPUT_WORDS_PER_REPEAT * REPEAT_COUNT;
constexpr uint32_t TOTAL_MASK_STORAGE_BYTES = MASK_STORAGE_WORD_COUNT * sizeof(uint32_t);
constexpr uint32_t TOTAL_MASK_OUTPUT_BYTES = MASK_OUTPUT_STORAGE_WORD_COUNT * sizeof(uint32_t);

// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = MASK_OUTPUT_WORD_COUNT;
constexpr uint32_t DATA_BYTES = TOTAL_LENGTH * sizeof(uint32_t);
__simd_vf__ inline void asc_loadalign_mask_upsample_vf(
    __ubuf__ uint32_t* output, __ubuf__ uint32_t* mask_input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    uint32_t remaining = MASK_OUTPUT_WORD_COUNT;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t valid_count = remaining > MASK_OUTPUT_WORDS_PER_REPEAT ?
            MASK_OUTPUT_WORDS_PER_REPEAT : remaining;
        const uint32_t input_offset = i * MASK_STORAGE_WORDS_PER_REPEAT;
        const uint32_t output_offset = i * MASK_OUTPUT_WORDS_PER_REPEAT;
        vector_bool mask = asc_loadalign_mask_upsample(mask_input + input_offset);
        asc_storealign(output + output_offset, mask);
        remaining -= valid_count;
    }
}

__global__ __vector__ void asc_loadalign_mask_upsample_kernel(
    __gm__ uint32_t* output, __gm__ uint32_t* mask_input)
{
    asc_init();
    __ubuf__ uint32_t output_local[MASK_OUTPUT_STORAGE_WORD_COUNT];
    __ubuf__ uint32_t mask_local[MASK_STORAGE_WORD_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(mask_local, mask_input, TOTAL_MASK_STORAGE_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = asc_get_vf_len() / sizeof(uint32_t) / 8;
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<asc_loadalign_mask_upsample_vf>(output_local, mask_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, TOTAL_MASK_OUTPUT_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint32_t> output(MASK_OUTPUT_STORAGE_WORD_COUNT, 0);
    std::vector<uint32_t> golden(MASK_OUTPUT_STORAGE_WORD_COUNT, 0x33333333u);
    std::vector<uint32_t> mask_input(MASK_STORAGE_WORD_COUNT, 0);
    for (uint32_t repeat = 0; repeat < REPEAT_COUNT; ++repeat) {
        const uint32_t offset = repeat * MASK_STORAGE_WORDS_PER_REPEAT;
        for (uint32_t i = 0; i < MASK_DATA_WORDS_PER_REPEAT; ++i) {
            mask_input[offset + i] = 0x55555555u;
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* output_device = nullptr;
    uint32_t* mask_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), TOTAL_MASK_OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&mask_device), TOTAL_MASK_STORAGE_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(mask_device, TOTAL_MASK_STORAGE_BYTES, mask_input.data(), TOTAL_MASK_STORAGE_BYTES,
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_loadalign_mask_upsample_kernel<<<1, 0>>>(output_device, mask_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), TOTAL_MASK_OUTPUT_BYTES, output_device, TOTAL_MASK_OUTPUT_BYTES,
        ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint32_t i = 0; i < MASK_OUTPUT_WORD_COUNT; ++i) passed = passed && output[i] == golden[i];
    std::cout << (passed ? "[Success]" : "[Failed]")
              << " asc_loadalign_mask_upsample example." << std::endl;
    aclrtFree(output_device);
    aclrtFree(mask_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
