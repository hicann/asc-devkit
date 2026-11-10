---
okftype: reference.api
description: "把两个矢量数据寄存器或两个掩码寄存器的元素按奇偶索引解交织写入两个目的寄存器，偶数索引进前半部分、奇数索引进后半部分，仅在 AIV 上生效。"
tag:
  - roles: [compute]
  - operations: [deinterleave]
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
    note: "声明位置由 include/c_api/reg_compute/reg_vector.h（9.1.0 及更早）统一到 include/c_api/reg_compute/compute/reg_permute_sel.h（9.2.0 起），与正文头文件路径一致；本页无函数返回值形式，也没有 9.1.0/9.2.0 的重载增减。"
---
# asc_deintlv

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

将src0和src1中的元素解交织存入dst0和dst1中，本接口提供矢量数据解交织和掩码解交织两种功能模式：

- **矢量数据解交织**：对两个源矢量数据寄存器中的有效元素逐对解交织。src0的偶数索引元素写入dst0前半部分，奇数索引元素写入dst1前半部分。src1的偶数索引元素写入dst0后半部分，奇数索引元素写入dst1后半部分。
- **掩码解交织**：根据接口后缀指定的粒度，对两个源掩码寄存器中的比特位分组后进行解交织。`asc_deintlv_b8`以1bit为一组，`asc_deintlv_b16`以2bit为一组，`asc_deintlv_b32`以4bit为一组，组内比特位的顺序保持不变。分组从低位到高位按0开始编号。src0的偶数索引分组写入dst0前半部分，奇数索引分组写入dst1前半部分。src1的偶数索引分组写入dst0后半部分，奇数索引分组写入dst1后半部分。

以int8_t数据类型为例，asc_deintlv的实现流程如图1所示：

**图1** 解交织实现流程

![解交织实现流程](../../../../figures/capi_deintlv_reg.png)

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

### 矢量数据解交织

```c
// 占位符形式
__simd_callee__ inline void asc_deintlv(vector_<dtype>& dst0,
                                        vector_<dtype>& dst1,
                                        vector_<dtype> src0,
                                        vector_<dtype> src1)
```

#### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

#### 函数原型典型示例

```c
// 示例：对half矢量数据寄存器执行解交织。
__simd_callee__ inline void asc_deintlv(vector_half& dst0,
                                        vector_half& dst1,
                                        vector_half src0,
                                        vector_half src1)
```

### 掩码解交织

```c
__simd_callee__ inline void asc_deintlv_b8(vector_bool& dst0,
                                            vector_bool& dst1,
                                            vector_bool src0,
                                            vector_bool src1)
__simd_callee__ inline void asc_deintlv_b16(vector_bool& dst0,
                                            vector_bool& dst1,
                                            vector_bool src0,
                                            vector_bool src1)
__simd_callee__ inline void asc_deintlv_b32(vector_bool& dst0,
                                            vector_bool& dst1,
                                            vector_bool src0,
                                            vector_bool src1)
```

## 参数说明

**表1** 参数说明（操作数为矢量数据寄存器）

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst0 | 输出 | 目的矢量数据寄存器，写入解交织后的偶数索引元素集合。dtype须与dst1、src0、src1一致。 |
| dst1 | 输出 | 目的矢量数据寄存器，写入解交织后的奇数索引元素集合。dtype须与dst0、src0、src1一致。 |
| src0 | 输入 | 源矢量数据寄存器，参与解交织的操作数。dtype须与dst0一致。其偶数索引元素写入dst0前半部分，奇数索引元素写入dst1前半部分。 |
| src1 | 输入 | 源矢量数据寄存器，参与解交织的操作数。dtype须与dst0一致。其偶数索引元素写入dst0后半部分，奇数索引元素写入dst1后半部分。 |

**表2** 参数说明（操作数为掩码寄存器）

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst0 | 输出 | 目的掩码寄存器，写入解交织后的偶数索引分组集合。 |
| dst1 | 输出 | 目的掩码寄存器，写入解交织后的奇数索引分组集合。 |
| src0 | 输入 | 源掩码寄存器，参与解交织的操作数。其偶数索引分组写入dst0前半部分，奇数索引分组写入dst1前半部分。 |
| src1 | 输入 | 源掩码寄存器，参与解交织的操作数。其偶数索引分组写入dst0后半部分，奇数索引分组写入dst1后半部分。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- src0、src1、dst0、dst1的数据类型需要保持一致。
- src0和src1可以为同一个矢量数据寄存器或掩码寄存器。
- dst0与dst1必须为不同的矢量数据寄存器或掩码寄存器，若两者引用同一寄存器将导致两个输出互相覆盖，结果未定义。
- 允许源操作数和目的操作数为同一个矢量数据寄存器，例如asc_deintlv(src0, src1, src0, src1)。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id10 -->
以Ascend 950PR&950DT系列产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id10 -->

