---
okftype: reference.api
description: "按掩码对两个矢量数据寄存器及输入借位标志逐元素求差，把补码加法和的低 32 位写入目的寄存器、进位标志写入目的掩码寄存器，支持 int32_t 与 uint32_t。"
tag:
  - roles: [compute]
  - operations: [subtract]
  - traits: [carry_input, elementwise, supports_carry_output]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: requires
    condition: "矢量数据寄存器与掩码寄存器的类型定义"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "Reg 矢量计算 C API 通用约束"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: related
    condition: "本接口输出完整计算结果，不受该控制接口影响"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: header_reorganized
    scope: "page"
    scope_basis: kind_default
    note: "声明位置由 include/c_api/reg_compute/reg_vector.h（9.1.0 及更早）统一到 include/c_api/reg_compute/compute/reg_arith.h（9.2.0 起）；本页只有 2 条引用输出声明，9.2.0 未增加函数返回值形式，故不登记 overload_added。"
---
# asc_subc

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

该接口根据mask，对源操作数src0、src1及输入进位carry_src进行按元素求差操作，减法运算在硬件底层通过补码加法实现，将结果写入目的操作数dst，同时将每个元素的进位结果写入carry（存放进位标志的掩码寄存器）。

Carry flag（进位/借位标志）用于表示加法进位或者减法无借位。补码加法实现中若`src0`、`~src1`、`carry_src`输入按位相加后最高位有进位，则`carry`中对应位置每4bit的最低位写1，否则写0。

硬件计算时按照32位无符号数处理，矢量数据寄存器写入补码加法结果的低32位，目的掩码寄存器写入每个元素补码加法产生的进位标志，计算公式如下：

