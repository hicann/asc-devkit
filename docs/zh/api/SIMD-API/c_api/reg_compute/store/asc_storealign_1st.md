# asc_storealign_1st

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

将矢量数据寄存器中的首个元素搬出到Unified Buffer（UB），单次搬出量为一个元素。本接口不支持设置掩码，固定搬出矢量数据寄存器中第一个元素到目的地址处，目的地址需按`sizeof(dtype)`对齐，搬运过程中数据格式与内容保持不变。本接口在Vector Function（`__simd_vf__`标记的函数）内使用。

本接口提供三种参数列表不同的功能模式：

- **对齐搬出模式**：从矢量数据寄存器首个元素搬出到UB起始地址，多次调用时由用户自行更新目的地址。
- **立即数偏移搬出模式**：通过`int32_t offset`指定相对目的起始地址的元素偏移，用户可选择更新偏移或更新目的地址。
- **地址寄存器偏移搬出模式**：通过地址寄存器`addr_reg offset`指定相对目的起始地址的偏移，常用于Hardware Loop内偏移随循环计数变化的对齐搬出场景。需要与[asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md)配合使用。

以**b16位宽单点搬出**过程为例，示意图如下：

**图1** b16单个元素搬出数据

![](../../../../figures/reg_store_onept.png)

本接口仅在AIV上执行有效。

## 函数原型

### 对齐搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storealign_1st(__ubuf__ <dtype>* dst,
                                               vector_<dtype> src)
```

#### dtype支持数据类型

`dtype`支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。对应矢量寄存器类型`vector_<dtype>`，`int4b_t`对应`vector_int4x2_t`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storealign_1st(__ubuf__ int8_t* dst,
                                               vector_int8_t src)
```

### 立即数偏移搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storealign_1st(__ubuf__ <dtype>* dst,
                                               vector_<dtype> src,
                                               int32_t offset)
```

#### dtype支持数据类型

`dtype`支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。对应矢量寄存器类型`vector_<dtype>`，`int4b_t`对应`vector_int4x2_t`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storealign_1st(__ubuf__ int8_t* dst,
                                               vector_int8_t src,
                                               int32_t offset)
```

### 地址寄存器偏移搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storealign_1st(__ubuf__ <dtype>* dst,
                                               vector_<dtype> src,
                                               addr_reg offset)
```

#### dtype支持数据类型

`dtype`支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。对应矢量寄存器类型`vector_<dtype>`，`int4b_t`对应`vector_int4x2_t`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storealign_1st(__ubuf__ int8_t* dst,
                                               vector_int8_t src,
                                               addr_reg offset)
```

## 参数说明

### 对齐搬出模式

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数的起始地址，类型为`__ubuf__ <dtype>*`，起始地址需dtype对齐。 |
| src | 输入 | 源操作数（矢量数据寄存器）。仅首个元素参与搬出，其余元素被忽略。 |

### 立即数偏移搬出模式

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数的起始地址，类型为`__ubuf__ <dtype>*`，起始地址需dtype对齐。 |
| src | 输入 | 源操作数（矢量数据寄存器）。仅首个元素参与搬出，其余元素被忽略。 |
| offset | 输入 | 相对`dst`起始地址的偏移量，类型为`int32_t`，单位为元素。 |

### 地址寄存器偏移搬出模式

**表3** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数的起始地址，类型为`__ubuf__ <dtype>*`，起始地址需dtype对齐。 |
| src | 输入 | 源操作数（矢量数据寄存器）。仅首个元素参与搬出，其余元素被忽略。 |
| offset | 输入 | 地址寄存器偏移，类型为`addr_reg`，由配套的地址寄存器构造接口预先构造，单位为元素。 |

矢量数据寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

### 通用约束

- 本接口仅在AIV上生效，非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用，源操作数为矢量数据寄存器，目的操作数为UB地址，UB地址空间外的指针不可作为`dst`传入。
- UB容量上限：UB总容量256KB，用户可用容量随编译选项与编程场景变化（默认预留6KB SIMD VF栈+2KB Ascend C预留，可用248KB；SIMD+SIMT混编时再划分32KB~128KB作Data Cache，可用容量进一步减少）。`dst`偏移后不可超过实际可用容量。
- 如果本指令与其他指令存在UB地址重叠，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化，防止出现异常数据。

### 指令约束

- `dst`起始地址需dtype对齐。，`offset`偏移后的实际访问地址需dtype对齐且需落在UB地址范围内。

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
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
constexpr uint32_t ELEMENT_COUNT = 128;
constexpr uint32_t REPEAT_TIMES = 3;
constexpr uint32_t DIRECT_BASE = 0;
constexpr uint32_t IMMEDIATE_BASE = 16;
constexpr uint32_t IMMEDIATE_OFFSET = 17;
constexpr uint32_t ADDR_BASE = 64;
constexpr uint32_t ADDR_STRIDE = 7;
constexpr uint16_t SENTINEL = 0xffff;
constexpr uint32_t BUFFER_BYTES = ELEMENT_COUNT * sizeof(uint16_t);

__simd_vf__ inline void store_first_vf(__ubuf__ uint16_t* output, __ubuf__ uint16_t* input)
{
    vector_uint16_t src;
    asc_loadalign(src, input);
    addr_reg offset;
    for (uint32_t i = 0; i < REPEAT_TIMES; ++i) {
        asc_storealign_1st(output + DIRECT_BASE + i, src);
        asc_storealign_1st(output + IMMEDIATE_BASE + i, src,
                           static_cast<int32_t>(IMMEDIATE_OFFSET));
        offset = asc_update_addr_reg_b16(ADDR_STRIDE);
        asc_storealign_1st(output + ADDR_BASE, src, offset);
    }
}

__global__ __vector__ void store_first_kernel(__gm__ uint16_t* output, __gm__ uint16_t* input)
{
    asc_init();
    __ubuf__ uint16_t input_local[ELEMENT_COUNT];
    __ubuf__ uint16_t output_local[ELEMENT_COUNT];
    asc_copy_gm2ub(input_local, input, BUFFER_BYTES);
    asc_copy_gm2ub(output_local, output, BUFFER_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_vf_call<store_first_vf>(output_local, input_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm(output, output_local, BUFFER_BYTES);
    asc_sync_pipe(PIPE_ALL);
}
} // namespace

int main()
{
    std::vector<uint16_t> input(ELEMENT_COUNT);
    std::vector<uint16_t> output(ELEMENT_COUNT, SENTINEL);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) input[i] = static_cast<uint16_t>(i + 1);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* input_device = nullptr;
    uint16_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BUFFER_BYTES, input.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, BUFFER_BYTES, output.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    store_first_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BUFFER_BYTES, output_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        const bool written = (i >= DIRECT_BASE && i < DIRECT_BASE + REPEAT_TIMES) ||
            (i >= IMMEDIATE_BASE + IMMEDIATE_OFFSET &&
             i < IMMEDIATE_BASE + IMMEDIATE_OFFSET + REPEAT_TIMES) ||
            (i >= ADDR_BASE && i < ADDR_BASE + REPEAT_TIMES * ADDR_STRIDE &&
             (i - ADDR_BASE) % ADDR_STRIDE == 0);
        passed = passed && output[i] == (written ? input[0] : SENTINEL);
    }
    std::cout << (passed ? "[Success] asc_storealign_1st completed."
                         : "[Failed] asc_storealign_1st output mismatch.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
