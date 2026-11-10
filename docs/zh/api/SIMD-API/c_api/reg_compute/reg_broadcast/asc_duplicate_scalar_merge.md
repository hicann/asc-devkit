---
okftype: reference.api
description: "把标量 value 广播到目的矢量数据寄存器被掩码筛选的位置，未筛选位置保留原值，是 asc_duplicate_scalar 的合并模式版本。"
tag:
  - roles: [compute]
  - operations: [broadcast]
  - traits: [elementwise, scalar_operand]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: requires
    condition: "矢量数据寄存器与掩码寄存器的类型定义"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "Reg 矢量计算 C API 通用约束"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_broadcast/asc_duplicate_scalar.md
    kind: related
    condition: "正文对比说明两者广播源相同但本接口保留未筛选元素"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: requires
    condition: "与前后 Reg 数据搬运接口访问同一 UB 地址且存在写后读/写后写依赖时需同步"
version: ">=9.2.0"
version_changes: []
---
# asc_duplicate_scalar_merge

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

头文件路径为：`"c_api/reg_compute/compute/reg_broadcast.h"`。

将标量`value`广播到目的操作数`dst`中被`mask`筛选的位置，`dst`中未被`mask`筛选的元素保留原值。

本接口与[asc_duplicate_scalar](asc_duplicate_scalar.md)的广播数据来源相同，区别是本接口采用合并模式保留`dst`中未参与计算的元素。

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```cpp
// 占位符形式
__simd_callee__ inline void asc_duplicate_scalar_merge(vector_<dtype>& dst,
                                                       <dtype> value,
                                                       vector_bool mask)
```

**占位符说明如下：**

- `<dtype>`取值为：`int8_t`、`uint8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```cpp
__simd_callee__ inline void asc_duplicate_scalar_merge(vector_float& dst,
                                                       float value,
                                                       vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输入/输出 | 目的操作数（矢量数据寄存器）。被`mask`筛选的元素写入`value`，未被筛选的元素保留调用接口前的值。元素数据类型需要与`value`保持一致。 |
| value | 输入 | 源操作数（标量），作为待广播的数据。数据类型需要与`dst`的元素数据类型保持一致。 |
| mask | 输入 | 目的操作数元素操作的有效指示（掩码寄存器）。对应比特位为1时，`dst`中的元素写入`value`；为0时，`dst`中的元素保留原值。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 调用接口前需要初始化`dst`，否则未被`mask`筛选的元素值不确定。
- 同一寄存器的数据依赖由硬件保序，无需额外插入同步指令。本接口与前后Reg数据搬运接口之间，如果不同寄存器访问同一Unified Buffer（UB）地址且存在写后读或写后写依赖，需要调用[asc_mem_bar](../reg_sync/asc_mem_bar.md)进行同步。

## 调用示例

将以下代码保存为`example.asc`后，可通过`bisheng`命令编译运行。其中，`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

以下示例将标量`1.0f`广播到`dst`的前4个元素，未被`mask`筛选的其余元素保留原值。

```cpp
#include <cstdint>
#include <iostream>
#include <vector>

#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t DATA_BYTES = 133 * sizeof(float);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(float);
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// 按完整寄存器块扩展UB容量，避免尾轮整寄存器访问越界；有效范围仍由掩码控制。
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t ACTIVE_COUNT = 16;
constexpr uint32_t BUFFER_BYTES = TOTAL_LENGTH * sizeof(float);
constexpr uint32_t PADDED_BUFFER_BYTES = PADDED_ELEMENT_COUNT * sizeof(float);
constexpr float INITIAL_VALUE = -1.0f;
constexpr float FILL_VALUE = 1.0f;

__simd_vf__ inline void duplicate_scalar_merge(__ubuf__ float* dst, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_float dst_reg;
    // 剩余有效元素数由asc_update_mask_b32逐轮更新，无需按offset重新计算。
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        // active_mask控制本轮广播的生效位置：最多启用前ACTIVE_COUNT个元素，尾块不足时只启用有效元素。
        // 例如133个元素按每轮64个处理，ACTIVE_COUNT为16时，三轮分别启用16、16、5个元素。
        // remaining_length_active是本轮的独立计数；update_mask会将它扣减到0，因此每轮都要重新计算。
        // 必须在生成store_mask之前计算，因为生成store_mask会更新跨轮使用的remaining_length。
        uint32_t remaining_length_active = remaining_length < ACTIVE_COUNT ? remaining_length : ACTIVE_COUNT;
        vector_bool active_mask = asc_update_mask_b32(remaining_length_active);
        // store_mask控制写回范围，三轮分别写回64、64、5个有效元素，并自动更新剩余数量。
        // 广播未生效的位置也可能需要写回，因此不能直接用active_mask代替store_mask。
        vector_bool store_mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(dst_reg, dst + offset);
        asc_duplicate_scalar_merge(dst_reg, FILL_VALUE, active_mask);
        asc_storealign(dst + offset, dst_reg, store_mask);
    }
}

__global__ __vector__ void asc_duplicate_scalar_merge_kernel(__gm__ float* dst)
{
    asc_init();
    __ubuf__ float dst_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(dst_local, dst, PADDED_BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<duplicate_scalar_merge>(dst_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst, dst_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> output(PADDED_ELEMENT_COUNT, INITIAL_VALUE);
    std::vector<float> golden(TOTAL_LENGTH, INITIAL_VALUE);
    for (uint32_t repeat = 0; repeat < REPEAT_COUNT; ++repeat) {
        const uint32_t offset = repeat * ELEMENTS_PER_REGISTER;
        const uint32_t remaining = TOTAL_LENGTH - offset;
        // 保留剩余有效元素数，供更新掩码处理尾块。
        const uint32_t remaining_length = remaining < ELEMENTS_PER_REGISTER ? remaining : ELEMENTS_PER_REGISTER;
        const uint32_t remaining_length_active = remaining_length < ACTIVE_COUNT ? remaining_length : ACTIVE_COUNT;
        for (uint32_t i = 0; i < remaining_length_active; ++i) golden[offset + i] = FILL_VALUE;
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), PADDED_BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(dst_device, PADDED_BUFFER_BYTES, output.data(), PADDED_BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_duplicate_scalar_merge_kernel<<<1, 0>>>(dst_device);
    aclrtSynchronizeDevice();
    output.resize(TOTAL_LENGTH);
    aclrtMemcpy(output.data(), output.size() * sizeof(float), dst_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_duplicate_scalar_merge passed."
                        : "[Failed] asc_duplicate_scalar_merge failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
