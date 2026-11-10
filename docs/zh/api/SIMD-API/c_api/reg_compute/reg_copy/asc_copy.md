---
okftype: reference.api
description: "完成寄存器间数据搬运的接口，支持矢量数据寄存器到矢量数据寄存器、掩码寄存器到掩码寄存器以及从指定数据块提取bit生成掩码三种模式，仅在AIV上生效。"
tag:
  - roles: [data_movement]
  - operations: [copy]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "矢量数据寄存器与掩码寄存器的类型定义"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "Reg矢量计算C API通用约束见该页通用约束节"
  - path: docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md
    kind: related
    condition: "产品型号与NPU架构映射、编译命令所需宏说明"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: overload_added
    scope: "overload:asc_copy__Rvb_vu16_i16"
    scope_basis: resolved
    note: "新增矢量数据寄存器复制到掩码寄存器的重载：源为 vector_uint16_t、第二参数为编译期常量 part（取值范围 0 至 15），把选定 16B 数据块的每个 bit 广播成 2bit 生成 32B 掩码；依据 CANN_9_1_0 Release Notes 第 128 行与 upstream/9.1.0 首次出现的该声明。"
  - since: "9.1.0"
    kind: overload_added
    scope: "overload:asc_copy__Rvb_vu32_i16"
    scope_basis: resolved
    note: "新增矢量数据寄存器复制到掩码寄存器的重载：源为 vector_uint32_t、第二参数为编译期常量 part（取值范围 0 至 31），把选定 8B 数据块的每个 bit 广播成 4bit 生成 32B 掩码；与 uint16_t 重载同批加入（CANN_9_1_0 Release Notes 第 128 行）。"
---
# asc_copy

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

头文件路径为：`"c_api/reg_compute/reg_copy.h"`。

完成数据在寄存器间的搬运，支持以下三种模式：

- 矢量数据寄存器复制到矢量数据寄存器：将src中被mask筛选的有效元素复制到dst对应位置，未被mask筛选的位置保持dst原值。

- 掩码寄存器复制到掩码寄存器：将src的所有bit位复制到dst中。支持带mask和不带mask两种重载：
    - 带mask：将src中被mask筛选的bit复制到dst。对于b8模式，mask的每个bit控制src的一个bit；对于b16模式，mask的每2bit为一组、仅LSB控制src的一个bit；对于b32模式，mask的每4bit为一组、仅LSB控制src的一个bit。
    - 不带mask：将src的所有bit复制到dst。

