---
okftype: reference.api
description: "2倍下采样搬入的Post Update形式：目的为矢量数据寄存器时读取2倍VL并保留偶数下标元素，目的为掩码寄存器时读取VL/4并保留偶数bit，搬运完成后按offset自动更新源UB指针src。"
tag:
  - roles: [data_movement]
  - operations: [compact, load]
  - traits: [aligned_transfer, post_update]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/defs/type/data_type_definition.md
    kind: related
    condition: "矢量数据寄存器与掩码寄存器的类型定义"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/overview.md
    kind: constrained_by
    condition: "Reg矢量计算C API通用约束与UB容量约束"
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_sync/asc_mem_bar.md
    kind: requires
    condition: "与其他指令存在数据依赖时需插入该同步指令"
version: ">=9.0.0"
version_changes:
  - since: "9.1.0"
    kind: parameter_changed
    scope: "param:src"
    scope_basis: resolved
    note: "src 由 __ubuf__ <dtype>* src 改为指针引用 __ubuf__ <dtype>*& src（上游 9.1.0 首次出现 vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset），保证搬运后的地址更新回写调用方；变更前自动更新只作用于局部指针副本。"
  - since: "9.2.0"
    kind: behavior_fixed
    scope: "param:src"
    scope_basis: resolved
    note: "修复 vector_int4x2_t（int4b_t）重载只更新局部源指针副本的问题，使连续调用时源地址更新能够回写调用方并从正确位置继续搬入（Release Notes 9.2.0 Bug修复，与带 block_stride、repeat_stride 的 asc_loadalign_postupdate 重载一并修复）。"
---
# asc_loadalign_downsample_postupdate

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

从Unified Buffer（UB）中32字节对齐的起始地址读取连续数据并进行2倍下采样。接口采用Post Update模式，搬运完成后自动更新源地址指针。

- 当`dst`为矢量数据寄存器时，读取2×VL（512字节）长度数据，保留偶数下标元素后写入VL长度数据。
- 当`dst`为掩码寄存器时，读取VL/4（64字节）长度数据，保留偶数下标bit后写入VL/8长度数据。

**图1** 下采样搬入掩码寄存器

![](../../../../figures/capi_loadalign_mask_downsample.png)

本接口为Reg矢量搬运接口，仅在AIV上生效。

## 函数原型

```c
// 搬入矢量数据寄存器。
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_<dtype>& dst,
                                                                __ubuf__ <dtype>*& src,
                                                                int32_t offset)
// 搬入掩码寄存器。
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_bool& dst,
                                                                __ubuf__ uint32_t*& src,
                                                                int32_t offset)
```

### dtype支持数据类型

`dtype`取值为：`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`。当`dtype`为`int4b_t`时，dst的实际类型为`vector_int4x2_t`。

### 函数原型典型示例

