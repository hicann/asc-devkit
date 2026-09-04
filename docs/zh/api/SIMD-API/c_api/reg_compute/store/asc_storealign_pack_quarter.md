# asc_storealign_pack_quarter

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

头文件路径为：`"c_api/reg_compute/store/storealign.h"`。

将矢量数据寄存器中的数据压缩搬出到Unified Buffer（UB）。`mask`用于指示参与搬出的元素，`mask`对应位置为1时，将`src`中每个32bit的有效元素中低8bit数据写入压缩后对应的目的位置，`mask`对应位置为0时，压缩后对应的目的位置保持原值。本接口在Vector Function（`__simd_vf__`标记的函数）内使用，目的操作数为UB地址。

本接口提供三种参数列表不同的功能模式：

- **对齐搬出模式**：偏移固定为0，将压缩后的数据搬出到UB起始地址，由用户自行更新目的地址。
- **立即数偏移搬出模式**：通过`int32_t offset`指定相对目的起始地址的偏移，偏移单位为元素，用户可选择更新偏移或更新目的地址。
- **地址寄存器偏移搬出模式**：通过地址寄存器`addr_reg offset`指定相对目的起始地址的偏移，常用于Hardware Loop内偏移随循环计数变化的压缩搬出场景。需要与[asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md)配合使用。

本接口仅在AIV上执行有效。

## 函数原型

### 对齐搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ <dtype>* dst,
                                                        vector_<dtype> src,
                                                        vector_bool mask)
```

#### dtype支持数据类型

`dtype`支持的数据类型为`int32_t`、`uint32_t`、`float`。对应矢量寄存器类型为`vector_<dtype>`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ int32_t* dst,
                                                        vector_int32_t src,
                                                        vector_bool mask)
```

### 立即数偏移搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ <dtype>* dst,
                                                        vector_<dtype> src,
                                                        int32_t offset,
                                                        vector_bool mask)
```

#### dtype支持数据类型

`dtype`支持的数据类型为`int32_t`、`uint32_t`、`float`。对应矢量寄存器类型为`vector_<dtype>`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ int32_t* dst,
                                                        vector_int32_t src,
                                                        int32_t offset,
                                                        vector_bool mask)
```

### 地址寄存器偏移搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ <dtype>* dst,
                                                        vector_<dtype> src,
                                                        addr_reg offset,
                                                        vector_bool mask)
```

#### dtype支持数据类型

`dtype`支持的数据类型为`int32_t`、`uint32_t`、`float`。对应矢量寄存器类型为`vector_<dtype>`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storealign_pack_quarter(__ubuf__ int32_t* dst,
                                                        vector_int32_t src,
                                                        addr_reg offset,
                                                        vector_bool mask)
```


## 参数说明

### 对齐搬出模式

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数的起始地址。类型为`__ubuf__ <dtype>*`，起始地址需32字节对齐。 |
| src | 输入 | 源操作数（矢量数据寄存器）。`dtype`须与`dst`一致。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示参与搬出的元素。对应位置为1时参与搬出，为0时不参与搬出。需通过掩码设置接口预先赋值后再传入。 |

### 立即数偏移搬出模式

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数的起始地址。类型为`__ubuf__ <dtype>*`，起始地址需32字节对齐。 |
| src | 输入 | 源操作数（矢量数据寄存器）。`dtype`须与`dst`一致。 |
| offset | 输入 | 相对`dst`起始地址的偏移量，类型为`int32_t`，单位为元素。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示参与搬出的元素。对应位置为1时参与搬出，为0时不参与搬出。需通过掩码设置接口预先赋值后再传入。 |

### 地址寄存器偏移搬出模式

**表3** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数的起始地址。类型为`__ubuf__ <dtype>*`，起始地址需32字节对齐。 |
| src | 输入 | 源操作数（矢量数据寄存器）。`dtype`须与`dst`一致。 |
| offset | 输入 | 地址寄存器偏移，类型为`addr_reg`，由配套的地址寄存器构造接口预先构造。 |
| mask | 输入 | 源操作数掩码（掩码寄存器），用于指示参与搬出的元素。对应位置为1时参与搬出，为0时不参与搬出。需通过掩码设置接口预先赋值后再传入。 |

矢量数据寄存器、掩码寄存器和地址寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

### 通用约束

