# asc_intlv

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

将两个源操作数矢量数据寄存器中的数据交织后分别写入两个目的矢量数据寄存器。根据操作数类型，本接口提供两种功能模式：

- **操作数为矢量数据寄存器：** 以元素为单位进行交织。`src0`和`src1`前半部分的元素依次交替写入`dst0`，后半部分的元素依次交替写入`dst1`。以`int8_t`类型为例，交织过程如下图所示。
- **操作数为掩码寄存器：** 根据接口后缀指定的粒度进行交织。`asc_intlv_b8`以1bit为一组，`asc_intlv_b16`以2bit为一组，`asc_intlv_b32`以4bit为一组。

**图 1**  int8_t类型交织过程

![](../../figures/asc_intlv.png)

本接口仅在AIV上生效，非AIV调用直接返回。

## 函数原型

### 矢量数据寄存器交织模式

```c
// 占位符形式
__simd_callee__ inline void asc_intlv(vector_<dtype>& dst0,
                                      vector_<dtype>& dst1,
                                      vector_<dtype> src0,
                                      vector_<dtype> src1)
```

#### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

#### 函数原型典型示例

```c
__simd_callee__ inline void asc_intlv(vector_int8_t& dst0,
                                      vector_int8_t& dst1,
                                      vector_int8_t src0,
                                      vector_int8_t src1)
```

### 掩码寄存器交织模式

```c
__simd_callee__ inline void asc_intlv_b8(vector_bool& dst0,
                                         vector_bool& dst1,
                                         vector_bool src0,
                                         vector_bool src1)
__simd_callee__ inline void asc_intlv_b16(vector_bool& dst0,
                                          vector_bool& dst1,
                                          vector_bool src0,
                                          vector_bool src1)
__simd_callee__ inline void asc_intlv_b32(vector_bool& dst0,
                                          vector_bool& dst1,
                                          vector_bool src0,
                                          vector_bool src1)
```

## 参数说明

### 矢量数据寄存器交织模式

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst0 | 输出 | 目的操作数（矢量数据寄存器），用于保存交织后的低半部分数据。数据类型须与`src0`、`src1`一致。 |
| dst1 | 输出 | 目的操作数（矢量数据寄存器），用于保存交织后的高半部分数据。数据类型须与`src0`、`src1`一致。 |
| src0 | 输入 | 源操作数（矢量数据寄存器）。 |
| src1 | 输入 | 源操作数（矢量数据寄存器）。数据类型须与`src0`一致。 |

### 掩码寄存器交织模式

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst0 | 输出 | 目的操作数（掩码寄存器），用于保存交织后的低半部分数据。 |
| dst1 | 输出 | 目的操作数（掩码寄存器），用于保存交织后的高半部分数据。 |
| src0 | 输入 | 源操作数（掩码寄存器）。 |
| src1 | 输入 | 源操作数（掩码寄存器）。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

### 通用约束

- 本接口仅在AIV上生效，非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用，源操作数和目的操作数为矢量数据寄存器或掩码寄存器。

### 寄存器约束

- `src0`和`src1`可以为同一个矢量数据寄存器或掩码寄存器。
- `dst0`和`dst1`不能为同一个矢量数据寄存器或掩码寄存器。
- 源操作数和目的操作数可以使用同一个矢量数据寄存器或掩码寄存器，例如`asc_intlv(src0, src1, src0, src1)`。

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
constexpr uint32_t HALF_COUNT = ELEMENT_COUNT / 2;
constexpr uint32_t BUFFER_BYTES = ELEMENT_COUNT * sizeof(uint8_t);

__simd_vf__ inline void intlv_vf(__ubuf__ uint8_t* output0, __ubuf__ uint8_t* output1,
                                 __ubuf__ uint8_t* input0, __ubuf__ uint8_t* input1)
{
    vector_uint8_t src0;
    vector_uint8_t src1;
    vector_uint8_t dst0;
    vector_uint8_t dst1;
    vector_bool mask = asc_create_mask_b8(PAT_ALL);
    asc_loadalign(src0, input0);
    asc_loadalign(src1, input1);
    asc_intlv(dst0, dst1, src0, src1);
    asc_storealign(output0, dst0, mask);
    asc_storealign(output1, dst1, mask);
}

__global__ __vector__ void asc_intlv_kernel(__gm__ uint8_t* output0, __gm__ uint8_t* output1,
                                            __gm__ uint8_t* input0, __gm__ uint8_t* input1)
{
    asc_init();
    __ubuf__ uint8_t output0_local[ELEMENT_COUNT];
    __ubuf__ uint8_t output1_local[ELEMENT_COUNT];
    __ubuf__ uint8_t input0_local[ELEMENT_COUNT];
    __ubuf__ uint8_t input1_local[ELEMENT_COUNT];
    asc_copy_gm2ub_align(input0_local, input0, BUFFER_BYTES);
    asc_copy_gm2ub_align(input1_local, input1, BUFFER_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    intlv_vf(output0_local, output1_local, input0_local, input1_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output0, output0_local, BUFFER_BYTES);
    asc_copy_ub2gm_align(output1, output1_local, BUFFER_BYTES);
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint8_t> input0(ELEMENT_COUNT);
    std::vector<uint8_t> input1(ELEMENT_COUNT);
    std::vector<uint8_t> output0(ELEMENT_COUNT, 0);
    std::vector<uint8_t> output1(ELEMENT_COUNT, 0);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        input0[i] = static_cast<uint8_t>(i);
        input1[i] = static_cast<uint8_t>(255 - i);
    }

    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input0_device = nullptr;
    uint8_t* input1_device = nullptr;
    uint8_t* output0_device = nullptr;
    uint8_t* output1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input0_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&input1_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output0_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output1_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input0_device, BUFFER_BYTES, input0.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(input1_device, BUFFER_BYTES, input1.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_intlv_kernel<<<1, 0>>>(output0_device, output1_device, input0_device, input1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output0.data(), BUFFER_BYTES, output0_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(output1.data(), BUFFER_BYTES, output1_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    bool passed = true;
    for (uint32_t i = 0; i < HALF_COUNT; ++i) {
        passed = passed && output0[2 * i] == input0[i] && output0[2 * i + 1] == input1[i];
        passed = passed && output1[2 * i] == input0[i + HALF_COUNT]
                 && output1[2 * i + 1] == input1[i + HALF_COUNT];
    }
    std::cout << (passed ? "[Success] asc_intlv completed." : "[Failed] asc_intlv output mismatch.")
              << std::endl;
    aclrtFree(input0_device);
    aclrtFree(input1_device);
    aclrtFree(output0_device);
    aclrtFree(output1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
