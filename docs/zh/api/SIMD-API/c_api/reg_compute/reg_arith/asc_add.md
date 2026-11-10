---
okftype: reference.api
description: "按mask对两个矢量寄存器逐元素求和，可只输出加法和，也可用双目的形式额外输出每个元素的进位到掩码寄存器，dtype覆盖整数与浮点。"
tag:
  - roles: [compute]
  - operations: [add]
  - traits: [elementwise, supports_carry_output]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "正文据此说明矢量数据寄存器与掩码寄存器"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: related
    condition: "正文据此引用 Reg 矢量计算通用约束"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_arith/asc_addc.md
    kind: related
    condition: "正文引用其功能说明解释 carry 输出的适用场景"
  - path: docs/zh/api/SIMD-API/c_api/spr/asc_set_ctrl.md
    kind: related
    condition: "带进位输出加法不受该配置影响"
version: ">=9.0.0"
version_changes:
  - since: "9.2.0"
    kind: overload_added
    scope: "section:函数原型"
    scope_basis: resolved
    note: "新增函数返回值形式重载：CANN 9.2.0 Release Notes 明确列出 asc_add 补充函数返回值形式重载；探测 inline vector_* asc_add 首次出现在 9.2.0 的 reg_base_impl/reg_arith_intf_impl.h:735，对应正文无进位输出加法的 vector_<dtype> asc_add(vector_<dtype> src0, vector_<dtype> src1, vector_bool mask)。"
---
# asc_add

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

该接口根据`mask`，对源操作数`src0`、`src1`进行按元素求和操作，将结果写入目的操作数`dst`。

Carry flag（进位/借位标志）用于表示加法进位或者减法无借位。该接口可以在`carry`（掩码寄存器）中标记每次加法是否产生进位，若`src0`，`src1`输入按位相加后最高位有进位，在`carry`中对应位置每4bit的最低位写1，否则写0。

