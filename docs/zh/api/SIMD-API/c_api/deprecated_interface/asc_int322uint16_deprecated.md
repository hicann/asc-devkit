# asc_int322uint16（废弃）

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

**该接口通过接口后缀来控制目的操作数写入位置（仅包括源操作数、目的操作数、掩码寄存器三个参数）的原型已废弃，请使用[asc_int322uint16](../reg_compute/reg_convert/asc_int322uint16.md)的显式位置参数重载替代。**

根据`mask`将`src`中的每个`int32_t`类型元素转换为`uint16_t`类型，并支持不同的饱和模式，结果写入`dst`。由于源操作数与目的操作数类型位宽比为2:1，写入数据时需要将一个`VL`大小的数据分为两部分，根据不同接口选择数据写入索引为奇数的位置或偶数的位置。参考伪代码：

```python
def asc_int322uint16(dst, src, mask):
    for i in range(64):
        if mask[i]:
            dst[2 * i] = uint16(src[i])   # int32 -> uint16，写入偶数索引
        else:
            dst[2 * i] = 0
    for i in range(64):
        dst[2 * i + 1] = 0                # 奇数索引置零

def asc_int322uint16_v2(dst, src, mask):
    for i in range(64):
        dst[2 * i] = 0                    # 偶数索引置零
        if mask[i]:
            dst[2 * i + 1] = uint16(src[i])  # int32 -> uint16，写入奇数索引
        else:
            dst[2 * i + 1] = 0
```

关于舍入模式和饱和/非饱和模式的详细说明，请参见[舍入模式](../reg_compute/reg_convert/rounding_mode.md)。

### 迁移说明

**表1** 新旧位置参数对应关系

| 旧原型位置后缀 | 新原型位置参数 |
| --- | --- |
| 无位置后缀 | `ASC_POSITION_EVEN` |
| `_v2` | `ASC_POSITION_ODD` |

迁移时移除函数名中的位置后缀，并将对应位置常量作为第四个参数传入。新原型请参见[asc_int322uint16](../reg_compute/reg_convert/asc_int322uint16.md)。

## 函数原型

### 模板原型（占位符形式）

```c
__simd_callee__ inline void asc_int322uint16<sat_mode><position_mode>(vector_uint16_t& dst,
                                                                      vector_int32_t src,
                                                                      vector_bool mask)
```

### 占位符说明

- `<sat_mode>`表示饱和/非饱和模式，为空时表示非饱和模式，取值为`_sat`时表示饱和模式。
- `<position_mode>`表示`dst`写入的位置，为空时，结果写入`dst`的偶数索引位置；取值为`_v2`时，结果写入`dst`的奇数索引位置。

### 函数原型典型示例

```c
// 非饱和模式，数据写入索引为偶数的位置
__simd_callee__ inline void asc_int322uint16(vector_uint16_t& dst,
                                             vector_int32_t src,
                                             vector_bool mask)
```

## 参数说明

**表2** 参数说明

| 参数名 | 输入/输出 | 描述                                                                                                               |
| ------ | --------- | ------------------------------------------------------------------------------------------------------------------ |
| dst  | 输出      | 目的操作数（矢量数据寄存器）。                                                                                     |
| src  | 输入      | 源操作数（矢量数据寄存器）。                                                                                       |
| mask | 输入      | 掩码寄存器，用于控制各元素是否参与计算。`mask`中与元素对应的比特位为1时，该元素参与计算；为0时，该元素不参与计算。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[reg数据类型定义](../defs/type/data_type_definition.md)。

## 返回值说明

无

## 约束说明

- `src`与`dst`的数据类型需要与函数原型匹配。
- `mask`掩码位为0时，`dst`对应元素置0。

## 调用示例

```cpp
__simd_vf__ inline void int322uint16_vf(__ubuf__ int32_t* src_addr, __ubuf__ uint16_t* dst_addr, uint32_t count, uint16_t one_repeat_size, uint16_t one_block_size, uint16_t repeat_time)
{
    vector_int32_t src;
    vector_uint16_t dst;
    vector_bool mask;
    for (uint16_t i = 0; i < repeat_time; ++i) {
        mask = asc_update_mask_b32(count);
        asc_loadalign_postupdate(src, src_addr, one_repeat_size);
        asc_int322uint16(dst, src, mask);
        asc_storealign_pack_postupdate(dst_addr, dst, one_block_size, mask);
    }
}
```