本示例包含133个有效float元素，输入补零至192个元素后按完整寄存器解交织，并校验全部192个输出元素。解交织不会压紧尾块，src1的偶数、奇数索引元素仍分别从两个目的寄存器的后半部分开始写入。

```cpp
#include <cstdint>
#include <algorithm>
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
constexpr uint32_t DATA_BYTES = 133 * sizeof(float);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(float);
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// 按完整寄存器块扩展UB容量，避免尾轮整寄存器访问越界；有效范围仍由掩码控制。
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
__simd_vf__ inline void rearrange(__ubuf__ float* dst0, __ubuf__ float* dst1,
    __ubuf__ float* src0, __ubuf__ float* src1, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // 补零区域也参与计算，确保搬出的所有UB元素均已写入。
    uint32_t remaining_length = PADDED_ELEMENT_COUNT;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_float dst0_reg, dst1_reg, src0_reg, src1_reg;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        vector_bool mask_dst0, mask_dst1;
        asc_deintlv_b32(mask_dst0, mask_dst1, mask, mask);
        asc_loadalign(src0_reg, src0 + offset);
        asc_loadalign(src1_reg, src1 + offset);
        asc_deintlv(dst0_reg, dst1_reg, src0_reg, src1_reg);
        asc_storealign(dst0 + offset, dst0_reg, mask_dst0);
        asc_storealign(dst1 + offset, dst1_reg, mask_dst1);
    }
}
__global__ __vector__ void asc_deintlv_kernel(__gm__ float* dst0, __gm__ float* dst1,
    __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float dst0_local[PADDED_ELEMENT_COUNT], dst1_local[PADDED_ELEMENT_COUNT];
    __ubuf__ float src0_local[PADDED_ELEMENT_COUNT], src1_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src0_local, src0, PADDED_ELEMENT_COUNT * sizeof(float));
    asc_copy_gm2ub_align(src1_local, src1, PADDED_ELEMENT_COUNT * sizeof(float));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(float));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<rearrange>(dst0_local, dst1_local, src0_local, src1_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst0, dst0_local, PADDED_ELEMENT_COUNT * sizeof(float));
    asc_copy_ub2gm_align(dst1, dst1_local, PADDED_ELEMENT_COUNT * sizeof(float));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> src0(PADDED_ELEMENT_COUNT), src1(PADDED_ELEMENT_COUNT);
    std::vector<float> output0(PADDED_ELEMENT_COUNT, 0), output1(PADDED_ELEMENT_COUNT, 0);
    std::vector<float> golden0(PADDED_ELEMENT_COUNT, 0), golden1(PADDED_ELEMENT_COUNT, 0);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) { src0[i] = static_cast<float>(i); src1[i] = 1000.0f + i; }
    for (uint32_t offset = 0; offset < PADDED_ELEMENT_COUNT; offset += ELEMENTS_PER_REGISTER) {
        // 解交织始终按完整寄存器分成前、后两半，尾块不会压紧。
        for (uint32_t i = 0; i < ELEMENTS_PER_REGISTER / 2; ++i) {
            golden0[offset + i] = src0[offset + 2 * i];
            golden1[offset + i] = src0[offset + 2 * i + 1];
            golden0[offset + ELEMENTS_PER_REGISTER / 2 + i] = src1[offset + 2 * i];
            golden1[offset + ELEMENTS_PER_REGISTER / 2 + i] = src1[offset + 2 * i + 1];
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    float* src0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), PADDED_ELEMENT_COUNT * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* src1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), PADDED_ELEMENT_COUNT * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* dst0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst0_device), PADDED_ELEMENT_COUNT * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* dst1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst1_device), PADDED_ELEMENT_COUNT * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, src0.size() * sizeof(float), src0.data(), src0.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, src1.size() * sizeof(float), src1.data(), src1.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_deintlv_kernel<<<1, 0>>>(dst0_device, dst1_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output0.data(), output0.size() * sizeof(float), dst0_device, output0.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(output1.data(), output1.size() * sizeof(float), dst1_device, output1.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input 0", src0);
    print_data("Input 1", src1);
    print_data("Output 0", output0);
    print_data("Golden 0", golden0);
    print_data("Output 1", output1);
    print_data("Golden 1", golden1);
    const bool passed = std::equal(output0.begin(), output0.end(), golden0.begin()) &&
        std::equal(output1.begin(), output1.end(), golden1.begin());
    std::cout << (passed ? "[Success] asc_deintlv passed." : "[Failed] asc_deintlv failed.") << std::endl;
    aclrtFree(dst0_device);
    aclrtFree(dst1_device);
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
