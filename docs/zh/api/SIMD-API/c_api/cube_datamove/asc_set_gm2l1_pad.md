# asc_set_gm2l1_pad

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

头文件路径为：`"c_api/cube_datamove/cube_datamove.h"`。

用于设置[asc_copy_gm2l1_align](asc_copy_gm2l1_align.md)接口的常量填充值。调用`asc_copy_gm2l1_align`时，以下场景使用本接口设置的值：

- `data_select_bit`设置为`true`，且`left_padding_count`与`right_padding_count`均为`0`时，Normal填充模式或Compact填充模式的填充数据使用本接口设置的值。
- `left_padding_count`或`right_padding_count`非`0`时，进入左右填充模式。此时硬件强制使用本接口设置的值，`data_select_bit`设置无效。

当`data_select_bit`设置为`false`且未开启左右填充模式时，填充数据取每个连续数据块的首元素，本接口设置的值不生效。

本接口仅在AIC上生效。

## 函数原型

```cpp
__aicore__ inline void asc_set_gm2l1_pad(uint32_t pad_val)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| pad_val | 输入 | 常量填充值。接口形参为`uint32_t`，`asc_copy_gm2l1_align`根据`dtype`使用对应低位作为单个填充元素的值。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 本接口非AIC调用直接返回。
- 调用[asc_copy_gm2l1_align](asc_copy_gm2l1_align.md)前调用本接口配置填充值。

## 调用示例

完整的Normal填充、Compact填充和左右填充场景示例请参见[asc_copy_gm2l1_align调用示例](asc_copy_gm2l1_align.md#调用示例)。

将代码保存为`examples.asc`后，可通过`bisheng`命令编译，其中`--npu-arch`参数需根据实际产品型号指定对应的NPU架构，具体产品与NPU架构的映射关系请参考[`__NPU_ARCH__`](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)。

<!-- npu="950" id8 -->
以Ascend 950PR/Ascend 950DT产品（对应NPU架构为`dav-3510`）为例，编译命令如下：

```bash
bisheng examples.asc -o main --npu-arch=dav-3510
```
<!-- end id8 -->

以下示例搬运3个48B的`uint8_t`数据块，使用常量`0xA5`填充每个数据块尾部的16B，使每个数据块按64B步长写入L1 Buffer。

```cpp
#include <cstdint>
#include "c_api/asc_simd.h"

// 该核函数在AIC上执行GM到L1的搬运。
__global__ __cube__ void Gm2L1AlignPadExample(__gm__ uint8_t* src)
{
    asc_init();
    __cbuf__ uint8_t dst[192];

    uint8_t PAD_VALUE = 0xA5;
    constexpr uint32_t N_BURST = 3;
    constexpr uint32_t LEN_BURST = 48;
    constexpr uint32_t DST_STRIDE = 64;
    constexpr uint8_t L2_CACHE_CTL = 4;
    // 配置常量填充值；uint8_t填充字节为0xA5，实参会提升为接口要求的uint32_t类型。
    asc_set_gm2l1_pad(*reinterpret_cast<uint8_t*>(&PAD_VALUE));
    // data_select_bit=true，Normal模式每个48B数据块的尾部填充16B常量值。
    asc_copy_gm2l1_align(dst, src, N_BURST, LEN_BURST, 0, 0, true, L2_CACHE_CTL, LEN_BURST, DST_STRIDE);
}
```
