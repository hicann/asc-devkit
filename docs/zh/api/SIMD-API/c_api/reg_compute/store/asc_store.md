# asc_store

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

头文件路径为：`"c_api/composite/storeunalign_composite.h"`。

将矢量数据寄存器中的数据搬出到Unified Buffer（UB），目的地址需按照`dtype`对齐。底层经非对齐搬出指令完成单次连续写入，搬运过程中数据格式和内容保持不变。在追求极致性能时，对齐场景推荐使用[asc_storealign](asc_storealign.md)接口。本接口在Vector Function（`__simd_vf__`标记的函数）内使用。本接口提供两种参数列表不同的功能模式：

- **连续搬出模式**：将整个矢量数据寄存器内容写入`dst`起始地址，搬出量为一个矢量长度（`VL`，默认256字节）。
- **带搬出数量模式**：仅将矢量数据寄存器前`count`个元素写入`dst`起始地址，用于部分搬出场景。

本接口在AIV上执行有效。

## 函数原型

### 连续搬出模式

```c
// 占位符形式
__simd_callee__ inline void asc_store(__ubuf__ <dtype>* dst,
                                      vector_<dtype> src)
```

#### dtype支持数据类型

`dtype`支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e1m2_t`、`fp4x2_e2m1_t`、`hifloat8_t`、`fp8_e4m3fn_t`、`fp8_e5m2_t`、`fp8_e8m0_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`、`int64_t`。对应矢量寄存器类型`vector_<dtype>`，`int4b_t`对应`vector_int4x2_t`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_store(__ubuf__ int8_t* dst,
                                      vector_int8_t src)
```

### 带搬出数量模式

```c
__simd_callee__ inline void asc_store(__ubuf__ <dtype>* dst,
                                      vector_<dtype> src,
                                      uint32_t count)
```

#### dtype支持数据类型

`dtype`支持的数据类型为`int4b_t`、`int8_t`、`uint8_t`、`fp4x2_e1m2_t`、`fp4x2_e2m1_t`、`hifloat8_t`、`fp8_e4m3fn_t`、`fp8_e5m2_t`、`fp8_e8m0_t`、`int16_t`、`uint16_t`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`、`int64_t`。对应矢量寄存器类型`vector_<dtype>`，`int4b_t`对应`vector_int4x2_t`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_store(__ubuf__ int8_t* dst,
                                      vector_int8_t src,
                                      uint32_t count)
```

## 参数说明

### 连续搬出模式

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数（矢量）的起始地址，类型为`__ubuf__ <dtype>*`。地址需按照`dtype`对齐。 |
| src | 输入 | 源矢量数据寄存器。 |

### 带搬出数量模式

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数（矢量）的起始地址，类型为`__ubuf__ <dtype>*`。地址需按照`dtype`对齐。 |
| src | 输入 | 源矢量数据寄存器，`dtype`须与`dst`一致。仅前`count`个元素参与搬出，其余元素被忽略。 |
| count | 输入 | 实际搬出的元素个数。不可大于矢量数据寄存器容纳的最大元素个数。 |

矢量数据寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

### 通用约束

- 本接口非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用，源操作数为矢量数据寄存器，目的操作数为UB地址，UB地址空间外的指针不可作为`dst`传入。
- `dst`起始地址需按照`dtype`对齐。本接口按非对齐方式将矢量数据寄存器内容写入UB，不要求对齐，不会报错。
- UB容量上限：UB容量上限为256KB，用户可用容量随编译选项与编程场景变化（默认预留6KB SIMD VF栈+2KB Ascend C预留，可用248KB；SIMD+SIMT混编时再划分32KB~128KB作Data Cache，可用容量进一步减少）。目的地址不可超过实际可用容量，否则会报错。
- 如果本指令与其他指令存在UB地址重叠，需要插入同步指令[asc_mem_bar](../reg_sync/asc_mem_bar.md)，保证多个指令串行化，防止出现异常数据。

### 带搬出数量模式约束

- `count`的取值范围为`[0, VL/sizeof(dtype)]`，`VL`为256字节。超出该范围会导致未定义行为。偏移后不可超过实际UB可用容量，否则会报错。

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
#include "acl/acl.h"
#include "c_api/asc_simd.h"