```c
// 示例：int16_t类型。
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_int16_t& dst,
                                                                __ubuf__ int16_t*& src,
                                                                int32_t offset)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的矢量数据寄存器或掩码寄存器。<br>&bull; 当`dst`为矢量数据寄存器，dtype必须与`src`一致，搬入VL长度数据。<br>&bull; 当`dst`为掩码寄存器，搬入VL/8长度数据。 |
| src | 输入/输出 | 源UB地址，实际读取地址必须按32字节对齐。 |
| offset | 输入 | Post Update步长。接口从当前`src`地址完成搬入后更新`src`。<br>&bull; 当`dst`为矢量数据寄存器时，单位为元素个数，`src`增加`offset × sizeof(dtype)`字节。<br>&bull; 当`dst`为掩码寄存器时，单位为字节，`src`增加`offset`字节。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

### 通用约束

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 当前实际读取地址及Post Update更新后的下一次实际读取地址必须按32字节对齐，且实际读取范围必须在UB地址空间内且不越界，否则会报错。
- UB容量上限为256KB，用户可用容量随编译选项与编程场景变化（默认预留6KB SIMD VF栈 + 2KB Ascend C预留，可用248KB；SIMD+SIMT混编时再划分32KB～128KB作Data Cache，可用容量进一步减少）。UB地址偏移后不可超过实际可用容量，否则会报错。
- 如果本指令与其他指令存在数据依赖，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化，防止出现异常数据。

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

constexpr uint32_t INPUT_ELEMENTS = 1030;
constexpr uint32_t OUTPUT_ELEMENTS = 517;
constexpr uint32_t INPUT_BYTES = INPUT_ELEMENTS * sizeof(uint8_t);
constexpr uint32_t OUTPUT_BYTES = OUTPUT_ELEMENTS * sizeof(uint8_t);
constexpr uint32_t ELEMENTS_PER_REGISTER = 256;
// 按单轮处理能力向上取整，确保非整寄存器长度的尾块也被处理。
constexpr uint32_t REPEAT_COUNT = AscendC::Std::ceil_div(OUTPUT_ELEMENTS, ELEMENTS_PER_REGISTER);
constexpr uint32_t OUTPUT_STORAGE_ELEMENTS = REPEAT_COUNT * ELEMENTS_PER_REGISTER;
constexpr uint32_t INPUT_STORAGE_ELEMENTS = REPEAT_COUNT * 512;

// 统一记录实际处理长度，用于计算迭代次数并限制尾轮的有效范围。
constexpr uint32_t TOTAL_LENGTH = OUTPUT_ELEMENTS;
constexpr uint32_t DATA_BYTES = TOTAL_LENGTH * sizeof(uint8_t);
__simd_vf__ inline void asc_loadalign_downsample_postupdate_vf(__ubuf__ uint8_t* output,
                                                              __ubuf__ uint8_t* input,
                                                              uint32_t one_repeat_size, uint16_t repeat_time)
{
    // 保留剩余有效元素数，供更新掩码处理尾块。
    uint32_t remaining_length = OUTPUT_ELEMENTS;
    vector_uint8_t dst_reg;
    __ubuf__ uint8_t* src = input;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        vector_bool mask = asc_update_mask_b8(remaining_length);
        asc_loadalign_downsample_postupdate(dst_reg, src, 512);
        asc_storealign(output + i * one_repeat_size, dst_reg, mask);
    }
}

__global__ __vector__ void asc_loadalign_downsample_postupdate_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t input_local[INPUT_STORAGE_ELEMENTS];
    __ubuf__ uint8_t output_local[OUTPUT_STORAGE_ELEMENTS];
    asc_lock(PIPE_MTE2, MUTEX_ID);
    asc_copy_gm2ub_align(input_local, input, INPUT_STORAGE_ELEMENTS);
    asc_unlock(PIPE_MTE2, MUTEX_ID);

    uint32_t one_repeat_size = static_cast<uint32_t>(asc_get_vf_len() / sizeof(uint8_t));
    uint16_t repeat_time = static_cast<uint16_t>(AscendC::Std::ceil_div(TOTAL_LENGTH, one_repeat_size));
    asc_lock(PIPE_V, MUTEX_ID);
    asc_vf_call<asc_loadalign_downsample_postupdate_vf>(output_local, input_local, one_repeat_size, repeat_time);
    asc_unlock(PIPE_V, MUTEX_ID);

    asc_lock(PIPE_MTE3, MUTEX_ID);
    asc_copy_ub2gm_align(output, output_local, OUTPUT_BYTES);
    asc_unlock(PIPE_MTE3, MUTEX_ID);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_STORAGE_ELEMENTS, 0);
    std::vector<uint8_t> output(OUTPUT_ELEMENTS, 0xff);
    std::vector<uint8_t> golden(OUTPUT_ELEMENTS, 0);
    for (uint16_t i = 0; i < INPUT_ELEMENTS; ++i) input[i] = static_cast<uint8_t>(i % 251 + 1);
    for (uint16_t i = 0; i < OUTPUT_ELEMENTS; ++i) {
        golden[i] = input[(i / 256) * 512 + 2 * (i % 256)];
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_STORAGE_ELEMENTS, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_STORAGE_ELEMENTS, input.data(), INPUT_STORAGE_ELEMENTS, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_loadalign_downsample_postupdate_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    const bool passed = output == golden;
    std::cout << (passed ? "[Success]" : "[Failed]") << " asc_loadalign_downsample_postupdate example." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
