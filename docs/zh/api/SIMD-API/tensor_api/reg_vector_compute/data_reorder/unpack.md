# unpack

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

选取源寄存器的低半部分或高半部分，将其中各元素的位宽扩展为原来的两倍，按原顺序写入结果寄存器。

- 无符号整型执行零扩展，高位补0。
- 有符号整型执行符号扩展，保持元素的符号。
- bool类型将所选半部分中的每个比特扩展为两个比特，低位为原比特，高位补0。

本接口仅在AIV上生效。

## 函数原型

```cpp
template <high_low_part part = high_low_part::low, typename T>
__simd_callee__ inline decltype(auto) unpack(reg_tensor<T> src);
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| part | 源数据的选取位置。`high_low_part::low`表示选取源寄存器的低半部分，`high_low_part::high`表示选取高半部分。默认值为`high_low_part::low`。 |
| T | 源寄存器的元素类型，支持的类型请参见[数据类型](#数据类型)。 |

**表2** 函数参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src | 输入 | 源操作数，类型为[reg_tensor&lt;T&gt;](../reg_tensor/reg_tensor.md)。 |

## 数据类型

| T | 返回值类型 |
| --- | --- |
| bool | reg_tensor&lt;bool&gt; |
| int8_t | reg_tensor<int16_t> |
| uint8_t | reg_tensor<uint16_t> |
| int16_t | reg_tensor<int32_t> |
| uint16_t | reg_tensor<uint32_t> |

## 返回值说明

返回类型由T推导，具体映射见数据类型表。`reg`保存扩展后的数据，`mask`为`all_mask<结果元素类型>().reg`，即按结果元素类型设置的全有效掩码。

## 约束说明

- `part`仅支持`high_low_part::low`和`high_low_part::high`。
- 扩展计算不使用`src.mask`屏蔽元素，须保证源寄存器所选半部分的数据已初始化。
- 返回值自动携带按结果元素类型设置的全有效掩码，无需为源寄存器绑定掩码。仅需使用部分结果时，应通过[with_mask](../reg_tensor/reg_tensor.md#with_mask)重新绑定适合有效位置的掩码。

## 调用示例

以下代码片段在VF中调用。`src.reg`的低半部分已初始化，将其符号扩展为int16_t，返回结果自动携带int16_t类型的全有效掩码，无需额外绑定掩码。

```cpp
__simd_callee__ inline asc::te::experimental::reg_tensor<int16_t> unpack_example(
    asc::te::experimental::reg_tensor<int8_t> src)
{
    return asc::te::experimental::unpack<asc::te::experimental::high_low_part::low>(src);
}
```
