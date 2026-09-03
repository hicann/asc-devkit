# asc_arange

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

头文件路径为：`"c_api/reg_compute/compute/reg_index.h"`。

以传入的`value`为起始值，生成递增/递减的索引，并将生成的索引保存在`dst`中，[Vector Length (VL)](../../defs/type/data_type_definition.md)表示矢量数据寄存器的位宽，`VL_T`表示该寄存器可存储的元素数量。算法逻辑表示如下：

```cpp
// 递增
{value, value + 1, value + 2, ... value + VL_T - 2, value + VL_T - 1}
// 递减
{value + VL_T - 1, value + VL_T - 2, value + VL_T - 3, ... value + 1, value}
```

以int16_t数据类型，起始值`value=10`为例：
递增索引为{10, 11, 12, 13, ... 135, 136, 137}，递减索引为{137, 136, 135, 134, ... 12, 11, 10}。

本接口仅在AIV上生效。

## 函数原型

### 递增模式

```c
// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_arange(vector_<dtype>& dst,
                                       <dtype> value)

// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dtype> asc_arange(<dtype> value)
```

#### dtype支持数据类型

`dtype`取值为：`int8_t`、`int16_t`、`half`、`int32_t`、`float`。

#### 函数原型典型示例

```c
// 示例：以half标量为基值生成递增序列
// 通过引用参数输出结果
__simd_callee__ inline void asc_arange(vector_half& dst,
                                       half value)

// 通过函数返回值返回结果
__simd_callee__ inline vector_half asc_arange(half value)
```

### 递减模式

```c
// 通过引用参数输出结果
__simd_callee__ inline void asc_arange_descend(vector_<dtype>& dst,
                                               <dtype> value)

// 通过函数返回值返回结果
__simd_callee__ inline vector_<dtype> asc_arange_descend(<dtype> value)
```

#### dtype支持数据类型

`dtype`取值为：`int8_t`、`int16_t`、`half`、`int32_t`、`float`。

#### 函数原型典型示例

```c
// 示例：以half标量为基值生成递减序列
// 通过引用参数输出结果
__simd_callee__ inline void asc_arange_descend(vector_half& dst,
                                               half value)

// 通过函数返回值返回结果
__simd_callee__ inline vector_half asc_arange_descend(half value)
```

## 参数说明

### 递增模式

**表1** 参数说明

| 参数名       | 输入/输出 | 描述                |
| --------- | ----- | ----------------- |
| dst       | 输出    | 目的操作数（矢量数据寄存器）。 |
| value     | 输入    | 源操作数（标量），dtype须与dst一致。作为递增序列的起点，序列第0个元素等于value，后续元素按1递增。取值范围为该dtype的可表示范围。 |

### 递减模式

**表2** 参数说明

| 参数名       | 输入/输出 | 描述                |
| --------- | ----- | ----------------- |
| dst       | 输出    | 目的操作数（矢量数据寄存器）。 |
| value     | 输入    | 源操作数（标量），dtype须与dst一致。作为递减序列的起点，序列第0个元素等于`value + VL_T - 1`，后续元素按1递减。取值范围为该dtype的可表示范围。 |

矢量数据寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 通过引用参数输出结果的函数原型无返回值。
- 通过函数返回值输出结果的函数原型返回计算结果，返回值类型与对应引用输出函数原型中`dst`参数的类型一致（去除引用）。

## 约束说明

- 通过引用参数输出结果的函数原型在非AIV上调用时直接返回。
- 通过函数返回值输出结果的函数原型在非AIV上调用时返回对应矢量类型的默认构造值。
- 整型dtype（int8_t、int16_t、int32_t）结果在超出该dtype可表示范围时回绕（wrap-around），不触发异常。例如int8_t取value=127时，序列前128个元素依次为127、−128、−127、…、-2；value=−128时，序列前128个元素依次为−128、−127、…、−1。

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译运行命令如下：

```bash
bisheng example.asc -o main --npu-arch=dav-3510; ./main
```
<!-- end id8 -->

```c
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

constexpr uint32_t ELEMENT_COUNT = 64;
constexpr int32_t START_VALUE = 10;

__simd_vf__ inline void arange_vf(__ubuf__ int32_t* ascending, __ubuf__ int32_t* descending)
{
    vector_int32_t ascending_reg;
    vector_int32_t descending_reg;
    uint32_t count = ELEMENT_COUNT;
    vector_bool mask = asc_update_mask_b32(count);
    asc_arange(ascending_reg, START_VALUE);
    asc_arange_descend(descending_reg, START_VALUE);
    asc_storealign(ascending, ascending_reg, mask);
    asc_storealign(descending, descending_reg, mask);
}

__global__ __vector__ void asc_arange_kernel(__gm__ int32_t* ascending, __gm__ int32_t* descending)
{
    asc_init();
    __ubuf__ int32_t ascending_local[ELEMENT_COUNT];
    __ubuf__ int32_t descending_local[ELEMENT_COUNT];
    arange_vf(ascending_local, descending_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(ascending, ascending_local, ELEMENT_COUNT * sizeof(int32_t));
    asc_copy_ub2gm_align(descending, descending_local, ELEMENT_COUNT * sizeof(int32_t));
    asc_sync();
}
} // namespace

int main()
{
    std::vector<int32_t> input = {START_VALUE};
    std::vector<int32_t> ascending(ELEMENT_COUNT, 0), descending(ELEMENT_COUNT, 0);
    aclInit(nullptr);
    aclrtSetDevice(0);
    int32_t* ascending_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&ascending_device), (ELEMENT_COUNT) * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    int32_t* descending_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&descending_device), (ELEMENT_COUNT) * sizeof(int32_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    asc_arange_kernel<<<1, 0>>>(ascending_device, descending_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(ascending.data(), ascending.size() * sizeof(int32_t), ascending_device, ascending.size() * sizeof(int32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    aclrtMemcpy(descending.data(), descending.size() * sizeof(int32_t), descending_device, descending.size() * sizeof(int32_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input start", input);
    print_data("Ascending output", ascending);
    print_data("Descending output", descending);
    aclrtFree(ascending_device);
    aclrtFree(descending_device);
    aclrtResetDevice(0);
    aclFinalize();
    return 0;
}
```
