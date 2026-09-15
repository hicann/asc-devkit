# asc_ndim_copy_gm2ub

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

该接口用于将多维数据从Global Memory（GM）搬运到Unified Buffer（UB）。相比基础数据搬运接口，本接口可以灵活配置搬入数据的维度信息。

本接口最多支持5个维度，每个维度均可单独配置Stride，并支持随路Padding功能。相关配置参数需通过本接口及配套配置接口传入。

**表1** Stride配置参数及其传入接口，其中`i`表示第`i`维，取值范围为[0, 4]

| 参数 | 传入接口 | 描述 |
|---|---|---|
| loopi_size | 本接口 | 每个维度内处理的元素个数，不包含Padding元素。<br>单位为元素个数，取值范围为[1, 2^20-1]。 |
| loopi_src_stride | [asc_set_ndim_loopi_stride](asc_set_ndim_loop_stride.md) | 每个维度内，当前源操作数元素与下一个元素间的间隔。<br>单位为元素个数，取值范围为[0, 2^40-1]。 |
| loopi_dst_stride | [asc_set_ndim_loopi_stride](asc_set_ndim_loop_stride.md) | 每个维度内，当前目的操作数元素与下一个元素间的间隔。<br>单位为元素个数，取值范围为[0, 2^20-1]。 |

**表2** Padding配置参数及其传入接口，其中`i`表示第`i`维，取值范围为[0, 4]

| 参数 | 传入接口 | 描述 |
|---|---|---|
| padding_mode | 本接口 | Padding值的填充方式，类型为`bool`。<br>&bull; `true`：使用常数填充，所有Padding值均为固定值`pad_value`。<br>&bull; `false`：使用最近值填充，左右Padding值选取当前维度最左侧或最右侧的值。 |
| pad_value | [asc_set_ndim_pad_value](asc_set_ndim_pad_value.md) | 当`padding_mode`为`true`时，该值作为Padding的固定填充值；当`padding_mode`为`false`时，该值不生效。 |
| loopi_lp_count | <br>&bull; 0维：本接口<br>&bull; 1至4维：[asc_set_ndim_pad_count](asc_set_ndim_pad_count.md) | 每个维度左侧需要补齐的元素个数。<br>单位为元素个数，取值范围为[0, 255]。 |
| loopi_rp_count | <br>&bull; 0维：本接口<br>&bull; 1至4维：[asc_set_ndim_pad_count](asc_set_ndim_pad_count.md) | 每个维度右侧需要补齐的元素个数。<br>单位为元素个数，取值范围为[0, 255]。 |

以下以2维的例子，介绍几个典型使用场景。

- 2D Padding场景，使用固定值填充方式
![](../figures/asc_ndim_copy_gm2ub_2d_padding_constant.png "2D Padding场景，使用固定值填充方式")

- 2D Padding场景，使用最近值填充方式
![](../figures/asc_ndim_copy_gm2ub_2d_padding_nearest.png "2D Padding场景，使用最近值填充方式")

- 2D Transpose场景
![](../figures/asc_ndim_copy_gm2ub_2d_transpose.png "2D Transpose场景")

- 2D Slice场景
![](../figures/asc_ndim_copy_gm2ub_2d_slice.png "2D Slice场景")

- 2D Broadcast场景
![](../figures/asc_ndim_copy_gm2ub_2d_broadcast.png "2D Broadcast场景")

本接口仅在AIV上生效。

## 函数原型

```c
// 占位符形式
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ <dtype>* dst,
                                           __gm__ <dtype>* src,
                                           uint32_t loop0_size,
                                           uint32_t loop1_size,
                                           uint32_t loop2_size,
                                           uint32_t loop3_size,
                                           uint32_t loop4_size,
                                           uint8_t loop0_lp_count,
                                           uint8_t loop0_rp_count,
                                           bool padding_mode,
                                           asc_load_l2_cache_mode l2_cache_mode)
```

### dtype支持数据类型

`dtype`取值为：`int8_t`、`uint8_t`、`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`fp8_e8m0_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`int16_t`、`uint16_t`、`half`、`bfloat16_t`、`int32_t`、`uint32_t`、`float`。

### 函数原型典型示例

```c
// 示例：源与目的数据类型为half
__aicore__ inline void asc_ndim_copy_gm2ub(__ubuf__ half* dst,
                                           __gm__ half* src,
                                           uint32_t loop0_size,
                                           uint32_t loop1_size,
                                           uint32_t loop2_size,
                                           uint32_t loop3_size,
                                           uint32_t loop4_size,
                                           uint8_t loop0_lp_count,
                                           uint8_t loop0_rp_count,
                                           bool padding_mode,
                                           asc_load_l2_cache_mode l2_cache_mode)
```

## 参数说明

**表3** 参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的UB的起始地址，指向padding后数据的起始位置。需要1字节对齐。 |
| src | 输入 | 源GM的起始地址。需要按dtype字节对齐（b8类型要求1字节对齐、b16类型要求2字节对齐、b32类型要求4字节对齐）。 |
| loop0_size | 输入 | 0维处理的元素个数，不包含Padding元素。<br>单位为元素个数，取值范围为[1, $2^{20}−1$]。 |
| loop1_size | 输入 | 1维处理的元素个数，不包含Padding元素。<br>单位为元素个数，取值范围为[1, $2^{20}−1$]。 |
| loop2_size | 输入 | 2维处理的元素个数，不包含Padding元素。<br>单位为元素个数，取值范围为[1, $2^{20}−1$]。 |
| loop3_size | 输入 | 3维处理的元素个数，不包含Padding元素。<br>单位为元素个数，取值范围为[1, $2^{20}−1$]。 |
| loop4_size | 输入 | 4维处理的元素个数，不包含Padding元素。<br>单位为元素个数，取值范围为[1, $2^{20}−1$]。 |
| loop0_lp_count | 输入 | 0维左侧需要补齐的元素个数。<br>单位为元素个数，取值范围为[0, 255]。 |
| loop0_rp_count | 输入 | 0维右侧需要补齐的元素个数。<br>单位为元素个数，取值范围为[0, 255]。 |
| padding_mode | 输入 | Padding值的填充方式。<br>&bull; `true`：使用常数填充，所有Padding值均为固定值`pad_value`。<br>&bull; `false`：使用最近值填充，左右Padding值选取当前维度最左侧或最右侧的值。 |
| l2_cache_mode | 输入 | [asc_load_l2_cache_mode](../defs/enum/asc_load_l2_cache_mode.md)类型的枚举值，用于配置数据在L2 Cache中的管理策略。 |
| cache_mode | 输入 | 已废弃的L2 Cache管理策略配置参数，请改用`l2_cache_mode`。 |

