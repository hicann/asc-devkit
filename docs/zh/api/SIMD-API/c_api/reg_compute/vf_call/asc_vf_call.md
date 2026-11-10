---
okftype: reference.api
description: "在 SIMD C API 场景下于 Kernel 中启动 Vector Function 子任务，用模板参数 funcPtr 指定以 __simd_vf__ 修饰的入口函数并通过可变参数传递实参，入口函数内可调用 Reg 矢量计算接口，仅 Ascend 950PR&950DT 生效。"
tag:
  - roles: [control]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: requires
    condition: "Vector Function 内存在 UB 地址重叠或跨流水依赖时需插入该同步接口"
  - path: docs/zh/api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md
    kind: related
    condition: "示例在启动 Vector Function 前后用该指令与 MTE2、MTE3 流水同步"
  - path: docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_gm2ub_align/asc_copy_gm2ub_align.md
    kind: related
    condition: "示例用该接口把 GM 数据搬入 UB 后再启动 Vector Function"
version: ">=9.0.0"
version_changes: []
---
# asc\_vf\_call

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

头文件路径为：`"utils/base/helpers.h"`。

在SIMD C API编程场景下使用。用于在Kernel中启动Vector Function（VF）子任务，VF函数内可调用Reg矢量计算C API完成寄存器级搬运和计算。

## 函数原型

```cpp
template <auto funcPtr, typename... Args>
__aicore__ inline void asc_vf_call(Args &&...args)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| :-- | :------------ |
| funcPtr | 用于指定Vector Function入口函数。 |
| Args | 定义可变参数，用于传递实参到Vector Function入口函数。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :-- | :------------ | :-- |
| args | 输入 | 可变参数，用于传递实参到Vector Function入口函数。 |

## 返回值说明

无

## 约束说明

- `asc_vf_call`启动Vector Function子任务时，子任务函数不能是类的成员函数，推荐使用普通函数或类静态函数，且入口函数必须使用`__simd_vf__`修饰宏。
- `asc_vf_call`启动Vector Function子任务时，入参仅支持Ascend C的内置数据类型及其组成的指针、数组、结构体类型；传入Reg矢量计算C API的源/目的地址时，指针类型必须指向Unified Buffer（UB）内存。
- Vector Function的流水类型为`PIPE_V`。Vector Function内部如存在UB地址重叠或跨流水依赖，需要根据具体接口约束插入[asc_mem_bar](../reg_sync/asc_mem_bar.md)。

## 调用示例

使用Reg矢量计算C API在Vector Function中对UB数据执行加法计算。

```cpp
#include <iostream>
#include <algorithm>
#include <vector>
#include "c_api/asc_simd.h"
#include "utils/std/cmath.h"
#include "acl/acl.h"

constexpr uint32_t ELEMENT_COUNT = 67;
// 按完整寄存器块扩展UB容量，避免尾轮整寄存器访问越界；有效范围仍由掩码控制。
constexpr uint32_t PADDED_ELEMENT_COUNT = 128;

constexpr uint32_t DATA_BYTES = ELEMENT_COUNT * sizeof(float);
constexpr uint32_t PADDED_DATA_BYTES = PADDED_ELEMENT_COUNT * sizeof(float);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(float);
__simd_vf__ inline void add_vf(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_float dst_reg;
        vector_float src0_reg;
        vector_float src1_reg;
        vector_bool mask = asc_update_mask_b32(remaining_length);

        asc_loadalign(src0_reg, src0 + offset);
        asc_loadalign(src1_reg, src1 + offset);
        asc_add(dst_reg, src0_reg, src1_reg, mask);
        asc_storealign(dst + offset, dst_reg, mask);
    }
}

constexpr uint8_t MUTEX_ID = 0;

__global__ __vector__ void asc_add_kernel(__gm__ float* dst, __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float dst_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float src0_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float src1_local[PADDED_ELEMENT_COUNT];

    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src0_local, src0, PADDED_DATA_BYTES);
    asc_copy_gm2ub_align(src1_local, src1, PADDED_DATA_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);


    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<add_vf>(dst_local, src0_local, src1_local, one_repeat_size, repeat_time);

    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst, dst_local, PADDED_DATA_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}

int main()
{
    std::vector<float> src0(PADDED_ELEMENT_COUNT), src1(PADDED_ELEMENT_COUNT);
    std::vector<float> output(PADDED_ELEMENT_COUNT, 0.0F), golden(PADDED_ELEMENT_COUNT, 0.0F);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        src0[i] = static_cast<float>(i);
        src1[i] = 100.0F + static_cast<float>(i);
        golden[i] = src0[i] + src1[i];
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    float* src0_device = nullptr;
    float* src1_device = nullptr;
    float* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), PADDED_DATA_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), PADDED_DATA_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), PADDED_DATA_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, PADDED_DATA_BYTES, src0.data(), PADDED_DATA_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, PADDED_DATA_BYTES, src1.data(), PADDED_DATA_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_add_kernel<<<1, 0>>>(dst_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), PADDED_DATA_BYTES, dst_device, PADDED_DATA_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = std::equal(output.begin(), output.begin() + ELEMENT_COUNT, golden.begin());
    std::cout << (passed ? "[Success] asc_vf_call example." : "[Failed] asc_vf_call example.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src1_device);
    aclrtFree(src0_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
