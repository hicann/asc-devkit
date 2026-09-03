# asc_copy_gm2ub_align

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

提供数据非对齐搬运的功能，将数据从Global Memory (GM)搬运到Unified Buffer（UB），并支持8位/16位/32位数据类型搬运。

本接口支持以下两种数据搬运方式：

- 连续数据搬运

    若搬运数据长度非32字节对齐，搬运数据会补齐至32字节对齐，支持以下两种填充方式：

    - 手动填充：搬运前调用[asc_set_copy_pad_val](../asc_set_copy_pad_val.md)配置填充值。
    - 自动填充：由硬件自动填充dummy假数据，dummy假数据的值为数据块的第一个元素的值。

- 高维切分数据搬运

    若搬运数据长度非32字节对齐，会将搬运数据补齐至32字节对齐。可通过配置参数`dst_stride`选择Normal模式或Compact模式。非32字节对齐场景支持以下两种填充方式：

    - 手动填充：搬运前调用`asc_set_copy_pad_val`设置填充值，并将`enable_constant_pad`设置为`true`。
    - 自动填充：将`enable_constant_pad`设置为`false`，由硬件自动填充dummy假数据，dummy假数据的值为数据块的第一个元素的值。

    数据搬运模式说明如下：

    - Compact模式：将`dst_stride`设置为`burst_len`，并将`left_padding_num`和`right_padding_num`设置为0。多个数据块在UB中紧密排列，在32字节补齐处理上被视为一个整体；若所有数据块的总长度非32字节对齐，则仅在整体末尾通过上述手动或自动填充方式补齐至32字节对齐。
    - Normal模式：将`dst_stride`设置为不等于`burst_len`且满足32字节对齐的值。相邻目的数据块的首地址间隔由`dst_stride`指定，数据块之间可以预留间隔，无需紧密排列。
        - 当`left_padding_num`和`right_padding_num`均为0时，通过上述手动或自动填充方式将每个数据块分别补齐至32字节对齐。
        - 当`left_padding_num`或`right_padding_num`非0时，`enable_constant_pad`不生效，必须在搬运前调用`asc_set_copy_pad_val`配置填充值。若数据块长度与左右Padding的总字节数非32字节对齐，硬件会在右侧继续填充dummy假数据至32字节对齐，dummy假数据的值为`asc_set_copy_pad_val`配置的值。

    例如，搬运3个长度为48字节且无左右Padding的数据块时：
    - Compact模式下将`dst_stride`设置为48，各数据块在UB中的起始偏移依次为0、48、96，仅在144字节有效数据的末尾补齐16字节。
    - Normal模式下将`dst_stride`设置为64，各数据块在UB中的起始偏移依次为0、64、128，每个数据块分别补齐16字节。

    当只搬运1个数据块，或`burst_len`已经32字节对齐且无左右Padding时，两种模式的搬运结果相同。

本接口仅在AIV上生效。

## 函数原型

### 连续数据搬运（占位符形式）

```c
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ <dtype>* dst,
                                            __gm__ <dtype>* src,
                                            uint32_t size)
```

#### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

#### 函数原型典型示例

```c
// 示例：源与目的数据类型为bfloat16_t
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ bfloat16_t* dst,
                                            __gm__ bfloat16_t* src,
                                            uint32_t size)
```

### 高维切分数据搬运（占位符形式）

```c
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ <dtype>* dst,
                                            __gm__ <dtype>* src,
                                            uint32_t burst_count,
                                            uint32_t burst_len,
                                            uint8_t left_padding_num,
                                            uint8_t right_padding_num,
                                            bool enable_constant_pad,
                                            asc_load_l2_cache_mode l2_cache_mode,
                                            uint64_t src_stride,
                                            uint32_t dst_stride)
```

#### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

#### 函数原型典型示例

```c
// 示例：源与目的数据类型为bfloat16_t
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ bfloat16_t* dst,
                                            __gm__ bfloat16_t* src,
                                            uint32_t burst_count,
                                            uint32_t burst_len,
                                            uint8_t left_padding_num,
                                            uint8_t right_padding_num,
                                            bool enable_constant_pad,
                                            asc_load_l2_cache_mode l2_cache_mode,
                                            uint64_t src_stride,
                                            uint32_t dst_stride)
```

## 参数说明

### 连续数据搬运

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的UB的起始地址。需要32字节对齐。 |
| src | 输入 | 源GM的起始地址。需要1字节对齐。 |
| size | 输入 | 搬运数据大小，单位为字节。取值范围：[1, $2^{21}−1$]。 |

