# asc_reduce_min

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

头文件路径为：`"c_api/reg_compute/compute/reg_reduce.h"`。

根据`mask`对源操作数`src`进行归约最小值操作，得到归约结果。结果保存在`dst`的第0个元素，最小值在`src`中的索引原始位模式保存在`dst`的第1个元素，`dst`中的其他元素置0。如果存在多个最小值，则保留最小的索引。计算公式如下：

$$
\begin{aligned}
dst_0 &= \min\{src_i \mid mask_i = 1\} \\
dst_1 &= \operatorname{argmin}\{src_i \mid mask_i = 1\}
\end{aligned}
$$

## 函数原型

```c
// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dtype> asc_reduce_min(vector_<dtype> src,
                                                      vector_bool mask)

// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_reduce_min(vector_<dtype>& dst,
                                            vector_<dtype> src,
                                            vector_bool mask)
```

### dtype支持数据类型

`dtype`取值为：`int16_t`、`uint16_t`、`half`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```c
// 示例：对half矢量数据寄存器执行归约最小值操作
__simd_callee__ inline void asc_reduce_min(vector_half& dst,
                                           vector_half src,
                                           vector_bool mask)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述                                     |
| ------ | --------- | ---------------------------------------- |
| dst  | 输出      | 目的操作数（矢量数据寄存器）。           |
| src  | 输入      | 源操作数（矢量数据寄存器）。             |
| mask | 输入      | 掩码寄存器，用于控制各元素是否参与归约。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 通过引用参数输出结果：归约结果写入`dst`。
- 通过函数返回值返回结果：返回归约结果，类型为矢量数据寄存器，与`src`的数据类型一致。

## 约束说明

- 通过引用参数输出结果的函数原型在非AIV上调用时直接返回。
- 通过函数返回值输出结果的函数原型在非AIV上调用时返回对应矢量类型的默认构造值。
- `mask`需通过掩码设置接口预先赋值后再传入，未赋值的掩码寄存器内容不确定，会导致有效元素位置错误。
- 未被`mask`选中的元素被视为对应数据类型的最大值，浮点数类型的最大值为`+inf`。如果`src`中的所有元素均未被`mask`选中，则将该最大值写入`dst`的第0个元素，并将其余元素置0。
- 比较时遵循$min(-0, +0) = -0$。
- 如果输入数据中存在nan，则将nan写入`dst`的第0个元素，并将第一个nan的索引写入`dst`的第1个元素。

## 关键特性

**规约产生值+索引两个结果，索引值需要强制类型转换**：

`dst`的索引按照`dst`的数据类型存储，比如`dst`为half类型时，索引按照half类型存储，因此读取索引需要使用  reinterpret\_cast方法转换到整数类型。若数据类型是half，需要使用reinterpret\_cast\<uint16_t\*\>；若数据类型是float，需要使用reinterpret\_cast\<uint32\_t\*\>。值+索引的分别提取方式请参见[调用示例](#调用示例)。

## 调用示例

<!-- npu="950" id8 -->
将以下代码保存为`example.asc`后，可通过`bisheng`命令编译运行。其中，`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[__NPU_ARCH__](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510 && ./main
```

```cpp
// 本例使用half类型演示归约结果"值+索引"的分别提取，
// 归约结果中dst的第0个元素为最小值，第1个元素为最小值索引的原始位模式。
#include <cstdint>
#include <iostream>
#include <vector>
#include "c_api/asc_simd.h"
#include "acl/acl.h"
namespace {
constexpr uint32_t ELEMENT_COUNT = 64;
__simd_vf__ inline void reduce(__ubuf__ half* dst, __ubuf__ half* src)
{
    vector_half dst_reg, src_reg;
    uint32_t count = ELEMENT_COUNT;
    vector_bool mask = asc_update_mask_b16(count);
    asc_loadalign(src_reg, src);
    asc_reduce_min(dst_reg, src_reg, mask);
    asc_storealign(dst, dst_reg, mask);
}
__global__ __vector__ void asc_reduce_min_kernel(__gm__ half* dst, __gm__ half* src)
{
    asc_init();
    __ubuf__ half dst_local[ELEMENT_COUNT], src_local[ELEMENT_COUNT];
    asc_copy_gm2ub_align(src_local, src, ELEMENT_COUNT * sizeof(half));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    reduce(dst_local, src_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst, dst_local, ELEMENT_COUNT * sizeof(half));
    asc_sync();
}
} // namespace
int main()
{
    std::vector<half> src(ELEMENT_COUNT), output(ELEMENT_COUNT, half(0));
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        src[i] = half(static_cast<float>(i) + 1.0f);  // 1, 2, ..., 64
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    half* src_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src_device), (ELEMENT_COUNT) * sizeof(half),
        ACL_MEM_MALLOC_HUGE_FIRST);
    half* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), (ELEMENT_COUNT) * sizeof(half),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(src_device, src.size() * sizeof(half), src.data(), src.size() * sizeof(half),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_reduce_min_kernel<<<1, 0>>>(dst_device, src_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(half), dst_device, output.size() * sizeof(half),
        ACL_MEMCPY_DEVICE_TO_HOST);
    half min_value = output[0];                                      // 提取最小值
    uint16_t min_index = *reinterpret_cast<uint16_t*>(&output[1]);   // 提取最小值索引：索引按half类型存储，需强制转换到整数类型
    const bool passed = (static_cast<float>(min_value) == 1.0f) && (min_index == 0);
    std::cout << (passed ? "[Success] asc_reduce_min passed." : "[Failed] asc_reduce_min failed.") << std::endl;
    aclrtFree(dst_device);
    aclrtFree(src_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```

<!-- end id8 -->
