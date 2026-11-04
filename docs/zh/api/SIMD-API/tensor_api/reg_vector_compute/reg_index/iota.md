# iota

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

头文件路径为：`"tensor_api/experimental/arch/vector/reg_index_compute.h"`，命名空间为`asc::te::experimental`。

以标量`start`为基值，生成步长为1的递增或递减序列，保存在返回的[reg_tensor](../reg_tensor/reg_tensor.md)中。设`VL_T`为一个矢量数据寄存器可容纳的结果元素个数，序列如下：

| order | 结果序列 |
| --- | --- |
| iota_order::increase | `{start, start + 1, ..., start + VL_T - 1}` |
| iota_order::decrease | `{start + VL_T - 1, start + VL_T - 2, ..., start}` |

递减模式将递增模式的整个序列反向排列，不是从`start`逐次减1。例如，int16_t的`VL_T`为128，`start`为10时，递增序列为`{10, 11, ..., 137}`，递减序列为`{137, 136, ..., 10}`。

本接口仅在AIV上生效。递增模式调用`asc_arange`，递减模式调用`asc_arange_descend`，底层语义请参见[asc_arange](../../../c_api/reg_compute/reg_index/asc_arange.md)。

## 函数原型

```cpp
template <iota_order order = iota_order::increase, typename T>
__simd_callee__ inline reg_tensor<Std::remove_cvref_t<T>> iota(const T& start);
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| order | 序列方向。`iota_order::increase`表示递增，`iota_order::decrease`表示递减。默认值为`iota_order::increase`。 |
| T | 标量start的类型，可由函数实参推导。去除const、volatile和引用限定后须为[数据类型](#数据类型)中支持的类型。 |

**表2** 函数参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| start | 输入 | 生成序列的标量基值。递增模式的首元素和递减模式的末元素为start，取值须在对应数据类型的可表示范围内。 |

## 数据类型

去除const、volatile和引用限定后的T支持：int8_t、int16_t、half、int32_t、float。

## 返回值说明

返回`reg_tensor<Std::remove_cvref_t<T>>`，`reg`保存生成的序列，`mask`为`all_mask<Std::remove_cvref_t<T>>().reg`，即按结果元素类型设置的全有效掩码。

## 约束说明

- `order`仅支持`iota_order::increase`和`iota_order::decrease`。
- 生成序列的长度由结果元素类型和寄存器位宽决定，不由`start`的数值决定。
- 整型结果超出对应数据类型的可表示范围时回绕，不触发异常。
- 返回对象自动携带全有效掩码。仅需使用部分序列时，应在后续计算或写回前通过[with_mask](../reg_tensor/reg_tensor.md#with_mask)重新绑定有效掩码。

## 调用示例

以下代码片段在VF中调用。分别生成以10为基值的int32_t递增和递减序列，两个结果自动携带int32_t类型的全有效掩码。

```cpp
__simd_callee__ inline void iota_example(
    asc::te::experimental::reg_tensor<int32_t>& ascending,
    asc::te::experimental::reg_tensor<int32_t>& descending)
{
    const int32_t start = 10;
    ascending = asc::te::experimental::iota(start);
    descending = asc::te::experimental::iota<asc::te::experimental::iota_order::decrease>(start);
}
```