$$
\{carry_i, dst_i\} = \{1'b0, src0_i\} + \{1'b0, \sim src1_i\} + \{32'b0, carry\_src_i\}
$$

![](../../../../figures/asc_subc_1.png)

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```cpp
// 占位符形式
__simd_callee__ inline void asc_subc(vector_bool& carry,
                                     vector_<dtype>& dst,
                                     vector_<dtype> src0,
                                     vector_<dtype> src1,
                                     vector_bool carry_src,
                                     vector_bool mask)
```

### dtype支持数据类型

`dtype`取值为：`int32_t`、`uint32_t`。

### 函数原型典型示例

```cpp
__simd_callee__ inline void asc_subc(vector_bool& carry,
                                     vector_int32_t& dst,
                                     vector_int32_t src0,
                                     vector_int32_t src1,
                                     vector_bool carry_src,
                                     vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| carry | 输出 | 目的操作数（掩码寄存器）。用于存储减法借位输出。减法不借位等价于补码加法进位。 |
| dst | 输出 | 目的操作数（矢量数据寄存器），用于存储计算结果的低32位。 |
| src0 | 输入 | 源操作数（矢量数据寄存器）。 |
| src1 | 输入 |源操作数（矢量数据寄存器）。 |
| carry_src | 输入 | 源操作数（掩码寄存器），作为输入的借位标志。 |
| mask | 输入 | 源操作数掩码（掩码寄存器）。用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 运算输出完整计算结果（包含借位标志），不受[asc_set_ctrl](../../spr/asc_set_ctrl.md)影响，硬件不会对输出进行饱和或截断。

## 关键特性

以uint64_t类型数据计算0xFFFFFFFF FFFFFFFF - 0x00000000 00000008 = 0xFFFFFFFF FFFFFFF7为例，asc_sub/asc_subc接口的适用场景如下图：

![](../../../../figures/asc_subc_2.png)

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

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

template <typename T>
bool compare_data(const std::vector<T>& actual, const std::vector<T>& expected)
{
    return actual == expected;
}

constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
constexpr uint32_t DATA_BYTES = 133 * sizeof(uint32_t);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(uint32_t);
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// 按完整寄存器块扩展UB容量，避免尾轮整寄存器访问越界；有效范围仍由掩码控制。
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t CARRY_WORDS = 8;
constexpr uint32_t TOTAL_CARRY_WORDS = REPEAT_COUNT * CARRY_WORDS;

__simd_vf__ inline void subc_vf(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* carry_out, __ubuf__ uint32_t* src0, __ubuf__ uint32_t* src1, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        const uint32_t offset = i * one_repeat_size;
        vector_uint32_t dst_reg;
        vector_uint32_t src0_reg;
        vector_uint32_t src1_reg;
        vector_bool carry;
        vector_bool mask = asc_update_mask_b32(remaining_length);
        vector_bool carry_src = mask;
        asc_loadalign(src0_reg, src0 + offset);
        asc_loadalign(src1_reg, src1 + offset);
        asc_subc(carry, dst_reg, src0_reg, src1_reg, carry_src, mask);
        asc_storealign(dst + offset, dst_reg, mask);
        asc_storealign(carry_out + offset / ELEMENTS_PER_REGISTER * CARRY_WORDS, carry);
    }
}

__global__ __vector__ void asc_subc_kernel(__gm__ uint32_t* dst, __gm__ uint32_t* carry, __gm__ uint32_t* src0, __gm__ uint32_t* src1)
{
    asc_init();
    __ubuf__ uint32_t dst_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint32_t carry_local[TOTAL_CARRY_WORDS];
    __ubuf__ uint32_t src0_local[PADDED_ELEMENT_COUNT];
    __ubuf__ uint32_t src1_local[PADDED_ELEMENT_COUNT];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src0_local, src0, PADDED_ELEMENT_COUNT * sizeof(uint32_t));
    asc_copy_gm2ub_align(src1_local, src1, PADDED_ELEMENT_COUNT * sizeof(uint32_t));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint32_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<subc_vf>(dst_local, carry_local, src0_local, src1_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(dst, dst_local, PADDED_ELEMENT_COUNT * sizeof(uint32_t));
    asc_copy_ub2gm_align(carry, carry_local, TOTAL_CARRY_WORDS * sizeof(uint32_t));
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}

} // namespace

int main()
{
    std::vector<uint32_t> src0(PADDED_ELEMENT_COUNT, 0);
    std::vector<uint32_t> src1(PADDED_ELEMENT_COUNT, 0);
    std::vector<uint32_t> output(PADDED_ELEMENT_COUNT, 0);
    std::vector<uint32_t> carry(TOTAL_CARRY_WORDS, 0);
    std::vector<uint32_t> golden(PADDED_ELEMENT_COUNT, 0);
    std::vector<uint32_t> golden_carry(TOTAL_CARRY_WORDS, 0);

    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) {
        src0[i] = i + 2;
        src1[i] = i;
        golden[i] = 2;
    }
    for (uint32_t repeat = 0; repeat < REPEAT_COUNT; ++repeat) {
        const uint32_t valid_count = (TOTAL_LENGTH - repeat * ELEMENTS_PER_REGISTER > ELEMENTS_PER_REGISTER) ?
            ELEMENTS_PER_REGISTER : TOTAL_LENGTH - repeat * ELEMENTS_PER_REGISTER;
        for (uint32_t i = 0; i < CARRY_WORDS; ++i) {
            const uint32_t remain = (valid_count > i * 8) ? valid_count - i * 8 : 0;
            const uint32_t lanes = (remain > 8) ? 8 : remain;
            uint32_t word = 0;
            for (uint32_t lane = 0; lane < lanes; ++lane) word |= 1U << (lane * 4);
            golden_carry[repeat * CARRY_WORDS + i] = word;
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* src0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), (PADDED_ELEMENT_COUNT) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint32_t* src1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), (PADDED_ELEMENT_COUNT) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint32_t* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), (PADDED_ELEMENT_COUNT) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint32_t* carry_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&carry_device), (TOTAL_CARRY_WORDS) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, src0.size() * sizeof(uint32_t), src0.data(), src0.size() * sizeof(uint32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, src1.size() * sizeof(uint32_t), src1.data(), src1.size() * sizeof(uint32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);

    asc_subc_kernel<<<1, 0>>>(dst_device, carry_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint32_t), dst_device, output.size() * sizeof(uint32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(carry.data(), carry.size() * sizeof(uint32_t), carry_device, carry.size() * sizeof(uint32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);

    print_data("Input src0", src0);
    print_data("Input src1", src1);
    print_data("Output", output);
    print_data("Carry words", carry);
    print_data("Golden output", golden);
    print_data("Golden carry words", golden_carry);
    bool passed = compare_data(carry, golden_carry);
    for (uint32_t i = 0; i < TOTAL_LENGTH; ++i) passed = passed && output[i] == golden[i];
    std::cout << (passed ? "[Success] asc_subc passed." : "[Failed] asc_subc failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(carry_device);
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
