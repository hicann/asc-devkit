---
okftype: reference.api
description: "把矢量数据寄存器前count个元素连续搬出到非32字节对齐的UB地址，主块直接写出、尾块暂存非对齐寄存器供下一次拼接，目的地址由用户更新。"
tag:
  - roles: [data_movement]
  - operations: [store]
  - traits: [unaligned_transfer]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "正文据此说明矢量数据寄存器与非对齐寄存器"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: related
    condition: "正文据此引用 Reg 矢量计算通用约束"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: related
    condition: "与其他指令存在数据依赖时插入同步指令串行化"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/store/asc_storeunalign_post.md
    kind: related
    condition: "上一次尾块与本次主块不连续时需用该后处理接口收尾"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: behavior_fixed
    scope: "page"
    scope_basis: kind_default
    note: "目的参数由指针引用修正为普通指针：CANN 9.1.0 Release Notes 记载将不更新地址的 asc_storeunalign 目的参数由指针引用修正为普通指针（与正文 __ubuf__ <dtype>* dst 的非引用形式一致）。"
  - since: "9.1.0"
    kind: capability_extended
    scope: "page"
    scope_basis: kind_default
    note: "新增 int4b_t、vector_int4x2_t 数据重载：CANN 9.1.0 Release Notes 明确列出 asc_storeunalign 新增低精度重载，对应正文 dtype 列表中的 int4b_t。"
---
# asc_storeunalign

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

头文件路径为：`"c_api/reg_compute/store/storeunalign.h"`。

将矢量数据寄存器中的前`count`个元素连续搬出到非32字节对齐的Unified Buffer（UB）地址。`src0` 为非对齐寄存器，按引用传入并在本接口调用后被更新为本次搬出尾部数据，供下一次本接口或非对齐搬出收尾接口继续拼接使用。每次调用后，用户需手动更新目的操作数在UB中的地址。

矢量数据寄存器搬运原理如下：
记目的操作数的起始地址为`dst_start`，结束地址为`dst_end`，尾块元素个数为`unalign_count = (dst_end - dst_end / 32 * 32) / sizeof(T)`。
则搬运的数据会分为两部分，分别是：
- 32B对齐的主块部分`UB[dst_start, dst_end - unalign_count * sizeof(T)]`。
- 非32B对齐的尾块部分`UB[dst_end - unalign_count * sizeof(T), dst_end]`。

本接口执行时，会将主块搬出至UB，尾块暂存至非对齐寄存器[0, `unalign_count`]。连续调用时，本接口可以拼接上一段**连续的尾块**和本次搬运主块部分，并同时更新本次尾块数据至非对齐寄存器中。首次搬运可以使用未初始化的非对齐寄存器。若上一次搬运的尾块和本次搬运主块不连续，则每一次尾块处理都需要调用后处理接口[asc_storeunalign_post](asc_storeunalign_post.md)。若连续，仅最后一次尾块处理需要调用后处理接口。

本接口为Reg矢量搬运接口，仅在AIV上生效。

## 函数原型

```c
// 占位符形式
__simd_callee__ inline void asc_storeunalign(__ubuf__ <dtype>* dst,
                                             vector_store_unalign& src0,
                                             vector_<dtype> src1,
                                             uint32_t count)
```

### dtype支持数据类型

`dtype`取值为：`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`、`int64_t`。对应矢量寄存器类型为`vector_<dtype>`，`int4b_t`对应`vector_int4x2_t`。

### 函数原型典型示例

```c
__simd_callee__ inline void asc_storeunalign(__ubuf__ uint8_t* dst,
                                             vector_store_unalign& src0,
                                             vector_uint8_t src1,
                                             uint32_t count)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数在UB中的起始地址，类型为`__ubuf__ <dtype>*`。地址无需32字节对齐，但须按`sizeof(dtype)`字节对齐。 |
| src0 | 输入/输出 | 非对齐寄存器，类型为`vector_store_unalign`，长度为32字节。用于承接前一次搬出的尾块，并暂存本次搬出的尾块。首次调用前无需初始化。 |
| src1 | 输入 | 源操作数（矢量数据寄存器）。数据类型须与`dst`一致。 |
| count | 输入 | 本次搬出的元素个数，类型为`uint32_t`，最大值不超过单个矢量数据寄存器可容纳的元素个数。 |

矢量数据寄存器和非对齐寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

### 通用约束

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- UB容量上限为256KB，用户可用容量随编译选项与编程场景变化（默认预留6KB SIMD VF栈+2KB Ascend C预留，可用248KB；SIMD+SIMT混编时再划分32KB~128KB作Data Cache，可用容量进一步减少）。目的操作数地址偏移后不可超过实际可用容量，否则会报错。
- 如果本指令与其他指令存在数据依赖，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化，防止出现异常数据。

### 指令约束

- 连续调用时须复用同一个`src0`，并按已搬出的元素个数更新`dst`。
- 调用该接口之后，需要调用[asc_storeunalign_post](asc_storeunalign_post.md)处理非对齐搬出的尾块，尾块数据使用同一个非对齐寄存器存储。
- `count`最大值不超过单个矢量数据寄存器可容纳的元素个数，目的访问范围不可越过UB可用容量上限，否则会报错。

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

constexpr uint32_t ELEMENTS_PER_REGISTER = 256;
constexpr uint32_t DATA_BYTES = 300 * sizeof(uint8_t);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint8_t);
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// 按完整寄存器块扩展UB容量，避免尾轮整寄存器访问越界；有效范围仍由掩码控制。
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t START_OFFSET = 8;
constexpr uint32_t PADDED_OUTPUT_BYTES = START_OFFSET + PADDED_ELEMENT_COUNT;

__simd_vf__ inline void storeunalign_vf(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_store_unalign unalign_reg;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        const uint32_t count = (TOTAL_LENGTH - offset > ELEMENTS_PER_REGISTER) ?
            ELEMENTS_PER_REGISTER : TOTAL_LENGTH - offset;
        vector_uint8_t src;
        asc_loadalign(src, input + offset);
        asc_storeunalign(output + START_OFFSET + offset, unalign_reg, src, count);
    }
    asc_storeunalign_post(output + START_OFFSET, unalign_reg, TOTAL_LENGTH);
}

__global__ __vector__ void storeunalign_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[PADDED_OUTPUT_BYTES];
    __ubuf__ uint8_t input_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, PADDED_ELEMENT_COUNT);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<storeunalign_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, PADDED_OUTPUT_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(PADDED_ELEMENT_COUNT, 0);
    std::vector<uint8_t> output(PADDED_OUTPUT_BYTES, 0);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) input[i] = static_cast<uint8_t>(i);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), PADDED_ELEMENT_COUNT, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), PADDED_OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, PADDED_ELEMENT_COUNT, input.data(), PADDED_ELEMENT_COUNT, ACL_MEMCPY_HOST_TO_DEVICE);
    storeunalign_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), PADDED_OUTPUT_BYTES, output_device, PADDED_OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) passed = passed && output[START_OFFSET + i] == input[i];
    std::cout << (passed ? "[Success] asc_storeunalign completed."
                         : "[Failed] asc_storeunalign output mismatch.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
