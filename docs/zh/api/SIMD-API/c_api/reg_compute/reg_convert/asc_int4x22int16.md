# asc_int4x22int16

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

`int4x2_t`将两个连续的4bit有符号整数打包为一个8bit存储单元。本接口根据`mask`将`src`中参与计算的4bit有符号整数转换为`int16_t`类型，结果写入`dst`。

由于每个4bit有符号整数的位宽小于1Byte，实际处理时，同一`int4x2_t`存储单元内的两个4bit有符号整数共同对应`mask`中的一个掩码位。该掩码位为1时，两个4bit有符号整数同时参与转换，转换结果作为两个连续的`int16_t`类型元素写入`dst`；为0时，对应的两个`int16_t`类型元素均置0。

由于单个源数据与目的数据的类型位宽比为1:4，`src`中的`int4x2_t`存储单元按索引分为四组。参数`src_quarter_pos`从`src`中离散选取每隔4个存储单元的位置读取数据，具体为索引0、4、8、…，索引1、5、9、…，索引2、6、10、…或索引3、7、11、…。

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

```cpp
// 占位符形式
__simd_callee__ inline void asc_int4x22int16(
    vector_int16_t& dst,
    vector_int4x2_t src,
    vector_bool mask,
    std::integral_constant<asc_position_quarter_mode, <quarter_pos>> src_quarter_pos)
```

`<quarter_pos>`支持`ASC_DISPERSE_FIRST_QUARTER`、`ASC_DISPERSE_SECOND_QUARTER`、`ASC_DISPERSE_THIRD_QUARTER`和`ASC_DISPERSE_FOURTH_QUARTER`。

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| ------ | --------- | ---- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量数据寄存器）。 |
| mask | 输入 | 掩码寄存器，用于控制各元素是否参与计算。`mask`中与元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |
| src_quarter_pos | 输入 | 四分之一位置选择标签（编译期标签分发，通过编译期重载选择对应实现），用于选择源操作数的读取位置。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无。

## 约束说明

- Reg矢量计算C API通用约束请参见[通用约束](../overview.md#通用约束)。
- 位置选择标签参数仅能使用上述编译期常量。
- `mask`掩码位为0时，`dst`对应元素置0。

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
constexpr uint32_t BUFFER_BYTES = 256;

__simd_vf__ inline void convert(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input)
{
    vector_bool mask = asc_create_mask_b8(PAT_ALL);
    vector_int16_t dst;
    vector_int4x2_t src;
    asc_loadalign(dst, reinterpret_cast<__ubuf__ int16_t*>(output));
    asc_loadalign(src, reinterpret_cast<__ubuf__ int4b_t*>(input));
    asc_int4x22int16(dst, src, mask, ASC_DISPERSE_FIRST_QUARTER);
    asc_storealign(reinterpret_cast<__ubuf__ int16_t*>(output), dst, mask);
}
} // namespace

__global__ __vector__ void asc_int4x22int16_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[BUFFER_BYTES];
    __ubuf__ uint8_t input_local[BUFFER_BYTES];
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_copy_gm2ub_align(output_local, output, BUFFER_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    convert(output_local, input_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_sync();
}

int main()
{
    constexpr uint8_t input_pattern[] = {0x11};
    constexpr uint8_t golden_pattern[] = {0x01, 0x00};
    std::vector<uint8_t> input(BUFFER_BYTES);
    std::vector<uint8_t> output(BUFFER_BYTES, 0);
    std::vector<uint8_t> golden(BUFFER_BYTES);
    for (size_t i = 0; i < BUFFER_BYTES; ++i) {
        input[i] = input_pattern[i % sizeof(input_pattern)];
        golden[i] = golden_pattern[i % sizeof(golden_pattern)];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), BUFFER_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, BUFFER_BYTES, input.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, BUFFER_BYTES, output.data(), BUFFER_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_int4x22int16_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), BUFFER_BYTES, output_device, BUFFER_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    bool passed = true;
    for (size_t i = 0; i < BUFFER_BYTES; ++i) passed = passed && (output[i] == golden[i]);
    std::cout << (passed ? "[Success] asc_int4x22int16 passed."
                         : "[Failed] asc_int4x22int16 failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
