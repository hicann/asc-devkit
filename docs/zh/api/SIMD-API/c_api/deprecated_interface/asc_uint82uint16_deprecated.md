# asc_uint82uint16（废弃）

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

头文件路径为：`"c_api/reg_compute/reg_convert.h"`。

**该接口通过接口后缀来控制源操作数读取位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用[asc_uint82uint16](../reg_compute/reg_convert/asc_uint82uint16.md)的显式位置参数重载替代。**

根据`mask`将源操作数`src`中的每个`uint8_t`类型元素转为`uint16_t`类型，结果写入`dst`。由于源操作数与目的操作数类型位宽比为1:2，读取数据时需要将一个`VL`大小的数据分为两部分，根据位置参数选择输入数据索引为奇数的位置或偶数的位置。参考伪代码如下：

```python
# 位置判断仅用于说明接口语义，实际位置参数为编译期常量。
# VL表示矢量数据寄存器位宽，取值256字节。
def asc_uint82uint16(dst, src, mask, src_pos):
    offset = 0 if src_pos == ASC_POSITION_EVEN else 1
    for i in range(VL // 2):
        src_index = 2 * i + offset
        dst[i] = uint16(src[src_index]) if mask[src_index] else 0
```

### 迁移说明

**表1** 新旧位置参数对应关系

| 旧原型位置后缀 | 新原型位置参数 |
| --- | --- |
| 无位置后缀 | `ASC_POSITION_EVEN` |
| `_v2` | `ASC_POSITION_ODD` |

迁移时移除函数名中的位置后缀，并将对应位置常量作为第四个参数传入。新原型请参见[asc_uint82uint16](../reg_compute/reg_convert/asc_uint82uint16.md)。

## 函数原型

```c
__simd_callee__ inline void asc_uint82uint16(vector_uint16_t& dst,
                                             vector_uint8_t src,
                                             vector_bool mask,
                                             std::integral_constant<asc_position_mode, asc_position_mode::EVEN> src_pos)

__simd_callee__ inline void asc_uint82uint16(vector_uint16_t& dst,
                                             vector_uint8_t src,
                                             vector_bool mask,
                                             std::integral_constant<asc_position_mode, asc_position_mode::ODD> src_pos)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述                                                                                                               |
| ------ | --------- | ------------------------------------------------------------------------------------------------------------------ |
| dst  | 输出      | 目的操作数（矢量数据寄存器）。                                                                                     |
| src  | 输入      | 源操作数（矢量数据寄存器）。                                                                                       |
| mask | 输入      | 掩码寄存器，用于控制各元素是否参与计算。`mask`中与元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |
| src_pos | 输入 | 位置选择标签（编译器标签类型），类型为`std::integral_constant<asc_position_mode, asc_position_mode::EVEN>`或`std::integral_constant<asc_position_mode, asc_position_mode::ODD>`。取`ASC_POSITION_EVEN`时选择读取源操作数索引为偶数的位置；取`ASC_POSITION_ODD`时选择读取源操作数索引为奇数的位置。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- `src`与`dst`的数据类型需要与函数原型匹配。
- `mask`掩码位为0时，`dst`对应元素置0。

## 调用示例

```c
vector_uint8_t src;
vector_half dst;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_loadalign(src, src_addr); // src_addr是外部输入的Unified Buffer（UB）内存空间地址。
asc_uint82uint16(dst, src, mask);
```
