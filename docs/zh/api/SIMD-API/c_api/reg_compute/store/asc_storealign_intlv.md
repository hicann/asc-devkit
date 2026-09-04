# asc_storealign_intlv

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

将两个矢量数据寄存器中的数据按元素顺序交织后连续对齐搬出到Unified Buffer（UB）。单次搬出量为`2 × VL`（共512字节），不支持配置掩码。搬运过程中数据格式与内容保持不变。本接口在Vector Function（`__simd_vf__`标记的函数）内使用。

本接口提供三种参数列表不同的功能模式：

- **对齐搬出模式**：偏移固定为0，将`src0`和`src1`中的数据交织搬出到UB起始地址，由用户自行更新目的地址。
- **立即数偏移搬出模式**：通过`int32_t offset`指定相对目的起始地址的偏移，用户可选择更新偏移或更新目的地址。
- **地址寄存器偏移搬出模式**：通过地址寄存器`addr_reg offset`指定相对目的起始地址的偏移，常用于Hardware Loop内偏移随循环计数变化的对齐搬出场景。需要与[asc_update_addr_reg](../reg_addr_reg/asc_update_addr_reg.md)配合使用。

本接口仅在AIV上执行有效。

## 函数原型

### 对齐搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ <dtype>* dst,
                                                 vector_<dtype> src0,
                                                 vector_<dtype> src1)
```

#### dtype支持数据类型

`dtype`支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e1m2_t`、`fp4x2_e2m1_t`、`hifloat8_t`、`fp8_e4m3fn_t`、`fp8_e5m2_t`、`fp8_e8m0_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`。对应矢量寄存器类型`vector_<dtype>`，`int4b_t`对应`vector_int4x2_t`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ int8_t* dst,
                                                 vector_int8_t src0,
                                                 vector_int8_t src1)
```

### 立即数偏移搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ <dtype>* dst,
                                                 vector_<dtype> src0,
                                                 vector_<dtype> src1,
                                                 int32_t offset)
```

#### dtype支持数据类型

`dtype`支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e1m2_t`、`fp4x2_e2m1_t`、`hifloat8_t`、`fp8_e4m3fn_t`、`fp8_e5m2_t`、`fp8_e8m0_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`。对应矢量寄存器类型`vector_<dtype>`，`int4b_t`对应`vector_int4x2_t`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ int8_t* dst,
                                                 vector_int8_t src0,
                                                 vector_int8_t src1,
                                                 int32_t offset)
```

### 地址寄存器偏移搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ <dtype>* dst,
                                                 vector_<dtype> src0,
                                                 vector_<dtype> src1,
                                                 addr_reg offset)
```

#### dtype支持数据类型

`dtype`支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e1m2_t`、`fp4x2_e2m1_t`、`hifloat8_t`、`fp8_e4m3fn_t`、`fp8_e5m2_t`、`fp8_e8m0_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`。对应矢量寄存器类型`vector_<dtype>`，`int4b_t`对应`vector_int4x2_t`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_storealign_intlv(__ubuf__ int8_t* dst,
                                                 vector_int8_t src0,
                                                 vector_int8_t src1,
                                                 addr_reg offset)
