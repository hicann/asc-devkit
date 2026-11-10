---
okftype: reference.api
description: "从UB中32字节对齐地址读取VL/2长度数据，在每个源元素后补1个0解包为VL长度后搬入矢量寄存器，提供对齐、立即数偏移和地址寄存器偏移三种搬入模式。"
tag:
  - roles: [data_movement]
  - operations: [load]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "正文据此说明矢量数据寄存器"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_addr_reg/asc_update_addr_reg.md
    kind: related
    condition: "地址寄存器偏移模式必须先调用它生成偏移"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: related
    condition: "与其他指令存在数据依赖时插入同步指令串行化"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: capability_extended
    scope: "page"
    scope_basis: kind_default
    note: "新增低精度数据类型重载：CANN 9.1.0 Release Notes 记载 asc_loadalign_unpack 等接口新增 int4b_t、vector_int4x2_t 重载，对应正文 dtype 列表中的 int4b_t（dst 实际类型为 vector_int4x2_t）。"
---
# asc_loadalign_unpack

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

头文件路径为：`"c_api/reg_compute/load/loadalign.h"`。

从Unified Buffer（UB）中32字节对齐的起始地址读取VL/2长度的连续数据，在每个源元素后补1个值为0的元素，并将得到的VL长度数据通过函数返回值返回或写入目的矢量数据寄存器。本接口提供三种功能模式：

- **对齐搬入模式**：将UB源地址的数据解包后搬入到矢量数据寄存器，由用户自行更新源地址。
- **立即数偏移搬入模式**：从相对源起始地址偏移指定距离的位置搬入数据。本接口不会自动更新源地址。
- **地址寄存器偏移搬入模式**：通过地址寄存器指定相对源起始地址的偏移，常用于Hardware Loop内偏移随循环计数变化的对齐搬入场景。需要与[asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md)配合使用。

**图1** 解包搬入矢量数据寄存器

![](../../../../figures/capi_loadalign_unpack.png)

本接口为Reg矢量搬运接口，仅在AIV上生效。

## 函数原型

### 对齐搬入模式

```c
// 通过函数返回值返回结果。
__simd_callee__ inline vector_<dtype> asc_loadalign_unpack(__ubuf__ <dtype>* src)

// 通过引用参数输出结果。
__simd_callee__ inline void asc_loadalign_unpack(vector_<dtype>& dst,
                                                 __ubuf__ <dtype>* src)
```

#### dtype支持数据类型

`dtype`取值为：`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。当`dtype`为`int4b_t`时，dst的实际类型为`vector_int4x2_t`。

#### 函数原型典型示例

```c
// 示例：half类型。
__simd_callee__ inline vector_half asc_loadalign_unpack(__ubuf__ half* src)

__simd_callee__ inline void asc_loadalign_unpack(vector_half& dst,
                                                 __ubuf__ half* src)
```

### 立即数偏移搬入模式

```c
__simd_callee__ inline void asc_loadalign_unpack(vector_<dtype>& dst,
                                                 __ubuf__ <dtype>* src,
                                                 int32_t offset)
```

#### dtype支持数据类型

`dtype`取值为：`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。当`dtype`为`int4b_t`时，dst的实际类型为`vector_int4x2_t`。

#### 函数原型典型示例

```c
// 示例：half类型。
__simd_callee__ inline void asc_loadalign_unpack(vector_half& dst,
                                                 __ubuf__ half* src,
                                                 int32_t offset)
```

### 地址寄存器偏移搬入模式

```c
__simd_callee__ inline void asc_loadalign_unpack(vector_<dtype>& dst,
                                                 __ubuf__ <dtype>* src,
                                                 addr_reg offset)
```

#### dtype支持数据类型

`dtype`取值为：`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。当`dtype`为`int4b_t`时，dst的实际类型为`vector_int4x2_t`。

#### 函数原型典型示例

```c
// 示例：half类型。
__simd_callee__ inline void asc_loadalign_unpack(vector_half& dst,
                                                 __ubuf__ half* src,
                                                 addr_reg offset)