- 矢量数据寄存器复制到掩码寄存器：
    - 数据类型为uint16_t时，矢量数据寄存器（256B）分成16个数据块（16B），用户通过参数可选择某一个数据块，数据块的每个bit broadcast成2bit后变为32B，即输出的mask。
    - 数据类型为uint32_t时，矢量数据寄存器（256B）分成32个数据块（8B），用户通过参数可选择某一个数据块，数据块的每个bit broadcast成4bit后变为32B，即输出的mask。

    具体搬运原理请参考[关键特性说明](#关键特性说明)。

本接口为Reg矢量搬运接口，仅在AIV上生效。

## 函数原型

### 矢量数据寄存器复制到矢量数据寄存器

```c
// 占位符形式
__simd_callee__ inline void asc_copy(vector_<dtype>& dst,
                                     vector_<dtype> src,
                                     vector_bool mask)
```

#### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

#### 函数原型典型示例

```c
// 示例：对half矢量数据寄存器执行复制。
__simd_callee__ inline void asc_copy(vector_half& dst,
                                     vector_half src,
                                     vector_bool mask)
```

### 掩码寄存器复制到掩码寄存器

```c
__simd_callee__ inline void asc_copy(vector_bool& dst,
                                     vector_bool src,
                                     vector_bool mask)
__simd_callee__ inline void asc_copy(vector_bool& dst,
                                     vector_bool src)
```

### 矢量数据寄存器复制到掩码寄存器

**part的值必须在编译期确定，可传入整数字面量或constexpr整数常量，不支持传入运行时变量。**

```c
__simd_callee__ inline void asc_copy(vector_bool& dst,
                                     vector_uint16_t src,
                                     int16_t part)
__simd_callee__ inline void asc_copy(vector_bool& dst,
                                     vector_uint32_t src,
                                     int16_t part)
```

## 参数说明

**表1** 参数说明（矢量数据寄存器复制到矢量数据寄存器）

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的矢量数据寄存器，dtype须与src完全一致。mask比特位为1的位置写入src对应元素（或对应比特），mask比特位为0的位置采用Merging模式写入dst原值。dst需初始化，否则当mask为0时将出现未知值。 |
| src | 输入 | 源矢量数据寄存器，dtype须与dst完全一致。 |
| mask | 输入 | 源掩码寄存器，用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中保持原值。 |

**表2** 参数说明（掩码寄存器复制到掩码寄存器）

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的掩码寄存器。 |
| src | 输入 | 源掩码寄存器。 |
| mask | 输入 | 源掩码寄存器，用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

**表3** 参数说明（矢量数据寄存器复制到掩码寄存器）

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的掩码寄存器，写入src的全部比特位。 |
| src | 输入 | 源矢量数据寄存器。 |
| part | 输入 | part决定了src中需要搬运的数据块，具体描述请参考[关键特性说明](#关键特性说明)。<br>&bull; 当数据类型为uint16_t，part∈[0, 15]。<br>&bull; 当数据类型为uint32_t，part∈[0, 31]。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- dst与src均为矢量数据寄存器或掩码寄存器。
- 针对矢量数据寄存器复制到矢量数据寄存器：dst中未被mask筛选的位置保持原值。
- 针对掩码寄存器复制到掩码寄存器：带mask的接口dst中未被mask筛选的位置填0。
- 针对矢量寄存器复制到掩码寄存器：**part的值必须在编译期确定，可传入整数字面量或constexpr整数常量，不支持传入运行时变量。**
    - 当数据类型为uint16_t时，part∈[0, 15]。
    - 当数据类型为uint32_t时，part∈[0, 31]。

## 关键特性说明

**针对于矢量数据寄存器复制到掩码寄存器：**

由[掩码寄存器](../../defs/type/data_type_definition.md#掩码寄存器)原理可知，当操作数类型为b16时，每一个element对应2bit的掩码寄存器，且仅2bit中的最低位是有效的；当操作数类型为b32时，每一个element对应4bit的掩码寄存器，且仅4bit中的最低位是有效的。

搬运原理如[图1](#fig-maskgen-b16)、[图2](#fig-maskgen-b32)所示。

- 当操作数类型为uint16_t时，矢量数据寄存器的一个数据块大小为16B，每个bit经过broadcast成2bit后变为32B，即一个MaskReg（32B）。此外，一个矢量数据寄存器有256B/16B = 16个数据块，偏移量part取值范围为[0, 15]。
- 当操作数类型为uint32_t时，矢量数据寄存器的一个数据块大小为8B，每个bit经过broadcast成4bit后变为32B，即一个MaskReg（32B）。此外，一个矢量数据寄存器有256B/8B = 32个数据块，偏移量part取值范围为[0, 31]。

**图1** 矢量寄存器复制到掩码寄存器（uint16_t）<a id="fig-maskgen-b16"></a>

![矢量寄存器复制到掩码寄存器（uint16_t）](../../../../figures/capi_copy_reg2mask_b16.png)

**图2** 矢量寄存器复制到掩码寄存器（uint32_t）<a id="fig-maskgen-b32"></a>

![矢量寄存器复制到掩码寄存器（uint32_t）](../../../../figures/capi_copy_reg2mask_b32.png)

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

constexpr uint32_t ELEMENT_COUNT = 260;
constexpr uint32_t ELEMENTS_PER_REGISTER = 256;
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT = (ELEMENT_COUNT + ELEMENTS_PER_REGISTER - 1) / ELEMENTS_PER_REGISTER;
constexpr uint32_t BUFFER_BYTES = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t DATA_BYTES = ELEMENT_COUNT * sizeof(uint8_t);
// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = ELEMENT_COUNT;
__simd_vf__ inline void load(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input, uint32_t one_repeat_size, uint16_t repeat_time)
{
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = ELEMENT_COUNT;
    for (uint16_t i = 0; i < repeat_time; ++i) {
    const uint32_t offset = i * ELEMENTS_PER_REGISTER;
    vector_bool mask = asc_update_mask_b8(remaining_length);
    vector_int8_t arg_0_0;
    asc_loadalign(arg_0_0, reinterpret_cast<__ubuf__ int8_t*>(input + offset));
    vector_int8_t arg_0_1;
    asc_loadalign(arg_0_1, reinterpret_cast<__ubuf__ int8_t*>(input + offset));
    asc_copy(arg_0_0, arg_0_1, mask);
    asc_storealign(reinterpret_cast<__ubuf__ int8_t*>(output + offset), arg_0_0, mask);
    }
}
__global__ __vector__ void asc_copy_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[BUFFER_BYTES], input_local[BUFFER_BYTES];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_copy_gm2ub_align(output_local, input, BUFFER_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<load>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace
int main()
{
    std::vector<uint8_t> input(BUFFER_BYTES), output(BUFFER_BYTES, 0xff);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) input[i] = static_cast<uint8_t>(i % 251);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), (BUFFER_BYTES) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (BUFFER_BYTES) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint8_t), input.data(), input.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_copy_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    const bool passed = true;
    std::cout << "[Success] asc_copy completed." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
