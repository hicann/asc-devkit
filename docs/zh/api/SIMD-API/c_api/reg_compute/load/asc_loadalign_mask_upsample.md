# asc_loadalign_mask_upsample

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

从Unified Buffer（UB）中16字节对齐的起始地址读取VL/16长度数据，将每个bit重复两次，得到VL/8长度数据，并通过函数返回值返回掩码寄存器。

本接口与[asc_loadalign_upsample](asc_loadalign_upsample.md)对齐搬入模式中目的操作数为掩码寄存器的原型功能相同，区别在于本接口通过函数返回值返回结果。

**图1** 上采样搬入掩码寄存器

![](../../figures/capi_loadalign_mask_upsample.png)

本接口仅在AIV上生效，非AIV调用直接返回。

## 函数原型

```c
__simd_callee__ inline vector_bool asc_loadalign_mask_upsample(__ubuf__ uint32_t* src)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| src | 输入 | 源UB地址，实际读取地址必须按16字节对齐，读取VL/16长度数据。 |

## 返回值说明

返回保存2倍上采样搬入结果的掩码寄存器，类型为`vector_bool`，有效数据长度为VL/8。

## 约束说明

- 本接口仅在AIV上生效，非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用。
- `src`的实际读取地址必须按16字节对齐，且实际读取范围必须在UB地址空间内且不越界，否则会报错。
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
constexpr uint32_t MASK_DATA_BYTES = 16;
constexpr uint32_t MASK_STORAGE_BYTES = 32;
constexpr uint32_t MASK_OUTPUT_BYTES = 32;

__simd_vf__ inline void asc_loadalign_mask_upsample_vf(
    __ubuf__ uint32_t* output, __ubuf__ uint32_t* mask_input)
{
    vector_bool mask = asc_loadalign_mask_upsample(mask_input);
    asc_storealign(output, mask);
}

__global__ __vector__ void asc_loadalign_mask_upsample_kernel(
    __gm__ uint32_t* output, __gm__ uint32_t* mask_input)
{
    asc_init();
    __ubuf__ uint32_t output_local[MASK_OUTPUT_BYTES / sizeof(uint32_t)];
    __ubuf__ uint32_t mask_local[MASK_STORAGE_BYTES / sizeof(uint32_t)];
    asc_copy_gm2ub_align(mask_local, mask_input, MASK_DATA_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_vf_call<asc_loadalign_mask_upsample_vf>(output_local, mask_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, MASK_OUTPUT_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint32_t> output(MASK_OUTPUT_BYTES / sizeof(uint32_t), 0);
    std::vector<uint32_t> golden(MASK_OUTPUT_BYTES / sizeof(uint32_t), 0x33333333u);
    std::vector<uint32_t> mask_input(MASK_STORAGE_BYTES / sizeof(uint32_t), 0);
    for (uint32_t i = 0; i < MASK_DATA_BYTES / sizeof(uint32_t); ++i) {
        mask_input[i] = 0x55555555u;
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint32_t* output_device = nullptr;
    uint32_t* mask_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), MASK_OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&mask_device), MASK_STORAGE_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(mask_device, MASK_STORAGE_BYTES, mask_input.data(), MASK_STORAGE_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_loadalign_mask_upsample_kernel<<<1, 0>>>(output_device, mask_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), MASK_OUTPUT_BYTES, output_device, MASK_OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == golden;
    std::cout << (passed ? "[Success]" : "[Failed]")
              << " asc_loadalign_mask_upsample example." << std::endl;
    aclrtFree(output_device);
    aclrtFree(mask_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