namespace {
constexpr uint32_t ELEMENT_COUNT = 128;
constexpr uint32_t PARTIAL_COUNT = 16;
constexpr uint32_t REPEAT_TIMES = 2;
constexpr uint32_t FULL_BASE = 1;
constexpr uint32_t FULL_STRIDE = 144;
constexpr uint32_t PARTIAL_BASE = 288;
constexpr uint32_t PARTIAL_OFFSET = 7;
constexpr uint32_t PARTIAL_STRIDE = 32;
constexpr uint32_t OUTPUT_COUNT = 384;
constexpr uint16_t SENTINEL = 0xffff;
constexpr uint32_t INPUT_BYTES = ELEMENT_COUNT * sizeof(uint16_t);
constexpr uint32_t OUTPUT_BYTES = OUTPUT_COUNT * sizeof(uint16_t);

__simd_vf__ inline void store_vf(__ubuf__ uint16_t* output, __ubuf__ uint16_t* input)
{
    vector_uint16_t src;
    asc_loadalign(src, input);
    for (uint32_t repeat = 0; repeat < REPEAT_TIMES; ++repeat) {
        asc_store(output + FULL_BASE + repeat * FULL_STRIDE, src);
        asc_store(output + PARTIAL_BASE + PARTIAL_OFFSET + repeat * PARTIAL_STRIDE,
                  src, PARTIAL_COUNT);
    }
}

__global__ __vector__ void store_kernel(__gm__ uint16_t* output, __gm__ uint16_t* input)
{
    asc_init();
    __ubuf__ uint16_t input_local[ELEMENT_COUNT];
    __ubuf__ uint16_t output_local[OUTPUT_COUNT];
    asc_copy_gm2ub(input_local, input, INPUT_BYTES);
    asc_copy_gm2ub(output_local, output, OUTPUT_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_vf_call<store_vf>(output_local, input_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm(output, output_local, OUTPUT_BYTES);
    asc_sync_pipe(PIPE_ALL);
}
} // namespace

#define CHECK_ACL(expr)                                                                     \
    do {                                                                                    \
        if ((expr) != ACL_SUCCESS) {                                                        \
            std::cerr << "ACL call failed: " << #expr << std::endl;                        \
            return 1;                                                                       \
        }                                                                                   \
    } while (0)

int main()
{
    std::vector<uint16_t> input(ELEMENT_COUNT);
    std::vector<uint16_t> output(OUTPUT_COUNT, SENTINEL);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        input[i] = static_cast<uint16_t>(i + 1);
    }

    CHECK_ACL(aclInit(nullptr));
    CHECK_ACL(aclrtSetDevice(0));
    aclrtStream stream = nullptr;
    uint16_t* input_device = nullptr;
    uint16_t* output_device = nullptr;
    CHECK_ACL(aclrtCreateStream(&stream));
    CHECK_ACL(aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST));
    CHECK_ACL(aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST));
    CHECK_ACL(aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE));
    CHECK_ACL(aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE));
    store_kernel<<<1, 0, stream>>>(output_device, input_device);
    CHECK_ACL(aclrtSynchronizeStream(stream));
    CHECK_ACL(aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST));

    std::vector<uint16_t> expected(OUTPUT_COUNT, SENTINEL);
    for (uint32_t repeat = 0; repeat < REPEAT_TIMES; ++repeat) {
        for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
            expected[FULL_BASE + repeat * FULL_STRIDE + i] = input[i];
        }
        for (uint32_t i = 0; i < PARTIAL_COUNT; ++i) {
            expected[PARTIAL_BASE + PARTIAL_OFFSET + repeat * PARTIAL_STRIDE + i] = input[i];
        }
    }

    bool passed = true;
    for (uint32_t i = 0; i < OUTPUT_COUNT; ++i) {
        if (output[i] != expected[i]) {
            std::cerr << "Mismatch at " << i << ", expected " << expected[i]
                      << ", actual " << output[i] << std::endl;
            passed = false;
            break;
        }
    }
    std::cout << (passed ? "[Success] asc_store completed."
                         : "[Failed] asc_store output mismatch.") << std::endl;

    CHECK_ACL(aclrtFree(output_device));
    CHECK_ACL(aclrtFree(input_device));
    CHECK_ACL(aclrtDestroyStream(stream));
    CHECK_ACL(aclrtResetDevice(0));
    CHECK_ACL(aclFinalize());
    return passed ? 0 : 1;
}
```
