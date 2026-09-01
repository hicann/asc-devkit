# asc_gather_datablock

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

给定源操作数在Unified Buffer（UB）中的基地址和索引，根据索引位置将源操作数按DataBlock收集到目的操作数中。每个DataBlock长度为32B。

![](../../figures/asc_gather_datablock.png)

其中，index中仅前8个元素有效，每个元素对应一个DataBlock。例如，第一个元素为96（3 * 32），表示选取DataBlock3写入dst中对应的位置。

## 函数原型

### 支持掩码控制

```c
// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_gather_datablock(vector_<dst_dtype>& dst,
                                                 __ubuf__ <src_dtype>* src,
                                                 vector_uint32_t index,
                                                 vector_bool mask)

// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dst_dtype> asc_gather_datablock(__ubuf__ <src_dtype>* src,
                                                               vector_uint32_t index,
                                                               vector_bool mask)
```

**表1** 支持的数据类型组合

| dst类型 | src类型 | 函数返回值形式 |
|---|---|---|
| `vector_int4x2_t` | `int4b_t` | `vector_int4x2_t` |
| `vector_int8_t` | `int8_t` | `vector_int8_t` |
| `vector_uint8_t` | `uint8_t` | `vector_uint8_t` |
| `vector_fp4x2_e2m1_t` | `fp4x2_e2m1_t` | `vector_fp4x2_e2m1_t` |
| `vector_fp4x2_e1m2_t` | `fp4x2_e1m2_t` | `vector_fp4x2_e1m2_t` |
| `vector_hifloat8_t` | `hifloat8_t` | `vector_hifloat8_t` |
| `vector_fp8_e8m0_t` | `fp8_e8m0_t` | `vector_fp8_e8m0_t` |
| `vector_fp8_e5m2_t` | `fp8_e5m2_t` | `vector_fp8_e5m2_t` |
| `vector_fp8_e4m3fn_t` | `fp8_e4m3fn_t` | `vector_fp8_e4m3fn_t` |
| `vector_int16_t` | `int16_t` | `vector_int16_t` |
| `vector_uint16_t` | `uint16_t` | `vector_uint16_t` |
| `vector_half` | `half` | `vector_half` |
| `vector_bfloat16_t` | `bfloat16_t` | `vector_bfloat16_t` |
| `vector_int32_t` | `int32_t` | `vector_int32_t` |
| `vector_uint32_t` | `uint32_t` | `vector_uint32_t` |
| `vector_float` | `float` | `vector_float` |
| `vector_int64_t` | `int64_t` | 不支持 |
| `vector_uint64_t` | `uint64_t` | 不支持 |

#### 函数原型典型示例

```c
// 通过引用参数输出结果示例：int8_t类型
__simd_callee__ inline void asc_gather_datablock(vector_int8_t& dst,
                                                 __ubuf__ int8_t* src,
                                                 vector_uint32_t index,
                                                 vector_bool mask)

// 通过函数返回值返回结果示例：int8_t类型
__simd_callee__ inline vector_int8_t asc_gather_datablock(__ubuf__ int8_t* src,
                                                          vector_uint32_t index,
                                                          vector_bool mask)
```

### 无掩码控制（占位符形式）

```c
// 通过引用参数输出结果
__simd_callee__ inline void asc_gather_datablock(vector_<dst_dtype>& dst,
                                                 __ubuf__ <src_dtype>* src,
                                                 vector_uint32_t index)

// 通过函数返回值返回结果
__simd_callee__ inline vector_<dst_dtype> asc_gather_datablock(__ubuf__ <src_dtype>* src,
                                                               vector_uint32_t index)
```

**表2** 支持的数据类型组合

| dst类型 | src类型 | 函数返回值形式 |
|---|---|---|
| `vector_int4x2_t` | `int4b_t` | `vector_int4x2_t` |
| `vector_int8_t` | `int8_t` | `vector_int8_t` |
| `vector_uint8_t` | `uint8_t` | `vector_uint8_t` |
| `vector_fp4x2_e2m1_t` | `fp4x2_e2m1_t` | `vector_fp4x2_e2m1_t` |
| `vector_fp4x2_e1m2_t` | `fp4x2_e1m2_t` | `vector_fp4x2_e1m2_t` |
| `vector_hifloat8_t` | `hifloat8_t` | `vector_hifloat8_t` |
| `vector_fp8_e8m0_t` | `fp8_e8m0_t` | `vector_fp8_e8m0_t` |
| `vector_fp8_e5m2_t` | `fp8_e5m2_t` | `vector_fp8_e5m2_t` |
| `vector_fp8_e4m3fn_t` | `fp8_e4m3fn_t` | `vector_fp8_e4m3fn_t` |
| `vector_int16_t` | `int16_t` | `vector_int16_t` |
| `vector_uint16_t` | `uint16_t` | `vector_uint16_t` |
| `vector_half` | `half` | `vector_half` |
| `vector_bfloat16_t` | `bfloat16_t` | `vector_bfloat16_t` |
| `vector_int32_t` | `int32_t` | `vector_int32_t` |
| `vector_uint32_t` | `uint32_t` | `vector_uint32_t` |
| `vector_float` | `float` | `vector_float` |
| `vector_int64_t` | `int64_t` | 不支持 |
| `vector_uint64_t` | `uint64_t` | 不支持 |

#### 函数原型典型示例

```c
// 通过引用参数输出结果示例：int8_t类型
__simd_callee__ inline void asc_gather_datablock(vector_int8_t& dst,
                                                 __ubuf__ int8_t* src,
                                                 vector_uint32_t index)

// 通过函数返回值返回结果示例：int8_t类型
__simd_callee__ inline vector_int8_t asc_gather_datablock(__ubuf__ int8_t* src,
                                                          vector_uint32_t index)
```

