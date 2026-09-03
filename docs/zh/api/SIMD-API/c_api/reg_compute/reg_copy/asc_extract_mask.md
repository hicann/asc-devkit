# asc_extract_mask

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

头文件路径为：`"c_api/reg_compute/reg_copy.h"`。

从矢量数据寄存器中提取掩码寄存器，并通过函数返回值返回结果。

- 数据类型为uint16_t时，矢量数据寄存器（256B）分成16个数据块（16B），用户通过参数可选择某一个数据块，数据块的每个bit 广播成2bit后变为32B，即输出的mask。
- 数据类型为uint32_t时，矢量数据寄存器（256B）分成32个数据块（8B），用户通过参数可选择某一个数据块，数据块的每个bit 广播成4bit后变为32B，即输出的mask。

具体搬运原理请参考[asc_copy关键特性说明](asc_copy.md#关键特性说明)。

本接口与[asc_copy](asc_copy.md)的矢量数据寄存器复制到掩码寄存器模式功能相同，区别在于本接口通过函数返回值返回结果。

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

**part的值必须在编译期确定，可传入整数字面量或constexpr整数常量，不支持传入运行时变量。**

```c
__simd_callee__ inline vector_bool asc_extract_mask(vector_uint16_t src,
                                                     int16_t part)
__simd_callee__ inline vector_bool asc_extract_mask(vector_uint32_t src,
                                                     int16_t part)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src | 输入 | 源矢量数据寄存器。 |
| part | 输入 | part决定了src中需要搬运的数据块，具体描述请参考[asc_copy关键特性说明](asc_copy.md#关键特性说明)。<br>&bull; 当数据类型为uint16_t，part∈[0, 15]。<br>&bull; 当数据类型为uint32_t，part∈[0, 31]。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

返回保存提取结果的掩码寄存器，类型为`vector_bool`。

## 约束说明

- 本接口仅在AIV上生效，非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用，src为矢量数据寄存器。
- **part的值必须在编译期确定，可传入整数字面量或constexpr整数常量，不支持传入运行时变量。**
    - 当数据类型为uint16_t时，part∈[0, 15]。
    - 当数据类型为uint32_t时，part∈[0, 31]。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510&& ./main
```
<!-- end id8 -->

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"
namespace {
template <typename T>
void print_data(const char* label, const std::vector<T>& values)
{
    std::cout << label << ":";
    const size_t count = values.size() < 8 ? values.size() : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[i];
    if (values.size() > count) std::cout << " ...";
    std::cout << std::endl;
}

constexpr uint32_t ELEMENT_COUNT = 128;
__simd_vf__ inline void extract_mask(__ubuf__ uint32_t* output, __ubuf__ uint16_t* input)
{
    vector_uint16_t src_reg;
    asc_loadalign(src_reg, input);
    vector_bool mask = asc_extract_mask(src_reg, 0);
    asc_storealign(output, mask);
}
__global__ __vector__ void asc_extract_mask_kernel(__gm__ uint32_t* output, __gm__ uint16_t* input)
{
    asc_init();
    __ubuf__ uint32_t output_local[ELEMENT_COUNT];
    __ubuf__ uint16_t input_local[ELEMENT_COUNT];
    asc_copy_gm2ub_align(input_local, input, ELEMENT_COUNT * sizeof(uint16_t));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    extract_mask(output_local, input_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, ELEMENT_COUNT * sizeof(uint32_t));
    asc_sync();
}
} // namespace
int main()
{
    std::vector<uint16_t> input(ELEMENT_COUNT);
    std::vector<uint32_t> output(ELEMENT_COUNT, 0);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) input[i] = static_cast<uint16_t>(i);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), (ELEMENT_COUNT) * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint32_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (ELEMENT_COUNT) * sizeof(uint32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint16_t), input.data(), input.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_extract_mask_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint32_t), output_device, output.size() * sizeof(uint32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    const bool passed = true;
    std::cout << "[Success] asc_extract_mask completed." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
