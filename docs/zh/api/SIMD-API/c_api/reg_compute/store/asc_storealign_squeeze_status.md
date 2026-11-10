---
okftype: reference.api
description: "把asc_squeeze_and_storeunalign后保存在AR特殊寄存器的有效数据长度写入UB，支持直接写入dst或按字节偏移写入两个重载。"
tag:
  - roles: [data_movement]
  - operations: [store]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "正文据此说明矢量数据寄存器与非对齐寄存器"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: related
    condition: "正文据此引用 Reg 矢量计算通用约束"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_squeeze_and_storeunalign.md
    kind: requires
    condition: "调用前需先由该接口完成squeeze操作以产生AR中的有效长度"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: related
    condition: "与其他指令存在数据依赖时插入同步指令串行化"
version: ">=9.2.0"
version_changes: []
---
# asc_storealign_squeeze_status

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

头文件路径为：`"c_api/reg_compute/store/storealign.h"`。

将[asc_squeeze_and_storeunalign](../reg_permute_sel/asc_squeeze_and_storeunalign.md)操作后保存在AR特殊寄存器中的有效数据长度写入Unified Buffer（UB）。调用无`offset`参数的重载时，写入`dst`；调用带`offset`参数的重载时，写入`dst`加字节偏移`offset`的位置。

本接口为Reg矢量搬运接口，仅在AIV上生效。

## 函数原型

```cpp
__simd_callee__ inline void asc_storealign_squeeze_status(__ubuf__ uint32_t* dst)

__simd_callee__ inline void asc_storealign_squeeze_status(__ubuf__ uint32_t* dst,
                                                          int32_t offset)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的操作数在UB上的基地址，类型为`__ubuf__ uint32_t*`。无`offset`参数时，实际写入地址为`dst`；带`offset`参数时，实际写入地址为`dst`加`offset`字节。 |
| offset | 输入 | 仅带`offset`参数的重载使用。相对`dst`基地址的字节偏移量，类型为`int32_t`；无`offset`参数时，默认值为`0`。 |

## 返回值说明

无

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 实际目的地址需4字节对齐，且不得超出实际可用UB范围；无`offset`参数时实际目的地址为`dst`，带`offset`参数时为`dst`加`offset`字节。
- 调用本接口前，需使用[asc_squeeze_and_storeunalign](../reg_permute_sel/asc_squeeze_and_storeunalign.md)完成squeeze操作；写入的值是其当前累计的有效数据字节数。
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
#include "acl/acl.h"
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"

namespace {
constexpr uint8_t MUTEX_ID = 0;

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t DATA_BYTES = 137 * sizeof(uint32_t);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint32_t);
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// 按完整寄存器块扩展UB容量，避免尾轮整寄存器访问越界；有效范围仍由掩码控制。
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t STATUS_WORD_COUNT = 128;
constexpr uint32_t SQUEEZE_STATUS_BYTES = TOTAL_LENGTH * sizeof(float);

__simd_vf__ inline void store_squeeze_status(
    __ubuf__ uint32_t* status, __ubuf__ float* packed, __ubuf__ float* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_float squeezed;
    vector_store_unalign unalign_reg;

    asc_squeeze_and_storeunalign_init();
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_float src;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(src, input + offset); // 将输入数据加载到矢量数据寄存器。
        asc_squeeze_and_storeunalign(packed, squeezed, unalign_reg, src, mask);
    }
    asc_squeeze_and_storeunalign_finalize(packed, unalign_reg);

    // 不传offset时，默认将AR中的有效数据长度写入status起始地址。
    asc_storealign_squeeze_status(status);
}

__global__ __vector__ void store_squeeze_status_kernel(__gm__ uint32_t* output, __gm__ float* input)
{
    asc_init();
    __ubuf__ float input_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float packed_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint32_t status_local[STATUS_WORD_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, sizeof(input_local));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint32_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<store_squeeze_status>(status_local, packed_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, status_local, sizeof(status_local));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> input(PADDED_ELEMENT_COUNT, 0.0F);
    std::vector<uint32_t> output(STATUS_WORD_COUNT, 0U);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        input[i] = static_cast<float>(i);
    }

    if (aclInit(nullptr) != ACL_SUCCESS || aclrtSetDevice(0) != ACL_SUCCESS) {
        return 1;
    }
    float* input_device = nullptr;
    uint32_t* output_device = nullptr;
    bool success =
        aclrtMalloc(reinterpret_cast<void**>(&input_device), input.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST) ==
            ACL_SUCCESS &&
        aclrtMalloc(
            reinterpret_cast<void**>(&output_device), output.size() * sizeof(uint32_t), ACL_MEM_MALLOC_HUGE_FIRST) ==
            ACL_SUCCESS;
    if (success) {
        success = aclrtMemcpy(
                      input_device, input.size() * sizeof(float), input.data(), input.size() * sizeof(float),
                      ACL_MEMCPY_HOST_TO_DEVICE) == ACL_SUCCESS;
    }
    if (success) {
        store_squeeze_status_kernel<<<1, 0>>>(output_device, input_device);
        success = aclrtSynchronizeDevice() == ACL_SUCCESS;
    }
    if (success) {
        success = aclrtMemcpy(
                      output.data(), output.size() * sizeof(uint32_t), output_device, output.size() * sizeof(uint32_t),
                      ACL_MEMCPY_DEVICE_TO_HOST) == ACL_SUCCESS;
    }
    if (input_device != nullptr) {
        aclrtFree(input_device);
    }
    if (output_device != nullptr) {
        aclrtFree(output_device);
    }
    aclrtResetDevice(0);
    aclFinalize();
    if (!success) {
        return 1;
    }
    success = success && output[0] == SQUEEZE_STATUS_BYTES;
    std::cout << (success ? "PASS" : "FAIL") << '\n';
    return success ? 0 : 1;
}
```
