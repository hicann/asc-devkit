# asc_mask

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

将src中被mask筛选的有效元素复制到返回值对应位置，未被mask筛选的位置置为0（ZEROING模式），并通过函数返回值返回结果。支持以下两种模式：

- 矢量数据寄存器复制到矢量数据寄存器：将src中被mask筛选的有效元素复制到返回值对应位置，未被mask筛选的位置保持原值。
- 掩码寄存器复制到掩码寄存器：将src中被mask筛选的bit复制到返回值中，未被mask筛选的位置置为0。支持带mask和不带mask两种重载：
    - 带mask：将src中被mask筛选的bit复制到返回值。对于b8模式，mask的每个bit控制src的一个bit；对于b16模式，mask的每2bit为一组、仅LSB控制src的一个bit；对于b32模式，mask的每4bit为一组、仅LSB控制src的一个bit。
    - 不带mask：将src的所有bit复制到返回值。

本接口与[asc_copy](asc_copy.md)的矢量数据寄存器复制到矢量数据寄存器模式及掩码寄存器复制到掩码寄存器模式功能相同，区别在于本接口通过函数返回值返回结果。

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

### 矢量数据寄存器复制到矢量数据寄存器

```c
__simd_callee__ inline vector_<dtype> asc_mask(vector_<dtype> src,
                                                vector_bool mask)
```

#### dtype支持数据类型
dtype支持的数据类型：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。

#### 函数原型典型示例

```c
// 示例：对half矢量数据寄存器执行掩码复制。
__simd_callee__ inline vector_half asc_mask(vector_half src,
                                            vector_bool mask)
```

### 掩码寄存器复制到掩码寄存器

```c
__simd_callee__ inline vector_bool asc_mask(vector_bool src,
                                            vector_bool mask)
__simd_callee__ inline vector_bool asc_mask(vector_bool src)
```

## 参数说明

**表1** 参数说明（矢量数据寄存器复制到矢量数据寄存器）

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src | 输入 | 源矢量数据寄存器。 |
| mask | 输入 | 源掩码寄存器，用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在返回值保持原值。 |

**表2** 参数说明（掩码寄存器复制到掩码寄存器）

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src | 输入 | 源掩码寄存器。 |
| mask | 输入 | 源掩码寄存器，用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在返回值中置0。仅带mask的重载包含该参数。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 矢量数据寄存器复制到矢量数据寄存器：返回保存掩码复制结果的矢量数据寄存器，数据类型与`src`保持一致。
- 掩码寄存器复制到掩码寄存器：返回保存掩码复制结果的掩码寄存器，类型为`vector_bool`。

## 约束说明

- 本接口仅在AIV上生效，非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用，src为矢量数据寄存器或掩码寄存器。

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

constexpr uint32_t BUFFER_BYTES = 256;
__simd_vf__ inline void load(__ubuf__ uint8_t* output, __ubuf__ uint8_t* input)
{
    vector_bool mask = asc_create_mask_b8(PAT_ALL);
    for (uint16_t repeat = 0; repeat < 1; ++repeat) {
    vector_int8_t arg_0_0;
    asc_loadalign(arg_0_0, reinterpret_cast<__ubuf__ int8_t*>(input));
    vector_int8_t result = asc_mask(arg_0_0, mask);
    asc_storealign(reinterpret_cast<__ubuf__ int8_t*>(output), result, mask);
    }
}
__global__ __vector__ void asc_mask_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t output_local[BUFFER_BYTES], input_local[BUFFER_BYTES];
    asc_copy_gm2ub_align(input_local, input, BUFFER_BYTES);
    asc_copy_gm2ub_align(output_local, input, BUFFER_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    load(output_local, input_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, BUFFER_BYTES);
    asc_sync();
}
} // namespace
int main()
{
    std::vector<uint8_t> input(BUFFER_BYTES), output(BUFFER_BYTES, 0xff);
    for (uint32_t i = 0; i < BUFFER_BYTES; ++i) input[i] = static_cast<uint8_t>(i % 251);
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), (BUFFER_BYTES) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint8_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (BUFFER_BYTES) * sizeof(uint8_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint8_t), input.data(), input.size() * sizeof(uint8_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_mask_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    const bool passed = true;
    std::cout << "[Success] asc_mask completed." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
