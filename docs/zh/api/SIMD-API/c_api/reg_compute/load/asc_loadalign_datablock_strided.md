# asc_loadalign_datablock_strided

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

头文件路径为：`"c_api/reg_compute/load/loadalign.h"`。

从Unified Buffer（UB）中32字节对齐的起始地址非连续搬入8个`DataBlock`，并通过函数返回值返回保存搬入结果的矢量数据寄存器。每个`DataBlock`的数据量为32字节，支持配置相邻数据块之间的地址步长和本次搬入的起始读取位置。

本接口与[asc_loadalign](asc_loadalign.md)的非连续对齐搬入模式功能相同，区别在于本接口通过函数返回值返回结果。

本接口仅在AIV上生效，非AIV调用直接返回。

## 函数原型

```c
__simd_callee__ inline vector_<dtype> asc_loadalign_datablock_strided(__ubuf__ <dtype>* src,
                                                                      uint16_t block_stride,
                                                                      uint16_t repeat_stride,
                                                                      vector_bool mask)
```

### dtype支持数据类型

dtype支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。当dtype为`int4b_t`时，返回值的实际类型为`vector_int4x2_t`。

### 函数原型典型示例

```c
// 示例：uint8_t类型。
__simd_callee__ inline vector_uint8_t asc_loadalign_datablock_strided(__ubuf__ uint8_t* src,
                                                                      uint16_t block_stride,
                                                                      uint16_t repeat_stride,
                                                                      vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| src | 输入 | 源UB地址，实际读取地址必须按32字节对齐。 |
| block_stride | 输入 | 源操作数相邻`DataBlock`之间起始地址的步长，单位为32字节。 |
| repeat_stride | 输入 | 本次搬入的起始读取地址相对`src`的偏移，单位为32字节。实际起始读取地址为`src`偏移`repeat_stride × 32`字节。 |
| mask | 输入 | 掩码寄存器，用于指示在计算过程中哪些元素参与计算。该接口以`DataBlock`为数据搬运单元。<br>&bull; 当`DataBlock`中的任意一个元素被`mask`筛选成有效元素时，该`DataBlock`中所有数据都会搬入至矢量数据寄存器。<br>&bull; 当`DataBlock`中所有元素都被`mask`筛选成无效元素时，该`DataBlock`中的数据不会搬入到矢量数据寄存器，对应位置的元素设置为0，即使UB越界也不会报错。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[Reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

返回保存非连续对齐搬入结果的矢量数据寄存器，数据类型与`src`保持一致。

## 约束说明

- 本接口仅在AIV上生效，非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用。
- 实际读取地址必须按32字节对齐，且有效`DataBlock`的读取范围必须在UB地址空间内且不越界，否则会报错。
- 当一个`DataBlock`中的元素全部被`mask`设置为无效时，该`DataBlock`即使越界也不会报错。
- `mask`需通过[掩码设置接口](../../defs/type/data_type_definition.md#掩码寄存器)预先赋值后再传入。
- 如果本指令与其他指令存在UB地址重叠，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->

以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t DATABLOCK_BYTES = 32;
constexpr uint32_t DATABLOCK_COUNT = 8;
constexpr uint16_t BLOCK_STRIDE = 2;
constexpr uint16_t REPEAT_STRIDE = 1;
constexpr uint32_t INPUT_BYTES = 16 * DATABLOCK_BYTES;
constexpr uint32_t OUTPUT_BYTES = DATABLOCK_COUNT * DATABLOCK_BYTES;

__simd_vf__ inline void asc_loadalign_datablock_strided_vf(
    __ubuf__ uint8_t* output, __ubuf__ uint8_t* input)
{
    vector_bool mask = asc_create_mask_b8(PAT_ALL);
    vector_uint8_t dst = asc_loadalign_datablock_strided(input, BLOCK_STRIDE, REPEAT_STRIDE, mask);
    asc_storealign(output, dst, mask);
}

__global__ __vector__ void asc_loadalign_datablock_strided_kernel(
    __gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t input_local[INPUT_BYTES];
    __ubuf__ uint8_t output_local[OUTPUT_BYTES];
    asc_copy_gm2ub_align(input_local, input, INPUT_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_vf_call<asc_loadalign_datablock_strided_vf>(output_local, input_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, OUTPUT_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_BYTES);
    std::vector<uint8_t> output(OUTPUT_BYTES, 0xff);
    std::vector<uint8_t> golden(OUTPUT_BYTES, 0);
    for (uint32_t i = 0; i < INPUT_BYTES; ++i) input[i] = static_cast<uint8_t>(i % 251 + 1);
    for (uint32_t block = 0; block < DATABLOCK_COUNT; ++block) {
        const uint32_t src_offset = (REPEAT_STRIDE + block * BLOCK_STRIDE) * DATABLOCK_BYTES;
        const uint32_t dst_offset = block * DATABLOCK_BYTES;
        for (uint32_t i = 0; i < DATABLOCK_BYTES; ++i) {
            golden[dst_offset + i] = input[src_offset + i];
        }
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_loadalign_datablock_strided_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == golden;
    std::cout << (passed ? "[Success]" : "[Failed]")
              << " asc_loadalign_datablock_strided example." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
