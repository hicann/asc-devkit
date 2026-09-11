# asc_copy_ub2gm

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

头文件路径为：`"c_api/vector_datamove/vector_datamove.h"`。

将数据从Unified Buffer（UB）搬运到Global Memory（GM）。

本接口支持连续数据搬运和高维切分数据搬运。连续数据搬运按`size`指定的字节数从源UB连续搬运到目的GM，如图1所示；高维切分数据搬运按`burst_count`指定搬运的数据块个数，每个数据块按`burst_len`指定的字节数搬运，并通过`src_stride`和`dst_stride`指定源操作数和目的操作数相邻数据块首地址间隔，如图2所示。

**图1**  连续搬运示意图
![](../../figures/asc_copy_ub2gm_continuous.png "连续搬运示意图")

**图2**  高维切分搬运示意图
![](../../figures/asc_copy_ub2gm_highdim_split.png "高维切分搬运示意图")

本接口仅在AIV上生效。

## 函数原型

### 连续数据搬运

```c
__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst,
                                      __ubuf__ void* src,
                                      uint32_t size)
```

### 高维切分数据搬运

```c
__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst,
                                      __ubuf__ void* src,
                                      uint16_t burst_count,
                                      uint16_t burst_len,
                                      uint16_t dst_stride,
                                      uint16_t src_stride)
```

## 参数说明

### 连续数据搬运

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的GM的起始地址。需要1字节对齐。 |
| src | 输入 | 源UB的起始地址。需要32字节对齐。 |
| size | 输入 | 搬运数据大小，单位为字节。取值范围：[1, $2^{21}−1$]。 |

### 高维切分数据搬运

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的GM的起始地址。需要1字节对齐。 |
| src | 输入 | 源UB的起始地址。需要32字节对齐。 |
| burst_count | 输入 | 待搬运的连续传输数据块个数。取值范围：[1, $2^{16}−1$]。 |
| burst_len | 输入 | 待搬运的每个连续传输数据块的长度，单位为字节。取值范围：[1, $2^{16}−1$]。 |
| dst_stride | 输入 | 目的操作数相邻连续数据块的距离（前面一个数据块的头与后面一个数据块的头的间隔），单位为字节。取值范围：[0, $2^{16}−1$]。<br>只搬运1个数据块，即`burst_count`设置为1时，可以将此参数设置为0。 |
| src_stride | 输入 | 源操作数相邻连续数据块的距离（前面一个数据块的头与后面一个数据块的头的间隔），单位为字节。取值范围：[0, $2^{16}−1$]。<br>只搬运1个数据块，即`burst_count`设置为1时，可以将此参数设置为0。 |

## 返回值说明

无

## 流水类型

PIPE_MTE3

## 约束说明

### 通用约束

- 本接口在非AIV上调用直接返回。
- 各存储单元的空间大小和对齐要求请参考[存储单元说明](../../general_description_and_constraints.md#存储单元说明)。
- 如果本指令与其他指令存在UB地址重叠，必须插入同步指令[asc_sync_notify](../../sync/asc_sync_notify.md)和[asc_sync_wait](../../sync/asc_sync_wait.md)，保证多个指令串行化，防止出现异常数据。

### 连续数据搬运约束

- `size`需满足dtype字节对齐：dtype为b16时需为2的倍数，dtype为b32时需为4的倍数。

### 高维切分数据搬运约束

- `len_burst`需满足dtype字节对齐：dtype为b16时需为2的倍数，dtype为b32时需为4的倍数。
- 当`src_stride`不等于`burst_len`时，`src_stride`要求32字节对齐。

## 关键特性说明

### Broadcast场景

使用高维切分搬运接口时，不建议通过将`src_stride`设置为0实现Broadcast。可通过loop功能实现Broadcast，将`burst_count`设置为1，并通过[asc_set_ub2gm_loop_size](../asc_set_ub2gm_loop_size.md)、[asc_set_ub2gm_loop1_stride](../asc_set_ub2gm_loop1_stride.md)和[asc_set_ub2gm_loop2_stride](../asc_set_ub2gm_loop2_stride.md)配置循环搬运：

- 将`loop1_size`设置为Broadcast次数，将`loop2_size`设置为1。
- 将`loop1_src_stride`设置为0，使每次内层循环读取同一个UB源数据块。
- 将`loop1_dst_stride`设置为单个数据块的字节数，使每次内层循环写入GM的下一个连续位置。
- 将`loop2_src_stride`和`loop2_dst_stride`设置为0。外层循环只执行1次，这两个参数不产生地址偏移。
- 将`asc_copy_ub2gm`的`burst_count`设置为1，并将`src_stride`设置为非0值。循环搬运结束后，调用`asc_set_ub2gm_loop_size`将循环次数复位为1。

以下示例将UB中的256字节源数据块重复搬运2048次，在GM中生成512KB的连续数据。使用前需要保证`src`指向的UB空间至少包含256字节有效数据，`dst`指向的GM空间至少包含512KB可写空间。

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

constexpr uint32_t INPUT_BYTES = 256;
constexpr uint32_t OUTPUT_BYTES = 256;

__global__ __vector__ void asc_copy_ub2gm_arch_3510_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[INPUT_BYTES];
    asc_copy_gm2ub_align(local, input, INPUT_BYTES);
    asc_sync_mte2(0);
    // Copy INPUT_BYTES from UB to GM, then wait only for PIPE_MTE3.
    asc_copy_ub2gm(output, local, INPUT_BYTES);
    asc_sync_mte3(0);
}

void print_data(const char* name, const std::vector<uint8_t>& data)
{
    std::cout << name << ":";
    const uint32_t count = data.size() < 32 ? data.size() : 32;
    for (uint32_t i = 0; i < count; ++i) std::cout << ' ' << +data[i];
    if (data.size() > count) std::cout << " ...";
    std::cout << std::endl;
}
} // namespace

int main()
{
    std::vector<uint8_t> input(INPUT_BYTES), output(OUTPUT_BYTES, 0), golden(OUTPUT_BYTES, 0);
    for (uint32_t i = 0; i < INPUT_BYTES; ++i) input[i] = static_cast<uint8_t>(i + 1);
    for (uint32_t i = 0; i < 256; ++i) golden[i] = input[i];
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_copy_ub2gm_arch_3510_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_ub2gm passed." : "[Failed] asc_copy_ub2gm failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
