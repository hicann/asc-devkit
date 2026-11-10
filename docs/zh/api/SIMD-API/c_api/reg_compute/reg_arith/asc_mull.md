---
okftype: reference.api
description: "按掩码将两个 32 位矢量数据寄存器的元素相乘得到 64 位乘积，低 32 位与高 32 位分别写入两个目的数据寄存器，支持 int32_t 与 uint32_t。"
tag:
  - roles: [compute]
  - operations: [multiply]
  - traits: [elementwise, split_high_low, widening_result]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: requires
    condition: "矢量数据寄存器与掩码寄存器的类型定义"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "Reg 矢量计算 C API 通用约束"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "声明位置由 include/c_api/reg_compute/reg_vector.h（9.1.0 及更早）统一到 include/c_api/reg_compute/compute/reg_arith.h（9.2.0 起）；本页只有 2 个引用输出重载，9.2.0 未为 asc_mull 增加函数返回值形式（include 中始终为 2 条），故不登记 overload_added。"
---
# asc_mull

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

头文件路径为：`"c_api/reg_compute/compute/reg_arith.h"`。

根据掩码将两个源矢量数据寄存器中的有效元素相乘，数据类型位宽为32位的元素相乘得到64位乘积，拆分为低32位与高32位分别写入两个目的矢量数据寄存器。掩码对应位置为1的元素参与计算，为0的元素在目的矢量数据寄存器中置零。计算公式如下：

$$
dst0_i = (src0_i \times src1_i) \& ((1 \ll bit) - 1)
$$

$$
dst1_i = (src0_i \times src1_i) \gg bit
$$

其中，`bit`表示`src0`和`src1`的数据类型位宽。

**图 1** asc_mull计算示意图

![](../../../../figures/asc_mull.png)

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```c
// 占位符形式
__simd_callee__ inline void asc_mull(vector_<dtype>& dst0,
                                     vector_<dtype>& dst1,
                                     vector_<dtype> src0,
                                     vector_<dtype> src1,
                                     vector_bool mask)
```

### dtype支持数据类型

`dtype`取值为：`int32_t`、`uint32_t`。

### 函数原型典型示例

```c
__simd_callee__ inline void asc_mull(vector_uint32_t& dst0,
                                     vector_uint32_t& dst1,
                                     vector_uint32_t src0,
                                     vector_uint32_t src1,
                                     vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst0 | 输出 | 目的操作数（矢量数据寄存器）。数据类型须与`src0`一致。 |
| dst1 | 输出 | 目的操作数（矢量数据寄存器）。数据类型须与`src0`一致。 |
| src0 | 输入 | 源操作数（矢量数据寄存器）。 |
| src1 | 输入 | 源操作数（矢量数据寄存器）。数据类型须与`src0`一致。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示参与计算的元素。对应位置为1时参与计算，为0时不参与计算且`dst0`、`dst1`对应元素置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

### 通用约束

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 掩码位为0的元素位置不参与乘法运算，`dst0`和`dst1`对应位置写0。

### 计算约束

- `dst0`与`dst1`必须为不同的矢量数据寄存器，否则存在未定义行为。
- 源操作数和目的操作数可以是相同的矢量数据寄存器，例如`asc_mull(src0, src1, src0, src1, mask);`。

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

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t DATA_BYTES = 137 * sizeof(uint32_t);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint32_t);
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// 按完整寄存器块扩展UB容量，避免尾轮整寄存器访问越界；有效范围仍由掩码控制。
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t BUFFER_BYTES = TOTAL_LENGTH * sizeof(uint32_t);

__simd_vf__ inline void mull_vf(__ubuf__ uint32_t* output_low,
                                __ubuf__ uint32_t* output_high,
                                __ubuf__ uint32_t* input0,
                                __ubuf__ uint32_t* input1, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_uint32_t src0;
    vector_uint32_t src1;
    vector_uint32_t dst0;
    vector_uint32_t dst1;
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(src0, input0 + offset);
        asc_loadalign(src1, input1 + offset);
        asc_mull(dst0, dst1, src0, src1, mask);
        asc_storealign(output_low + offset, dst0, mask);
        asc_storealign(output_high + offset, dst1, mask);
    }
}

__global__ __vector__ void asc_mull_kernel(__gm__ uint32_t* output_low,
                                           __gm__ uint32_t* output_high,
                                           __gm__ uint32_t* input0,
                                           __gm__ uint32_t* input1)
{
    asc_init();
    __ubuf__ uint32_t output_low_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint32_t output_high_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint32_t input0_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint32_t input1_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input0_local, input0, BUFFER_BYTES);
    asc_copy_gm2ub_align(input1_local, input1, BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint32_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<mull_vf>(output_low_local, output_high_local, input0_local, input1_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output_low, output_low_local, BUFFER_BYTES);
    asc_copy_ub2gm_align(output_high, output_high_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint32_t> input0(TOTAL_LENGTH);
    std::vector<uint32_t> input1(TOTAL_LENGTH);
    std::vector<uint32_t> output_low(TOTAL_LENGTH, 0);
    std::vector<uint32_t> output_high(TOTAL_LENGTH, 0);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        input0[i] = i + 1;
        input1[i] = 0x10000000U + i;
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* input0_device = nullptr;
    uint32_t* input1_device = nullptr;
    uint32_t* output_low_device = nullptr;
    uint32_t* output_high_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input0_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&input1_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_low_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_high_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input0_device, BUFFER_BYTES, input0.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(input1_device, BUFFER_BYTES, input1.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_mull_kernel<<<1, 0>>>(output_low_device, output_high_device, input0_device, input1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output_low.data(), BUFFER_BYTES, output_low_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(output_high.data(), BUFFER_BYTES, output_high_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        const uint64_t expected = static_cast<uint64_t>(input0[i]) * input1[i];
        if (output_low[i] != static_cast<uint32_t>(expected) ||
            output_high[i] != static_cast<uint32_t>(expected >> 32)) {
            passed = false;
            break;
        }
    }
    std::cout << (passed ? "[Success] asc_mull completed."
                         : "[Failed] asc_mull output mismatch.")
              << std::endl;
    aclrtFree(input0_device);
    aclrtFree(input1_device);
    aclrtFree(output_low_device);
    aclrtFree(output_high_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