```

## 参数说明

### 对齐搬入模式

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的矢量数据寄存器。仅无返回值类型接口包含该参数。dtype必须与`src`一致，每个源元素后补1个值为0的元素，搬入VL长度数据。 |
| src | 输入 | 源UB地址，实际读取地址必须按32字节对齐。 |

### 立即数偏移搬入模式

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的矢量数据寄存器。dtype必须与`src`一致，每个源元素后补1个值为0的元素，搬入VL长度数据。 |
| src | 输入 | 源UB地址，实际读取地址必须按32字节对齐。 |
| offset | 输入 | 相对`src`起始地址的偏移，单位为元素个数。 |

### 地址寄存器偏移搬入模式

**表3** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的矢量数据寄存器。dtype必须与`src`一致，每个源元素后补1个0，搬入VL长度数据。 |
| src | 输入 | 源UB地址，实际读取地址必须按32字节对齐。 |
| offset | 输入 | 地址寄存器，类型为`addr_reg`，必须通过`asc_update_addr_reg`生成。该寄存器记录相对`src`起始地址的偏移，单位为元素个数。b8、b16、b32模式分别表示每个元素占1字节、2字节、4字节。 |

矢量数据寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

对于返回值类型接口，返回保存解包搬入结果的矢量数据寄存器，数据类型与`src`保持一致。

## 约束说明

### 通用约束

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 各功能模式下的实际读取地址必须按32字节对齐，且实际读取范围必须在UB地址空间内且不越界，否则会报错。
- UB容量上限为256KB，用户可用容量随编译选项与编程场景变化（默认预留6KB SIMD VF栈 + 2KB Ascend C预留，可用248KB；SIMD+SIMT混编时再划分32KB～128KB作Data Cache，可用容量进一步减少）。UB地址偏移后不可超过实际可用容量，否则会报错。
- 如果本指令与其他指令存在数据依赖，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化，防止出现异常数据。

### 地址寄存器偏移搬入模式

必须先调用`asc_update_addr_reg`接口。

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

constexpr uint32_t INPUT_ELEMENTS = 260;
constexpr uint32_t OUTPUT_ELEMENTS = 520;
constexpr uint32_t INPUT_BYTES = INPUT_ELEMENTS * sizeof(uint8_t);
constexpr uint32_t OUTPUT_BYTES = OUTPUT_ELEMENTS * sizeof(uint8_t);
constexpr uint32_t ELEMENTS_PER_REGISTER = 256;
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT = AscendC::Std::ceil_div(OUTPUT_ELEMENTS, ELEMENTS_PER_REGISTER);
constexpr uint32_t OUTPUT_STORAGE_ELEMENTS = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t INPUT_STORAGE_ELEMENTS = REPEAT_COUNT * 128;

// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = OUTPUT_ELEMENTS;
constexpr uint32_t DATA_BYTES = TOTAL_LENGTH * sizeof(uint8_t);
__simd_vf__ inline void asc_loadalign_unpack_vf(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input,
                                                uint32_t one_repeat_size, uint16_t repeat_time)
{
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = OUTPUT_ELEMENTS;
    vector_uint8_t dst_reg;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        vector_bool mask = asc_update_mask_b8(remaining_length);
        asc_loadalign_unpack(dst_reg, input + i * 128);
        asc_storealign(output + i * one_repeat_size, dst_reg, mask);
    }
}

__global__ __vector__ void asc_loadalign_unpack_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t input_local[INPUT_STORAGE_ELEMENTS];
    __ubuf__ uint8_t output_local[OUTPUT_STORAGE_ELEMENTS];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, INPUT_BYTES);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<asc_loadalign_unpack_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, OUTPUT_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_ELEMENTS);
    std::vector<uint8_t> output(OUTPUT_ELEMENTS, 0xff);
    std::vector<uint8_t> golden(OUTPUT_ELEMENTS, 0);
    for (uint16_t i = 0; i < INPUT_ELEMENTS; ++i) input[i] = static_cast<uint8_t>(i % 251 + 1);
    for (uint16_t i = 0; i < OUTPUT_ELEMENTS; ++i) golden[i] = (i % 2 == 0) ? input[i / 2] : 0;

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_loadalign_unpack_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == golden;
    std::cout << (passed ? "[Success]" : "[Failed]") << " asc_loadalign_unpack example." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
