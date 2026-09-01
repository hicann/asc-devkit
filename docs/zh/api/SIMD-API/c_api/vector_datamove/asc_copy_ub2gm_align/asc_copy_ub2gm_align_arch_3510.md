# asc_copy_ub2gm_align

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

提供数据非对齐搬运的功能，将数据从Unified Buffer (UB)搬运到Global Memory (GM)，并支持8位/16位/32位数据类型搬运。

本接口支持以下两种数据搬运方式，均不支持设置填充值：

- 连续数据搬运

    若搬运数据长度非32字节对齐，硬件在读取UB数据时自动补充dummy假数据，将搬运数据补齐至32字节对齐。写入GM时会丢弃补充的dummy假数据。

- 高维切分数据搬运

    若搬运数据长度非32字节对齐，硬件在读取UB数据时补充dummy假数据，将搬运数据补齐至32字节对齐。写入GM时会丢弃补充的dummy假数据。可通过配置参数`src_stride`选择Normal模式或Compact模式：

    - Compact模式：将`src_stride`设置为`burst_len`。多个数据块在UB中紧密排列，在32字节补齐处理上被视为一个整体；若所有数据块的总长度非32字节对齐，则仅在整体末尾补充dummy假数据至32字节对齐，写入GM时会丢弃补充的dummy假数据。
    - Normal模式：将`src_stride`设置为不等于`burst_len`且满足32字节对齐的值，每个数据块单独进行32字节对齐处理。
        - 若`burst_len`为32字节对齐，不会读取dummy假数据，从UB读取的所有数据都会写入GM。
        - 若`burst_len`非32字节对齐，读取UB数据时会补充dummy假数据至32字节对齐，写入GM时会丢弃补充的dummy假数据。

    例如，搬运3个长度为48字节的数据块时：
    - Compact模式下将`src_stride`设置为48，各数据块在UB中的起始偏移依次为0、48、96，仅在144字节有效数据的末尾补充16字节dummy假数据。
    - Normal模式下将`src_stride`设置为64，各数据块在UB中的起始偏移依次为0、64、128，每个数据块分别补充16字节dummy假数据。

    两种模式下，dummy假数据均不会写入GM。当只搬运1个数据块，或`burst_len`已经32字节对齐时，两种模式的搬运结果相同。

本接口仅在AIV上生效。

## 函数原型

### 连续数据搬运（占位符形式）

```c
__aicore__ inline void asc_copy_ub2gm_align(__gm__ <dtype>* dst,
                                            __ubuf__ <dtype>* src,
                                            uint32_t size)
```

#### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

#### 函数原型典型示例

```c
// 示例：源与目的数据类型为bfloat16_t
__aicore__ inline void asc_copy_ub2gm_align(__gm__ bfloat16_t* dst,
                                            __ubuf__ bfloat16_t* src,
                                            uint32_t size)
```

### 高维切分数据搬运（占位符形式）

```c
__aicore__ inline void asc_copy_ub2gm_align(__gm__ <dtype>* dst,
                                            __ubuf__ <dtype>* src,
                                            uint32_t burst_count,
                                            uint32_t burst_len,
                                            asc_store_l2_cache_mode l2_cache_mode,
                                            uint64_t dst_stride,
                                            uint32_t src_stride)
```

#### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

#### 函数原型典型示例

