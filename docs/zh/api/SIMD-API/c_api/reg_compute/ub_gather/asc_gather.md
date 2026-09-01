# asc_gather

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

根据索引位置`index`将源操作数`src`按元素收集到目的操作数`dst`中。

**UB源收集模式**：源操作数为UB地址，目的操作数为矢量数据寄存器。按`index`矢量寄存器中保存的逐元素索引，从UB中读取对应位置的元素写入`dst`对应位置，受`mask`掩码控制；`mask`比特位为0的位置对应`dst`位置写0。

**图 1**  UB源收集模式

![UB源收集模式](../../figures/asc_gather_ub.png)

## 函数原型

```c
// 通过引用参数输出结果（占位符形式）
__simd_callee__ inline void asc_gather(vector_<dst_dtype>& dst,
                                       __ubuf__ <src_dtype>* src,
                                       vector_<index_dtype> index,
                                       vector_bool mask)

// 通过函数返回值返回结果（占位符形式）
__simd_callee__ inline vector_<dst_dtype> asc_gather(__ubuf__ <src_dtype>* src,
                                                     vector_<index_dtype> index,
                                                     vector_bool mask)
```

### 数据类型组合列表
**表1** UB源收集模式支持数据类型组合列表

| dst_dtype | src_dtype | index_dtype |
| :---------- | :---------- | :-------------- |
| int16_t | int8_t | uint16_t |
| int8_t | int8_t | uint16_t |
| uint16_t | uint8_t | uint16_t |
| uint8_t | uint8_t | uint16_t |
| hifloat8_t | hifloat8_t | uint16_t |
| fp8_e8m0_t | fp8_e8m0_t | uint16_t |
| fp8_e5m2_t | fp8_e5m2_t | uint16_t |
| fp8_e4m3fn_t | fp8_e4m3fn_t | uint16_t |
| int16_t | int16_t | uint16_t |
| int16_t | int16_t | uint32_t |
| uint16_t | uint16_t | uint16_t |
| uint16_t | uint16_t | uint32_t |
| half | half | uint16_t |
| half | half | uint32_t |
| bfloat16_t | bfloat16_t | uint16_t |
| bfloat16_t | bfloat16_t | uint32_t |
| int32_t | int32_t | uint32_t |
| uint32_t | uint32_t | uint32_t |
| float | float | uint32_t |

> 说明：`int16_t/int8_t`、`uint16_t/uint8_t` 两组位宽扩展组合仅支持“通过引用参数输出结果”形式，“通过函数返回值返回结果”形式不支持。

### 函数原型典型示例

```c
// 示例：dst_dtype/src_dtype/index_dtype数据类型分别为: int16_t/int16_t/uint16_t
// 通过引用参数输出结果
__simd_callee__ inline void asc_gather(vector_int16_t& dst,
                                       __ubuf__ int16_t* src,
                                       vector_uint16_t index,
                                       vector_bool mask)

// 通过函数返回值返回结果
__simd_callee__ inline vector_int16_t asc_gather(__ubuf__ int16_t* src,
                                                 vector_uint16_t index,
                                                 vector_bool mask)
```

## 参数说明

**表2** 参数说明

| 参数名  | 输入/输出 | 描述 |
| :----- | :------- | :------- |
| dst | 输出 | 目的操作数（矢量数据寄存器）。 |
| src | 输入 | 源操作数（矢量）的起始地址。在UB中的起始地址需要32B对齐，须落在UB地址空间内。 |
| index | 输入 | 数据索引（矢量数据寄存器）。dst中每个元素在UB中相对于src的索引位置，单位是元素个数。 |
| mask | 输入 | 源操作数掩码（掩码寄存器）。mask用于指示在计算过程中哪些元素参与计算。对应位置为1时参与计算，为0时不参与计算。mask未筛选的元素在输出中置零。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../../defs/type/data_type_definition.md)。

## 返回值说明