```


## 参数说明

### 对齐搬出模式

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数（矢量）的起始地址。类型为`__ubuf__ <dtype>*`，起始地址需32字节对齐。 |
| src0 | 输入 | 源操作数0（矢量数据寄存器）。`dtype`须与`dst`一致。 |
| src1 | 输入 | 源操作数1（矢量数据寄存器）。`dtype`须与`dst`一致。 |

### 立即数偏移搬出模式

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数（矢量）的起始地址。类型为`__ubuf__ <dtype>*`，起始地址需32字节对齐。 |
| src0 | 输入 | 源操作数0（矢量数据寄存器）。`dtype`须与`dst`一致。 |
| src1 | 输入 | 源操作数1（矢量数据寄存器）。`dtype`须与`dst`一致。 |
| offset | 输入 | 相对`dst`起始地址的偏移量，类型为`int32_t`，单位为元素。 |

### 地址寄存器偏移搬出模式

**表3** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数（矢量）的起始地址。类型为`__ubuf__ <dtype>*`，起始地址需32字节对齐。 |
| src0 | 输入 | 源操作数0（矢量数据寄存器）。`dtype`须与`dst`一致。 |
| src1 | 输入 | 源操作数1（矢量数据寄存器）。`dtype`须与`dst`一致。 |
| offset | 输入 | 地址寄存器偏移，类型为`addr_reg`，由配套的地址寄存器构造接口预先构造。 |

矢量数据寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

### 通用约束

- 本接口非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用。
- `dst`起始地址需32字节对齐，否则会报错。
- UB容量上限为256KB，用户可用容量随编译选项与编程场景变化（默认预留6KB SIMD VF栈+2KB Ascend C预留，可用248KB；SIMD+SIMT混编时再划分32KB~128KB作Data Cache，可用容量进一步减少）。目的操作数地址偏移后不可超过实际可用容量，否则会报错。
- 通过`offset`参数偏移后的实际访问地址需落在UB地址范围内，且实际访问地址仍需32字节对齐，否则会报错。
- 如果本指令与其他指令存在UB地址重叠，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化，防止出现异常数据。

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
constexpr uint32_t INPUT_COUNT = 128;
constexpr uint32_t STORE_COUNT = INPUT_COUNT * 2;
constexpr uint32_t REPEAT_TIMES = 2;
constexpr uint32_t DIRECT_STRIDE = 272;
constexpr uint32_t IMMEDIATE_BASE = 544;
constexpr uint32_t IMMEDIATE_OFFSET = 272;
constexpr uint32_t ADDR_BASE = 1360;
constexpr uint32_t ADDR_STRIDE = 272;
constexpr uint32_t OUTPUT_COUNT = 1904;
constexpr uint16_t SENTINEL = 0xffff;
constexpr uint32_t INPUT_BYTES = INPUT_COUNT * sizeof(uint16_t);
constexpr uint32_t OUTPUT_BYTES = OUTPUT_COUNT * sizeof(uint16_t);

__simd_vf__ inline void store_intlv_vf(__ubuf__ uint16_t* output,
                                       __ubuf__ uint16_t* input0,
                                       __ubuf__ uint16_t* input1)
{
    vector_uint16_t src0;
    vector_uint16_t src1;
    asc_loadalign(src0, input0);
    asc_loadalign(src1, input1);
    addr_reg offset;
    for (uint32_t i = 0; i < REPEAT_TIMES; ++i) {
        asc_storealign_intlv(output + i * DIRECT_STRIDE, src0, src1);
        asc_storealign_intlv(output + IMMEDIATE_BASE + i * DIRECT_STRIDE,
                             src0, src1, static_cast<int32_t>(IMMEDIATE_OFFSET));
        offset = asc_update_addr_reg_b16(ADDR_STRIDE);
        asc_storealign_intlv(output + ADDR_BASE, src0, src1, offset);
    }
}

__global__ __vector__ void store_intlv_kernel(__gm__ uint16_t* output,
                                               __gm__ uint16_t* input0,
                                               __gm__ uint16_t* input1)
{
    asc_init();
    __ubuf__ uint16_t output_local[OUTPUT_COUNT];
    __ubuf__ uint16_t input0_local[INPUT_COUNT];
    __ubuf__ uint16_t input1_local[INPUT_COUNT];
    asc_copy_gm2ub(output_local, output, OUTPUT_BYTES);
    asc_copy_gm2ub(input0_local, input0, INPUT_BYTES);
    asc_copy_gm2ub(input1_local, input1, INPUT_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    store_intlv_vf(output_local, input0_local, input1_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm(output, output_local, OUTPUT_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint16_t> input0(INPUT_COUNT), input1(INPUT_COUNT);
    std::vector<uint16_t> output(OUTPUT_COUNT, SENTINEL);
    for (uint32_t i = 0; i < INPUT_COUNT; ++i) {
        input0[i] = static_cast<uint16_t>(i + 1);
        input1[i] = static_cast<uint16_t>(i + 1001);
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* input0_device = nullptr;
    uint16_t* input1_device = nullptr;
    uint16_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input0_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&input1_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input0_device, INPUT_BYTES, input0.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(input1_device, INPUT_BYTES, input1.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    store_intlv_kernel<<<1, 0>>>(output_device, input0_device, input1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (uint32_t i = 0; i < OUTPUT_COUNT; ++i) {
        uint32_t relative = 0;
        bool written = false;
        const uint32_t starts[] = {
            0, DIRECT_STRIDE,
            IMMEDIATE_BASE + IMMEDIATE_OFFSET,
            IMMEDIATE_BASE + DIRECT_STRIDE + IMMEDIATE_OFFSET,
            ADDR_BASE, ADDR_BASE + ADDR_STRIDE
        };
        for (uint32_t start : starts) {
            if (i >= start && i < start + STORE_COUNT) {
                relative = i - start;
                written = true;
            }
        }
        const uint16_t expected = !written ? SENTINEL :
            (relative % 2 == 0 ? input0[relative / 2] : input1[relative / 2]);
        passed = passed && output[i] == expected;
    }
    std::cout << (passed ? "[Success] asc_storealign_intlv completed."
                         : "[Failed] asc_storealign_intlv output mismatch.") << std::endl;
    aclrtFree(input0_device);
    aclrtFree(input1_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
