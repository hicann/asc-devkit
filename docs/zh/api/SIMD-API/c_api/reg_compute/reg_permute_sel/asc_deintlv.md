# asc_deintlv

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

头文件路径为：`"c_api/reg_compute/compute/reg_permute_sel.h"`。

将src0和src1中的元素解交织存入dst0和dst1中，本接口提供矢量数据解交织和掩码解交织两种功能模式：

- **矢量数据解交织**：对两个源矢量数据寄存器中的有效元素逐对解交织。src0的偶数索引元素写入dst0前半部分，奇数索引元素写入dst1前半部分。src1的偶数索引元素写入dst0后半部分，奇数索引元素写入dst1后半部分。
- **掩码解交织**：对两个源掩码寄存器中的比特位按指定位宽解交织。src0的偶数索引位写入dst0前半部分，奇数索引位写入dst1前半部分。src1的偶数索引位写入dst0后半部分，奇数索引位写入dst1后半部分。

以int8_t数据类型为例，asc_deintlv的实现流程如图1所示：

**图1** 解交织实现流程

![解交织实现流程](../../figures/capi_deintlv_reg.png)

本接口为Reg矢量计算接口，仅在AIV上生效。

## 函数原型

### 矢量数据解交织

```c
// 占位符形式
__simd_callee__ inline void asc_deintlv(vector_<dtype>& dst0,
                                        vector_<dtype>& dst1,
                                        vector_<dtype> src0,
                                        vector_<dtype> src1)
```

#### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

#### 函数原型典型示例

```c
// 示例：对half矢量数据寄存器执行解交织。
__simd_callee__ inline void asc_deintlv(vector_half& dst0,
                                        vector_half& dst1,
                                        vector_half src0,
                                        vector_half src1)
```

### 掩码解交织

```c
__simd_callee__ inline void asc_deintlv_b8(vector_bool& dst0,
                                            vector_bool& dst1,
                                            vector_bool src0,
                                            vector_bool src1)
__simd_callee__ inline void asc_deintlv_b16(vector_bool& dst0,
                                            vector_bool& dst1,
                                            vector_bool src0,
                                            vector_bool src1)
__simd_callee__ inline void asc_deintlv_b32(vector_bool& dst0,
                                            vector_bool& dst1,
                                            vector_bool src0,
                                            vector_bool src1)
```

## 参数说明

**表1** 参数说明（操作数为矢量数据寄存器）

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst0 | 输出 | 目的矢量数据寄存器，写入解交织后的偶数索引元素集合。dtype须与dst1、src0、src1一致。 |
| dst1 | 输出 | 目的矢量数据寄存器，写入解交织后的奇数索引元素集合。dtype须与dst0、src0、src1一致。 |
| src0 | 输入 | 源矢量数据寄存器，参与解交织的操作数。dtype须与dst0一致。其偶数索引元素写入dst0前半部分，奇数索引元素写入dst1前半部分。 |
| src1 | 输入 | 源矢量数据寄存器，参与解交织的操作数。dtype须与dst0一致。其偶数索引元素写入dst0后半部分，奇数索引元素写入dst1后半部分。 |