## 返回值说明

无

## 流水类型

PIPE_MTE2

## 约束说明

- 本接口在非AIV上调用直接返回。
- 各存储单元的空间大小和对齐要求请参考[存储单元说明](../general_description_and_constraints.md#存储单元说明)。
- 如果本指令与其他指令存在UB地址重叠，必须插入同步指令[asc_sync_notify](../sync/asc_sync_notify.md)和[asc_sync_wait](../sync/asc_sync_wait.md)，保证多个指令串行化，防止出现异常数据。
- 多核场景下，如果多个AI Core可能读取同一块GM地址，且该GM地址中的数据可能被其他核更新，应在调用本接口前调用[asc_ndim_copy_dci](asc_ndim_copy_dci.md)刷新NDDMA DataCache，保证多核间数据一致性。
- 一条指令所能获取的所有数据的地址范围宽度不能超过40位（1TB），即：源操作数的i维大小为（loopi_lp_size + loopi_size + loopi_rp_size - 1） * loop0_src_stride，目的操作数的i维大小为：（loopi_lp_size + loopi_size + loopi_rp_size - 1） * loop0_dst_stride，所有的维度的大小加起来不超过2^40位。
- 当每维的loopi_dst_stride为升序序列，则不同循环间的地址空间不能交织或重叠。
- 各维源/目步长须通过配套接口（[asc_set_ndim_loop_stride](asc_set_ndim_loop_stride.md)）预先配置，单位为元素个数。各维目的步长需保证不发生写覆盖：若某维实际迭代次数大于1，则该维目的步长需大于低维累计写入元素字节数，否则触发写覆盖异常。
- loop1~loop4维度的左右填充元素数须通过配套接口（[asc_set_ndim_pad_count](asc_set_ndim_pad_count.md)）预先配置，本接口不设置这四个维度的填充。

## 调用示例

2D Padding场景，使用固定值填充方式
![](../figures/asc_ndim_copy_gm2ub_2d_padding_constant.png "2D Padding场景，使用固定值填充方式")

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[\_\_NPU\_ARCH\_\_](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

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

constexpr uint32_t INPUT_BYTES = 12;
constexpr uint32_t OUTPUT_COLUMNS = 6;
constexpr uint32_t OUTPUT_BYTES = OUTPUT_COLUMNS * 9;
constexpr uint8_t PAD_VALUE = 88;

__global__ __vector__ void asc_ndim_copy_gm2ub_kernel(__gm__ uint8_t* output, __gm__ uint8_t* input)
{
    asc_init();
    __ubuf__ uint8_t local[64];
    asc_set_ndim_loop0_stride(1, 1);
    asc_set_ndim_loop1_stride(6, 3);
    asc_set_ndim_loop2_stride(0, 0);
    asc_set_ndim_loop3_stride(0, 0);
    asc_set_ndim_loop4_stride(0, 0);
    asc_set_ndim_pad_value(PAD_VALUE);
    asc_ndim_pad_count_config config;
    config.loop1_lp_count = 2;
    config.loop1_rp_count = 3;
    asc_set_ndim_pad_count(config);
    asc_ndim_copy_gm2ub(local, input, 3, 4, 1, 1, 1, 2, 1, true,
        asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM);
    asc_sync_mte2(0);
    asc_copy_ub2gm_align(output, local, OUTPUT_BYTES);
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
    std::vector<uint8_t> input(INPUT_BYTES), output(OUTPUT_BYTES, 0), golden(OUTPUT_BYTES, PAD_VALUE);
    for (uint32_t i = 0; i < INPUT_BYTES; ++i) input[i] = static_cast<uint8_t>(i);
    for (uint32_t row = 0; row < 4; ++row) {
        for (uint32_t column = 0; column < 3; ++column) {
            golden[(row + 2) * OUTPUT_COLUMNS + column + 2] = input[row * 3 + column];
        }
    }
    aclInit(nullptr);
    aclrtSetDevice(0);
    uint8_t *input_device = nullptr, *output_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&input_device), INPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(reinterpret_cast<void**>(&output_device), OUTPUT_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMemcpy(input_device, INPUT_BYTES, input.data(), INPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(output_device, OUTPUT_BYTES, output.data(), OUTPUT_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    asc_ndim_copy_gm2ub_kernel<<<1, 0>>>(output_device, input_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(output.data(), OUTPUT_BYTES, output_device, OUTPUT_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
    print_data("Input", input);
    print_data("Output", output);
    print_data("Golden", golden);
    const bool passed = output == golden;
    std::cout << (passed ? "[Success] asc_ndim_copy_gm2ub passed." : "[Failed] asc_ndim_copy_gm2ub failed.") << std::endl;
    aclrtFree(input_device);
    aclrtFree(output_device);
    aclrtResetDevice(0);
    aclFinalize();
    return passed ? 0 : 1;
}
```