- 本接口非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用。
- `dst`起始地址需32字节对齐，否则会报错。
- `mask`必须为`vector_bool`类型掩码寄存器，需通过掩码设置接口预先赋值后再传入。未赋值的掩码寄存器内容不确定，会导致有效元素位置错误。
- UB总容量为256KB，用户可用容量随编译选项与编程场景变化（默认预留6KB SIMD VF栈和2KB Ascend C预留空间，可用248KB；SIMD+SIMT混编时再划分32KB~128KB作为Data Cache，可用容量进一步减少）。`dst`偏移与写出区域对应的UB范围不可超过实际可用容量，否则会报错。
- 如果本指令与其他指令存在UB地址重叠，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化，防止出现异常数据。

### int32_t偏移压缩搬出场景

- `offset`单位为元素，通过`offset`参数偏移后的实际访问地址需落在UB地址范围内，且仍需32字节对齐，否则会报错。
- `mask`比特位为1的源操作数元素参与搬出，为0的元素不参与搬出，压缩后对应的目的位置保持原值。

### addr_reg偏移压缩搬出场景

- `offset`必须由配套的地址寄存器构造接口预先构造。
- 通过`offset`参数偏移后的实际访问地址需落在UB地址范围内，且仍需32字节对齐，否则会报错。
- `mask`比特位为1的源操作数元素参与搬出，为0的元素不参与搬出，压缩后对应的目的位置保持原值。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```c
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t ELEMENT_COUNT = 64;
constexpr uint32_t INPUT_BYTES = 256;
constexpr uint32_t PACKED_BYTES = 64;
constexpr uint32_t REPEAT_TIMES = 2;
constexpr uint32_t STEP_BYTES = 128;
constexpr uint32_t SECTION_BYTES = REPEAT_TIMES * STEP_BYTES;
constexpr uint32_t OUTPUT_BYTES = 3 * SECTION_BYTES;
constexpr uint8_t SENTINEL = 0xa5;

__simd_vf__ inline void pack_quarter_vf(__ubuf__ uint8_t* output,
                                        __ubuf__ uint32_t* input)
{
    vector_bool mask = asc_create_mask_b32(PAT_ALL);
    vector_int32_t src_i32;
    vector_uint32_t src_u32;
    vector_float src_f32;
    asc_loadalign(src_i32, reinterpret_cast<__ubuf__ int32_t*>(input));
    asc_loadalign(src_u32, input);
    asc_loadalign(src_f32, reinterpret_cast<__ubuf__ float*>(input));
    for (uint32_t repeat = 0; repeat < REPEAT_TIMES; ++repeat) {
        asc_storealign_pack_quarter(reinterpret_cast<__ubuf__ int32_t*>(
            output + repeat * STEP_BYTES), src_i32, mask);
        asc_storealign_pack_quarter(reinterpret_cast<__ubuf__ uint32_t*>(
            output + SECTION_BYTES + repeat * STEP_BYTES), src_u32, mask);
        asc_storealign_pack_quarter(reinterpret_cast<__ubuf__ float*>(
            output + 2 * SECTION_BYTES + repeat * STEP_BYTES), src_f32, mask);
    }
}

__global__ __vector__ void asc_storealign_pack_quarter_kernel(
    __gm__ uint8_t* output, __gm__ uint32_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[OUTPUT_BYTES];
    __ubuf__ uint32_t input_local[ELEMENT_COUNT];
    asc_copy_gm2ub(output_local, output, OUTPUT_BYTES);
    asc_copy_gm2ub(input_local, input, INPUT_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    pack_quarter_vf(output_local, input_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm(output, output_local, OUTPUT_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint32_t> input(ELEMENT_COUNT);
    std::vector<uint8_t> output(OUTPUT_BYTES, SENTINEL);
    std::vector<uint8_t> expected(OUTPUT_BYTES, SENTINEL);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        input[i] = 0x5a000000u | i;
        const uint8_t value = static_cast<uint8_t>(input[i]);
        for (uint32_t section = 0; section < 3; ++section) {
            for (uint32_t repeat = 0; repeat < REPEAT_TIMES; ++repeat) {
                expected[section * SECTION_BYTES + repeat * STEP_BYTES + i] = value;
            }
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_storealign_pack_quarter_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == expected;
    std::cout << (passed ? "[Success] asc_storealign_pack_quarter passed."
                         : "[Failed] asc_storealign_pack_quarter output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