- 通过引用参数输出结果的函数原型无返回值。
- 通过函数返回值输出结果的函数原型返回收集结果，返回值类型与对应引用输出函数原型中`dst`参数的类型一致（去除引用）。

## 约束说明

### 通用约束

- 通过引用参数输出结果的函数原型在非AIV上调用时直接返回。
- 通过函数返回值输出结果的函数原型在非AIV上调用时返回对应矢量类型的默认构造值。
- 本接口在Vector Function（`__simd_vf__` 标记的函数）内调用，源操作数为UB地址、目的操作数为矢量数据寄存器。

### UB源收集模式约束

- UB地址空间外的指针不可作为`src`传入，源操作数在UB中的起始地址需要32B对齐。
- `mask`需通过掩码设置接口预先赋值后再传入，未赋值的掩码寄存器内容不确定，会导致有效元素位置错误。
- 对于`mask`筛选需要搬运的元素，对应的地址需要在UB有效范围内；对于`mask`未筛选的元素，对应的地址不会触发任何地址越界异常，同时`dst`中对应的元素将被置零。
- 当`src`为b8数据类型，`dst`为b16数据类型时，实现是编译器的软件仿真实现，通过函数返回值输出结果的函数原型不做支持。目的操作数的低8位与源操作数相同，高8位自动补0。例如`src`为`int8_t`数据类型，`dst`为`int16_t`数据类型：

    src：40 = 0b00101000 -> 0b0000000000101000，扩充至16位后等于40，即对应dst为40；

    src：-40 = 0b11011000 -> 0b0000000011011000，扩充至16位后等于216，即对应dst为216。

- 当`src`与`dst`数据类型一致，但是与`index`数据类型不一致时，数据写入`dst`索引为偶数的位置，奇数索引位置置零。例如`src`为`int8_t`数据类型，`index`为`uint16_t`数据类型时，适用场景如下图：

![](../../figures/asc_gather_different_type.png)

## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md)。

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

__simd_vf__ inline void gather_vf(__ubuf__ uint16_t* output, __ubuf__ uint16_t* input)
{
    vector_bool mask = asc_create_mask_b16(PAT_ALL);
    vector_uint16_t index;
    asc_loadalign(index, input);
    vector_uint16_t dst;
    asc_gather(dst, input, index, mask);
    asc_storealign(output, dst, mask);
}

__global__ __vector__ void asc_gather_kernel(__gm__ uint16_t* output, __gm__ uint16_t* input)
{
    asc_init();
    __ubuf__ uint16_t output_local[ELEMENT_COUNT];
    __ubuf__ uint16_t input_local[ELEMENT_COUNT];
    asc_copy_gm2ub_align(input_local, input, ELEMENT_COUNT * sizeof(uint16_t));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
    gather_vf(output_local, input_local);
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(output, output_local, ELEMENT_COUNT * sizeof(uint16_t));
    asc_sync();
}
} // namespace

int main()
{
    std::vector<uint16_t> input(ELEMENT_COUNT);
    std::vector<uint16_t> output(ELEMENT_COUNT, 0xff);
    std::vector<uint16_t> golden(ELEMENT_COUNT);
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        input[i] = static_cast<uint16_t>(ELEMENT_COUNT - 1 - i);
    }
    for (uint32_t i = 0; i < ELEMENT_COUNT; ++i) {
        golden[i] = input[input[i]];
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint16_t* input_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), (ELEMENT_COUNT) * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    uint16_t* output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&output_device), (ELEMENT_COUNT) * sizeof(uint16_t),
        ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, input.size() * sizeof(uint16_t), input.data(), input.size() * sizeof(uint16_t),
        ACL_MEMCPY_HOST_TO_DEVICE);
    asc_gather_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), output.size() * sizeof(uint16_t), output_device, output.size() * sizeof(uint16_t),
        ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input bytes", input);
    print_data("Output bytes", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_gather completed." : "[Failed] asc_gather output mismatch.")
              << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
