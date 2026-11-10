---
okftype: reference.api
description: "按掩码对两个矢量数据寄存器或两个掩码寄存器执行按位与，结果可经函数返回值或目的参数输出，矢量形式支持 int8_t 至 uint32_t 六种整型，掩码位为 0 的输出位写 0。"
tag:
  - roles: [compute]
  - operations: [bitwise_and]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "需遵守 Reg 矢量计算 C API 通用约束"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "section:函数原型"
    scope_basis: resolved
    note: "补充函数返回值形式的重载（返回 vector_<dtype> 或 vector_bool），引用参数输出形式保留；history_probe 显示 vector_int32_t asc_and(vector_int32_t src0, vector_int32_t src1, vector_bool mask) 最早出现于 9.2.0 的 impl/c_api/reg_base_impl/reg_logic_intf_impl.h:699，CANN_9_2_0 增强节列出该批返回值形式重载。"
---
# asc_and

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

头文件路径为：`"c_api/reg_compute/compute/reg_logic.h"`。

根据`mask`对源操作数`src0`、`src1`执行按位与（&）操作，将结果作为返回值返回或写入目的操作数`dst`。

- 矢量数据寄存器按位与：对两个矢量数据寄存器按`dtype`位宽执行按位与（&），结果为矢量数据寄存器。

- 掩码寄存器按位与：对两个掩码寄存器执行按位与（&），结果为掩码寄存器。

计算公式如下：

$$
dst_i = src0_i \& src1_i
$$

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

### 矢量数据寄存器按位与

```c
// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dtype> asc_and(vector_<dtype> src0,
                                              vector_<dtype> src1,
                                              vector_bool mask)

// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_and(vector_<dtype>& dst,
                                    vector_<dtype> src0,
                                    vector_<dtype> src1,
                                    vector_bool mask)
```

#### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`。

#### 函数原型典型示例

```c
// 示例：对int32_t矢量数据寄存器执行按位与
__simd_callee__ inline vector_int32_t asc_and(vector_int32_t src0,
                                              vector_int32_t src1,
                                              vector_bool mask)

__simd_callee__ inline void asc_and(vector_int32_t& dst,
                                    vector_int32_t src0,
                                    vector_int32_t src1,
                                    vector_bool mask)
```

### 掩码寄存器按位与

```c
// 通过函数返回值返回结果
__simd_callee__ inline vector_bool asc_and(vector_bool src0,
                                           vector_bool src1,
                                           vector_bool mask)

// 通过引用参数输出结果
__simd_callee__ inline void asc_and(vector_bool& dst,
                                    vector_bool src0,
                                    vector_bool src1,
                                    vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量数据寄存器或掩码寄存器），仅用于无返回值原型。 |
| src0 | 输入 | 源操作数（矢量数据寄存器或掩码寄存器）。 |
| src1 | 输入 | 源操作数（矢量数据寄存器或掩码寄存器）。 |
| mask | 输入 | 源操作数掩码（掩码寄存器）。<br>&bull;源操作数为矢量数据寄存器时，对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。<br>&bull;源操作数为掩码寄存器时，指示在计算过程中哪些bit有效。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 通过函数返回值返回结果的函数原型返回按位与结果，返回类型与`src0`、`src1`的数据类型一致。
- 通过引用参数输出结果的函数原型无返回值。

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 参与计算的元素个数由矢量长度（VL）决定：
    - 矢量数据寄存器按位与中元素个数 = VL ÷ sizeof(dtype)；
    - 掩码寄存器按位与中比特个数 = VL。
- `mask`比特位为0时，计算结果对应比特位写0。

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

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t DATA_BYTES = 137 * sizeof(int32_t);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(int32_t);
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// 按完整寄存器块扩展UB容量，避免尾轮整寄存器访问越界；有效范围仍由掩码控制。
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;

__simd_vf__ inline void compute(__ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_int32_t dst_reg;
    vector_int32_t src0_reg;
    vector_int32_t src1_reg;
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(src0_reg, src0 + offset);
        asc_loadalign(src1_reg, src1 + offset);
        asc_and(dst_reg, src0_reg, src1_reg, mask);
        asc_storealign(dst + offset, dst_reg, mask);
    }
}

__global__ __vector__ void asc_and_kernel(__gm__ int32_t* dst, __gm__ int32_t* src0, __gm__ int32_t* src1)
{
    asc_init();
    __ubuf__ int32_t dst_local[PADDED_ELEMENT_COUNT];
    __ubuf__ int32_t src0_local[PADDED_ELEMENT_COUNT];
    __ubuf__ int32_t src1_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src0_local, src0, TOTAL_LENGTH * sizeof(int32_t));
    asc_copy_gm2ub_align(src1_local, src1, TOTAL_LENGTH * sizeof(int32_t));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(int32_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<compute>(dst_local, src0_local, src1_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst, dst_local, TOTAL_LENGTH * sizeof(int32_t));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<int32_t> src0(TOTAL_LENGTH);
    std::vector<int32_t> src1(TOTAL_LENGTH);
    std::vector<int32_t> output(TOTAL_LENGTH, 0);
    std::vector<int32_t> golden(TOTAL_LENGTH);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        src0[i] = static_cast<int32_t>(0x101010U + i);
        src1[i] = static_cast<int32_t>(0x0f0f0000U + i);
        golden[i] = src0[i] & src1[i];
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    int32_t* src0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), (TOTAL_LENGTH) * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    int32_t* src1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), (TOTAL_LENGTH) * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    int32_t* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), (TOTAL_LENGTH) * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, src0.size() * sizeof(int32_t), src0.data(), src0.size() * sizeof(int32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, src1.size() * sizeof(int32_t), src1.data(), src1.size() * sizeof(int32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);

    asc_and_kernel<<<1, 0>>>(dst_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int32_t), dst_device, output.size() * sizeof(int32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input src0", src0);
    print_data("Input src1", src1);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_and passed." : "[Failed] asc_and failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
