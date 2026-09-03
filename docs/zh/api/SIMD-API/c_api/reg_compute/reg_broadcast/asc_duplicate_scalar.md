# asc_duplicate_scalar

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

头文件路径为：`"c_api/reg_compute/compute/reg_broadcast.h"`。

将标量值广播到目的操作数中。支持带输出参数和带返回值两种接口形式，每种形式均支持以下两类重载：

- 带mask：将标量值写入dst中被mask筛选的位置，未被mask筛选的位置置零。
- 不带mask：将标量值写入dst的全部元素位置。

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

### 带mask

```c
// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dtype> asc_duplicate_scalar(<dtype> value,
                                                           vector_bool mask)
// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_duplicate_scalar(vector_<dtype>& dst,
                                                 <dtype> value,
                                                 vector_bool mask)
```

#### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

#### 函数原型典型示例

```c
// 通过函数返回值返回结果
__simd_callee__ inline vector_int8_t asc_duplicate_scalar(int8_t value,
                                                          vector_bool mask)
// 通过引用参数输出结果
__simd_callee__ inline void asc_duplicate_scalar(vector_int8_t& dst,
                                                 int8_t value,
                                                 vector_bool mask)
```

### 不带mask（占位符形式）

```c
// 通过函数返回值返回结果
__simd_callee__ inline vector_<dtype> asc_duplicate_scalar(<dtype> value)

// 通过引用参数输出结果
__simd_callee__ inline void asc_duplicate_scalar(vector_<dtype>& dst,
                                                 <dtype> value)
```

#### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。


#### 函数原型典型示例

```c
// 通过函数返回值返回结果
__simd_callee__ inline vector_int8_t asc_duplicate_scalar(int8_t value)

// 通过引用参数输出结果
__simd_callee__ inline void asc_duplicate_scalar(vector_int8_t& dst,
                                                 int8_t value)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数（矢量数据寄存器），保存广播结果。仅带输出参数的接口包含该参数，dst的元素数据类型用于确定函数重载。 |
| value | 输入 | 源操作数（标量），作为待广播的数据。对于带返回值接口，value的数据类型用于确定函数重载和返回类型，建议传入类型明确的变量或使用显式类型转换。 |
| mask | 输入 | 源操作数元素操作的有效指示（掩码寄存器），仅带mask的重载包含该参数。mask筛选的元素在dst中填充为value，未筛选的元素在dst中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 带返回值的接口返回广播结果，返回类型为与`dtype`对应的`vector_<dtype>`。

## 约束说明

- 本接口为Reg矢量计算接口，只能在使用`__simd_vf__`标记的VF函数内调用，不支持在`__aicore__`函数中直接调用，仅在AIV上生效，在AIC上调用将直接返回。
- 同一寄存器的数据依赖由硬件保序，无需额外插入同步指令。本接口与前后Reg数据搬运接口之间，如果不同寄存器访问同一UB地址且存在写后读或写后写依赖，需要调用[asc_mem_bar](../reg_sync/asc_mem_bar.md)进行同步。
- 使用mask前，需要通过掩码设置或搬入接口完成初始化；未初始化的掩码寄存器内容不确定。
- 调用带返回值接口时，应使用类型明确的变量或显式类型转换，例如`static_cast<uint8_t>(1)`，以匹配正确的函数原型。

<!-- npu="950" id8 -->
## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

以下调用示例代码仅Ascend 950PR/Ascend 950DT产品支持。

```cpp
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"

namespace {
template <typename T>
void print_data(const char* label, const std::vector<T>& values, uint32_t offset = 0)
{
    std::cout << label << ":";
    const size_t remaining = values.size() - offset;
    const size_t count = remaining < 8 ? remaining : 8;
    for (size_t i = 0; i < count; ++i) std::cout << ' ' << +values[offset + i];
    if (remaining > count) std::cout << " ...";
    std::cout << std::endl;
}

constexpr uint32_t ELEMENT_COUNT = 64;
constexpr float FILL_VALUE = 3.5f;

__simd_vf__ inline void duplicate_scalar(__ubuf__ float* dst)
{
    uint32_t count = ELEMENT_COUNT;
    vector_bool mask = asc_update_mask_b32(count);
    vector_float dst_reg = asc_duplicate_scalar(FILL_VALUE, mask);
    asc_storealign(dst, dst_reg, mask);
}

__global__ __vector__ void asc_duplicate_scalar_kernel(__gm__ float* dst)
{
    asc_init();
    __ubuf__ float dst_local[ELEMENT_COUNT];
    duplicate_scalar(dst_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst, dst_local, sizeof(dst_local));
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> output(ELEMENT_COUNT, -1.0f);
    std::vector<float> golden(ELEMENT_COUNT, FILL_VALUE);

    aclInit(nullptr);
    aclrtSetDevice(0);
    float *dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), output.size() * sizeof(float), ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(dst_device, output.size() * sizeof(float), output.data(), output.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_duplicate_scalar_kernel<<<1, 0>>>(dst_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(float), dst_device, output.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);

    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_duplicate_scalar passed."
                        : "[Failed] asc_duplicate_scalar failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
<!-- end id8 -->