## 参数说明

### 支持掩码控制

**表3** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数（矢量数据寄存器），仅适用于通过引用参数输出结果的函数原型。 |
| src | 输入 | 源操作数（矢量）的起始地址。 |
| index | 输入 | 源操作数（矢量数据寄存器），表示dst中每个DataBlock在UB中相对于src的索引位置。索引位置要大于等于0且32B对齐，索引可以存在相同的值。**index仅前8个数有效，单位是字节。** |
| mask | 输入 | 源操作数掩码（掩码寄存器）。**DataBlock搬运的有效指示，按b32格式解释。一个DataBlock对应4bit，仅每4bit中的最低位有效。由于index仅前8个元素有效，因此mask仅使用前8个b32元素对应的bit 0、4、8、12、16、20、24、28，分别控制dst中DataBlock0至DataBlock7是否更新，其余bit无效。** |

### 无掩码控制

**表4** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数（矢量数据寄存器），仅适用于通过引用参数输出结果的函数原型。 |
| src | 输入 | 源操作数（矢量）的起始地址。 |
| index | 输入 | 源操作数（矢量数据寄存器），表示dst中每个DataBlock在UB中相对于src的索引位置。索引位置要大于等于0且32B对齐，索引可以存在相同的值。**index仅前8个数有效，单位是字节。** |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 通过引用参数输出结果的函数原型无返回值。
- 通过函数返回值输出结果的函数原型返回收集结果，返回值类型与对应引用输出函数原型中`dst`参数的类型一致（去除引用）。

## 约束说明

- 通过引用参数输出结果的函数原型在非AIV上调用时直接返回。
- 通过函数返回值输出结果的函数原型在非AIV上调用时返回对应矢量类型的默认构造值。
- b64数据类型属于编译器软件仿真实现，通过函数返回值输出结果的函数原型不做支持。
- 源操作数在UB中的起始地址需要32B对齐。
- 索引位置要大于等于0且32B对齐，即一个索引值对应一个DataBlock。
- 索引可以存在相同的值，即可以多次读取源操作数中同一个DataBlock的数据。
- 索引值对应的数据必须在UB有效地址范围内。
- index仅前8个元素有效。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)。

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
template <typename T>
void PrintData(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

constexpr uint32_t DATA_BLOCK_SIZE = 32;
constexpr uint32_t DATA_BLOCK_COUNT = 8;
constexpr uint32_t BUFFER_BYTES = DATA_BLOCK_SIZE * DATA_BLOCK_COUNT;
constexpr uint32_t INDEX_WORDS = 64;

__simd_vf__ inline void gather_datablock_vf(
    __ubuf__ uint8_t* output, __ubuf__ uint8_t* data, __ubuf__ uint32_t* index)
{
    vector_bool gather_mask = asc_create_mask_b32(PAT_ALL);
    vector_uint32_t index_reg;
    asc_loadalign(index_reg, index);
    vector_uint8_t dst;
    asc_gather_datablock(dst, data, index_reg, gather_mask);
    vector_bool store_mask = asc_create_mask_b8(PAT_ALL);
    asc_storealign(output, dst, store_mask);
}

__global__ __vector__ void asc_gather_datablock_kernel(
    __gm__ uint8_t* output, __gm__ uint8_t* data, __gm__ uint32_t* index)
{
    asc_init();
    __ubuf__ uint8_t output_local[BUFFER_BYTES];
    __ubuf__ uint8_t data_local[BUFFER_BYTES];
    __ubuf__ uint32_t index_local[INDEX_WORDS];
    asc_copy_gm2ub_align(data_local, data, BUFFER_BYTES);
    asc_copy_gm2ub_align(index_local, index, INDEX_WORDS * sizeof(uint32_t));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    gather_datablock_vf(output_local, data_local, index_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> data(BUFFER_BYTES);
    std::vector<uint32_t> index(INDEX_WORDS);
    std::vector<uint8_t> output(BUFFER_BYTES, 0xff);
    std::vector<uint8_t> golden(BUFFER_BYTES);
    for (uint32_t i = 0; i < BUFFER_BYTES; ++i) {
        data[i] = static_cast<uint8_t>(i);
    }
    for (uint32_t b = 0; b < DATA_BLOCK_COUNT; ++b) {
        index[b] = (DATA_BLOCK_COUNT - 1 - b) * DATA_BLOCK_SIZE;
    }
    for (uint32_t b = 0; b < DATA_BLOCK_COUNT; ++b) {
        for (uint32_t j = 0; j < DATA_BLOCK_SIZE; ++j) {
            golden[b * DATA_BLOCK_SIZE + j] =
                static_cast<uint8_t>((DATA_BLOCK_COUNT - 1 - b) * DATA_BLOCK_SIZE + j);
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* data_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&data_device), BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint32_t* index_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&index_device), INDEX_WORDS * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BUFFER_BYTES,
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(data_device, data.size() * sizeof(uint8_t), data.data(), data.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(index_device, index.size() * sizeof(uint32_t), index.data(), index.size() * sizeof(uint32_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_gather_datablock_kernel<<<1, 0>>>(output_device, data_device, index_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    PrintData("Data", data);
    PrintData("Output bytes", output);
    PrintData("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_gather_datablock completed."
                         : "[Failed] asc_gather_datablock output mismatch.")
              << std::endl;
    aclrtFree(data_device);
    aclrtFree(index_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