**表2** 参数说明（操作数为掩码寄存器）

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst0 | 输出 | 目的掩码寄存器，写入解交织后的偶数索引位集合。 |
| dst1 | 输出 | 目的掩码寄存器，写入解交织后的奇数索引位集合。 |
| src0 | 输入 | 源掩码寄存器，参与解交织的操作数。其偶数索引位写入dst0前半部分，奇数索引位写入dst1前半部分。 |
| src1 | 输入 | 源掩码寄存器，参与解交织的操作数。其偶数索引位写入dst0后半部分，奇数索引位写入dst1后半部分。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- 本接口仅在AIV上生效，非AIV调用直接返回。
- 本接口在Vector Function（`__simd_vf__`标记的函数）内调用。
- src0、src1、dst0、dst1的数据类型需要保持一致。
- src0和src1可以为同一个矢量数据寄存器或掩码寄存器。
- dst0与dst1必须为不同的矢量数据寄存器或掩码寄存器，若两者引用同一寄存器将导致两个输出互相覆盖，结果未定义。
- 允许源操作数和目的操作数为同一个矢量数据寄存器，例如asc_deintlv(src0, src1, src0, src1)。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id10 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510; ./main
```
<!-- end id10 -->

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

template <typename T>
bool compare_data(const std::vector<T>& actual, const std::vector<T>& expected, double tolerance = 0.0)
{
    if (actual.size() != expected.size()) return false;
    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] == expected[i]) continue;
        const double diff = static_cast<double>(actual[i]) - static_cast<double>(expected[i]);
        if (diff > tolerance || diff < -tolerance) return false;
    }
    return true;
}

constexpr uint32_t ELEMENT_COUNT = 64;
__simd_vf__ inline void rearrange(__ubuf__ float* dst0, __ubuf__ float* dst1,
    __ubuf__ float* src0, __ubuf__ float* src1)
{
    vector_float dst0_reg, dst1_reg, src0_reg, src1_reg;
    uint32_t count = ELEMENT_COUNT;
    vector_bool mask = asc_update_mask_b32(count);
    vector_bool mask_dst0, mask_dst1;
    asc_deintlv_b8(mask_dst0, mask_dst1, mask, mask);
    asc_deintlv_b16(mask_dst0, mask_dst1, mask, mask);
    asc_deintlv_b32(mask_dst0, mask_dst1, mask, mask);
    // 解交织前：src0 = [0,1,2,...,63], src1 = [1000,1001,...,1063]
    // 解交织后：dst0 = [0,2,...,62, 1000,1002,...,1062]（偶数索引元素集合）
    //          dst1 = [1,3,...,63, 1001,1003,...,1063]（奇数索引元素集合）
    asc_loadalign(src0_reg, src0);
    asc_loadalign(src1_reg, src1);
    asc_deintlv(dst0_reg, dst1_reg, src0_reg, src1_reg);
    asc_storealign(dst0, dst0_reg, mask);
    asc_storealign(dst1, dst1_reg, mask);
}
__global__ __vector__ void asc_deintlv_kernel(__gm__ float* dst0, __gm__ float* dst1,
    __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float dst0_local[ELEMENT_COUNT], dst1_local[ELEMENT_COUNT];
    __ubuf__ float src0_local[ELEMENT_COUNT], src1_local[ELEMENT_COUNT];
    asc_copy_gm2ub_align(src0_local, src0, ELEMENT_COUNT * sizeof(float));
    asc_copy_gm2ub_align(src1_local, src1, ELEMENT_COUNT * sizeof(float));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    rearrange(dst0_local, dst1_local, src0_local, src1_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst0, dst0_local, ELEMENT_COUNT * sizeof(float));
    asc_copy_ub2gm_align(dst1, dst1_local, ELEMENT_COUNT * sizeof(float));
    asc_sync();
}
} // namespace

int main()
{
    std::vector<float> src0(ELEMENT_COUNT), src1(ELEMENT_COUNT);
    std::vector<float> output0(ELEMENT_COUNT, 0), output1(ELEMENT_COUNT, 0);
    std::vector<float> golden0(ELEMENT_COUNT), golden1(ELEMENT_COUNT);
    // src0 = [0, 1, 2, ..., 63], src1 = [1000, 1001, ..., 1063]
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) { src0[i] = static_cast<float>(i); src1[i] = 1000.0f + i; }
    // golden：解交织后偶数索引元素写入dst0，奇数索引元素写入dst1
    //   golden0 = [0, 2, ..., 62, 1000, 1002, ..., 1062]
    //   golden1 = [1, 3, ..., 63, 1001, 1003, ..., 1063]
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        const uint32_t combined_index = 2 * i;
        golden0[i] = combined_index < ELEMENT_COUNT ? src0[combined_index] : src1[combined_index - ELEMENT_COUNT];
        golden1[i] = combined_index + 1 < ELEMENT_COUNT ? src0[combined_index + 1] : src1[combined_index + 1 - ELEMENT_COUNT];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    float* src0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), (ELEMENT_COUNT) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* src1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), (ELEMENT_COUNT) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* dst0_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst0_device), (ELEMENT_COUNT) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    float* dst1_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst1_device), (ELEMENT_COUNT) * sizeof(float),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src0_device, src0.size() * sizeof(float), src0.data(), src0.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, src1.size() * sizeof(float), src1.data(), src1.size() * sizeof(float),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_deintlv_kernel<<<1, 0>>>(dst0_device, dst1_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output0.data(), output0.size() * sizeof(float), dst0_device, output0.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(output1.data(), output1.size() * sizeof(float), dst1_device, output1.size() * sizeof(float),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input 0", src0);
    print_data("Input 1", src1);
    print_data("Output 0", output0);
    print_data("Golden 0", golden0);
    print_data("Output 1", output1);
    print_data("Golden 1", golden1);
    const bool passed = compare_data(output0, golden0) && compare_data(output1, golden1);
    std::cout << (passed ? "[Success] asc_deintlv passed." : "[Failed] asc_deintlv failed.") << std::endl;
    aclrtFree(dst0_device);
    aclrtFree(dst1_device);
    aclrtFree(src0_device);
    aclrtFree(src1_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
