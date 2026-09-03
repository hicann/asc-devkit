# asc_addc

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"c_api/reg_compute/compute/reg_arith.h"`。

该接口根据mask，对源操作数src0、src1及输入进位carry_src进行按元素求和操作，将结果写入目的操作数dst，同时将每个元素的进位结果写入carry（存放进位标志的掩码寄存器）。

Carry flag（进位/借位标志）用于表示加法进位或者减法无借位。若src0，src1，carry_src输入按位相加后最高位有进位，在carry中对应位置每4bit的最低位写1，否则写0。

硬件计算时按照32位无符号数处理，矢量数据寄存器写入加法和的低32位，目的掩码寄存器写入每个元素加法产生的进位输出，计算公式如下：

$$
\{carry_i, dst_i\} = \{1'b0, src0_i\} + \{1'b0, src1_i\} + \{32'b0, carry\_src_i\}
$$

![](../../figures/asc_addc_1.png)

本接口仅在AIV上生效。

## 函数原型

```cpp
// 占位符形式
__simd_callee__ inline void asc_addc(vector_bool& carry,
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
__simd_callee__ inline void asc_addc(vector_bool& carry,
                                     vector_int32_t& dst,
                                     vector_int32_t src0,
                                     vector_int32_t src1,
                                     vector_bool carry_src,
                                     vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| carry | 输出 | 目的操作数（掩码寄存器）。存储加法计算后的进位数据。 |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src0 | 输入 | 源操作数（矢量数据寄存器）。 |
| src1 | 输入 |源操作数（矢量数据寄存器）。 |
| carry_src | 输入 | 源操作数（掩码寄存器），输入的进位标志。 |
| mask | 输入 | 源操作数掩码（掩码寄存器）。用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- 本接口在非AIV上调用直接返回。
- `mask`需通过掩码设置接口预先赋值后再传入，未赋值的掩码寄存器内容不确定，会导致有效元素位置错误。
- 运算输出完整计算结果（包含进位位），不受[asc_set_ctrl](../../spr/asc_set_ctrl.md)影响，硬件不会对输出进行饱和或截断。

## 关键特性

以uint64_t类型数据计算0xFFFFFFFF FFFFFFFF + 0x00000000 00000008 = 0x00000000 00000007为例，asc_add/asc_addc接口的适用场景如下图：

![](../../figures/asc_addc_2.png)

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```c
#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>

#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
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

constexpr uint32_t ELEMENT_COUNT = 64;
constexpr uint32_t CARRY_WORDS = 8;

__simd_vf__ inline void addc_vf(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* carry_out, __ubuf__ uint32_t* src0, __ubuf__ uint32_t* src1)
{
    vector_uint32_t dst_reg;
    vector_uint32_t src0_reg;
    vector_uint32_t src1_reg;
    vector_bool carry;
    uint32_t count = ELEMENT_COUNT;
    vector_bool mask = asc_update_mask_b32(count);
    vector_bool carry_src = mask;
    asc_loadalign(src0_reg, src0);
    asc_loadalign(src1_reg, src1);
    asc_addc(carry, dst_reg, src0_reg, src1_reg, carry_src, mask);
    asc_storealign(dst, dst_reg, mask);
    asc_storealign(carry_out, carry);
}

__global__ __vector__ void asc_addc_kernel(__gm__ uint32_t* dst, __gm__ uint32_t* carry, __gm__ uint32_t* src0, __gm__ uint32_t* src1)
{
    asc_init();
    __ubuf__ uint32_t dst_local[ELEMENT_COUNT];
    __ubuf__ uint32_t carry_local[CARRY_WORDS];
    __ubuf__ uint32_t src0_local[ELEMENT_COUNT];
    __ubuf__ uint32_t src1_local[ELEMENT_COUNT];
    asc_copy_gm2ub_align(src0_local, src0, ELEMENT_COUNT * sizeof(uint32_t));
    asc_copy_gm2ub_align(src1_local, src1, ELEMENT_COUNT * sizeof(uint32_t));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    addc_vf(dst_local, carry_local, src0_local, src1_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst, dst_local, ELEMENT_COUNT * sizeof(uint32_t));
    asc_copy_ub2gm_align(carry, carry_local, CARRY_WORDS * sizeof(uint32_t));
    asc_sync();
}

} // namespace

int main()
{
    std::vector<uint32_t> src0(ELEMENT_COUNT);
    std::vector<uint32_t> src1(ELEMENT_COUNT);
    std::vector<uint32_t> output(ELEMENT_COUNT, 0);
    std::vector<uint32_t> carry(CARRY_WORDS, 0);
    std::vector<uint32_t> golden(ELEMENT_COUNT, 2);
    std::vector<uint32_t> golden_carry(CARRY_WORDS, 0x11111111U);

    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        src0[i] = std::numeric_limits<uint32_t>::max() - i;
        src1[i] = i + 2;
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* src0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), (ELEMENT_COUNT) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint32_t* src1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), (ELEMENT_COUNT) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint32_t* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), (ELEMENT_COUNT) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint32_t* carry_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&carry_device), (CARRY_WORDS) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, src0.size() * sizeof(uint32_t), src0.data(), src0.size() * sizeof(uint32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, src1.size() * sizeof(uint32_t), src1.data(), src1.size() * sizeof(uint32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);

    asc_addc_kernel<<<1, 0>>>(dst_device, carry_device, src0_device, src1_device);
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
    const bool passed = compare_data(output, golden) && compare_data(carry, golden_carry);
    std::cout << (passed ? "[Success] asc_addc passed." : "[Failed] asc_addc failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(carry_device);
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