### 高维切分数据搬运

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输出 | 目的UB的起始地址。需要32字节对齐。 |
| src | 输入 | 源GM的起始地址。需要1字节对齐。 |
| burst_count | 输入 | 待搬运的连续传输数据块个数。取值范围：[1, $2^{12}−1$]。 |
| burst_len | 输入 | 待搬运的每个连续传输数据块的长度，单位为字节。取值范围：[1, $2^{21}−1$]。 |
| left_padding_num | 输入 | 连续搬运数据块左侧需要补充的元素个数。该参数对应的填充数据大小不能超过32字节。Compact模式下需要设置为0。 |
| right_padding_num | 输入 | 连续搬运数据块右侧需要补充的元素个数。该参数对应的填充数据大小不能超过32字节。Compact模式下需要设置为0。 |
| enable_constant_pad | 输入 | 当`left_padding_num`和`right_padding_num`均为0时，配置非对齐场景的填充方式。取值说明如下：  <br>&bull; `true`：手动填充，填充值为接口`asc_set_copy_pad_val`设置的值。 <br>&bull; `false`：自动填充，由硬件填充dummy假数据，dummy假数据的值为数据块的第一个元素的值。<br>当`left_padding_num`或`right_padding_num`非0时，该参数不生效。 |
| l2_cache_mode | 输入 | [asc_load_l2_cache_mode](../../defs/enum/asc_load_l2_cache_mode.md)类型的枚举值，配置数据在L2 Cache中的管理策略。 |
| src_stride | 输入 | 源操作数相邻连续数据块的距离（前面一个数据块的头与后面一个数据块的头的间隔），单位为字节。取值范围：[0, $2^{40}−1$]。<br>只搬运1个数据块，即`burst_count`设置为1时，可以将此参数设置为0。 |
| dst_stride | 输入 | 目的操作数相邻连续数据块的距离（前面一个数据块的头与后面一个数据块的头的间隔），单位为字节，用于选择数据搬运模式。取值范围：[0, $2^{21}−1$]。<br>&bull; 等于`burst_len`：Compact模式，目的数据块在UB中紧密排列，`dst_stride`支持字节对齐。<br>&bull; 不等于`burst_len`：Normal模式，`dst_stride`需要满足32字节对齐要求。<br>只搬运1个数据块，即`burst_count`设置为1时，可以将此参数设置为0。 |

## 返回值说明

无

## 流水类型

PIPE_MTE2

## 约束说明

### 通用约束

- 本接口在非AIV上调用直接返回。
- 各存储单元的空间大小和对齐要求请参考[存储单元说明](../../general_description_and_constraints.md#存储单元说明)。
- 如果本指令与其他指令存在UB地址重叠，必须插入同步指令[asc_sync_notify](../../sync/asc_sync_notify.md)和[asc_sync_wait](../../sync/asc_sync_wait.md)，保证多个指令串行化，防止出现异常数据。

### 连续数据搬运约束

- 若`size`非32字节对齐，搬运数据会补齐至32字节对齐，目的UB需要预留补齐后的空间。手动填充时，调用`asc_set_copy_pad_val`配置填充值；自动填充时，由硬件填充dummy假数据，dummy假数据的值为数据块的第一个元素的值。
- `size`需满足dtype字节对齐：dtype为`int16_t`、`uint16_t`、`half`、`bfloat16_t`时需为2的倍数，dtype为`int32_t`、`uint32_t`、`float`时需为4的倍数。

### 高维切分数据搬运约束

- `len_burst`需满足dtype字节对齐：dtype为`int16_t`、`uint16_t`、`half`、`bfloat16_t`时需为2的倍数，dtype为`int32_t`、`uint32_t`、`float`时需为4的倍数。
- 当`left_padding_num`或`right_padding_num`非0时，`enable_constant_pad`不生效，必须在搬运前调用`asc_set_copy_pad_val`配置填充值。`left_padding_num`、`right_padding_num`对应的填充数据大小均不能超过32字节。
- 当`dst_stride`不等于`burst_len`时，`dst_stride`要求32字节对齐。

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

__global__ __vector__ void asc_copy_gm2ub_align_arch3510_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[INPUT_BYTES];
    // Copy INPUT_BYTES from GM to UB, then wait only for PIPE_MTE2.
    asc_copy_gm2ub_align(local, input, INPUT_BYTES);
    asc_sync_mte2(0);
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
    for (uint32_t i = 0; i < 256; ++i) golden[i] = input[i];
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_copy_gm2ub_align_arch3510_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_copy_gm2ub_align passed." : "[Failed] asc_copy_gm2ub_align failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
