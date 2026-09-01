# asc_copy

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

完成数据在寄存器间的搬运，支持以下三种模式：

- 矢量数据寄存器复制到矢量数据寄存器：将src中被mask筛选的有效元素复制到dst对应位置，未被mask筛选的位置保持dst原值。

- 掩码寄存器复制到掩码寄存器：将src的所有bit位复制到dst中。支持带mask和不带mask两种重载：
    - 带mask：将src中被mask筛选的bit复制到dst。对于b8模式，mask的每个bit控制src的一个bit；对于b16模式，mask的每2bit为一组、仅LSB控制src的一个bit；对于b32模式，mask的每4bit为一组、仅LSB控制src的一个bit。
    - 不带mask：将src的所有bit复制到dst。

- 矢量数据寄存器复制到掩码寄存器：
    - 数据类型为uint16_t时，矢量数据寄存器（256B）分成16个数据块（16B），用户通过参数可选择某一个数据块，数据块的每个bit broadcast成2bit后变为32B，即输出的mask。
    - 数据类型为uint32_t时，矢量数据寄存器（256B）分成32个数据块（8B），用户通过参数可选择某一个数据块，数据块的每个bit broadcast成4bit后变为32B，即输出的mask。
    
    具体搬运原理请参考[关键特性说明](#关键特性说明)。

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

### 矢量数据寄存器复制到矢量数据寄存器

```c
// 占位符形式
__simd_callee__ inline void asc_copy(vector_<dtype>& dst,
                                     vector_<dtype> src,
                                     vector_bool mask)
```

#### dtype支持数据类型
dtype支持的数据类型：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。

#### 函数原型典型示例

```c
// 示例：对half矢量数据寄存器执行复制。
__simd_callee__ inline void asc_copy(vector_half& dst,
                                     vector_half src,
                                     vector_bool mask)
```

### 掩码寄存器复制到掩码寄存器

```c
__simd_callee__ inline void asc_copy(vector_bool& dst,
                                     vector_bool src,
                                     vector_bool mask)
__simd_callee__ inline void asc_copy(vector_bool& dst,
                                     vector_bool src)
```

### 矢量数据寄存器复制到掩码寄存器

**part的值必须在编译期确定，可传入整数字面量或constexpr整数常量，不支持传入运行时变量。**

```c
__simd_callee__ inline void asc_copy(vector_bool& dst,
                                     vector_uint16_t src,
                                     int16_t part)
__simd_callee__ inline void asc_copy(vector_bool& dst,
                                     vector_uint32_t src,
                                     int16_t part)
```

## 参数说明

**表1** 参数说明（矢量数据寄存器复制到矢量数据寄存器）

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的矢量数据寄存器，dtype须与src完全一致。mask比特位为1的位置写入src对应元素（或对应比特），mask比特位为0的位置采用Merging模式写入dst原值。dst需初始化，否则当mask为0时将出现未知值。 |
| src | 输入 | 源矢量数据寄存器，dtype须与dst完全一致。 |
| mask | 输入 | 源掩码寄存器，用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中保持原值。 |

**表2** 参数说明（掩码寄存器复制到掩码寄存器）

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的掩码寄存器。 |
| src | 输入 | 源掩码寄存器。 |
| mask | 输入 | 源掩码寄存器，用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

**表3** 参数说明（矢量数据寄存器复制到掩码寄存器）

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的掩码寄存器，写入src的全部比特位。 |
| src | 输入 | 源矢量数据寄存器。 |
| part | 输入 | part决定了src中需要搬运的数据块，具体描述请参考[关键特性说明](#关键特性说明)。<br>&bull; 当数据类型为uint16_t，part∈[0, 15]。<br>&bull; 当数据类型为uint32_t，part∈[0, 31]。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- 本接口仅在AIV上生效，非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用，dst与src均为矢量数据寄存器或掩码寄存器。
- 针对矢量数据寄存器复制到矢量数据寄存器：dst中未被mask筛选的位置保持原值。
- 针对掩码寄存器复制到掩码寄存器：带mask的接口dst中未被mask筛选的位置填0。
- 针对矢量寄存器复制到掩码寄存器：**part的值必须在编译期确定，可传入整数字面量或constexpr整数常量，不支持传入运行时变量。**
    - 当数据类型为uint16_t时，part∈[0, 15]。
    - 当数据类型为uint32_t时，part∈[0, 31]。

## 关键特性说明

**针对于矢量数据寄存器复制到掩码寄存器：**

由[掩码寄存器](../../defs/type/data_type_definition.md#掩码寄存器)原理可知，当操作数类型为b16时，每一个element对应2bit的掩码寄存器，且仅2bit中的最低位是有效的；当操作数类型为b32时，每一个element对应4bit的掩码寄存器，且仅4bit中的最低位是有效的。

搬运原理如[图1](#fig-maskgen-b16)、[图2](#fig-maskgen-b32)所示。

- 当操作数类型为uint16_t时，矢量数据寄存器的一个数据块大小为16B，每个bit经过broadcast成2bit后变为32B，即一个MaskReg（32B）。此外，一个矢量数据寄存器有256B/16B = 16个数据块，偏移量part取值范围为[0, 15]。
- 当操作数类型为uint32_t时，矢量数据寄存器的一个数据块大小为8B，每个bit经过broadcast成4bit后变为32B，即一个MaskReg（32B）。此外，一个矢量数据寄存器有256B/8B = 32个数据块，偏移量part取值范围为[0, 31]。

**图1**  矢量寄存器复制到掩码寄存器（uint16_t）<a id="fig-maskgen-b16"></a>

![矢量寄存器复制到掩码寄存器（uint16_t）](../../figures/capi_copy_reg2mask_b16.png)

**图2**  矢量寄存器复制到掩码寄存器（uint32_t）<a id="fig-maskgen-b32"></a>

![矢量寄存器复制到掩码寄存器（uint32_t）](../../figures/capi_copy_reg2mask_b32.png)

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
    vector_int8_t arg_0_1;
    asc_loadalign(arg_0_1, reinterpret_cast<__ubuf__ int8_t*>(input));
    asc_copy(arg_0_0, arg_0_1, mask);
    asc_storealign(reinterpret_cast<__ubuf__ int8_t*>(output), arg_0_0, mask);
    }
}
__global__ __vector__ void asc_copy_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
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
    asc_copy_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint8_t), output_device, output.size() * sizeof(uint8_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    const bool passed = true;
    std::cout << "[Success] asc_copy completed." << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