**[无进位输出加法](#无进位输出加法)**：单目的操作数，矢量数据寄存器写入加法和，计算公式如下：

$$
dst_i = src0_i + src1_i
$$

**[带进位输出加法](#带进位输出加法)**：双目的操作数，硬件计算时按照32位无符号数处理，矢量数据寄存器写入加法和的低32位，目的掩码寄存器写入每个元素加法产生的进位输出，计算公式如下：

$$
\{carry_i, dst_i\} = \{1'b0, src0_i\} + \{1'b0, src1_i\}
$$

![](../../../../figures/asc_add.png)

输出`carry`适用场景请参考[asc_addc功能说明](asc_addc.md#功能说明)。

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

### 无进位输出加法

```c
// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_add(vector_<dtype>& dst,
                                    vector_<dtype> src0,
                                    vector_<dtype> src1,
                                    vector_bool mask)

// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dtype> asc_add(vector_<dtype> src0,
                                              vector_<dtype> src1,
                                              vector_bool mask)
```

#### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

#### 函数原型典型示例

```c
// 通过引用参数输出结果示例：float类型的无进位输出加法
__simd_callee__ inline void asc_add(vector_float& dst,
                                    vector_float src0,
                                    vector_float src1,
                                    vector_bool mask)

// 通过函数返回值返回结果示例：float类型的无进位输出加法
__simd_callee__ inline vector_float asc_add(vector_float src0,
                                            vector_float src1,
                                            vector_bool mask)
```

### 带进位输出加法

```c
__simd_callee__ inline void asc_add(vector_bool& carry,
                                    vector_<dtype>& dst,
                                    vector_<dtype> src0,
                                    vector_<dtype> src1,
                                    vector_bool mask)
```

#### dtype支持数据类型

`dtype`取值为：`int32_t`、`uint32_t`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_add(vector_bool& carry,
                                    vector_int32_t& dst,
                                    vector_int32_t src0,
                                    vector_int32_t src1,
                                    vector_bool mask)
```

## 参数说明

### 无进位输出加法

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数（矢量数据寄存器），用于存储加法和。 |
| src0 | 输入 | 源操作数（矢量数据寄存器）。 |
| src1 | 输入 | 源操作数（矢量数据寄存器）。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

### 带进位输出加法

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| carry | 输出 | 目的操作数（掩码寄存器），用于存储加法计算后的进位数据。 |
| dst | 输出 | 目的操作数（矢量数据寄存器），用于存储加法和。 |
| src0 | 输入 | 源操作数（矢量数据寄存器）。 |
| src1 | 输入 | 源操作数（矢量数据寄存器）。 |
| mask | 输入 | 源操作数掩码（掩码寄存器）。用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 通过引用参数输出结果的函数原型无返回值。
- 通过函数返回值输出结果的函数原型返回无进位加法结果，返回值类型与源操作数类型一致。
- 带进位输出加法仅支持通过引用参数输出结果，无返回值。

## 约束说明

### 通用约束

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。

### 无进位输出加法约束

- 整型dtype加法溢出时按环绕（wrap-around）策略处理：有符号类型溢出回绕到对应dtype的最小值或最大值（MAX+1->MIN，MIN-1->MAX），无符号类型溢出回绕到0（UMAX+1->0）。
- 浮点dtype（half、bfloat16_t、float）加法按IEEE 754浮点加法语义执行，溢出与无效输入（如nan、inf）的处理遵循浮点运算规则。

### 带进位输出加法约束

- 运算输出完整计算结果（包含进位位），不受[asc_set_ctrl](../../spr/asc_set_ctrl.md)影响，硬件不会对输出进行饱和或截断。
- 进位输出按32位无符号加法产生：每个元素的进位为`(src0_i + src1_i) >> 32`取最高位1bit。
- 和值截断为32位：`dst`写入`(src0_i + src1_i) & 0xFFFFFFFF`，超出32位的部分不保留，仅通过`carry`的进位输出体现。

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

// 本示例按256字节的寄存器宽度预留UB空间，每个寄存器可容纳256 / sizeof(int32_t) = 64个元素。
constexpr uint32_t ELEMENTS_PER_REGISTER = 64;
// 选取137个有效元素，覆盖两个完整寄存器和一个仅含9个有效元素的尾块。
constexpr uint32_t DATA_BYTES = 137 * sizeof(int32_t);
// 将字节数换算为元素数，供循环次数和有效元素掩码计算使用。
constexpr uint32_t TOTAL_LENGTH = DATA_BYTES / sizeof(int32_t);
// 向上取整得到3个寄存器块；直接整除只得到2，会漏掉尾块。
constexpr uint32_t REPEAT_COUNT =
    (TOTAL_LENGTH + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
// UB容量扩展为3 * 64 = 192个元素，保证尾轮整寄存器加载时不越过已分配空间。
// 额外空间仅用于安全加载，计算和写回的有效范围仍由掩码及TOTAL_LENGTH控制。
constexpr uint32_t PADDED_ELEMENT_COUNT = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint8_t MUTEX_ID = 0;

__simd_vf__ inline void add_vf(__ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t one_repeat_size, uint16_t repeat_time)
{
    vector_int32_t dst_reg;
    vector_int32_t src0_reg;
    vector_int32_t src1_reg;
    // 记录尚未处理的有效元素数，由asc_update_mask_b32逐轮扣减。
    uint32_t remaining_length = TOTAL_LENGTH;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        // 每轮前进一个寄存器块，offset以int32_t元素为单位，而不是字节。
        const uint32_t offset = i * one_repeat_size;
        // 三轮分别启用64、64、9个元素；尾轮只计算并存储前9个有效元素。
        vector_bool mask = asc_update_mask_b32(remaining_length);
        asc_loadalign(src0_reg, src0 + offset);
        asc_loadalign(src1_reg, src1 + offset);
        asc_add(dst_reg, src0_reg, src1_reg, mask);
        asc_storealign(dst + offset, dst_reg, mask);
    }
}

__global__ __vector__ void asc_add_kernel(__gm__ int32_t* dst, __gm__ int32_t* src0, __gm__ int32_t* src1)
{
    asc_init();
    __ubuf__ int32_t dst_local[PADDED_ELEMENT_COUNT];
    __ubuf__ int32_t src0_local[PADDED_ELEMENT_COUNT];
    __ubuf__ int32_t src1_local[PADDED_ELEMENT_COUNT];

    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(src0_local, src0, TOTAL_LENGTH * sizeof(int32_t));
    asc_copy_gm2ub_align(src1_local, src1, TOTAL_LENGTH * sizeof(int32_t));
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    // asc_get_vf_len返回字节数，除以元素字节数得到每轮可处理的元素数（本示例为64）。
    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(int32_t));
    // 按实际每轮处理量向上取整：ceil(137 / 64) = 3，确保最后9个元素也被处理。
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<add_vf>(dst_local, src0_local, src1_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    // 只搬出137个有效元素，不把UB中为整寄存器加载预留的填充空间写入GM。
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
        src0[i] = static_cast<int32_t>(i) - 64;
        src1[i] = static_cast<int32_t>(TOTAL_LENGTH - i);
        golden[i] = src0[i] + src1[i];
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

    asc_add_kernel<<<1, 0>>>(dst_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(int32_t), dst_device, output.size() * sizeof(int32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);

    print_data("Input src0", src0);
    print_data("Input src1", src1);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = compare_data(output, golden);
    std::cout << (passed ? "[Success] asc_add passed." : "[Failed] asc_add failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