```c
// 示例：源与目的数据类型为bfloat16_t
__aicore__ inline void asc_copy_ub2gm_align(__gm__ bfloat16_t* dst,
                                            __ubuf__ bfloat16_t* src,
                                            uint32_t burst_count,
                                            uint32_t burst_len,
                                            asc_store_l2_cache_mode l2_cache_mode,
                                            uint64_t dst_stride,
                                            uint32_t src_stride)
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
| burst_count | 输入 | 待搬运的连续传输数据块个数。取值范围：[1, $2^{12}−1$]。 |
| burst_len | 输入 | 待搬运的每个连续传输数据块的长度，单位为字节。取值范围：[1, $2^{21}−1$]。 |
| l2_cache_mode | 输入 | [asc_store_l2_cache_mode](../../defs/enum/asc_store_l2_cache_mode.md)类型的枚举值，配置数据在L2 Cache中的管理策略。 |
| dst_stride | 输入 | 目的操作数相邻连续数据块的距离（前面一个数据块的头与后面一个数据块的头的间隔），单位为字节。取值范围：[0, $2^{40}−1$]。<br>只搬运1个数据块，即`burst_count`设置为1时，可以将此参数设置为0。 |
| src_stride | 输入 | 源操作数相邻连续数据块的距离（前面一个数据块的头与后面一个数据块的头的间隔），单位为字节，用于选择数据搬运模式。取值范围：[0, $2^{21}−1$]。<br>&bull; 等于`burst_len`：Compact模式，源数据块在UB中紧密排列，`src_stride`支持字节对齐。<br>&bull; 不等于`burst_len`：Normal模式，`src_stride`需要满足32字节对齐要求。<br>只搬运1个数据块，即`burst_count`设置为1时，可以将此参数设置为0。 |

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

- `size`需满足dtype字节对齐：dtype为`int16_t`、`uint16_t`、`half`、`bfloat16_t`时需为2的倍数，dtype为`int32_t`、`uint32_t`、`float`时需为4的倍数。

### 高维切分数据搬运约束

- `len_burst`需满足dtype字节对齐：dtype为`int16_t`、`uint16_t`、`half`、`bfloat16_t`时需为2的倍数，dtype为`int32_t`、`uint32_t`、`float`时需为4的倍数。
- 当`src_stride`不等于`burst_len`时，`src_stride`要求32字节对齐。
- 当`src_stride`设置为0时，可能会出现**硬件未定义行为**。如需将同一个源数据块Broadcast到GM的多个连续位置，建议使用loop功能，具体请参考[关键特性说明](#关键特性说明)。

## 关键特性说明

### Broadcast场景

使用高维切分搬运接口时，不建议通过将`src_stride`设置为0实现Broadcast。可通过loop功能实现Broadcast，将`burst_count`设置为1，并通过[asc_set_ub2gm_loop_size](../asc_set_ub2gm_loop_size.md)、[asc_set_ub2gm_loop1_stride](../asc_set_ub2gm_loop1_stride.md)和[asc_set_ub2gm_loop2_stride](../asc_set_ub2gm_loop2_stride.md)配置循环搬运：

- 将`loop1_size`设置为Broadcast次数，将`loop2_size`设置为1。
- 将`loop1_src_stride`设置为0，使每次内层循环读取同一个UB源数据块。
- 将`loop1_dst_stride`设置为单个数据块的字节数，使每次内层循环写入GM的下一个连续位置。
- 将`loop2_src_stride`和`loop2_dst_stride`设置为0。外层循环只执行1次，这两个参数不产生地址偏移。
- 将`asc_copy_ub2gm_align`的`burst_count`设置为1，并将`src_stride`设置为非0值。循环搬运结束后，调用`asc_set_ub2gm_loop_size`将循环次数复位为1。

以下示例将UB中的256字节源数据块重复搬运2048次，在GM中生成512KB的连续数据。使用前需要保证`src`指向的UB空间至少包含256字节有效数据，`dst`指向的GM空间至少包含512KB可写空间。

```cpp
constexpr uint32_t block_bytes = 256;
constexpr uint32_t broadcast_count = 2048;

asc_set_ub2gm_loop_size(broadcast_count, 1);
asc_set_ub2gm_loop1_stride(0, block_bytes);
asc_set_ub2gm_loop2_stride(0, 0);
asc_copy_ub2gm_align(dst, src, 1, block_bytes,
    asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, block_bytes, block_bytes);
asc_set_ub2gm_loop_size(1, 1);
```

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

__global__ __vector__ void asc_copy_ub2gm_align_arch3510_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[INPUT_BYTES];
    asc_copy_gm2ub_align(local, input, INPUT_BYTES);
    asc_sync_mte2(0);
    // Copy INPUT_BYTES from UB to GM, then wait only for PIPE_MTE3.
    asc_copy_ub2gm_align(output, local, INPUT_BYTES);
    asc_sync_mte3(0);
    asc_sync_pipe(PIPE_ALL);
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
    for (uint32_t i = 0; i < OUTPUT_BYTES; ++i) golden[i] = input[i];
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_copy_ub2gm_align_arch3510_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_ub2gm_align passed." : "[Failed] asc_copy_ub2gm_align failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
