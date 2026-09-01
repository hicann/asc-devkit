# asc_storeunalign_post

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

将非对齐寄存器中暂存的尾块写入Unified Buffer（UB），用于连续非对齐搬运收尾。本接口提供以下两种模式：

- **立即数偏移搬出模式**：通过立即数指定相对目的起始地址的偏移，单位为元素，用户可选择手动更新偏移或更新目的地址，用于配合前序非对齐搬出接口[asc_storeunalign](asc_storeunalign.md)或[asc_storeunalign_postupdate掩码寄存器搬出模式](asc_storeunalign_postupdate.md)收尾。
- **地址寄存器偏移搬出模式**：通过地址寄存器指定相对目的起始地址的偏移，常用于Hardware Loop内偏移随循环计数变化的搬出场景。需要与[asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md)配合使用。用于配合前序非对齐搬出接口[asc_storeunalign_postupdate地址寄存器偏移搬出模式](asc_storeunalign_postupdate.md)收尾。

本接口仅在AIV上生效。

## 函数原型

### 立即数偏移搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ <dtype>* dst,
                                                  vector_store_unalign unalign_reg,
                                                  int32_t offset)
```

#### dtype支持数据类型

`dtype`支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`、`int64_t`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint8_t* dst,
                                                  vector_store_unalign unalign_reg,
                                                  int32_t offset)
```

### 地址寄存器偏移搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ <dtype>* dst,
                                                  vector_store_unalign unalign_reg,
                                                  addr_reg offset)
```

#### dtype支持数据类型

`dtype`支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`、`int64_t`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storeunalign_post(__ubuf__ uint8_t* dst,
                                                  vector_store_unalign unalign_reg,
                                                  addr_reg offset)
```

## 参数说明

### 立即数偏移搬出模式

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数在UB中的基地址，类型为`__ubuf__ <dtype>*`。地址无需32字节对齐，但须按`sizeof(dtype)`字节对齐。 |
| unalign_reg | 输入 | 非对齐寄存器，类型为`vector_store_unalign`，长度为32字节。须与前序主搬出接口使用同一个寄存器。 |
| offset | 输入 | 目的操作数结束地址的偏移，类型为`int32_t`，单位为元素。 |

### 地址寄存器偏移搬出模式

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数在UB中的基地址，类型为`__ubuf__ <dtype>*`。地址无需32字节对齐，但须按`sizeof(dtype)`字节对齐。 |
| unalign_reg | 输入 | 非对齐寄存器，类型为`vector_store_unalign`，长度为32字节。须与前序主搬出接口使用同一个寄存器。 |
| offset | 输入 | 目的操作数结束地址的偏移，类型为`addr_reg`。须与前序主搬出接口使用同一个寄存器。 |

非对齐寄存器和地址寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

### 通用约束

- 非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用。
- 需要保证目的操作数的地址加上`offset`对应的偏移地址，访问范围须位于实际可用UB范围内。
- 该接口中的目的地址不需要32B对齐，但数据类型为`dtype`的`dst`需要`sizeof(dtype)`字节对齐。
- UB容量上限为256KB，用户可用容量随编译选项与编程场景变化（默认预留6KB SIMD VF栈+2KB Ascend C预留，可用248KB；SIMD+SIMT混编时再划分32KB~128KB作Data Cache，可用容量进一步减少）。目的操作数地址不可超过实际可用容量，否则会报错。
- 如果本指令与其他指令存在UB地址重叠，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化，防止出现异常数据。

### 立即数偏移搬出模式

- 与[asc_storeunalign](asc_storeunalign.md)或[asc_storeunalign_postupdate](asc_storeunalign_postupdate.md)的**掩码寄存器搬出模式**配套使用，需要复用同一个`unalign_reg`。
- 需要保证目的地址加上偏移后的地址，等于[asc_storeunalign](asc_storeunalign.md)或[asc_storeunalign_postupdate](asc_storeunalign_postupdate.md)的**掩码寄存器搬出模式**搬运的结束地址，否则无法正确收尾。

### 地址寄存器偏移搬出模式

- 与[asc_storeunalign_postupdate](asc_storeunalign_postupdate.md)的**地址寄存器偏移搬出模式**配套使用，需要复用同一个非对齐寄存器`unalign_reg`及地址寄存器。

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
constexpr uint32_t ELEMENT_COUNT = 256;
constexpr uint32_t START_OFFSET = 8;
constexpr uint32_t OUTPUT_BYTES = START_OFFSET + ELEMENT_COUNT;

__simd_vf__ inline void storeunalign_post_vf(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input)
{
    vector_uint8_t src;
    vector_store_unalign unalign_reg;
    asc_loadalign(src, input);
    asc_storeunalign(output + START_OFFSET, unalign_reg, src, ELEMENT_COUNT);
    asc_storeunalign_post(output + START_OFFSET, unalign_reg, ELEMENT_COUNT);
}

__global__ __vector__ void storeunalign_post_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[288];
    __ubuf__ uint8_t input_local[ELEMENT_COUNT];
    asc_copy_gm2ub_align(input_local, input, ELEMENT_COUNT);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    storeunalign_post_vf(output_local, input_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, OUTPUT_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input(ELEMENT_COUNT);
    std::vector<uint8_t> output(OUTPUT_BYTES, 0);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) input[i] = static_cast<uint8_t>(i);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), ELEMENT_COUNT, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, ELEMENT_COUNT, input.data(), ELEMENT_COUNT, ACL_MEMCPY_HOST_TO_DEVICE);
    storeunalign_post_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) passed = passed && output[START_OFFSET + i] == input[i];
    std::cout << (passed ? "[Success] asc_storeunalign_post completed."
                         : "[Failed] asc_storeunalign_post output mismatch.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
