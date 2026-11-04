# expand

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"tensor_api/experimental/arch/vector/reg_compute_sel.h"`，命名空间为`asc::te::experimental`。

根据掩码生成排除当前位置的前缀计数索引：结果第0个元素为0，第i个元素等于`mask.reg`中第0至第i-1个元素对应的有效比特中1的数量。按T的元素位宽解释掩码，最后一个元素对应的掩码位不参与任何结果元素的计数。

例如，掩码前六个元素对应的有效比特为`{1, 0, 1, 1, 0, 1}`时，结果前六个元素为`{0, 1, 1, 2, 3, 3}`。

本接口生成索引，不接收待展开的数据，也不直接将压缩数据恢复到原位置。本接口仅在AIV上生效。

## 函数原型

```cpp
template <typename T>
__simd_callee__ inline reg_tensor<T> expand(reg_tensor<bool> mask);
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 结果索引的元素类型，须显式指定。支持的类型请参见[数据类型](#数据类型)。 |

**表2** 函数参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| mask | 输入 | 源掩码，类型为[reg_tensor&lt;bool&gt;](../reg_tensor/reg_tensor.md)。`mask.reg`参与前缀计数，不作为返回对象的有效性掩码。 |

## 数据类型

T支持：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t。

## 返回值说明

返回`reg_tensor<T>`，`reg`保存前缀计数索引，`mask`为`all_mask<T>().reg`，即按结果元素类型设置的全有效掩码。

## 约束说明

- `mask.reg`须已初始化，且须按T的元素位宽构造。`mask.mask`不参与计算。
- 返回对象的掩码为全有效掩码，可直接在后续操作中使用全部前缀计数。若仅需使用输入掩码为1的位置，应通过[with_mask](../reg_tensor/reg_tensor.md#with_mask)为结果重新绑定输入掩码。
- 有符号结果类型仍受对应类型可表示范围限制。例如，int8_t不能表示大于127的非负计数，需要保留该范围以上计数时应选用适合的数据类型。

## 调用示例

以下代码片段在VF中调用。`selection.reg`已初始化，按uint16_t的元素位宽构造。返回对象的掩码为uint16_t类型的全有效掩码，可使用全部前缀计数。

```cpp
__simd_callee__ inline asc::te::experimental::reg_tensor<uint16_t> expand_example(
    asc::te::experimental::reg_tensor<bool> selection)
{
    return asc::te::experimental::expand<uint16_t>(selection);
}
```
