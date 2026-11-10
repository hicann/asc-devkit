---
okftype: reference.api
description: "把源矢量寄存器中被 squeeze_mask 选中的元素压缩到目的寄存器并连续非对齐搬出到 UB，满 32B 主块直接写入、不足 32B 的非对齐尾块暂存在 src0 非对齐寄存器中等待 finalize 收尾，用 AR 寄存器记录已搬出字节数作为下次写入偏移，仅在 AIV 上生效。"
tag:
  - roles: [data_movement]
  - operations: [compact, store]
  - traits: [post_update, unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: requires
    condition: "矢量数据寄存器、掩码寄存器与 AR 寄存器的类型定义"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "Reg 矢量计算 C API 通用约束"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_finalize.md
    kind: related
    condition: "结束连续搬出时需调用该接口写入尾块"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign_init.md
    kind: requires
    condition: "首次调用前需用该接口清零 AR 寄存器"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: requires
    condition: "与前后 Reg 数据搬运接口访问同一 UB 地址且存在依赖时需同步"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_get_squeeze_status.md
    kind: related
    condition: "用于读取 AR 寄存器累计的已搬出字节数"
version: ">=9.2.0"
version_changes: []
---
# asc_squeeze_and_storeunalign

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

头文件路径为：`"c_api/reg_compute/compute/reg_permute_sel.h"`。

Reg计算数据搬运接口。将源矢量数据寄存器`src1`中被掩码`squeeze_mask`选中的元素压缩到目的矢量数据寄存器`dst`，并将这些元素连续搬出到Unified Buffer（UB）。压缩后的元素按其在`src1`中的顺序从`dst`的低位开始连续排列，`dst`中的剩余元素置0。

本接口使用AR特殊寄存器记录当前连续搬出的有效数据总字节数。首次调用本接口前，需调用[asc_squeeze_and_storeunalign_init](asc_squeeze_and_storeunalign_init.md)清零AR寄存器。接口执行前，AR寄存器中的值表示相对于`dst_addr`的写入偏移，单位为字节；接口执行后，本次选中元素的总字节数会累加到AR寄存器中。连续调用本接口并保持`dst_addr`和`src0`不变，可将多组筛选结果连续写入UB，无需手动更新地址。AR寄存器中的值可通过[asc_get_squeeze_status](../../spr/asc_get_squeeze_status.md)接口获取。

接口将可完整组成32B的主块写入UB，未满32B的尾块暂存在非对齐寄存器`src0`中。完成一次或多次连续搬出后，需调用[asc_squeeze_and_storeunalign_finalize](asc_squeeze_and_storeunalign_finalize.md)将尾块写入UB。

本接口为Reg矢量搬运接口，仅在AIV上生效。

## 函数原型

```c
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ <dtype>* dst_addr,
                                                         vector_<dtype>& dst,
                                                         vector_store_unalign& src0,
                                                         vector_<dtype> src1,
                                                         vector_bool squeeze_mask)
```

### dtype支持数据类型

`dtype`取值为：`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```c
__simd_callee__ inline void asc_squeeze_and_storeunalign(__ubuf__ float* dst_addr,
                                                         vector_float& dst,
                                                         vector_store_unalign& src0,
                                                         vector_float src1,
                                                         vector_bool squeeze_mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst_addr | 输出 | 目的操作数在UB上的起始地址。地址不要求32B对齐，但需按`sizeof(dtype)`字节对齐。连续搬出时，实际写入位置为`dst_addr`加AR寄存器记录的字节偏移。接口不会修改`dst_addr`的值。 |
| dst | 输出 | 目的操作数（矢量数据寄存器），通过引用返回压缩结果。被选中的元素连续排列在低位，剩余元素置0。 |
| src0 | 输入、输出 | 非对齐寄存器，长度为32B，用于暂存未满32B的尾块。连续搬出及收尾时需使用同一个寄存器；首次调用无需预先初始化该寄存器。 |
| src1 | 输入 | 源操作数（矢量数据寄存器）。 |
| squeeze_mask | 输入 | 源操作数掩码（掩码寄存器）。对应位置为1时选择该元素，为0时不选择该元素。 |

矢量数据寄存器、掩码寄存器和非对齐寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

### 通用约束

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- UB容量上限为256KB，用户可用容量随编译选项与编程场景变化。`dst_addr`加AR寄存器记录的累计字节数不可超过实际可用容量，否则会触发写越界异常。
- 如果本指令与其他指令存在数据依赖，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化，防止出现异常数据。

### 指令约束

- 每组连续搬出操作开始前，需调用一次[asc_squeeze_and_storeunalign_init](asc_squeeze_and_storeunalign_init.md)。连续调用本接口时，需保持`dst_addr`和`src0`不变；最后一次调用后，需调用一次[asc_squeeze_and_storeunalign_finalize](asc_squeeze_and_storeunalign_finalize.md)完成收尾。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;


constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t INPUT_COUNT = 139;
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT = (INPUT_COUNT + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
constexpr uint32_t PADDED_INPUT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t SELECTED_PER_REGISTER = 22;
constexpr uint32_t OUTPUT_COUNT = (INPUT_COUNT / ELEMENTS_PER_REGISTER) * SELECTED_PER_REGISTER +
    ((INPUT_COUNT % ELEMENTS_PER_REGISTER) + 2) / 3;
constexpr uint32_t PADDED_OUTPUT_COUNT = ((OUTPUT_COUNT + ELEMENTS_PER_REGISTER - 1) /
    ELEMENTS_PER_REGISTER) * ELEMENTS_PER_REGISTER;
constexpr uint32_t LAST_SELECTED_COUNT = ((INPUT_COUNT % ELEMENTS_PER_REGISTER) + 2) / 3;

constexpr uint32_t DATA_BYTES = INPUT_COUNT * sizeof(float);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = INPUT_COUNT;
__simd_vf__ inline void pack_selected(
    __ubuf__ float* output, __ubuf__ float* register_output, __ubuf__ float* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_float src;
    vector_float dst;
    vector_store_unalign unalign_reg;
    vector_bool pattern_mask = asc_create_mask_b32(PAT_M3);
    vector_bool full_mask = asc_create_mask_b32(PAT_ALL);
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = INPUT_COUNT;
    vector_bool store_mask = asc_create_mask_b32(PAT_ALL);

    asc_squeeze_and_storeunalign_init();
    for (uint16_t i = 0; i < repeat_time; ++i) {
        vector_bool valid_mask = asc_update_mask_b32(remaining_length);
        vector_bool squeeze_mask = asc_and(pattern_mask, valid_mask, full_mask);
        asc_loadalign(src, input + i * ELEMENTS_PER_REGISTER);
        asc_squeeze_and_storeunalign(output, dst, unalign_reg, src, squeeze_mask);
    }
    asc_squeeze_and_storeunalign_finalize(output, unalign_reg);
    asc_storealign(register_output, dst, store_mask);
}

__global__ __vector__ void squeeze_and_store_kernel(
    __gm__ float* output, __gm__ float* register_output, __gm__ float* input)
{
    asc_init();
    __ubuf__ float input_local[PADDED_INPUT_COUNT];
    __ubuf__ float output_local[PADDED_OUTPUT_COUNT];
    __ubuf__ float register_local[ELEMENTS_PER_REGISTER];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, PADDED_INPUT_COUNT * sizeof(float));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<pack_selected>(output_local, register_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, PADDED_OUTPUT_COUNT * sizeof(float));
    asc_copy_ub2gm_align(register_output, register_local, ELEMENTS_PER_REGISTER * sizeof(float));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> input(PADDED_INPUT_COUNT, 0.0F);
    std::vector<float> output(PADDED_OUTPUT_COUNT, 0.0F);
    std::vector<float> register_output(ELEMENTS_PER_REGISTER, 0.0F);
    std::vector<float> golden;
    std::vector<float> register_golden(ELEMENTS_PER_REGISTER, 0.0F);
    for (uint32_t i = 0; i < INPUT_COUNT; ++i) input[i] = static_cast<float>(i) + 0.5F;
    for (uint32_t repeat = 0; repeat < REPEAT_COUNT; ++repeat) {
        const uint32_t offset = repeat * ELEMENTS_PER_REGISTER;
        const uint32_t valid_count = INPUT_COUNT - offset > ELEMENTS_PER_REGISTER ?
            ELEMENTS_PER_REGISTER : INPUT_COUNT - offset;
        for (uint32_t i = 0; i < valid_count; i += 3) {
            golden.push_back(input[offset + i]);
        }
    }
    const uint32_t last_offset = (REPEAT_COUNT - 1) * ELEMENTS_PER_REGISTER;
    for (uint32_t i = 0; i < LAST_SELECTED_COUNT; ++i) {
        register_golden[i] = input[last_offset + i * 3];
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float* input_device = nullptr;
    float* output_device = nullptr;
    float* register_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), input.size() * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), PADDED_OUTPUT_COUNT * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&register_device), register_output.size() * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(float), input.data(), input.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    squeeze_and_store_kernel<<<1, 0>>>(output_device, register_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), output_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(register_output.data(), register_output.size() * sizeof(float), register_device,
        register_output.size() * sizeof(float), ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = std::equal(output.begin(), output.begin() + OUTPUT_COUNT, golden.begin()) &&
        std::equal(register_output.begin(), register_output.begin() + LAST_SELECTED_COUNT, register_golden.begin());
    std::cout << (passed ? "[Success] squeeze and unaligned store completed."
                         : "[Failed] output mismatch.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtFree(register_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
